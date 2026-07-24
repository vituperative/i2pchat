// SPDX-License-Identifier: AGPL-3.0-or-later

#include "Core.h"

#include "ConnectionManager.h"
#include "FileTransferManager.h"
#include "FileTransferReceive.h"
#include "FileTransferSend.h"
#include "I2PStream.h"
#include "PacketManager.h"
#include "UserManager.h"

#include <QApplication>
#include <QDir>
#include <QFileInfoList>
#include <QImage>
#include <QMessageBox>
#include <QStandardPaths>
#include <QtGlobal>

#include <algorithm>
#include <cmath>
#include <utility>

CCore::CCore(const QString &configPath) {
  mConfigPath = configPath;
  mCurrentOnlineStatus = User::USEROFFLINE;
  mNextOnlineStatus = User::USEROFFLINE;

  mDebugMessageHandler = new CDebugMessageManager("General", configPath);
  mSoundManager = new CSoundManager(mConfigPath);

  QSettings settings(mConfigPath + "/application.ini", QSettings::IniFormat);
  settings.beginGroup("Users");
  m_access_anyone_incoming = settings.value("allow_incoming_new_users", true).toBool();
  settings.endGroup();
  settings.beginGroup("Network");
  mMyDestinationB32 = settings.value("MyDestinationB32", "").toString();

  mConnectionManager = new CConnectionManager(
    settings.value("SamHost", "127.0.0.1").toString(), settings.value("SamPort", "7656").toString(), mConfigPath);

  connect(mConnectionManager,
          SIGNAL(signNamingReplyReceived(const SAM_Message_Types::RESULT, QString, QString, QString)),
          this,
          SLOT(slotNamingReplyReceived(const SAM_Message_Types::RESULT, QString, QString, QString)),
          Qt::DirectConnection);

  connect(
    mConnectionManager, SIGNAL(signStreamControllerStatusOK(bool)), this, SLOT(slotStreamControllerStatusOK(bool)));

  connect(mConnectionManager, SIGNAL(signReconnectAttempt()), this, SLOT(slotReconnectAttempt()));

  connect(mConnectionManager,
          SIGNAL(signNewSamPrivKeyGenerated(const QString)),
          this,
          SLOT(slotNewSamPrivKeyGenerated(const QString)));

  connect(mConnectionManager,
          SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)),
          this,
          SLOT(slotStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, QString)));

  connect(mConnectionManager, SIGNAL(signIncomingStream(CI2PStream *)), this, SLOT(slotIncomingStream(CI2PStream *)));

  connect(mConnectionManager,
          SIGNAL(signDebugMessages(const QString)),
          mDebugMessageHandler,
          SLOT(slotNewIncomingDebugMessage(const QString)));

  if (settings.contains("Destination")) {
    settings.remove("Destination");
  }
  settings.endGroup();
  settings.sync();

  mProtocol = new CProtocol(*this);
  this->mCurrentOnlineStatus = User::USEROFFLINE;

  loadUserInfos();
  mUnsentChatMessageStorage = new CUnsentChatMessageStorage(mConfigPath + "/UnsentChatMessageStorage.ini");
  mUserBlockManager = new CUserBlockManager(*this, mConfigPath + "/UserBlockList.dat");
  mUserManager = new CUserManager(*this, mConfigPath + "/users.config", *mUnsentChatMessageStorage);

  mUserBlockManager->readBlockListe();

  connect(mUserManager, SIGNAL(signUserStatusChanged()), this, SIGNAL(signUserStatusChanged()));
  mUserManager->loadUserList();

  mFileTransferManager = new CFileTransferManager(*this);
  connect(mFileTransferManager, SIGNAL(signUserStatusChanged()), this, SIGNAL(signUserStatusChanged()));
  connect(mFileTransferManager,
          SIGNAL(signFileTransferCreated(qint32, QString, quint64, bool, QString)),
          this,
          SIGNAL(signFileTransferCreated(qint32, QString, quint64, bool, QString)));

  QSettings keepAliveSettings(mConfigPath + "/application.ini", QSettings::IniFormat);
  int keepAliveInterval = keepAliveSettings.value("General/KeepAliveIntervalSecs", 20).toInt();
  keepAliveInterval = std::max(keepAliveInterval, 5);
  mKeepAliveTimer.setInterval(keepAliveInterval * 1000);
  connect(&mKeepAliveTimer, SIGNAL(timeout()), this, SLOT(slotPingClients()));

  connect(&mAutoAwayTimer, SIGNAL(timeout()), this, SLOT(slotAutoAwayTimeout()));
  QSettings autoSettings(mConfigPath + "/application.ini", QSettings::IniFormat);
  mAutoAwayMinutes = autoSettings.value("General/AutoAwayMinutes", 0).toInt();
  if (mAutoAwayMinutes > 0 && autoSettings.value("General/AutoAwayEnabled", false).toBool())
    mAutoAwayTimer.start(mAutoAwayMinutes * 60000);
}

