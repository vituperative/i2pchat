// SPDX-License-Identifier: AGPL-3.0-or-later

#include "Protocol.h"

#include "Core.h"
#include "FileTransferSend.h"
#include "I2PStream.h"
#include "LoadHTML.cpp" // NOLINT: intentional inclusion of inline functions
#include "User.h"
#include "UserManager.h"

#include <QErrorMessage>
#include <QSettings>

#include <iostream>

CProtocol::CProtocol(CCore &Core)
  : mCore(Core) {}

void CProtocol::newConnectionChat(const qint32 ID) {
  using namespace Protocol_Info;
  CI2PStream *stream = mCore.getI2PStreamObjectByID(ID);
  if (stream == NULL)
    return;

  // send the ChatSystem\tProtocolVersion
  if (stream->getFIRSTPACKETCHAT_alreadySent() == false) {
    // sometime StreamStatusReceived is called again with streamstatus Ok
    stream->setFIRSTPACKETCHAT_alreadySent(true);
    *(stream) << (QString)FIRSTPACKETCHAT;
  }
}

void CProtocol::slotInputKnown(const qint32 ID, const QByteArray &Data) {
  using namespace Protocol_Info;

  if (Data.length() < 4)
    return;

  QString ProtocolInfoTag(Data.left(4));

  if (ProtocolInfoTag == "1000" || ProtocolInfoTag == "1001" || ProtocolInfoTag == "1002" ||
      ProtocolInfoTag == "1003" || ProtocolInfoTag == "1004" || ProtocolInfoTag == "1005" ||
      ProtocolInfoTag == "1006" || ProtocolInfoTag == "1007" || ProtocolInfoTag == "1008") {
    dispatchKnownCommand(ID, Data, ProtocolInfoTag);
  } else if (ProtocolInfoTag == "0001" || ProtocolInfoTag == "0002" || ProtocolInfoTag == "0003" ||
             ProtocolInfoTag == "0004" || ProtocolInfoTag == "0005" || ProtocolInfoTag == "0006" ||
             ProtocolInfoTag == "0007" || ProtocolInfoTag == "0008" || ProtocolInfoTag == "0009" ||
             ProtocolInfoTag == "0010" || ProtocolInfoTag == "0011" || ProtocolInfoTag == "0012" ||
             ProtocolInfoTag == "0013" || ProtocolInfoTag == "0014" || ProtocolInfoTag == "0015" ||
             ProtocolInfoTag == "0016" || ProtocolInfoTag == "0017" || ProtocolInfoTag == "0018" ||
             ProtocolInfoTag == "0019" || ProtocolInfoTag == "0020" || ProtocolInfoTag == "0021" ||
             ProtocolInfoTag == "0022") {
    dispatchKnownMessage(ID, Data, ProtocolInfoTag);
  } else {
    qWarning() << "File\t" << __FILE__ << Qt::endl
               << "Line:\t" << __LINE__ << Qt::endl
               << "Function:\t"
               << "CProtocol::slotInputKnown" << Qt::endl
               << "Message:\t"
               << "Unknown ProtocolInfoTag: " << ProtocolInfoTag << Qt::endl
               << "Packet ignored" << Qt::endl;
  }
}

void CProtocol::slotInputUnknown(const qint32 ID, const QByteArray &Data) {
  using namespace Protocol_Info;

  CI2PStream *stream = mCore.getI2PStreamObjectByID(ID);

  if (stream == 0) {
    auto msg = "Protocol.cpp : stream object is null, ignoring incoming data\n";
    qDebug() << msg;
    return;
  }

  if (stream->getConnectionType() != UNKNOWN)
    return;

  if (Data.contains("CHATSYSTEM\t") == true) {
    handleChatProtocolPacket(ID, Data, stream);
  } else if (Data.contains("CHATSYSTEMFILETRANSFER\t") == true) {
    handleFileTransferProtocolPacket(ID, Data, stream);
  } else {
    handleWebProfileProtocolPacket(ID, Data, stream);
  }
}

