/***************************************************************************
 *   Copyright (C) 2008 by I2P-Messenger                                   *
 *   Messenger-Dev@I2P-Messenger                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "ConnectionManager.h"

CConnectionManager::CConnectionManager(QString SamHost, QString SamPort,
                                       QString ConfigPath)
    : mSamHost(SamHost), mSamPort(SamPort), mConfigPath(ConfigPath) {
  mComponentStateStopped = false;
  StreamController = NULL;
  mSessionStreamStatusOK = false;
  emit signDebugMessages("• I2PChat Connection Manager started");
}

bool CConnectionManager::doCreateSession(
    SESSION_ENUMS::SESSION_STYLEV3 SessionStyle, QString SamPrivKey,
    QString SessionOptions) {
  using namespace SESSION_ENUMS;

  QString BridgeName = generateBridgeName();

  if (SessionStyle == STREAM && StreamController == NULL) {
    this->StreamController =
        new CSessionController(mSamHost, mSamPort, BridgeName, SamPrivKey,
                               mConfigPath, SessionOptions);

    connect(StreamController, SIGNAL(signDebugMessages(const QString)), this,
            SIGNAL(signDebugMessages(const QString)));

    connect(StreamController, SIGNAL(signSessionStreamStatusOK(bool)), this,
            SLOT(slotSessionStreamStatusOK(bool)));

    connect(StreamController, SIGNAL(signSessionStreamStatusOK(bool)), this,
            SIGNAL(signStreamControllerStatusOK(bool)));

    connect(StreamController, SIGNAL(signReconnectAttempt()), this,
            SIGNAL(signReconnectAttempt()));

    connect(StreamController,
            SIGNAL(signNamingReplyReceived(const SAM_Message_Types::RESULT,
                                           QString, QString, QString)),
            this,
            SIGNAL(signNamingReplyReceived(const SAM_Message_Types::RESULT,
                                           QString, QString, QString)));

    connect(StreamController, SIGNAL(signNewSamPrivKeyGenerated(const QString)),
            this, SIGNAL(signNewSamPrivKeyGenerated(const QString)));

    StreamController->doConnect();
  } else {
    return false;
  }

  return false;
}

void CConnectionManager::slotSessionStreamStatusOK(bool Status) {
  QString Message;
  mSessionStreamStatusOK = Status;
  // start StreamListener
  CI2PStream *t =
      new CI2PStream(mSamHost, mSamPort, nextFreeNegID(),
                     StreamController->getBridgeName(), ACCEPT, false);
  t->setUsedFor("Incoming Stream Listener");
  connect(t, SIGNAL(signModeAcceptIncomingStream(qint32)), this,
          SLOT(slotModeAcceptIncomingStream(qint32)));

  connect(t, SIGNAL(signDebugMessages(const QString)), this,
          SIGNAL(signDebugMessages(const QString)));

  connect(t,
          SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT,
                                          const qint32, const QString)),
          this,
          SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT,
                                          const qint32, const QString)));

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

  for (int i = 0; i < allStreams.size(); i++) {
    if (allStreams.contains(nextNumber) == true) {
      nextNumber++;
    } else {
      break;
      ;
    }
  }
  return nextNumber;
}

qint32 CConnectionManager::nextFreeNegID() const {
  qint32 nextNumber = -1;

  for (int i = 0; i < allStreams.size(); i++) {
    if (allStreams.contains(nextNumber) == true) {
      nextNumber--;
    } else {
      break;
    }
  }
  return nextNumber;
}

bool CConnectionManager::doDestroyStreamObjectByID(qint32 ID) {
  QString Message;
  if (allStreams.contains(ID) == false)
    return false;
  CI2PStream *t = allStreams.take(ID);

  disconnect(t,
             SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT,
                                             const qint32, const QString)),
             this,
             SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT,
                                             const qint32, const QString)));

  disconnect(t, SIGNAL(signDebugMessages(const QString)), this,
             SIGNAL(signDebugMessages(const QString)));

  Message = "• Deleted StreamObject [ID: ";
  Message += QString::number(t->getID(), 10);
  Message += "]";

  t->deleteLater();

  emit signDebugMessages(Message);
  return true;
}

CI2PStream *
CConnectionManager::doCreateNewStreamObject(StreamMode Mode, bool Silence,
                                            bool dontConnectSendStreamStatus) {
  QString Message;

  if (mSessionStreamStatusOK == true) {
    qint32 IDforNewObject = 0;
    QString StreamControllerBridgeName = StreamController->getBridgeName();

    if (Mode == CONNECT)
      IDforNewObject = nextFreePosID();
    else if (Mode == ACCEPT)
      IDforNewObject = nextFreeNegID();

    CI2PStream *t = new CI2PStream(mSamHost, mSamPort, IDforNewObject,
                                   StreamControllerBridgeName, Mode, Silence);
    connect(t, SIGNAL(signDebugMessages(const QString)), this,
            SIGNAL(signDebugMessages(const QString)));

    if (dontConnectSendStreamStatus == false) {
      connect(t,
              SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT,
                                              const qint32, const QString)),
              this,
              SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT,
                                              const qint32, const QString)));
    }

    Message = QDateTime::currentDateTime().toString("hh:mm:ss") + " • [Stream ID: ";
    Message += QString::number(t->getID(), 10);
    Message += "] Created new StreamObject";

    emit signDebugMessages(Message);
    allStreams.insert(IDforNewObject, t);
    return t;
  } else {
    return NULL;
  }
}

void CConnectionManager::doNamingLookUP(QString Name) {
  SessionStreamStatusOKCheck();

  if (StreamController != NULL) {
    StreamController->doNamingLookUP(Name);
  }
}

CI2PStream *CConnectionManager::getStreamObjectByID(qint32 ID) const {
  if (allStreams.contains(ID) == false) {
    return NULL;
  } else {
    return *(allStreams.find(ID));
  }
}

QString CConnectionManager::getStreamControllerBridgeName() const {
  if (StreamController != NULL) {
    return StreamController->getBridgeName();
  }
  return 0;
}

CI2PStream *
CConnectionManager::getStreamObjectByDestination(QString Destination) const {
  QMapIterator<qint32, CI2PStream *> i(allStreams);
  while (i.hasNext()) {
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
    disconnect(t, SIGNAL(signModeAcceptIncomingStream(qint32)), this,
               SLOT(slotModeAcceptIncomingStream(qint32)));

    connect(t,
            SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT,
                                            const qint32, const QString)),
            this,
            SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT,
                                            const qint32, const QString)));
    allStreams.insert(ID, t);
    //----------------------------------------------------

    // create new StreamIncomingListener
    CI2PStream *t2 =
        new CI2PStream(mSamHost, mSamPort, nextFreeNegID(),
                       StreamController->getBridgeName(), ACCEPT, false);
    t2->setUsedFor("Incoming Stream Listener");
    connect(t2, SIGNAL(signDebugMessages(const QString)), this,
            SIGNAL(signDebugMessages(const QString)));

    connect(t2, SIGNAL(signModeAcceptIncomingStream(qint32)), this,
            SLOT(slotModeAcceptIncomingStream(qint32)));

    connect(t2,
            SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT,
                                            const qint32, const QString)),
            this,
            SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT,
                                            const qint32, const QString)));

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

CConnectionManager::~CConnectionManager() { stopp(); }

QString CConnectionManager::getSamPrivKey() const {
  if (StreamController != NULL) {
    return StreamController->getSamPrivKey();
  } else {
    return "";
  }
}

QString CConnectionManager::generateBridgeName() const {
  QString Name;
  int length = 0;

  qsrand(QDateTime::currentMSecsSinceEpoch());

  while (length < 3) {
    length = rand() % 9;
  }

  for (int i = 0; i < length; i++) {
    Name.append(("ABCDEFGHIJKLMNOPQRSTUVWXYZ"[qrand() % 26]));
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
  emit signDebugMessages("• I2PChat: Connection Manager restarted");
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
    delete i2.value();
  }
  StreamIncomingListener.clear();

  // close all StreamContoller
  delete StreamController;
  StreamController = NULL;
  emit signDebugMessages("• I2PChat: Connection Manager stopped");
}