CCore::~CCore() {

  this->closeAllActiveConnections();
  this->mUserManager->saveUserList();
  delete mUnsentChatMessageStorage;
  delete mUserManager;
  delete mSoundManager;

  while (!mDataPacketsManagers.isEmpty()) {
    delete mDataPacketsManagers.takeFirst();
  }

  if (mProtocol != NULL) {
    delete this->mProtocol;
  }

  delete mConnectionManager;

  delete mUserBlockManager;
  delete mDebugMessageHandler;
  delete mFileTransferManager;
}

void CCore::doNamingLookUP(const QString &Name) const {
  mConnectionManager->doNamingLookUP(Name);
}

QString CCore::calcSessionOptionString() const {
  /* Builds SAM session options string from QSettings.
     Keys are passed through as i2cp/i2p.streaming parameters — anything
     not recognized by the router is silently ignored. */
  QString SessionOptionString = "";
  QString Signature_Type = "";

  QSettings settings(mConfigPath + "/application.ini", QSettings::IniFormat);

  settings.beginGroup("Network");

  // + " " for void CSessionController::doSessionCreate() a session option.

  SessionOptionString.append("i2cp.gzip=" + settings.value("i2cp.gzip", "true").toString() + " ");
  SessionOptionString.append(
    "i2cp.messageReliability=" + settings.value("i2cp.messageReliability", "BestEffort").toString() + " ");
  SessionOptionString.append("i2cp.fastReceive=" + settings.value("i2cp.fastReceive", "true").toString() + " ");

  SessionOptionString.append(
    "inbound.nickname=" + settings.value("TunnelName", "I2PChat").toString().replace(" ", "_") + " ");

  // inbound options
  SessionOptionString.append("inbound.quantity=" + settings.value("inbound.quantity", "1").toString() + " ");
  SessionOptionString.append("inbound.backupQuantity=" + settings.value("inbound.backupQuantity", "1").toString() +
                             " ");
  SessionOptionString.append("inbound.length=" + settings.value("inbound.length", "3").toString() + " ");

  // outbound options
  SessionOptionString.append("outbound.quantity=" + settings.value("outbound.quantity", "1").toString() + " ");
  SessionOptionString.append("outbound.backupQuantity=" + settings.value("outbound.backupQuantity", "1").toString() +
                             " ");
  SessionOptionString.append("outbound.length=" + settings.value("outbound.length", "3").toString() + " ");

  // SIGNATURE_TYPE

  {
    QStringList AllowSignTypes = {
      "ECDSA_SHA256_P256", "ECDSA_SHA384_P384", "ECDSA_SHA512_P521", "EdDSA_SHA512_Ed25519", "RedDSA_SHA512_Ed25519"};

    auto sign_type = settings.value("SIGNATURE_TYPE", "EdDSA_SHA512_Ed25519").toString();
    auto notfound = true;
    for (int i = 0; i < AllowSignTypes.size(); ++i) {
      if (sign_type.contains(AllowSignTypes.at(i))) {
        SessionOptionString.append("SIGNATURE_TYPE=" + sign_type + " ");
        notfound = false;
        break;
      }
    }
    if (notfound)
      SessionOptionString.append("SIGNATURE_TYPE=" + QString("EdDSA_SHA512_Ed25519") + " ");
  }

  // Encryption
  {
    QStringList AllowEncTypes = {"4", "4,0", "4, 0", "5,4", "6,4", "7,4"};

    QString defEncType = "4";
    auto enc_type = settings.value("i2cp.leaseSetEncType", defEncType).toString();
    auto encnotfound = true;
    for (int i = 0; i < AllowEncTypes.size(); ++i) {
      if (enc_type.contains(AllowEncTypes.at(i))) {
        SessionOptionString.append("i2cp.leaseSetEncType=" + enc_type + " ");
        encnotfound = false;
        break;
      }
    }
    if (encnotfound)
      SessionOptionString.append("i2cp.leaseSetEncType=" + defEncType + " ");
  }

  SessionOptionString.append("i2cp.leaseSetType=3 "); // i2pd fix
  SessionOptionString.append("I2PClient.PROP_GZIP=true ");

  settings.remove("SessionOptionString"); // no longer used,- so erase it
  settings.endGroup();
  settings.sync();

  return SessionOptionString;
}

void CCore::init() {
  using namespace SESSION_ENUMS;

  // Clean up transient chat previews from previous session
  QDir tmpDir("/tmp/i2pchat");
  if (tmpDir.exists())
    tmpDir.removeRecursively();

  this->mMyDestination = "";

  QSettings settings(mConfigPath + "/application.ini", QSettings::IniFormat);
  settings.beginGroup("Network");

  QString SamHost = settings.value("SamHost", "127.0.0.1").toString();
  QString SamPort = settings.value("SamPort", "7656").toString();
  bool nonPersist = settings.value("NonPersistentDestination", false).toBool();
  QString SamPrivKey = nonPersist ? "" : settings.value("SamPrivKey", "").toString();

  if (mConnectionManager->isComponentStopped()) {
    mConnectionManager->doReStart();
  }

  mConnectionManager->doCreateSession(STREAM, SamPrivKey, calcSessionOptionString());

  settings.endGroup();
  settings.sync();
}