void CProtocol::send(const COMMANDS_TAGS TAG, const qint32 ID) const {
  using namespace Protocol_Info;

  CI2PStream *stream = mCore.getI2PStreamObjectByID(ID);

  QString ProtocolInfoTag;
  QString Data = "";
  switch (TAG) {
  case PING: {
    ProtocolInfoTag = "1000";
    break;
  }
  case GET_CLIENTVERSION: {
    ProtocolInfoTag = "1001";
    break;
  }
  case GET_CLIENTNAME: {
    ProtocolInfoTag = "1002";
    break;
  }
  case GET_USER_ONLINESTATUS: {
    ProtocolInfoTag = "1003";
    break;
  }
  case GET_PROTOCOLVERSION: {
    ProtocolInfoTag = "1004";
    break;
  }
  case GET_MAX_PROTOCOLVERSION_FILETRANSFER: {
    ProtocolInfoTag = "1005";
    break;
  }
  case GET_USER_INFOS: {
    ProtocolInfoTag = "1006";
    break;
  }
  case GET_MIN_PROTOCOLVERSION_FILETRANSFER: {
    ProtocolInfoTag = "1007";
    break;
  }
  case GET_AVATARIMAGE: {
    ProtocolInfoTag = "1008";
    break;
  }
  default: {
    qCritical() << "File\t" << __FILE__ << Qt::endl
                << "Line:\t" << __LINE__ << Qt::endl
                << "Function:\t"
                << "CProtocol::send" << Qt::endl
                << "Message:\t"
                << "unhandeld Command-TAG"
                << "exit" << Qt::endl;
    exit(-1);
  }
  }
  Data.insert(0, ProtocolInfoTag);
  Data.insert(0, "0004"); // No packet data
  *(stream) << Data;
}

void CProtocol::send(const MESSAGES_TAGS TAG, const qint32 ID, const QString &Data) const {
  QByteArray t = Data.toUtf8();

  send(TAG, ID, t);
}
void CProtocol::send(const MESSAGES_TAGS TAG, const qint32 ID, QByteArray Data) const {
  QString ProtocolInfoTag;

  CI2PStream *stream = mCore.getI2PStreamObjectByID(ID);

  switch (TAG) {
  case ECHO_OF_PING: {
    ProtocolInfoTag = "0000";
    break;
  }
  case ANSWER_OF_GET_CLIENTVERSION: {
    ProtocolInfoTag = "0001";
    break;
  }
  case ANSWER_OF_GET_CLIENTNAME: {
    ProtocolInfoTag = "0002";
    break;
  }
  case CHATMESSAGE: {
    ProtocolInfoTag = "0003";
    break;
  }
  case USER_ONLINESTATUS_ONLINE: {
    ProtocolInfoTag = "0004";
    break;
  }
  case USER_ONLINESTATUS_OFFLINE:
  case USER_ONLINESTATUS_INVISIBLE: {
    ProtocolInfoTag = "0005";
    break;
  }
  case USER_ONLINESTATUS_WANTTOCHAT: {
    ProtocolInfoTag = "0006";
    break;
  }
  case USER_ONLINESTATUS_AWAY: {
    ProtocolInfoTag = "0007";
    break;
  }
  case USER_ONLINESTATUS_DONT_DISTURB: {
    ProtocolInfoTag = "0008";
    break;
  }
  case ANSWER_OF_GET_PROTOCOLVERSION: {
    ProtocolInfoTag = "0009";
    break;
  }
  case ANSWER_OF_GET_MAX_PROTOCOLVERSION_FILETRANSFER: {
    ProtocolInfoTag = "0010";
    break;
  }
  case USER_INFO_NICKNAME: {
    ProtocolInfoTag = "0011";
    break;
  }
  case USER_INFO_GENDER: {
    ProtocolInfoTag = "0012";
    break;
  }
  case USER_INFO_AGE: {
    ProtocolInfoTag = "0013";
    break;
  }
  case USER_INFO_INTERESTS: {
    ProtocolInfoTag = "0014";
    break;
  }
  case USER_BLOCK_INVISIBLE: {
    ProtocolInfoTag = "0015";
    break;
  }
  case USER_BLOCK_NORMAL: {
    ProtocolInfoTag = "0016";
    break;
  }
  case ANSWER_OF_GET_MIN_PROTOCOLVERSION_FILETRANSFER: {
    ProtocolInfoTag = "0017";
    break;
  }
  case ANSWER_OF_GET_AVATARIMAGE_IMAGE: {
    ProtocolInfoTag = "0018";
    break;
  }
  case AVATARIMAGE_CHANGED: {
    ProtocolInfoTag = "0019";
    break;
  }
  case FILE_OFFER: {
    ProtocolInfoTag = "0020";
    break;
  }
  case FILE_OFFER_ACCEPTED: {
    ProtocolInfoTag = "0021";
    break;
  }
  case FILE_OFFER_REJECTED: {
    ProtocolInfoTag = "0022";
    break;
  }
  default: {
    qCritical() << "File\t" << __FILE__ << Qt::endl
                << "Line:\t" << __LINE__ << Qt::endl
                << "Function:\t"
                << "CProtocol::send" << Qt::endl
                << "Message:\t"
                << "unhandeld Message-TAG"
                << "exit" << Qt::endl;

    exit(-1);
  }
  }
  QString temp;

  temp.setNum(Data.length() + 4, 16); // hex
  QString Packetlength = QString("%1").arg(temp, 4, '0');

  Data.insert(0, ProtocolInfoTag.toUtf8());
  Data.insert(0, Packetlength.toUtf8());
  *(stream) << Data;
}

