// SPDX-License-Identifier: AGPL-3.0-or-later

#include "SessionController.h"

#include <QIcon>

#include <utility>

const QString SAM_HANDSHAKE_V3 = "HELLO VERSION MIN=3.1 MAX=3.3\n";

CSessionController::CSessionController(QString SamHost,
                                       QString SamPort,
                                       QString BridgeName,
                                       QString SamPrivKey,
                                       QString ConfigPath,
                                       QString SessionOptions)
  : mSamHost(std::move(SamHost))
  , mSamPort(std::move(SamPort))
  , mBridgeName(std::move(BridgeName))
  , mConfigPath(std::move(ConfigPath))
  , mSessionOptions(std::move(SessionOptions)) {

  mIncomingPackets = new QByteArray();
  mDoneDisconnect = false;
  mReconnectTimer = new QTimer(this);

  mAnalyser = new CI2PSamMessageAnalyser("CStreamController");
  mHandshakeSuccessful = false;
  mSessionWasSuccesfullCreated = false;
  mSamPrivKey = std::move(SamPrivKey);

  connect(&mTcpSocket, SIGNAL(connected()), this, SLOT(slotConnected()), Qt::DirectConnection);

  connect(&mTcpSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()), Qt::DirectConnection);

  connect(
    &mTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotDisconnected()), Qt::DirectConnection);

  connect(&mTcpSocket, SIGNAL(readyRead()), this, SLOT(slotReadFromSocket()), Qt::DirectConnection);

  connect(mReconnectTimer, SIGNAL(timeout()), this, SLOT(slotReconnectTimeout()), Qt::DirectConnection);

  emit signDebugMessages("• I2P Stream Controller started");
}

CSessionController::~CSessionController() {
  doDisconnect();
  mTcpSocket.deleteLater();
  delete mReconnectTimer;
  delete mAnalyser;
  delete mIncomingPackets;
  emit signDebugMessages("• I2P Stream Controller stopped");
}

void CSessionController::slotConnected() {
  emit signDebugMessages("• I2P Stream Controller connected");
  mReconnectTimer->stop();
  emit signDebugMessages(SAM_HANDSHAKE_V3);
  if (mTcpSocket.state() == QAbstractSocket::ConnectedState) {
    mTcpSocket.write(SAM_HANDSHAKE_V3.toUtf8());
    mTcpSocket.flush();
  }
}

void CSessionController::slotDisconnected() {
  if (mDoneDisconnect == false) {
    mTcpSocket.close();
    emit signDebugMessages("• I2P Stream Controller disconnected ‣ SAM or I2P "
                           "unavailable [SAM Host: " +
                           mSamHost + ":" + mSamPort + "]");
    emit signSessionStreamStatusOK(false);

    // Start auto-reconnect timer
    if (!mReconnectTimer->isActive()) {
      emit signDebugMessages("• I2P Stream Controller ‣ Scheduling reconnect in 60 seconds");
      mReconnectTimer->start(60000); // 60 seconds
    }

    // emit SamConnectionClosed();
  }
}

