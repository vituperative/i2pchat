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
#include "SessionController.h"
#include <QIcon>

const QString SAM_HANDSHAKE_V3 = "HELLO VERSION MIN=3.1 MAX=3.1\n";

CSessionController::CSessionController(QString SamHost, QString SamPort,
                                       QString BridgeName, QString SamPrivKey,
                                       QString ConfigPath,
                                       QString SessionOptions)
    : mSamHost(SamHost), mSamPort(SamPort), mBridgeName(BridgeName),
      mConfigPath(ConfigPath), mSessionOptions(SessionOptions) {

  mIncomingPackets = new QByteArray();
  mDoneDisconnect = false;

  mAnalyser = new CI2PSamMessageAnalyser("CStreamController");
  mHandShakeWasSuccesfullDone = false;
  mSessionWasSuccesfullCreated = false;
  mSamPrivKey = SamPrivKey;

  connect(&mTcpSocket, SIGNAL(connected()), this, SLOT(slotConnected()),
          Qt::DirectConnection);

  connect(&mTcpSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()),
          Qt::DirectConnection);

  connect(&mTcpSocket, SIGNAL(readyRead()), this, SLOT(slotReadFromSocket()),
          Qt::DirectConnection);

  emit signDebugMessages("• I2P Stream Controller started");
}

CSessionController::~CSessionController() {
  doDisconnect();
  mTcpSocket.deleteLater();
  emit signDebugMessages("• I2P Stream Controller stopped");
}

void CSessionController::slotConnected() {
  emit signDebugMessages("• I2P Stream Controller connected");
  emit signDebugMessages(SAM_HANDSHAKE_V3);
  if (mTcpSocket.state() == QAbstractSocket::ConnectedState) {
    mTcpSocket.write(SAM_HANDSHAKE_V3.toUtf8());
    mTcpSocket.flush();
  }
}

void CSessionController::slotDisconnected() {
  if (mDoneDisconnect == false) {
    mTcpSocket.close();
    emit signDebugMessages("• I2P Stream Controller can't connect ‣ SAM or I2P "
                           "crashed [SAM Host: " +
                           mSamHost + ":" + mSamPort + "]");
    emit signSessionStreamStatusOK(false);

    QMessageBox msgBox(NULL);
    QPixmap pixmap = QPixmap(":/icons/avatar.svg");
    msgBox.setWindowIcon(QIcon(pixmap));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("\nI2P Stream Controller can't connect\n SAM or I2P "
                   "crashed\nSAM Host: " +
                   mSamHost + ":" + mSamPort);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setWindowModality(Qt::NonModal);
    msgBox.exec();

    // emit SamConnectionClosed();
  }
}

void CSessionController::slotReadFromSocket() {
  using namespace SAM_Message_Types;

  QByteArray newData = mTcpSocket.readAll();
  QByteArray CurrentPacket;
  mIncomingPackets->append(newData);

  while (mIncomingPackets->contains("\n") == true) {
    CurrentPacket =
        mIncomingPackets->left(mIncomingPackets->indexOf("\n", 0) + 1);

    QString t(CurrentPacket.data());

    SAM_MESSAGE sam = mAnalyser->Analyse(t);
    switch (sam.type) { // emit the signals
    case HELLO_REPLAY: {
      emit signDebugMessages(t);
      if (sam.result == OK) {
        this->mHandShakeWasSuccesfullDone = true;
        if (mSamPrivKey == "") {
          QSettings settings(mConfigPath + "/application.ini",
                             QSettings::IniFormat);
          QString Signature = "SIGNATURE_TYPE=%s";
          settings.beginGroup("Network");
          Signature.replace(
              "%s",
              //              settings.value("Signature_Type",
              //              "EdDSA_SHA512_Ed25519").toString());
              settings.value("Signature_Type", "ECDSA_SHA512_P521").toString());
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
          msgBox.setText(
              tr("\nDUPLICATE DESTINATION DETECTED!\nDo not attempt to run "
                 "I2PChat\nwith the same destination twice!\nSAM may need to "
                 "be restarted."));
          msgBox.setStandardButtons(QMessageBox::Ok);
          msgBox.setDefaultButton(QMessageBox::Ok);
          msgBox.setWindowModality(Qt::NonModal);
          msgBox.exec();

          qCritical() << "File\t" << __FILE__ << endl
                      << "Line:\t" << __LINE__ << endl
                      << "Function:\t"
                      << "CStreamController::slotReadFromSocket()" << endl
                      << "Message:\t"
                      << "Session: DUPLICATED_DEST" << endl
                      << "Only one Messenger per Destination,\nor SAMv3 "
                         "crashed (Tunnel will persist if I2PChat was closed)"
                      << endl;
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
      emit signNamingReplyReceived(sam.result, sam.Name, sam.Value,
                                   sam.Message);
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

  if (mTcpSocket.state() == QAbstractSocket::UnconnectedState) {
    mTcpSocket.connectToHost(mSamHost, mSamPort.toInt());
  }
  if (!mTcpSocket.waitForConnected(1000))
    slotDisconnected();
}

void CSessionController::doDisconnect() {
  mDoneDisconnect = true;

  if (mTcpSocket.state() != 0) {
    mTcpSocket.disconnectFromHost();
    emit signDebugMessages("• I2P Stream Controller: Socket disconnected");
  } else if (mTcpSocket.state() == QAbstractSocket::UnconnectedState) {
    emit signDebugMessages("• I2P Stream Controller: Socket unavailable");
  }
}

void CSessionController::doNamingLookUP(QString Name) {
  ConnectionReadyCheck();

  QByteArray Message = "NAMING LOOKUP NAME=";
  Message += Name + "\n";
  emit signDebugMessages(Message);
  mTcpSocket.write(Message);
  mTcpSocket.flush();
}

void CSessionController::doSessionCreate() {
  ConnectionReadyCheck();

  QByteArray Message = "SESSION CREATE STYLE=STREAM ID=";
  Message += mBridgeName + " DESTINATION=" + mSamPrivKey;

  if (mSessionOptions.isEmpty() == false) {
    Message += " " + mSessionOptions;
  }

  Message += "\n";
  emit signDebugMessages(Message);
  mTcpSocket.write(Message);
  mTcpSocket.flush();
}

void CSessionController::doDestGenerate(const QString Options) {
  ConnectionReadyCheck();
  QByteArray Message = "DEST GENERATE ";
  if (Options.isEmpty() == false) {
    Message += Options;
  }
  Message += "\n";

  emit signDebugMessages(Message);

  mTcpSocket.write(Message);
  mTcpSocket.flush();
}