// ---- extracted dispatch helpers ----

void CProtocol::dispatchKnownCommand(const qint32 ID, const QByteArray &Data, const QString &tag) {
  Q_UNUSED(Data);
  using namespace Protocol_Info;

  using CmdFn = void (*)(CProtocol &, qint32);
  static const QHash<QString, CmdFn> handlers = {
    {"1000", [](CProtocol &p, qint32 id) { p.send(ECHO_OF_PING, id, QString("")); }},
    {"1001", [](CProtocol &p, qint32 id) { p.send(ANSWER_OF_GET_CLIENTVERSION, id, p.mCore.getClientVersion()); }},
    {"1002", [](CProtocol &p, qint32 id) { p.send(ANSWER_OF_GET_CLIENTNAME, id, p.mCore.getClientName()); }},
    {"1003",
     [](CProtocol &p, qint32 id) {
       if (p.mCore.getUserManager()->getUserByI2P_ID(id)->getIsInvisible() == true) {
         p.send(USER_ONLINESTATUS_OFFLINE, id, QString(""));
       } else {
         switch (p.mCore.getOnlineStatus()) {
         case USERONLINE:
           p.send(USER_ONLINESTATUS_ONLINE, id, QString(""));
           break;
         case USEROFFLINE:
         case USERINVISIBLE:
           p.send(USER_ONLINESTATUS_OFFLINE, id, QString(""));
           break;
         case USERAWAY:
           p.send(USER_ONLINESTATUS_AWAY, id, QString(""));
           break;
         case USERWANTTOCHAT:
           p.send(USER_ONLINESTATUS_WANTTOCHAT, id, QString(""));
           break;
         case USERDONT_DISTURB:
           p.send(USER_ONLINESTATUS_DONT_DISTURB, id, QString(""));
           break;
         default: {
           QMessageBox *msgBox = new QMessageBox(NULL);
           msgBox->setIcon(QMessageBox::Warning);
           msgBox->setText(p.tr("cProtocol(inputKnown)"));
           msgBox->setInformativeText(p.tr("Unknown USERSTATE"));
           msgBox->setStandardButtons(QMessageBox::Ok);
           msgBox->setDefaultButton(QMessageBox::Ok);
           msgBox->setWindowModality(Qt::NonModal);
           msgBox->setAttribute(Qt::WA_DeleteOnClose);
           msgBox->show();
         }
         }
       }
     }},
    {"1004", [](CProtocol &p, qint32 id) { p.send(ANSWER_OF_GET_PROTOCOLVERSION, id, p.mCore.getProtocolVersion()); }},
    {"1005",
     [](CProtocol &p, qint32 id) {
       CUser *u = p.mCore.getUserManager()->getUserByI2P_ID(id);
       if (u != NULL)
         p.send(ANSWER_OF_GET_MAX_PROTOCOLVERSION_FILETRANSFER, id, FileTransferProtocol::MAXPROTOCOLVERSION);
     }},
    {"1006",
     [](CProtocol &p, qint32 id) {
       CReceivedInfos Infos = p.mCore.getUserInfos();
       p.send(USER_INFO_NICKNAME, id, Infos.Nickname);
       p.send(USER_INFO_GENDER, id, Infos.Gender);
       p.send(USER_INFO_INTERESTS, id, Infos.Interests);
       QString sAge;
       sAge.setNum(Infos.Age, 10);
       p.send(USER_INFO_AGE, id, sAge);
     }},
    {"1007",
     [](CProtocol &p, qint32 id) {
       p.send(ANSWER_OF_GET_MIN_PROTOCOLVERSION_FILETRANSFER, id, FileTransferProtocol::MINPROTOCOLVERSION);
     }},
    {"1008",
     [](CProtocol &p, qint32 id) { p.send(ANSWER_OF_GET_AVATARIMAGE_IMAGE, id, p.mCore.getUserInfos().AvatarImage); }},
  };

  auto it = handlers.constFind(tag);
  if (it != handlers.constEnd())
    it.value()(*this, ID);
}