void CCore::slotStreamStatusReceived(const SAM_Message_Types::RESULT result, const qint32 ID, const QString &Message) {

  CI2PStream *stream = mConnectionManager->getStreamObjectByID(ID);
  CUser *user = NULL;
  QString Destination;

  if (stream == NULL) {
    // Stream where delete,- asyn SIGNAL/SLOT
    return;
  }

  if (stream != NULL) {
    Destination = stream->getDestination();
    user = mUserManager->getUserByI2P_ID(ID);
  }

  if (result == SAM_Message_Types::OK && ID > 0) {
    if (mFileTransferManager->isThisID_a_FileSendID(ID) == false) {
      mProtocol->newConnectionChat(ID);
    }
  } else if (result == SAM_Message_Types::CANT_REACH_PEER || result == SAM_Message_Types::TIMEOUT) {
    if (user == NULL) {
      mConnectionManager->doDestroyStreamObjectByID(ID);
      return;
    }
    if (user->getOnlineState() != USEROFFLINE && user->getOnlineState() != USERTRYTOCONNECT) {
      user->slotIncomingMessageFromSystem(tr("The Connection is broken: %1\nConnection closed").arg(Message));
    }
    deletePacketManagerByID(ID);
    user->setConnectionStatus(TRYTOCONNECT);
    user->setOnlineState(USEROFFLINE);
    // reconnect stream

  } else if (result == SAM_Message_Types::CLOSED) {
    if (ID < 0) {
      // incoming stream
      QString Destination = stream->getDestination();

      // remove old stream
      deletePacketManagerByID(ID);
      mConnectionManager->doDestroyStreamObjectByID(ID);
      //---------------------------

      if (user != NULL && user->getI2PStreamID() == ID) {
        user->setConnectionStatus(OFFLINE);
        user->setOnlineState(USEROFFLINE);
        // create a new outgoing stream for this user
        createStreamObjectForUser(*user);
      }
    } else { // ID>0
      // outgoing stream
      if (user != NULL) {
        user->setConnectionStatus(TRYTOCONNECT);
        user->setOnlineState(USEROFFLINE);
      }
      stream->setConnectionType(UNKNOWN);

      deletePacketManagerByID(ID);

      connect(stream,
              SIGNAL(signDataReceived(const qint32, const QByteArray)),
              mProtocol,
              SLOT(slotInputUnknown(const qint32, const QByteArray)));
    }
  }

  else if (result == SAM_Message_Types::INVALID_KEY) {
    if (user == NULL) {
      mConnectionManager->doDestroyStreamObjectByID(ID);
      return;
    }
    if (user->getUsedB32Dest() == true) {
      user->setOnlineState(USEROFFLINE);
      user->setConnectionStatus(TRYTOCONNECT);
    } else {
      mConnectionManager->doDestroyStreamObjectByID(ID);
      user->slotIncomingMessageFromSystem(tr("Invalid Contact Destination: please delete the user\n"));
      user->setConnectionStatus(CONNECTERROR);
    }
    deletePacketManagerByID(ID);

  } else if (result == SAM_Message_Types::I2P_ERROR) {
    if (user == NULL) {
      mConnectionManager->doDestroyStreamObjectByID(ID);
      return;
    }
    user->slotIncomingMessageFromSystem(tr("I2P_Error: %1").arg(Message));
    deletePacketManagerByID(ID);
    mConnectionManager->doDestroyStreamObjectByID(ID);
    user->setConnectionStatus(CONNECTERROR);
  }
}

void CCore::setUserProtocolVersionByStreamID(qint32 ID, QString Version) {
  CUser *theUser = mUserManager->getUserByI2P_ID(ID);

  if (theUser != NULL) {
    theUser->setProtocolVersion(std::move(Version));
  }
}

void CCore::closeAllActiveConnections() {
  // close all known Online||TrytoConnect Connections
  QList<CUser *> users = mUserManager->getUserList();

  for (int i = 0; i < users.size(); i++) {
    if (users.at(i)->getConnectionStatus() == ONLINE || users.at(i)->getConnectionStatus() == TRYTOCONNECT) {
      deletePacketManagerByID(users.at(i)->getI2PStreamID());
      mConnectionManager->doDestroyStreamObjectByID(users.at(i)->getI2PStreamID());
      users.at(i)->setConnectionStatus(User::OFFLINE);
      users.at(i)->setOnlineState(USEROFFLINE);
    }
  }
}

