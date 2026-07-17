// SPDX-License-Identifier: AGPL-3.0-or-later

#include "ConnectionManager.h"

#include <QDateTime>
#include <QRandomGenerator>

#include <utility>

CConnectionManager::CConnectionManager(QString SamHost, QString SamPort, QString ConfigPath)
  : mSamHost(std::move(SamHost))
  , mSamPort(std::move(SamPort))
  , mConfigPath(std::move(ConfigPath)) {
  mComponentStateStopped = false;
  StreamController = NULL;
  mSessionStreamStatusOK = false;
  emit signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") + " • I2PChat Connection Manager started");
}

bool CConnectionManager::doCreateSession(SESSION_ENUMS::SESSION_STYLEV3 SessionStyle,
                                         QString SamPrivKey,
                                         QString SessionOptions) {
  using namespace SESSION_ENUMS;

  QString BridgeName = generateBridgeName();

  if (SessionStyle == STREAM && StreamController == NULL) {
    this->StreamController = new CSessionController(
      mSamHost, mSamPort, BridgeName, std::move(SamPrivKey), mConfigPath, std::move(SessionOptions));

    connect(StreamController, SIGNAL(signDebugMessages(const QString)), this, SIGNAL(signDebugMessages(const QString)));

    connect(StreamController, SIGNAL(signSessionStreamStatusOK(bool)), this, SLOT(slotSessionStreamStatusOK(bool)));

    connect(
      StreamController, SIGNAL(signSessionStreamStatusOK(bool)), this, SIGNAL(signStreamControllerStatusOK(bool)));

    connect(StreamController, SIGNAL(signReconnectAttempt()), this, SIGNAL(signReconnectAttempt()));

    connect(StreamController,
            SIGNAL(signNamingReplyReceived(const SAM_Message_Types::RESULT, QString, QString, QString)),
            this,
            SIGNAL(signNamingReplyReceived(const SAM_Message_Types::RESULT, QString, QString, QString)));

    connect(StreamController,
            SIGNAL(signNewSamPrivKeyGenerated(const QString)),
            this,
            SIGNAL(signNewSamPrivKeyGenerated(const QString)));

    StreamController->doConnect();
  } else {
    return false;
  }

  return true;
}

void CConnectionManager::slotSessionStreamStatusOK(bool Status) {
  QString Message;
  mSessionStreamStatusOK = Status;
  // start StreamListener
  CI2PStream *t = new CI2PStream(mSamHost, mSamPort, nextFreeNegID(), StreamController->getBridgeName(), ACCEPT, false);
  t->setUsedFor("Incoming Stream Listener");
  connect(t, SIGNAL(signModeAcceptIncomingStream(qint32)), this, SLOT(slotModeAcceptIncomingStream(qint32)));

  connect(t, SIGNAL(signDebugMessages(const QString)), this, SIGNAL(signDebugMessages(const QString)));

  connect(t,
          SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)),
          this,
          SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)));

  t->doAccept();
  Message = QDateTime::currentDateTime().toString("hh:mm:ss") + " • [Stream ID: ";
  Message += QString::number(t->getID(), 10);
  Message += "] Created new StreamObjectListener";

  emit signDebugMessages(Message);
  StreamIncomingListener.insert(t->getID(), t);

  emit signStreamControllerStatusOK(Status);
}

qint32 CConnectionManager::nextFreePosID() const {
  qint32 nextNumber = 1;
  for (auto it = allStreams.constBegin(); it != allStreams.constEnd(); ++it) {
    qint32 id = it.key();
    if (id > 0 && id >= nextNumber)
      nextNumber = id + 1;
  }
  return nextNumber;
}

qint32 CConnectionManager::nextFreeNegID() const {
  qint32 nextNumber = -1;
  for (auto it = allStreams.constBegin(); it != allStreams.constEnd(); ++it) {
    qint32 id = it.key();
    if (id < 0 && id <= nextNumber)
      nextNumber = id - 1;
  }
  return nextNumber;
}

bool CConnectionManager::doDestroyStreamObjectByID(qint32 ID) {
  QString Message;
  if (allStreams.contains(ID) == false)
    return false;
  CI2PStream *t = allStreams.take(ID);

  disconnect(t,
             SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)),
             this,
             SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)));

  disconnect(t, SIGNAL(signDebugMessages(const QString)), this, SIGNAL(signDebugMessages(const QString)));

  Message = "• Deleted StreamObject [ID: ";
  Message += QString::number(t->getID(), 10);
  Message += "]";

  t->deleteLater();

  emit signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") + " " + Message);
  return true;
}

