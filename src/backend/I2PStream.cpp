// SPDX-License-Identifier: AGPL-3.0-or-later

#include "I2PStream.h"

#include <utility>

const QString SAM_HANDSHAKE_V3 = "HELLO VERSION MIN=3.1 MAX=3.3\n";
const int CONNECTIONTIMEOUT = 60 * 1000;

CI2PStream::CI2PStream(QString mSamHost,
                       QString mSamPort,
                       qint32 mID,
                       QString mStreamBridgeName,
                       StreamMode mMode,
                       bool mSilence,
                       QString UsedFor)
  : mSamHost(std::move(mSamHost))
  , mSamPort(std::move(mSamPort))
  , mID(mID)
  , mStreamBridgeName(std::move(mStreamBridgeName))
  , mMode(mMode)
  , mSilence(mSilence)
  , mUsedFor(std::move(UsedFor)) {
  mAnalyser = NULL;
  mIncomingPackets = NULL;
  mDoneDisconnect = false;
  mStatusReceived = false;
  mHandshakeSuccessful = false;
  mConnectionType = UNKNOWN;
  mIncomingPackets = new QByteArray();
  mDestinationReceived = false;
  mFIRSTPACKETCHAT_alreadySent = false;
  mTimer = NULL;
  mUnKnownConnectionTimeout.setInterval(CONNECTIONTIMEOUT);

  connect(&mTcpSocket, SIGNAL(connected()), this, SLOT(slotConnected()));
  connect(&mTcpSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
  connect(&mTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotDisconnected()));
  connect(&mTcpSocket, SIGNAL(readyRead()), this, SLOT(slotReadFromSocket()));

  connect(&mUnKnownConnectionTimeout, SIGNAL(timeout()), this, SLOT(slotInitConnectionTimeout()));
}