void CProtocol::dispatchKnownMessage(const qint32 ID, const QByteArray &Data, const QString &tag) {
  using namespace Protocol_Info;

  using UserFn = void (*)(CUser *, const QByteArray &);
  static const QHash<QString, UserFn> userHandlers = {
    {"0001", [](CUser *u, const QByteArray &d) { u->setClientVersion(d.mid(4)); }},
    {"0002", [](CUser *u, const QByteArray &d) { u->setClientName(d.mid(4)); }},
    {"0003", [](CUser *u, const QByteArray &d) { u->slotIncomingNewChatMessage(d.mid(4)); }},
    {"0004", [](CUser *u, const QByteArray &) { u->setOnlineState(USERONLINE); }},
    {"0005", [](CUser *u, const QByteArray &) { u->setOnlineState(USEROFFLINE); }},
    {"0006", [](CUser *u, const QByteArray &) { u->setOnlineState(USERWANTTOCHAT); }},
    {"0007", [](CUser *u, const QByteArray &) { u->setOnlineState(USERAWAY); }},
    {"0008", [](CUser *u, const QByteArray &) { u->setOnlineState(USERDONT_DISTURB); }},
    {"0009", [](CUser *u, const QByteArray &d) { u->setProtocolVersion(d.mid(4)); }},
    {"0010", [](CUser *u, const QByteArray &d) { u->setMaxProtocolVersionFiletransfer(d.mid(4)); }},
    {"0011", [](CUser *u, const QByteArray &d) { u->setReceivedUserInfos(User::NICKNAME, d.mid(4)); }},
    {"0012", [](CUser *u, const QByteArray &d) { u->setReceivedUserInfos(User::GENDER, d.mid(4)); }},
    {"0013", [](CUser *u, const QByteArray &d) { u->setReceivedUserInfos(User::AGE, d.mid(4)); }},
    {"0014", [](CUser *u, const QByteArray &d) { u->setReceivedUserInfos(User::INTERESTS, d.mid(4)); }},
    {"0017", [](CUser *u, const QByteArray &d) { u->setMinProtocolVersionFiletransfer(d.mid(4)); }},
    {"0018",
     [](CUser *u, const QByteArray &d) {
       QByteArray temp = d.mid(4);
       u->setAvatarImage(temp);
     }},
  };

  auto it = userHandlers.constFind(tag);
  if (it != userHandlers.constEnd()) {
    CUser *user = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (user != NULL)
      it.value()(user, Data);
    return;
  }

  // Protocol-level handlers (need mCore access)
  using ProtoFn = void (*)(CProtocol &, qint32, const QByteArray &);
  static const QHash<QString, ProtoFn> protoHandlers = {
    {"0015",
     [](CProtocol &p, qint32 id, const QByteArray &) {
       CUser *u = p.mCore.getUserManager()->getUserByI2P_ID(id);
       if (u != NULL) {
         p.mCore.deletePacketManagerByID(id);
         if (p.mCore.getConnectionManager()->isComponentStopped() == false)
           p.mCore.getConnectionManager()->doDestroyStreamObjectByID(id);
         u->setConnectionStatus(OFFLINE);
       }
     }},
    {"0016",
     [](CProtocol &p, qint32 id, const QByteArray &) {
       CUser *u = p.mCore.getUserManager()->getUserByI2P_ID(id);
       if (u != NULL) {
         u->setOnlineState(USERBLOCKEDYOU);
         u->setConnectionStatus(OFFLINE);
         p.mCore.deletePacketManagerByID(id);
         if (p.mCore.getConnectionManager()->isComponentStopped() == false)
           p.mCore.getConnectionManager()->doDestroyStreamObjectByID(id);
       }
     }},
    {"0020",
     [](CProtocol &p, qint32 id, const QByteArray &d) {
       // FILE_OFFER — forward to user handler for display
       CUser *u = p.mCore.getUserManager()->getUserByI2P_ID(id);
       if (u != NULL) {
         u->slotIncomingFileOffer(QString::fromUtf8(d.mid(4)));
       }
     }},
    {"0019", [](CProtocol &p, qint32 id, const QByteArray &) { p.send(GET_AVATARIMAGE, id); }},
    {"0021",
     [](CProtocol &p, qint32 id, const QByteArray &d) {
       // FILE_OFFER_ACCEPTED — recipient accepted; start the file transfer
       QString fileName = QString::fromUtf8(d.mid(4));
       CUser *u = p.mCore.getUserManager()->getUserByI2P_ID(id);
       if (u == NULL)
         return;
       QString filePath = u->takeAcceptedFileOffer(fileName);
       if (!filePath.isEmpty()) {
         p.mCore.getFileTransferManager()->addNewFileTransfer(filePath, u->getI2PDestination());
       }
     }},
    {"0022",
     [](CProtocol &p, qint32 id, const QByteArray &d) {
       // FILE_OFFER_REJECTED — recipient rejected
       QString fileName = QString::fromUtf8(d.mid(4));
       CUser *u = p.mCore.getUserManager()->getUserByI2P_ID(id);
       if (u != NULL) {
         u->slotIncomingMessageFromSystem(u->tr("File offer for \"%1\" was rejected by the recipient.").arg(fileName));
         u->removeFileOffer(fileName);
       }
     }},
  };

  auto it2 = protoHandlers.constFind(tag);
  if (it2 != protoHandlers.constEnd())
    it2.value()(*this, ID, Data);
}