void CSessionController::slotReadFromSocket() {
  using namespace SAM_Message_Types;

  QByteArray newData = mTcpSocket.readAll();
  QByteArray CurrentPacket;
  mIncomingPackets->append(newData);

  while (mIncomingPackets->contains("\n") == true) {
    CurrentPacket = mIncomingPackets->left(mIncomingPackets->indexOf("\n", 0) + 1);

    QString t(CurrentPacket.data());

    SAM_MESSAGE sam = mAnalyser->Analyse(t);
    switch (sam.type) { // emit the signals
    case HELLO_REPLAY: {
      emit signDebugMessages(t);
      if (sam.result == OK) {
        this->mHandshakeSuccessful = true;
        if (mSamPrivKey == "" || mSamPrivKey.length() <= 0) {
          QSettings settings(mConfigPath + "/application.ini", QSettings::IniFormat);
          QString Signature = "SIGNATURE_TYPE=%s";
          settings.beginGroup("Network");
          Signature.replace("%s", settings.value("Signature_Type", "EdDSA_SHA512_Ed25519").toString());
          this->doDestGenerate(Signature);
          settings.endGroup();
          settings.sync();

        } else {
          doSessionCreate();
        }
      } else {
        emit signSessionStreamStatusOK(false);
      }

      break;
    }
    case SESSION_STATUS: {
      emit signDebugMessages(t);
      if (sam.result == OK) {
        mSessionWasSuccesfullCreated = true;
        emit signSessionStreamStatusOK(true);
      } else {
        if (sam.result == DUPLICATED_DEST) {
          QMessageBox msgBox(NULL);
          msgBox.setIcon(QMessageBox::Critical);
          msgBox.setText(tr("DUPLICATE DESTINATION DETECTED!"));
          msgBox.setInformativeText(tr("Do not attempt to run I2PChat with the same destination twice!"
                                       "\nThe SAM client may need to be restarted."));
          msgBox.setStandardButtons(QMessageBox::Ok);
          msgBox.setDefaultButton(QMessageBox::Ok);
          msgBox.setWindowModality(Qt::NonModal);
          msgBox.exec();

          qCritical() << "File\t" << __FILE__ << Qt::endl
                      << "Line:\t" << __LINE__ << Qt::endl
                      << "Function:\t"
                      << "CStreamController::slotReadFromSocket()" << Qt::endl
                      << "Message:\t"
                      << "Session: DUPLICATED_DEST" << Qt::endl
                      << "Only one Messenger per Destination,\nor SAMv3 "
                         "crashed (Tunnel will persist if I2PChat was closed)"
                      << Qt::endl;
        }

        emit signSessionStreamStatusOK(false);
      }

      break;
    }
    case STREAM_STATUS: {
      emit signDebugMessages(t);
      // emit StreamStatusReceived(sam.result,sam.ID,sam.Message);
      break;
    }
    case NAMING_REPLY: {
      emit signDebugMessages(t);
      emit signNamingReplyReceived(sam.result, sam.Name, sam.Value, sam.Message);
      break;
    }
    case DEST_REPLY: {
      emit signDebugMessages(t);
      mSamPrivKey = sam.PRIV;
      emit signNewSamPrivKeyGenerated(mSamPrivKey);
      if (mSessionWasSuccesfullCreated == false) {
        doSessionCreate();
      }
      break;
    }
    case ERROR_IN_ANALYSE: {
      emit signDebugMessages("CStreamController: <ERROR_IN_ANALYSE>\n" + t);
      break;
    }
    default: {
      emit signDebugMessages("CStreamController: <Unknown Packet>\n" + t);
      break;
    }
    }
    mIncomingPackets->remove(0, mIncomingPackets->indexOf("\n", 0) + 1);
  } // while
}

void CSessionController::doConnect() {
  mDoneDisconnect = false;

  qDebug() << "CSessionController::doConnect() - SamHost:" << mSamHost << "SamPort:" << mSamPort;

  if (mTcpSocket.state() == QAbstractSocket::UnconnectedState) {
    qDebug() << "CSessionController::doConnect() - Connecting to SAM host...";
    mTcpSocket.connectToHost(mSamHost, mSamPort.toInt());
  }
  qDebug() << "CSessionController::doConnect() - Connection initiated, result handled asynchronously";
}

void CSessionController::doDisconnect() {
  mDoneDisconnect = true;
  mReconnectTimer->stop();

  if (mTcpSocket.state() != 0) {
    mTcpSocket.disconnectFromHost();
    emit signDebugMessages("• I2P Stream Controller: Socket disconnected");
  } else if (mTcpSocket.state() == QAbstractSocket::UnconnectedState) {
    emit signDebugMessages("• I2P Stream Controller: Socket unavailable");
  }
}

void CSessionController::doNamingLookUP(const QString &Name) {
  ConnectionReadyCheck();

  QByteArray Message;
  Message += "NAMING LOOKUP NAME=" + Name.toUtf8() + '\n';
  emit signDebugMessages(Message);
  mTcpSocket.write(Message);
  mTcpSocket.flush();
}

void CSessionController::doSessionCreate() {
  ConnectionReadyCheck();

  QByteArray Message;
  Message += "SESSION CREATE STYLE=STREAM ID=" + mBridgeName.toUtf8() + " DESTINATION=" + mSamPrivKey.toUtf8();
  // TODO: Enable as option for Non-persistent destination
  // Message += mBridgeName + " DESTINATION=TRANSIENT";

  if (mSessionOptions.isEmpty() == false) {
    Message += " " + mSessionOptions.toUtf8();
  }

  Message += '\n';
  emit signDebugMessages(Message);
  mTcpSocket.write(Message);
  mTcpSocket.flush();
}

void CSessionController::doDestGenerate(const QString &Options) {
  ConnectionReadyCheck();
  QByteArray Message = "DEST GENERATE ";
  if (Options.isEmpty() == false) {
    Message += Options.toUtf8();
  }
  Message += '\n';

  emit signDebugMessages(Message);

  mTcpSocket.write(Message);
  mTcpSocket.flush();
}

void CSessionController::slotReconnectTimeout() {
  mReconnectTimer->stop();
  emit signDebugMessages("• I2P Stream Controller ‣ Attempting to reconnect to SAM");
  emit signReconnectAttempt();
  doConnect();
}
