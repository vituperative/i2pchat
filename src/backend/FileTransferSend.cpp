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

#include "FileTransferSend.h"
#include "Core.h"
#include "UserManager.h"

CFileTransferSend::CFileTransferSend(CCore &Core,
                                     CConnectionManager &ConnectionManager,
                                     QString FilePath, QString Destination,
                                     QString Protocolversion,
                                     double ProtocolversionD)

    : mCore(Core), mConnectionManager(ConnectionManager), mFilePath(FilePath),
      mDestination(Destination), mUsingProtocolVersion(Protocolversion),
      mUsingProtocolVersionD(ProtocolversionD) {

  mStream = ConnectionManager.doCreateNewStreamObject(CONNECT, false);
  mStream->setUsedFor("FileTransferSend");

  connect(mStream,
          SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT,
                                          const qint32, const QString)),
          this,
          SLOT(slotStreamStatus(const SAM_Message_Types::RESULT, const qint32,
                                QString)));

  connect(mStream, SIGNAL(signDataReceived(const qint32, const QByteArray)),
          this, SLOT(slotDataReceived(const qint32, QByteArray)));

  connect(&mTimerForActAverageTransferSpeed, SIGNAL(timeout()), this,
          SLOT(slotCalcAverageTransferSpeed()));

  mStreamID = mStream->getID();
  mCore.setStreamTypeToKnown(mStreamID, NULL, true);
  mStream->doConnect(Destination);

  mAlreadyFinished = false;
  mSendFirstPacket = true;
  mFileName = FilePath.mid(FilePath.lastIndexOf("/") + 1);
  mFileForSend.setFileName(mFilePath);
  mFileTransferAccepted = false;
  mFileSize = mFileForSend.size();
  mAlreadySentSize = 0;
  mCurrentPacketSize = NORMPACKETSIZE;
  mRemoteReceivedSize = 0;
}

void CFileTransferSend::slotAbbortFileSend() {
  mFileForSend.close();
  mTimerForActAverageTransferSpeed.stop();
  mConnectionManager.doDestroyStreamObjectByID(mStream->getID());
  mCore.getFileTransferManager()->removeFileTransfer(mStreamID);
}

void CFileTransferSend::slotStreamStatus(const SAM_Message_Types::RESULT result,
                                         const qint32 ID, QString Message) {
  using namespace FileTransferProtocol;

  if (mStreamID != ID) {
    qCritical() << "File\t" << __FILE__ << Qt::endl
                << "Line:\t" << __LINE__ << Qt::endl
                << "Function:\t"
                << "CFileTransferSend::slotStreamStatus" << Qt::endl
                << "Message:\t"
                << "mStreamID!=ID WTF" << Qt::endl
                << "mStreamID:\t" << mStreamID << Qt::endl
                << "ID:\t" << ID << Qt::endl;
  }

  switch (result) {
  case (SAM_Message_Types::OK): {
    if (mSendFirstPacket == true) {
      QString StringFileSize;
      StringFileSize.setNum(mFileSize);

      mStream->operator<<(QString("CHATSYSTEMFILETRANSFER\t" +
                                  mUsingProtocolVersion + "\n" +
                                  StringFileSize + "\n" + mFileName));
      // mStream->operator <<(FIRSTPACKET+StringFileSize+"\n"+mFileName);
      mSendFirstPacket = false;
    }
    break;
  }

  case (SAM_Message_Types::CANT_REACH_PEER):
  case (SAM_Message_Types::TIMEOUT):
  case (SAM_Message_Types::CLOSED): {
    mTimerForActAverageTransferSpeed.stop();

    if (mAlreadySentSize == mFileSize) {
      emit signFileTransferFinishedOK();
      if (mAlreadyFinished == false) {
        mCore.getUserManager()
            ->getUserByI2P_Destination(mDestination)
            ->slotIncomingMessageFromSystem(
                tr("Upload complete [%1]").arg(mFileName));
        mAlreadyFinished = true;
      }
    } else {
      emit signFileTransferAborted();
      if (mAlreadySentSize == 0) {
        mCore.getUserManager()
            ->getUserByI2P_Destination(mDestination)
            ->slotIncomingMessageFromSystem(
                tr("Cannot connect: Upload failed [%1]").arg(mFileName));
      } else {
        mCore.getUserManager()
            ->getUserByI2P_Destination(mDestination)
            ->slotIncomingMessageFromSystem(
                tr("Recipient aborted transfer [%1]").arg(mFileName));
      }
    }
    mFileForSend.close();
    mConnectionManager.doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileTransfer(mStreamID);

    break;
  }
  case (SAM_Message_Types::I2P_ERROR): {
    emit signFileTransferAborted();
    mCore.getUserManager()
        ->getUserByI2P_Destination(mDestination)
        ->slotIncomingMessageFromSystem(
            tr("I2P Stream Error: Upload failed [%1]<br>%2")
                .arg(mFileName)
                .arg(Message));

    mFileForSend.close();
    mConnectionManager.doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileTransfer(mStreamID);
    break;
  }
  case (SAM_Message_Types::INVALID_KEY): {
    emit signFileTransferAborted();
    mCore.getUserManager()
        ->getUserByI2P_Destination(mDestination)
        ->slotIncomingMessageFromSystem(
            tr("I2P Stream Error (Invalid Key): Upload failed [%1]<br>%2")
                .arg(mFileName)
                .arg(Message));

    mFileForSend.close();
    mConnectionManager.doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileTransfer(mStreamID);
    break;
  }
  case (SAM_Message_Types::INVALID_ID): {
    emit signFileTransferAborted();
    mCore.getUserManager()
        ->getUserByI2P_Destination(mDestination)
        ->slotIncomingMessageFromSystem(
            "I2P Stream Error (Invalid ID): Upload failed [" + mFileName +
            "]<br>" + Message);

    mFileForSend.close();
    mConnectionManager.doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileTransfer(mStreamID);
    break;
  }
  default: {
    break;
  }
  }
}