// ---- extracted protocol first-packet handlers ----

void CProtocol::handleChatProtocolPacket(const qint32 ID, const QByteArray &Data, CI2PStream *stream) {
  using namespace Protocol_Info;
  QByteArray temp = Data.mid(Data.indexOf("\t") + 1, Data.indexOf("\n") - Data.indexOf("\t") - 1);
  QString version(temp);
  bool OK = false;
  double versiond = version.toDouble(&OK);
  if (OK == false) {
    qCritical() << "File\t" << __FILE__ << "\nLine:\t" << __LINE__ << "\nFunction:\t"
                << "CProtocol::handleChatProtocolPacket"
                << "\nMessage:\t"
                << "Can't convert QString to double"
                << "\nQString:\t" << version << Qt::endl;
  }

  if (ID < 0) {
    newConnectionChat(ID);
    if (mCore.getUserBlockManager()->isDestinationInBlockList(stream->getDestination()) == true) {
      if (versiond < 0.4) {
        send(CHATMESSAGE, ID, QString("You have been blocked, all packets will be ignored!"));
        mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
        return;
      }
      QSettings settings(mCore.getConfigPath() + "/application.ini", QSettings::IniFormat);
      settings.beginGroup("Security");
      if (settings.value("BlockStyle", "Normal").toString() == "Normal") {
        send(CHATMESSAGE, ID, QString("You have been blocked, all packets will be ignored!"));
        send(USER_BLOCK_NORMAL, ID, QString(""));
      } else {
        send(USER_BLOCK_INVISIBLE, ID, QString(""));
      }
      settings.endGroup();
      settings.sync();
      mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
      return;
    }
  }

  QByteArray Data2 = Data;
  Data2 = Data2.remove(0, Data.indexOf("\n") + 1);
  if (mCore.getUserManager()->checkIfUserExistsByI2PDestination(stream->getDestination()) == false) {
    QSettings settings(mCore.getConfigPath() + "/application.ini", QSettings::IniFormat);
    settings.beginGroup("Security");
    bool blockAllUnknown = settings.value("BlockAllUnknownUsers", false).toBool();
    bool requestAuth = settings.value("RequestAuthorization", true).toBool();
    settings.endGroup();

    if (blockAllUnknown) {
      mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
      return;
    }
    if (requestAuth) {
      emit mCore.signIncomingUserAuthorizationRequest(stream->getDestination(), ID, Data);
      return;
    }

    bool added = false;
    if (versiond >= 0.3)
      added = mCore.getUserManager()->addNewUser("...identifying...", stream->getDestination(), ID);
    else
      added = mCore.getUserManager()->addNewUser("Unknown", stream->getDestination(), ID);

    if (!added) {
      mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
      return;
    }
    CUser *User = mCore.getUserManager()->getUserByI2P_Destination(stream->getDestination());
    if (User != NULL) {
      User->setI2PStreamID(ID);
      User->setProtocolVersion(version);
      User->setConnectionStatus(ONLINE);
      mCore.setStreamTypeToKnown(ID, Data2, false);
      if (versiond >= 0.3)
        User->setReceivedNicknameToUserNickname();
    }
  } else {
    if (mCore.useThisChatConnection(stream->getDestination(), ID) == true) {
      CUser *User = mCore.getUserManager()->getUserByI2P_Destination(stream->getDestination());
      if (User != NULL) {
        User->setI2PStreamID(ID);
        User->setProtocolVersion(version);
        User->setConnectionStatus(ONLINE);
        mCore.setStreamTypeToKnown(ID, Data2, false);
      }
    }
  }
}