CI2PStream::~CI2PStream() {
  if (mTimer != NULL) {
    delete mTimer;
  }

  disconnect(&mTcpSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
  doDisconnect();

  mTcpSocket.deleteLater();

  if (mAnalyser != NULL) {
    delete mAnalyser;
  }

  if (mIncomingPackets != NULL) {
    delete mIncomingPackets;
  }
}

bool CI2PStream::doConnect(QString mDestination) {
  if (mMode != CONNECT) {
    return false;
  }

  this->mDestination = std::move(mDestination);
  this->mModeStreamConnect = true;
  this->mModeStreamAccept = false;

  if (mTcpSocket.state() != QAbstractSocket::UnconnectedState)
    return false;

  mTcpSocket.connectToHost(mSamHost, mSamPort.toInt());
  return true;
}

bool CI2PStream::doAccept() {
  if (mMode != ACCEPT) {
    return false;
  }

  this->mDestination = "";
  this->mModeStreamConnect = false;
  this->mModeStreamAccept = true;

  if (mTcpSocket.state() != QAbstractSocket::UnconnectedState)
    return false;

  mTcpSocket.connectToHost(mSamHost, mSamPort.toInt());
  return true;
}

void CI2PStream::slotConnected() {
  QString smID = QString::number(mID, 10);

  emit signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") + " • [Stream ID: " + smID +
                         "] Controller ‣ Connected to SAM v3");
  mDoneDisconnect = false;
  emit signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") + " • [Stream ID: " + smID +
                         "] Outgoing ‣ " + SAM_HANDSHAKE_V3);
  if (mTcpSocket.isWritable()) {
    mTcpSocket.write(SAM_HANDSHAKE_V3.toUtf8());
  }
}

void CI2PStream::slotDisconnected() {
  QString smID = QString::number(mID, 10);

  mStatusReceived = false;
  mDestinationReceived = false;
  mDoneDisconnect = false;
  mHandshakeSuccessful = false;
  mFIRSTPACKETCHAT_alreadySent = false;

  mUnKnownConnectionTimeout.stop();

  emit signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") + " • [Stream ID: " + smID +
                         "] Controller ‣ Disconnected from SAM");
  emit signStreamStatusReceived(SAM_Message_Types::CLOSED, mID, "");
}

void CI2PStream::slotReadFromSocket() {
  using namespace SAM_Message_Types;
  QString smID = QString::number(mID, 10);
  QByteArray newData;
  if (mTcpSocket.state() == QTcpSocket::ConnectedState) {
    newData = mTcpSocket.readAll();
  } else {
    return;
  }

  emit signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") + " • [Stream ID: " + smID +
                         "] Incoming ‣ " + newData);

  if (mHandshakeSuccessful == false) {

    mIncomingPackets->append(newData);
    if (mIncomingPackets->indexOf("\n", 0) == -1) {
      // Incomplete packet received???
      return;
    }

    QByteArray CurrentPacket;
    CurrentPacket = mIncomingPackets->left(mIncomingPackets->indexOf("\n", 0) + 1);

    mAnalyser = new CI2PSamMessageAnalyser("CI2PStream");

    QString t(CurrentPacket.data());
    SAM_MESSAGE sam = mAnalyser->Analyse(t);
    if (sam.type == HELLO_REPLAY && sam.result == OK) {
      mHandshakeSuccessful = true;
    }

    delete mAnalyser;
    mAnalyser = NULL;
    QByteArray Data;

    if (mMode == ACCEPT) {
      Data.append(QString("STREAM ACCEPT ID=" + mStreamBridgeName).toUtf8());
    } else if (mMode == CONNECT) {
      Data.append(QString("STREAM CONNECT ID=" + mStreamBridgeName + " DESTINATION=" + mDestination).toUtf8());
    }

    if (mSilence == true) {
      Data.append(" Silence=true\n");
    } else {
      Data.append(" Silence=false\n");
    }

    mIncomingPackets->remove(0, mIncomingPackets->indexOf("\n", 0) + 1);
    *(this) << Data;
  } else if (mStatusReceived == false) {
    mIncomingPackets->append(newData);
    if (mIncomingPackets->indexOf("\n", 0) == -1) {
      // Incomplete packet received???
      return;
    }

    QByteArray CurrentPacket;
    CurrentPacket = mIncomingPackets->left(mIncomingPackets->indexOf("\n", 0) + 1);

    // Get Stream Status
    mAnalyser = new CI2PSamMessageAnalyser("CI2PStream");

    QString t(CurrentPacket.data());

    SAM_MESSAGE sam = mAnalyser->Analyse(t);
    emit signStreamStatusReceived(sam.result, mID, sam.Message);

    delete mAnalyser;
    mAnalyser = NULL;
    mStatusReceived = true;

    mIncomingPackets->remove(0, mIncomingPackets->indexOf("\n", 0) + 1);
    if (mModeStreamConnect == true) {
      if (mIncomingPackets->length() != 0) {
        emit signDataReceived(mID, *(mIncomingPackets));
      }
    }

  } else if (mStatusReceived == true && mModeStreamAccept == true && mDestinationReceived == false) {
    // get Destination
    mIncomingPackets->append(newData);
    if (mIncomingPackets->indexOf("\n", 0) == -1) {
      // Incomplete packet received???
      return;
    }

    QByteArray CurrentPacket;
    CurrentPacket = mIncomingPackets->left(mIncomingPackets->indexOf("\n", 0) + 1);

    mDestination = QString(CurrentPacket.data());
    mDestination = mDestination.trimmed();
    mDestinationReceived = true;

    mIncomingPackets->remove(0, mIncomingPackets->indexOf("\n", 0) + 1);

    emit signModeAcceptIncomingStream(mID);

    if (mIncomingPackets->length() != 0) {
      emit signDataReceived(mID, *(mIncomingPackets));
    }

    // start mUnKnownConnectionTimeout
    mUnKnownConnectionTimeout.start();
    emit signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") + " • [Stream ID: " + smID +
                           "] Controller ‣ [Unknown] Connection Timeout");
    //--------------------------------
  } else {
    emit signDataReceived(mID, newData);
  }
}

void CI2PStream::doDisconnect() {
  mDoneDisconnect = true;
  if (mTcpSocket.bytesToWrite() > 0) {
    mTcpSocket.flush();
  }
  mTcpSocket.disconnectFromHost();
}

void CI2PStream::operator<<(const QByteArray &Data) {
  QString smID = QString::number(mID, 10);
  QString timeNow = QString(QDateTime::currentDateTime().toString("hh:mm:ss"));

  if (mTcpSocket.state() == QTcpSocket::ConnectedState && mHandshakeSuccessful) {
    emit signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") + " • [Stream ID: " + smID +
                           "] Outgoing ‣ " + Data);

    if (mTcpSocket.isWritable()) {
      mTcpSocket.write(Data);
    }
  } else {
    QString Message = QString(" • [Stream ID: %1] Controller ‣ Not connected").arg(smID);
    emit signDebugMessages(timeNow + " " + Message);
  }
}
void CI2PStream::operator<<(const QString &Data) {
  QByteArray t = Data.toUtf8();

  *(this) << t;
}

void CI2PStream::setConnectionType(const Type newTyp) {
  QString smID = QString::number(mID, 10);

  mConnectionType = newTyp;
  if (newTyp == KNOWN) {
    mUnKnownConnectionTimeout.stop();
    emit signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") + " • [Stream ID: " + smID +
                           "] Controller ‣ Connection Type changed to KNOWN");
  }
}

void CI2PStream::startUnlimintedReconnect(qint32 msec) {
  if (mTimer == NULL) {
    mTimer = new QTimer();
    connect(mTimer, SIGNAL(timeout()), this, SLOT(slotCheckForReconnect()));
  }
  mTimer->start(msec);
}

void CI2PStream::stopUnlimintedReconnect() {
  mTimer->stop();
  delete mTimer;
  mTimer = NULL;
}

void CI2PStream::slotCheckForReconnect() {
  if (mTimer == NULL)
    return;
  mTimer->stop();
  if (mMode == CONNECT) {
    doConnect(mDestination);
  }

  mTimer->start();
}

void CI2PStream::slotInitConnectionTimeout() {
  QString smID = QString::number(mID, 10);
  emit signStreamStatusReceived(SAM_Message_Types::CLOSED, mID, QString(""));
  emit signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") + " • [Stream ID: " + smID +
                         "] Controller ‣ Disconnected after Initialization Timeout");
  doDisconnect();
}

void CI2PStream::setFIRSTPACKETCHAT_alreadySent(bool theValue) {
  mFIRSTPACKETCHAT_alreadySent = theValue;
}