void CCore::slotNamingReplyReceived(const SAM_Message_Types::RESULT result,
                                    const QString &Name,
                                    const QString &Value,
                                    const QString &Message) {
  if (result == SAM_Message_Types::OK && Name == "ME" && mMyDestination.isEmpty()) {
    this->mMyDestination = Value;
  } else if (result == SAM_Message_Types::OK) {
    CUser *theUser = mUserManager->getUserByI2P_Destination(Name);
    if (theUser != NULL && theUser->getUsedB32Dest() == true) {
      theUser->setReplaceB32WithB64(Value);
    }
  } else if (result == SAM_Message_Types::FAILED) {
    qWarning() << "File\t" << __FILE__ << Qt::endl
               << "Line:\t" << __LINE__ << Qt::endl
               << "Function:\t"
               << "CCore::slotNamingReplyReceived" << Qt::endl
               << "Message:\t"
               << "slotNamingReplyReceived\nSAM_Message_Types::FAILED" << Qt::endl
               << "Name:\t" << Name << Qt::endl
               << "Value:\t" << Value << Qt::endl
               << "Message:\t" << Message << Qt::endl;
  }
}
const QString CCore::getMyDestination() const {
  return this->mMyDestination;
}

void CCore::deletePacketManagerByID(qint32 ID) {

  const CI2PStream *Stream = NULL;
  if (mConnectionManager->isComponentStopped() == false) {
    Stream = mConnectionManager->getStreamObjectByID(ID);
  }

  for (int i = 0; i < mDataPacketsManagers.count(); i++) {

    if (ID == mDataPacketsManagers.at(i)->getID()) {
      CPacketManager *CurrentManager = mDataPacketsManagers.takeAt(i);

      if (Stream != NULL) {
        disconnect(Stream,
                   SIGNAL(signDataReceived(const qint32, const QByteArray)),
                   (CurrentManager),
                   SLOT(slotDataInput(qint32, QByteArray)));
      }
      CurrentManager->deleteLater();
      break;
    }
  }
}

QString CCore::getConnectionDump() const {
  QString Message;
  QString StreamControllerBridgeName;

  if (mConnectionManager->isComponentStopped() == true) {
    Message = "Not connected to network";
    return Message;
  }
  StreamControllerBridgeName = mConnectionManager->getStreamControllerBridgeName();

  Message = "• Stream Controller\n";
  Message += "\tNetwork:\t\tI2P\n";
  Message += "\tStreamControllerBridgeName:\t" + StreamControllerBridgeName + "\n";

  const QMap<qint32, CI2PStream *> *allListener = mConnectionManager->getAllStreamIncomingListenerObjects();
  const QList<CI2PStream *> allStreamsListenerList = allListener->values();
  Message += "• Incoming Stream Listener\n";
  for (int i = 0; i < allStreamsListenerList.count(); i++) {
    CI2PStream *Stream = allStreamsListenerList.value(i);
    Message += "\n\tStream ID:\t\t" + QString::number(Stream->getID()) + "\n";
    // Print StreamMode
    if (Stream->getStreamMode() == STREAMS::CONNECT) {
      Message += "\tStream Mode:\tCONNECT\n";
    } else if (Stream->getStreamMode() == STREAMS::ACCEPT) {
      Message += "\tStream Mode:\tACCEPT\n";
    } else {
      Message += "\tStream Mode:\t???\n";
    }
  }

  Message += "• Streams\n\n";
  const QMap<qint32, CI2PStream *> *allStreams = mConnectionManager->getAllStreamObjects();
  const QList<CI2PStream *> allStreamsList = allStreams->values();

  for (int n = 0; n < allStreamsList.size(); n++) {

    CI2PStream *Stream = allStreamsList.value(n);
    QString StreamID;
    CUser *theUser = NULL;

    theUser = mUserManager->getUserByI2P_ID(Stream->getID());

    if (theUser != NULL) {
      Message += "\tUser:\t\t" + theUser->getName() + "\n";
    }

    StreamID.setNum(Stream->getID(), 10);

    Message += "\tStream ID:\t\t" + StreamID + "\n";

    // Print StreamMode
    if (Stream->getStreamMode() == STREAMS::CONNECT) {
      Message += "\tStream Mode:\tCONNECT\n";
    } else if (Stream->getStreamMode() == STREAMS::ACCEPT) {
      Message += "\tStream Mode:\tACCEPT\n";
    } else {
      Message += "\tStream Mode:\t???\n";
    }

    if (Stream->getUsedFor() != nullptr) {
      Message += "\tPurpose:\t\t" + Stream->getUsedFor() + "\n";
    }

    // Print ConnectionType
    if (Stream->getConnectionType() == UNKNOWN) {
      Message += "\n";
    } else if (Stream->getConnectionType() == KNOWN) {
      Message += "\tTrust:\t\tKNOWN\n";
      if (Stream->getUsedFor() == "FileTransferSend")
        Message += "\n";
    } else {
      Message += "\tTrust:\t\t???\n";
    }

    theUser = mUserManager->getUserByI2P_ID(Stream->getID());
    if (theUser != NULL) {
      if (theUser->getClientName() != nullptr) {
        Message += "\tClient:\t\t" + theUser->getClientName();
      }
      if (theUser->getClientVersion() != nullptr) {
        Message += " " + theUser->getClientVersion() + "\n";
      }
      if (theUser->getProtocolVersion() != nullptr && Stream->getConnectionType() != UNKNOWN) {
        Message += "\tProtocol:\t\t" + theUser->getProtocolVersion() + "\n\n";
      }
    }
  }
  return Message;
}