CI2PStream *
CConnectionManager::doCreateNewStreamObject(StreamMode Mode, bool Silence, bool dontConnectSendStreamStatus) {
  QString Message;

  if (mSessionStreamStatusOK == true) {
    qint32 IDforNewObject = 0;
    QString StreamControllerBridgeName = StreamController->getBridgeName();

    if (Mode == CONNECT)
      IDforNewObject = nextFreePosID();
    else if (Mode == ACCEPT)
      IDforNewObject = nextFreeNegID();

    CI2PStream *t = new CI2PStream(mSamHost, mSamPort, IDforNewObject, StreamControllerBridgeName, Mode, Silence);
    connect(t, SIGNAL(signDebugMessages(const QString)), this, SIGNAL(signDebugMessages(const QString)));

    if (dontConnectSendStreamStatus == false) {
      connect(t,
              SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)),
              this,
              SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)));
    }

    Message = QDateTime::currentDateTime().toString("hh:mm:ss") + " • [Stream ID: ";
    Message += QString::number(t->getID(), 10);
    Message += "] Created new StreamObject";

    emit signDebugMessages(Message);
    allStreams.insert(IDforNewObject, t);
    return t;
  }
  return NULL;
}

void CConnectionManager::doNamingLookUP(const QString &Name) {
  if (SessionStreamStatusOKCheck() == false)
    return;

  if (StreamController != NULL) {
    StreamController->doNamingLookUP(Name);
  }
}

CI2PStream *CConnectionManager::getStreamObjectByID(qint32 ID) const {
  if (allStreams.contains(ID) == false) {
    return NULL;
  }
  return *(allStreams.find(ID));
}

QString CConnectionManager::getStreamControllerBridgeName() const {
  if (StreamController != NULL) {
    return StreamController->getBridgeName();
  }
  return QString();
}

CI2PStream *CConnectionManager::getStreamObjectByDestination(const QString &Destination) const {
  QMapIterator<qint32, CI2PStream *> i(allStreams);
  while (i.hasNext()) {
    i.next();
    if (i.value()->getDestination() == Destination)
      return i.value();
  }
  return NULL;
}

void CConnectionManager::slotModeAcceptIncomingStream(qint32 ID) {
  QString Message;

  if (StreamIncomingListener.contains(ID) == true) {
    // change old StreamIncomingListener to a normal Stream
    CI2PStream *t = StreamIncomingListener.take(ID);
    t->setUsedFor("");
    disconnect(t, SIGNAL(signModeAcceptIncomingStream(qint32)), this, SLOT(slotModeAcceptIncomingStream(qint32)));

    connect(t,
            SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)),
            this,
            SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)));
    allStreams.insert(ID, t);
    //----------------------------------------------------

    // create new StreamIncomingListener
    if (StreamIncomingListener.size() >= 32) {
      emit signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") + " • [Stream ID: " +
                             QString::number(t->getID(), 10) + "] Listener cap reached (32), not creating new");
      return;
    }
    CI2PStream *t2 =
      new CI2PStream(mSamHost, mSamPort, nextFreeNegID(), StreamController->getBridgeName(), ACCEPT, false);
    t2->setUsedFor("Incoming Stream Listener");
    connect(t2, SIGNAL(signDebugMessages(const QString)), this, SIGNAL(signDebugMessages(const QString)));

    connect(t2, SIGNAL(signModeAcceptIncomingStream(qint32)), this, SLOT(slotModeAcceptIncomingStream(qint32)));

    connect(t2,
            SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)),
            this,
            SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)));

    Message = QDateTime::currentDateTime().toString("hh:mm:ss") + " • [Stream ID: ";
    Message += QString::number(t2->getID(), 10);
    Message += "] Created new StreamObjectListener";

    emit signDebugMessages(Message);
    t2->doAccept();
    StreamIncomingListener.insert(t2->getID(), t2);
    //-----------------------------------
    emit signIncomingStream(t);
  }
}

CConnectionManager::~CConnectionManager() {
  stopp();
}

QString CConnectionManager::getSamPrivKey() const {
  if (StreamController != NULL) {
    return StreamController->getSamPrivKey();
  }
  return "";
}

QString CConnectionManager::generateBridgeName() const {
  QString Name;
  int length = 0;

  while (length < 3) {
    length = QRandomGenerator::global()->bounded(9);
  }

  for (int i = 0; i < length; i++) {
    Name.append(QLatin1Char('A' + QRandomGenerator::global()->bounded(26)));
  }

  return Name;
}

void CConnectionManager::doStopp() {
  mComponentStateStopped = true;
  this->stopp();
}

void CConnectionManager::doReStart() {
  mComponentStateStopped = false;
  StreamController = NULL;
  mSessionStreamStatusOK = false;
  emit signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") + " • I2PChat: Connection Manager restarted");
}

void CConnectionManager::stopp() {
  // close all StreamObjects
  QMapIterator<qint32, CI2PStream *> i(allStreams);
  while (i.hasNext()) {
    i.next();
    doDestroyStreamObjectByID((i.value())->getID());
  }
  allStreams.clear();

  // close all StreamIncomingListeners
  QMapIterator<qint32, CI2PStream *> i2(StreamIncomingListener);
  while (i2.hasNext()) {
    i2.next();
    i2.value()->deleteLater();
  }
  StreamIncomingListener.clear();

  // close all StreamContoller
  delete StreamController;
  StreamController = NULL;
  emit signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") + " • I2PChat: Connection Manager stopped");
}