void CFileTransferSend::slotDataReceived(const qint32 ID, QByteArray t) {
  if (mUsingProtocolVersionD <= 0.2) {
    if (t.length() == 1) {

      if (t.contains("0")) { // true
        emit signFileTransferAccepted(true);
        mFileTransferAccepted = true;
        StartFileTransfer();

      } else if (t.contains("1")) { // false
        emit signFileTransferAccepted(false);
        mCore.getUserManager()
            ->getUserByI2P_Destination(mDestination)
            ->slotIncomingMessageFromSystem(
                tr("Upload declined [%1]").arg(mFileName));
        mConnectionManager.doDestroyStreamObjectByID(ID);
        mCore.getFileTransferManager()->removeFileTransfer(mStreamID);

      } else if (t.contains("2")) { // next block	(Proto 0.2)
        SendFile_v0dot2();
      }
    } else {
      emit signFileTransferAccepted(false);
      mCore.getUserManager()
          ->getUserByI2P_Destination(mDestination)
          ->slotIncomingMessageFromSystem(
              tr("Upload declined [%1]").arg(mFileName));
      mConnectionManager.doDestroyStreamObjectByID(ID);
      mCore.getFileTransferManager()->removeFileTransfer(mStreamID);
    }
  } else if (mUsingProtocolVersionD == 0.3) {
    mRemoteDataReceiveBuffer.append(t);
    while (mRemoteDataReceiveBuffer.contains('\n') == true) {
      QString CurrentPacket = mRemoteDataReceiveBuffer.mid(
          0, mRemoteDataReceiveBuffer.indexOf('\n', 0) + 1);
      mRemoteDataReceiveBuffer.remove(0, CurrentPacket.length());

      QString CurrentAction = CurrentPacket.mid(0, 1);
      CurrentPacket.remove(0, 3); // remove {0,1,2}:\t

      if (CurrentAction == "0") {
        // Filetransfer Accepted
        emit signFileTransferAccepted(true);
        qint64 from = CurrentPacket.remove('\n').toInt();
        mFileTransferAccepted = true;
        StartFileTransfer(from);
      } else if (CurrentAction == "1") {
        // File transfer declined
        emit signFileTransferAccepted(false);
        mCore.getUserManager()
            ->getUserByI2P_Destination(mDestination)
            ->slotIncomingMessageFromSystem(
                tr("Upload declined [%1]").arg(mFileName));
        mConnectionManager.doDestroyStreamObjectByID(ID);
        mCore.getFileTransferManager()->removeFileTransfer(mStreamID);
      } else if (CurrentAction == "2") {
        // next block & current size received remote
        CurrentPacket.remove('\n');
        mRemoteReceivedSize += CurrentPacket.toInt();
        emit signAlreadySentSizeChanged(mRemoteReceivedSize);
        if ((mAlreadySentSize - mRemoteReceivedSize) <= 1024) {
          SendFile_v0dot3();
        }
      }
    }
  }
}

void CFileTransferSend::StartFileTransfer(qint64 mFromPos) {
  mAlreadySentSize = 0;
  mFileForSend.open(QIODevice::ReadOnly);
  mFileForSend.seek(mFromPos);
  if (mFromPos != 0) {
    mAlreadySentSize = mFromPos;
  }

  mTimer.start();
  mTimerForActAverageTransferSpeed.start(AVERAGETRANSFERSPEEDPERIOD);

  if (mUsingProtocolVersionD == 0.1) {
    SendFile_v0dot1();
  } else if (mUsingProtocolVersionD == 0.2) {
    SendFile_v0dot2();
  } else if (mUsingProtocolVersionD == 0.3) {
    mCurrentPacketSize = MAXPACKETSIZE;
    SendFile_v0dot3();
  } else {
    qWarning() << "File\t" << __FILE__ << Qt::endl
               << "Line:\t" << __LINE__ << Qt::endl
               << "Function:\t"
               << "CFileTransferSend::StartFileTransfer" << Qt::endl
               << "Message:\t"
               << "Unsupported Protocolversion:" << mUsingProtocolVersion
               << Qt::endl;

    mCore.getFileTransferManager()->removeFileTransfer(mStreamID);
  }
}