ONLINESTATE CCore::getOnlineStatus() const {
  return this->mCurrentOnlineStatus;
}

void CCore::setOnlineStatus(const ONLINESTATE newStatus) {
  /* State machine:
     - OFFLINE → transition through TRYTOCONNECT (triggers init())
     - TRYTOCONNECT → re-init connection
     - OFFLINE explicitly → stopCore() + kill keepalive
     - all other states → broadcast to connected peers */
  qDebug() << "setOnlineStatus: newStatus =" << newStatus << "currentStatus =" << mCurrentOnlineStatus;

  if (mCurrentOnlineStatus == newStatus)
    return;

  if (mCurrentOnlineStatus == USEROFFLINE) {
    mNextOnlineStatus = newStatus;
    mCurrentOnlineStatus = USERTRYTOCONNECT;
    mKeepAliveTimer.stop();
    init();
    emit signOnlineStatusChanged();
    return;
  }

  if (newStatus == User::USEROFFLINE) {
    this->mCurrentOnlineStatus = newStatus;
    mKeepAliveTimer.stop();
    stopCore();
  } else if (newStatus == USERTRYTOCONNECT) {
    if (mCurrentOnlineStatus == USEROFFLINE) {
      this->mCurrentOnlineStatus = newStatus;
      restartCore();
    }
  } else {
    // send new Status to every connected User
    this->mCurrentOnlineStatus = newStatus;
    QList<CUser *> Users = mUserManager->getUserList();

    for (int i = 0; i < Users.size(); i++) {
      if (Users.at(i)->getIsInvisible() == true) {
        mProtocol->send(USER_ONLINESTATUS_OFFLINE, Users.at(i)->getI2PStreamID(), QString(""));
      } else {
        if (Users.at(i)->getConnectionStatus() == ONLINE) {
          switch (this->mCurrentOnlineStatus) {
          case USERONLINE: {
            mProtocol->send(USER_ONLINESTATUS_ONLINE, Users.at(i)->getI2PStreamID(), QString(""));
            break;
          }
          case USEROFFLINE: {
            break;
          }
          case USERINVISIBLE: {
            mProtocol->send(USER_ONLINESTATUS_OFFLINE, Users.at(i)->getI2PStreamID(), QString(""));
            break;
          }
          case USERAWAY: {
            mProtocol->send(USER_ONLINESTATUS_AWAY, Users.at(i)->getI2PStreamID(), QString(""));
            break;
          }
          case USERWANTTOCHAT: {
            mProtocol->send(USER_ONLINESTATUS_WANTTOCHAT, Users.at(i)->getI2PStreamID(), QString(""));
            break;
          }
          case USERDONT_DISTURB: {
            mProtocol->send(USER_ONLINESTATUS_DONT_DISTURB, Users.at(i)->getI2PStreamID(), QString(""));
            break;
          }
          case USERTRYTOCONNECT: {
            break;
          }
          default: {
            QMessageBox *msgBox = new QMessageBox(NULL);
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setText("CCore(setOnlineStatus)");
            msgBox->setInformativeText("Unknown USERSTATE");
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->setDefaultButton(QMessageBox::Ok);
            msgBox->setWindowModality(Qt::NonModal);
            msgBox->setAttribute(Qt::WA_DeleteOnClose);
            msgBox->show();
          }
          }
        } // if
      } // else
    } // for
  }
  emit signOnlineStatusChanged();
}

void CCore::stopCore() {
  mMyDestination = "";
  closeAllActiveConnections();
  mConnectionManager->doStopp();
}

void CCore::restartCore() {
  this->init();
}

QString CCore::getDestinationByID(qint32 ID) const {
  CUser *user = mUserManager->getUserByI2P_ID(ID);
  if (user != NULL) {
    return user->getI2PDestination();
  }

  CFileTransferSend *send = mFileTransferManager->getFileSendByID(ID);
  if (send != NULL) {
    return send->getDestination();
  }

  CFileTransferReceive *receive = mFileTransferManager->getFileReceiveByID(ID);
  if (receive != NULL) {
    return receive->getDestination();
  }

  return "";
}