void CProtocol::handleFileTransferProtocolPacket(const qint32 ID, const QByteArray &Data, CI2PStream *stream) {
  QByteArray Data2 = Data;

  QString ProtovolVersion = Data2.mid(Data.indexOf("\t") + 1, Data2.indexOf("\n") - Data2.indexOf("\t") - 1);
  Data2.remove(0, Data2.indexOf("\n") + 1);

  QString FileSize = Data2.mid(0, Data2.indexOf("\n"));
  Data2.remove(0, Data2.indexOf("\n") + 1);

  QString FileName = Data2;
  QString Destination = stream->getDestination();

  if (mCore.getUserBlockManager()->isDestinationInBlockList(stream->getDestination()) == true) {
    mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
    Data2.clear();
    return;
  }

  Data2.clear();
  mCore.setStreamTypeToKnown(ID, Data2, true);
  mCore.setUserProtocolVersionByStreamID(ID, ProtovolVersion);
  mCore.getFileTransferManager()->addNewFileReceive(ID, FileName, FileSize, Destination, ProtovolVersion);
}

void CProtocol::handleWebProfileProtocolPacket(const qint32 ID, const QByteArray &Data, CI2PStream *stream) {
  Q_UNUSED(Data);
  bool webprofileenabled = false;
  if (mCore.getUserBlockManager()->isDestinationInBlockList(stream->getDestination()) == true) {
    mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
    return;
  }
  QSettings settings(mCore.getConfigPath() + "/application.ini", QSettings::IniFormat);
  settings.beginGroup("Security");
  if (settings.value("WebProfile", "Enabled").toString() == "Enabled")
    webprofileenabled = true;
  if (settings.value("HideWebProfileWhenInvisible", "True").toString() == "True") {
    QList<CUser *> users = mCore.getUserManager()->getUserList();
    for (int i = 0; i < users.size(); i++) {
      if (users.at(i)->getIsInvisible() == true) {
        webprofileenabled = false;
        break;
      }
    }
    if (mCore.getOnlineStatus() == USERINVISIBLE)
      webprofileenabled = false;
  }
  settings.endGroup();

  if (webprofileenabled == true) {
    QString TEMPHTTPPAGE = loadfile(mCore.getConfigPath() + "/www/index.html");
    if (TEMPHTTPPAGE.isEmpty())
      TEMPHTTPPAGE = HTTPPAGE;
    QString myNick = mCore.getUserInfos().Nickname;
    myNick = myNick.replace("<", "").replace(">", "");
    TEMPHTTPPAGE.replace("[USERNAME]", myNick);
    TEMPHTTPPAGE.replace("[AVATARIMAGE]", mCore.getUserInfos().AvatarImage.toBase64());
    TEMPHTTPPAGE.replace("[MYDEST]", mCore.getMyDestination());
    *(stream) << (QString)(gethttpheader(TEMPHTTPPAGE) + TEMPHTTPPAGE);
    mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
  }
}

CProtocol::~CProtocol() {}