void CFileTransferSend::SendFile_v0dot3() {
  QByteArray Buffer;

  // Use larger packet sizes for better performance
  Buffer = mFileForSend.read(mCurrentPacketSize);
  mAlreadySentSize += Buffer.length();

  mStream->operator<<(Buffer);

  if (mAlreadySentSize == mFileSize && mAlreadyFinished == false) {
    emit signFileTransferFinishedOK();
    mCore.getUserManager()
        ->getUserByI2P_Destination(mDestination)
        ->slotIncomingMessageFromSystem(
            tr("Upload completed [%1]").arg(mFileName));
    mAlreadyFinished = true;
  }
}

void CFileTransferSend::SendFile_v0dot2() {

  QByteArray Buffer;

  Buffer = mFileForSend.read(mCurrentPacketSize);
  mAlreadySentSize += Buffer.length();

  mStream->operator<<(Buffer);
  emit signAlreadySentSizeChanged(mAlreadySentSize);

  if (mAlreadySentSize == mFileSize && mAlreadyFinished == false) {
    emit signFileTransferFinishedOK();
    mCore.getUserManager()
        ->getUserByI2P_Destination(mDestination)
        ->slotIncomingMessageFromSystem(
            tr("Upload completed [%1]").arg(mFileName));
    mAlreadyFinished = true;
  }
}

void CFileTransferSend::SendFile_v0dot1() {
  while (mAlreadySentSize < mFileSize) {
    QByteArray Buffer;

    // Use larger packet size for better performance
    Buffer = mFileForSend.read(mCurrentPacketSize);
    mAlreadySentSize += Buffer.length();

    mStream->operator<<(Buffer);
    emit signAlreadySentSizeChanged(mAlreadySentSize);
  }

  if (mAlreadySentSize == mFileSize && mAlreadyFinished == false) {
    emit signFileTransferFinishedOK();
    mCore.getUserManager()
        ->getUserByI2P_Destination(mDestination)
        ->slotIncomingMessageFromSystem(
            tr("Upload completed [%1]").arg(mFileName));
    mAlreadyFinished = true;
  }
}

CFileTransferSend::~CFileTransferSend() {
  mTimerForActAverageTransferSpeed.stop();
}

bool CFileTransferSend::getIsTransferring() {
  if (mFileTransferAccepted == true && mAlreadyFinished == false) {
    return true;
  } else {
    return false;
  }
}

void CFileTransferSend::slotCalcAverageTransferSpeed() {
  QString speedSize;
  QString speedType;
  int speed;
  int departedtime = (mTimer.elapsed() / 1000);

  if (departedtime <= 0)
    departedtime = 1;

  if (mUsingProtocolVersionD <= 0.2) {
    speed = mAlreadySentSize / departedtime;
  } else {
    speed = mRemoteReceivedSize / departedtime;
  }

  mCore.doConvertNumberToTransferSize(speed, speedSize, speedType);
  emit signAverageTransferSpeed(speedSize, speedType);
  CalcETA(speed);
}

void CFileTransferSend::doConvertNumberToTransferSize(quint64 inNumber,
                                                      QString &outNumber,
                                                      QString &outType,
                                                      bool addStoOutType) {
  return mCore.doConvertNumberToTransferSize(inNumber, outNumber, outType,
                                             addStoOutType);
}

void CFileTransferSend::CalcETA(int speed) {
  QString EmitString;
  int secLeft;

  if (speed > 0) {
    if (mUsingProtocolVersionD <= 0.2) {
      secLeft = (mFileSize - mAlreadySentSize) / speed;
    } else {
      secLeft = (mFileSize - mRemoteReceivedSize) / speed;
    }
  } else {
    if (mUsingProtocolVersionD <= 0.2) {
      secLeft = mFileSize - mAlreadySentSize;
    } else {
      secLeft = mFileSize - mRemoteReceivedSize;
    }
  }

  if (secLeft > 86400) {
    //> 24h
    emit signETA(tr("Over a day..."));
  } else {
    int hours = 0;
    int minutes = 0;
    int secs = 0;

    if (secLeft >= 3600) {
      // hours
      hours = secLeft / 3600;
      secLeft -= hours * 3600;
    }
    if (secLeft >= 60) {
      minutes = secLeft / 60;
      secLeft -= minutes * 60;
    }
    secs = secLeft;

    // hours
    if (hours <= 9) {
      EmitString.append("0");
    }
    EmitString.append(QString::number(hours, 10) + ":");
    //---------------------------------------------------------
    // minutes
    if (minutes <= 9) {
      EmitString.append("0");
    }
    EmitString.append(QString::number(minutes, 10) + ":");
    //---------------------------------------------------------
    // secs
    if (secs <= 9) {
      EmitString.append("0");
    }
    EmitString.append(QString::number(secs, 10));

    signETA(EmitString);
  }
}