void CCore::slotStreamControllerStatusOK(bool Status) {
  if (Status == true) {
    mCurrentOnlineStatus = mNextOnlineStatus;
    if (mMyDestination.isEmpty()) {
      mConnectionManager->doNamingLookUP("ME"); // get the current Destination from this client
    }
    createStreamObjectsForAllUsers();
    mKeepAliveTimer.start();
    emit signOnlineStatusChanged();
  } else {
    // Session lost - close active connections but remember desired status
    mCurrentOnlineStatus = User::USEROFFLINE;
    closeAllActiveConnections();
    emit signOnlineStatusChanged();
  }
}

void CCore::slotPingClients() {
  QList<CUser *> users = mUserManager->getUserList();
  for (int i = 0; i < users.size(); i++) {
    if (users.at(i)->getConnectionStatus() == ONLINE) {
      mProtocol->send(PING, users.at(i)->getI2PStreamID());
    }
  }
}

void CCore::slotAutoAwayTimeout() {
  if (mCurrentOnlineStatus == USERONLINE || mCurrentOnlineStatus == USERWANTTOCHAT) {
    setOnlineStatus(USERAWAY);
  }
}

void CCore::resetAutoAway() {
  if (mAutoAwayMinutes > 0) {
    mAutoAwayTimer.start(mAutoAwayMinutes * 60000);
    if (mCurrentOnlineStatus == USERAWAY)
      setOnlineStatus(USERONLINE);
  }
}

void CCore::applyAutoAwaySettings(int minutes) {
  mAutoAwayMinutes = minutes;
  if (minutes > 0)
    mAutoAwayTimer.start(minutes * 60000);
  else
    mAutoAwayTimer.stop();
}

void CCore::slotReconnectAttempt() {
  // Session reconnect attempt started - show connecting status
  mCurrentOnlineStatus = User::USERTRYTOCONNECT;
  emit signOnlineStatusChanged();
}

void CCore::createStreamObjectsForAllUsers() {
  QList<CUser *> users = mUserManager->getUserList();

  for (int i = 0; i < users.size(); i++) {
    if (mConnectionManager->isComponentStopped() == false) {
      this->createStreamObjectForUser(*(users.at(i)));
    }
  }
}

void CCore::setStreamTypeToKnown(qint32 ID, const QByteArray &Data, bool isFileTransfer_Receive) {
  CI2PStream *t = mConnectionManager->getStreamObjectByID(ID);
  if (t == NULL)
    return;
  t->setConnectionType(KNOWN);
  disconnect(t,
             SIGNAL(signDataReceived(const qint32, const QByteArray)),
             mProtocol,
             SLOT(slotInputUnknown(const qint32, const QByteArray)));

  if (isFileTransfer_Receive == false) {
    CPacketManager *packetManager = new CPacketManager(*mConnectionManager, ID);
    connect(t,
            SIGNAL(signDataReceived(const qint32, const QByteArray)),
            packetManager,
            SLOT(slotDataInput(qint32, QByteArray)));

    connect(packetManager,
            SIGNAL(signAPacketIsComplete(const qint32, const QByteArray)),
            mProtocol,
            SLOT(slotInputKnown(const qint32, const QByteArray)));

    t->setUsedFor("Chat Connection");
    if (Data.isEmpty() == false) {
      packetManager->slotDataInput(ID, Data);
    }
    mDataPacketsManagers.push_back(packetManager);
  }
}

CI2PStream *CCore::getI2PStreamObjectByID(qint32 ID) const {
  return mConnectionManager->getStreamObjectByID(ID);
}

void CCore::slotIncomingStream(CI2PStream *stream) {
  // all incoming stream are first Unknown
  connect(stream,
          SIGNAL(signDataReceived(const qint32, const QByteArray)),
          mProtocol,
          SLOT(slotInputUnknown(const qint32, const QByteArray)));
}

void CCore::createStreamObjectForUser(CUser &User) {
  qint32 msec = 0;
  qint32 ID = User.getI2PStreamID();

  if (mConnectionManager->isComponentStopped() == true)
    return;

  if (mConnectionManager->getStreamObjectByID(ID) != NULL) {
    return;
  }

  QSettings *settings = new QSettings(mConfigPath + "/application.ini", QSettings::IniFormat);
  settings->beginGroup("General");
  msec = settings->value("Waittime_between_rechecking_offline_mUsers", "60000").toInt();
  settings->endGroup();

  CI2PStream *t = mConnectionManager->doCreateNewStreamObject(CONNECT);
  if (t == NULL) {
    User.slotIncomingMessageFromSystem(
      tr("Unable to create a new I2P stream — SAM session may not be ready or crypto types are incompatible.\n"
         "Check your SAM bridge version and encryption type settings."));
    return;
  }

  connect(t,
          SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)),
          this,
          SLOT(slotStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, QString)));

  User.setI2PStreamID(t->getID());
  connect(t,
          SIGNAL(signDataReceived(const qint32, const QByteArray)),
          mProtocol,
          SLOT(slotInputUnknown(const qint32, const QByteArray)));
  t->doConnect(User.getI2PDestination());
  t->startUnlimintedReconnect(msec);
  settings->sync();
  delete settings;
}

void CCore::slotNewSamPrivKeyGenerated(const QString &SamPrivKey) {
  QSettings *settings = new QSettings(mConfigPath + "/application.ini", QSettings::IniFormat);
  settings->beginGroup("Network");
  bool nonPersist = settings->value("NonPersistentDestination", false).toBool();
  if (!nonPersist)
    settings->setValue("SamPrivKey", SamPrivKey);
  settings->endGroup();
  settings->sync();
  delete settings;
}

bool CCore::useThisChatConnection(const QString &Destination, const qint32 ID) {
  CUser *theUser = NULL;

  theUser = mUserManager->getUserByI2P_Destination(Destination);
  if (theUser != NULL) {
    // check if connection to user already exists
    if (theUser->getI2PStreamID() != ID) {
      if (theUser->getConnectionStatus() == ONLINE) {
        // close the new connection, we already have a connection to this user
        mConnectionManager->doDestroyStreamObjectByID(ID);
        return false;
      }
      if (theUser->getConnectionStatus() == TRYTOCONNECT) {
        // use the new connection
        // close the TRYTOCONNECT connection
        deletePacketManagerByID(theUser->getI2PStreamID());
        mConnectionManager->doDestroyStreamObjectByID(theUser->getI2PStreamID());
        theUser->setI2PStreamID(ID);
        return true;
      }
    }
  }
  return true;
}

void CCore::loadUserInfos() {
  QSettings settings(mConfigPath + "/application.ini", QSettings::IniFormat);
  settings.sync();

  settings.beginGroup("User");
  QString savedNickname = settings.value("Nickname", "").toString();

  if (mUserInfos.Nickname != savedNickname) {
    mUserInfos.Nickname = savedNickname;
    emit signNicknameChanged();
  }

  if (mUserInfos.Nickname.isEmpty() == true) {
    mUserInfos.Nickname = "Undefined";
    emit signNicknameChanged();

    QMessageBox *msgBox = new QMessageBox(NULL);
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setText(tr("\nNo username configured\nUsing \'%1\'  \n\nChange in "
                       "Settings -> User Details")
                      //            .arg(randomString));
                      .arg(mUserInfos.Nickname));

    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->show();
  }

  mUserInfos.Age = settings.value("Age", "0").toInt();
  mUserInfos.Interests = settings.value("Interests", "").toString();

  if (settings.value("Gender", "").toString() == "Male") {
    mUserInfos.Gender = "Male";
  } else if (settings.value("Gender", "").toString() == "Female") {
    mUserInfos.Gender = "Female";
  }
  if (mUserInfos.AvatarImage != settings.value("AvatarBinaryImage", "").toByteArray()) {
    mUserInfos.AvatarImage = settings.value("AvatarBinaryImage", "").toByteArray();
    emit signOwnAvatarImageChanged();
  }
  settings.endGroup();
  settings.sync();

  if (!nicknameRegExp.exactMatch(mUserInfos.Nickname)) {
    mUserInfos.Nickname = "NonValidNick";
    emit signNicknameChanged();
  }
}

const CReceivedInfos CCore::getUserInfos() const {
  return mUserInfos;
}

void CCore::doConvertNumberToTransferSize(quint64 inNumber,
                                          QString &outNumber,
                                          QString &outType,
                                          bool addStoOutType) const {
  QString SSize;

  quint64 Size = inNumber;
  if (Size >= (1024 * 1024)) { // MB
    double dMB = double(Size) / double(1024 * 1024);
    SSize = QString("%1").arg(dMB, 0, 'd', 2);
    outNumber = SSize;
    addStoOutType == true ? outType = "MB/s" : outType = "MB";

    return;
  }
  if (Size >= 1024) {
    // KB
    double dKB = double(Size) / double(1024);
    SSize = QString("%1").arg(dKB, 0, 'd', 2);
    outNumber = SSize;
    addStoOutType == true ? outType = "KB/s" : outType = "KB";
    return;
  }
  // Byte
  SSize.setNum(Size, 10);
  outNumber = SSize;
  addStoOutType == true ? outType = "Bytes/s" : outType = "Bytes";
}

QString CCore::formatETA(quint64 secLeft) {
  if (secLeft > 86400)
    return QObject::tr("Over a day...");

  int hours = static_cast<int>(secLeft / 3600);
  secLeft %= 3600;
  int minutes = static_cast<int>(secLeft / 60);
  int secs = static_cast<int>(secLeft % 60);

  return QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
}

const QString CCore::getMyDestinationB32() const {
  return mMyDestinationB32;
}

void CCore::setMyDestinationB32(const QString &B32Dest) {
  if (mMyDestinationB32 == B32Dest)
    return;

  if (!B32Dest.right(8).contains(".b32.i2p", Qt::CaseInsensitive)) {
    qCritical() << "File\t" << __FILE__ << Qt::endl
                << "Line:\t" << __LINE__ << Qt::endl
                << "Function:\t"
                << "CCore::setMyDestinationB32" << Qt::endl
                << "Message:\t"
                << "the last 8 char must be .b32.i2p" << Qt::endl
                << "\tDestination:\n"
                << B32Dest << Qt::endl
                << "\tAction apported" << Qt::endl;
  }

  QSettings settings(mConfigPath + "/application.ini", QSettings::IniFormat);
  settings.beginGroup("Network");
  settings.setValue("MyDestinationB32", B32Dest);
  settings.endGroup();
  settings.sync();

  mMyDestinationB32 = B32Dest;
}

void CCore::changeAccessIncomingUsers(bool m) {
  m_access_anyone_incoming = m;
}

static double lanczosKernel(double x, double a) {
  if (x == 0.0)
    return 1.0;
  double ax = std::abs(x);
  if (ax >= a)
    return 0.0;
  double pix = M_PI * x;
  return a * std::sin(pix) * std::sin(pix / a) / (pix * pix);
}

static void lanczosHorizontalPass(const QImage &src, QImage &dst, double a) {
  int newWidth = dst.width();
  for (int y = 0; y < src.height(); ++y) {
    const QRgb *srcRow = reinterpret_cast<const QRgb *>(src.constScanLine(y));
    QRgb *dstRow = reinterpret_cast<QRgb *>(dst.scanLine(y));
    for (int x = 0; x < newWidth; ++x) {
      double srcX = (double)x / newWidth * src.width();
      int center = (int)srcX;
      double r = 0, g = 0, b = 0, totalWeight = 0;
      int start = std::max(0, center - (int)a + 1);
      int end = std::min(src.width() - 1, center + (int)a);
      for (int i = start; i <= end; ++i) {
        double weight = lanczosKernel(srcX - i, a);
        QRgb pixel = srcRow[i];
        r += qRed(pixel) * weight;
        g += qGreen(pixel) * weight;
        b += qBlue(pixel) * weight;
        totalWeight += weight;
      }
      if (totalWeight > 0) {
        r /= totalWeight;
        g /= totalWeight;
        b /= totalWeight;
      }
      dstRow[x] = qRgb(std::min(255, std::max(0, static_cast<int>(std::lround(r)))),
                       std::min(255, std::max(0, static_cast<int>(std::lround(g)))),
                       std::min(255, std::max(0, static_cast<int>(std::lround(b)))));
    }
  }
}

static void lanczosVerticalPass(const QImage &src, QImage &dst, double a) {
  int newHeight = dst.height();
  for (int y = 0; y < newHeight; ++y) {
    double srcY = (double)y / newHeight * src.height();
    int center = (int)srcY;
    QRgb *dstRow = reinterpret_cast<QRgb *>(dst.scanLine(y));
    int start = std::max(0, center - static_cast<int>(a) + 1);
    int end = std::min(src.height() - 1, center + static_cast<int>(a));
    for (int x = 0; x < dst.width(); ++x) {
      double r = 0, g = 0, b = 0, totalWeight = 0;
      for (int i = start; i <= end; ++i) {
        double weight = lanczosKernel(srcY - i, a);
        QRgb pixel = reinterpret_cast<const QRgb *>(src.constScanLine(i))[x];
        r += qRed(pixel) * weight;
        g += qGreen(pixel) * weight;
        b += qBlue(pixel) * weight;
        totalWeight += weight;
      }
      if (totalWeight > 0) {
        r /= totalWeight;
        g /= totalWeight;
        b /= totalWeight;
      }
      dstRow[x] = qRgb(std::min(255, std::max(0, static_cast<int>(std::lround(r)))),
                       std::min(255, std::max(0, static_cast<int>(std::lround(g)))),
                       std::min(255, std::max(0, static_cast<int>(std::lround(b)))));
    }
  }
}

QImage CCore::scaleImageLanczos(const QImage &src, int maxWidth, int maxHeight) {
  if (src.isNull())
    return src;

  int newWidth = src.width();
  int newHeight = src.height();

  if (maxHeight > 0 && newHeight > maxHeight) {
    newWidth = newWidth * maxHeight / newHeight;
    newHeight = maxHeight;
  }
  if (maxWidth > 0 && newWidth > maxWidth) {
    newHeight = newHeight * maxWidth / newWidth;
    newWidth = maxWidth;
  }

  if (newWidth == src.width() && newHeight == src.height())
    return src;

  const double a = 3.0;

  QImage temp(newWidth, src.height(), QImage::Format_ARGB32_Premultiplied);
  lanczosHorizontalPass(src, temp, a);

  QImage result(newWidth, newHeight, QImage::Format_ARGB32_Premultiplied);
  lanczosVerticalPass(temp, result, a);

  return result;
}
