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

// #include "Core.h"
#include "FileTransferRecive.h"
#include "UserManager.h"

CFileTransferRecive::CFileTransferRecive(CCore &Core, CI2PStream &Stream,
                                         qint32 StreamID, QString FileName,
                                         quint64 FileSize, QString Destination,
                                         QString Protocolversion,
                                         double ProtocolversionD)

    : mCore(Core), mStream(Stream), mStreamID(StreamID), mFileSize(FileSize),
      mDestination(Destination), mUsingProtocolVersion(Protocolversion),
      mUsingProtocolVersionD(ProtocolversionD) {

  mFileName = FileName;

  mConnectionManager = mCore.getConnectionManager();

  QSettings settings(mCore.getConfigPath() + "/application.ini",
                     QSettings::IniFormat);
  bool AutoAcceptFileRecive;
  QString AutoAcceptedFilePath;
  mStream.setUsedFor("FileTransferRecive");

  connect(&Stream,
          SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT,
                                         const qint32, const QString)),
          this,
          SLOT(slotStreamStatusReceived(const SAM_Message_Types::RESULT,
                                       const qint32, const QString)));

  connect(&Stream, SIGNAL(signDataReceived(const qint32, const QByteArray)),
          this, SLOT(slotDataReceived(const qint32, QByteArray)));

  connect(&mTimerForActAverageTransferSpeed, SIGNAL(timeout()), this,
          SLOT(slotCalcAverageTransferSpeed()));

  mAllreadyReceivedSize = 0;
  mRequestAccepted = false;

  settings.beginGroup("General");
  AutoAcceptFileRecive =
      (settings.value("AutoAcceptFileRecive", false).toBool());
  AutoAcceptedFilePath =
      (settings.value("IncomingFileFolder", mCore.getConfigPath() + "/Incoming")
           .toString());
  if (settings.value("UseIncomingSubFolderForEveryUser", false).toBool() ==
      true) {
    CUser *theUser = mCore.getUserManager()->getUserByI2P_Destination(
        mStream.getDestination());
    if (theUser != NULL) {
      AutoAcceptedFilePath += "/" + theUser->getName();
    }
  }

  settings.endGroup();
  settings.sync();

  if (AutoAcceptFileRecive == true) {
    mCore.getUserManager()
        ->getUserByI2P_Destination(Destination)
        ->slotIncomingMessageFromSystem(
            tr(" Autoaccepted incoming Filetransfer<br>Filename: %1")
                .arg(mFileName),
            true);

    QDir dir(AutoAcceptedFilePath);
    if (dir.exists() == false) {
      dir.mkpath(AutoAcceptedFilePath);
    }

    start(AutoAcceptedFilePath + "/" + mFileName, true);
  } else {
    mCore.getUserManager()
        ->getUserByI2P_Destination(Destination)
        ->slotIncomingMessageFromSystem(
            tr(" Incoming Filetransfer<br>Filename: %1"
               "<br>please accept or reject it in the user list")
                .arg(mFileName));
  }
}

CFileTransferRecive::~CFileTransferRecive() {
  mTimerForActAverageTransferSpeed.stop();
}

void CFileTransferRecive::slotStreamStatusReceived(
    const SAM_Message_Types::RESULT result, const qint32 ID, QString Message) {
  if (mStreamID != ID) {
    qDebug() << "CFileTransferRecive::slotStreamStatusReceived\n"
             << "mStreamID!=ID WTF";
  }

  switch (result) {
  case (SAM_Message_Types::OK): {
    break;
  }

  case (SAM_Message_Types::CANT_REACH_PEER):
  case (SAM_Message_Types::TIMEOUT):
  case (SAM_Message_Types::CLOSED): {
    mTimerForActAverageTransferSpeed.stop();
    if (mAllreadyReceivedSize == mFileSize) {
      emit signFileReceivedFinishedOK();

      QString SizeName;
      QString SSize;

      if (mFileSize >= (1024 * 1024)) {
        // MB
        QString MB;
        QString KB;

        int tmp = mFileSize / (1024 * 1024);
        int tmp2 = (mFileSize - (tmp * (1024 * 1024))) / 1024;
        MB.setNum(tmp, 10);
        KB.setNum(tmp2, 10);

        SSize = MB + "." + KB;
        SizeName = "MB";
      } else if (mFileSize >= 1024) {
        // KB
        QString KB;
        QString Bytes;

        qint32 tmp = mFileSize / (1024);
        qint32 tmp2 = (mFileSize - (tmp * (1024))) / 1024;

        KB.setNum(tmp, 10);
        Bytes.setNum(tmp2, 10);

        SSize = KB + "." + Bytes;
        SizeName = "KB";
      } else {
        // Byte
        SSize.setNum(mFileSize, 10);
        SizeName = "bytes";
      }
      mCore.getUserManager()
          ->getUserByI2P_Destination(mDestination)
          ->slotIncomingMessageFromSystem(tr("Transfer of %1 complete [%2 %3]")
                                              .arg(mFileName)
                                              .arg(SSize)
                                              .arg(SizeName));
    } else {
      emit signFileReciveAborted();
      if (mRequestAccepted == true) {
        mFileForRecive.remove();
        mCore.getUserManager()
            ->getUserByI2P_Destination(mDestination)
            ->slotIncomingMessageFromSystem(
                tr("Sender cancelled file transfer of %1 ‣ Incomplete file "
                   "deleted")
                    .arg(mFileName));

      } else {
        mFileForRecive.remove();
        mCore.getUserManager()
            ->getUserByI2P_Destination(mDestination)
            ->slotIncomingMessageFromSystem(tr("You cancelled file transfer of "
                                               "%1 ‣ Incomplete file deleted")
                                                .arg(mFileName));
      }
    }

    mFileForRecive.close();

    mConnectionManager->doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileRecive(mStreamID);
    break;
  }
  case (SAM_Message_Types::I2P_ERROR): {
    emit signFileReciveAborted();
    mCore.getUserManager()
        ->getUserByI2P_Destination(mDestination)
        ->slotIncomingMessageFromSystem(
            tr("I2P Stream Error while receiving %1<br>%2")
                .arg(mFileName)
                .arg(Message));
    mFileForRecive.close();

    mConnectionManager->doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileRecive(mStreamID);
    break;
  }
  case (SAM_Message_Types::INVALID_KEY): {
    emit signFileReciveAborted();
    mCore.getUserManager()
        ->getUserByI2P_Destination(mDestination)
        ->slotIncomingMessageFromSystem(
            tr("I2P Stream Error while receiving %1 [INVALID_KEY]<br>%2")
                .arg(mFileName)
                .arg(Message));

    mFileForRecive.close();

    mConnectionManager->doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileRecive(mStreamID);
    break;
  }
  case (SAM_Message_Types::INVALID_ID): {
    emit signFileReciveAborted();
    mCore.getUserManager()
        ->getUserByI2P_Destination(mDestination)
        ->slotIncomingMessageFromSystem(
            tr("I2P Stream Error while receiving %1 [INVALID_ID]<br>%2")
                .arg(mFileName)
                .arg(Message));

    mFileForRecive.close();

    mConnectionManager->doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileRecive(mStreamID);
    break;
  }
  default: {
    break;
  }
  }
}

void CFileTransferRecive::slotDataReceived(const qint32 ID, QByteArray t) {

  if (mStreamID != ID) {
    qDebug() << "CFileTransferRecive::slotDataReceived\n"
             << "mStreamID!=ID WTF";
  }

  mAllreadyReceivedSize += t.length();
  mFileForRecive.write(t);
  mFileForRecive.flush();

  emit signAllreadyReceivedSizeChanged(mAllreadyReceivedSize);

  if (mUsingProtocolVersionD == 0.2) {
    mStream.operator<<(QString("2")); // next block
  } else if (mUsingProtocolVersionD == 0.3) {
    // next block & length of recived data
    mStream.operator<<(
        QString("2:\t" + QString::number(t.length(), 10) + '\n'));
  }

  if (mAllreadyReceivedSize == mFileSize) {

    QString SizeName;
    QString SSize;

    mFileForRecive.close();

    if (mFileSize >= (1024 * 1024)) {
      // MB
      QString MB;
      QString KB;

      qint32 tmp = mFileSize / (1024 * 1024);
      qint32 tmp2 = mFileSize - (tmp * (1024 * 1024));
      tmp2 = tmp2 / 1024;

      MB.setNum(tmp, 10);
      KB.setNum(tmp2, 10);

      SSize = MB + "." + KB;
      SizeName = "MB";
    } else if (mFileSize >= 1024) {
      // KB
      QString KB;
      QString Bytes;

      qint32 tmp = mFileSize / (1024);
      qint32 tmp2 = mFileSize - (tmp * (1024));
      tmp2 = (tmp2 / 1024);

      KB.setNum(tmp, 10);
      Bytes.setNum(tmp2, 10);

      SSize = KB + "." + Bytes;
      SizeName = "KB";
    } else {
      // Byte
      SSize.setNum(mFileSize, 10);
      SizeName = "Bytes";
    }
    mCore.getUserManager()
        ->getUserByI2P_Destination(mDestination)
        ->slotIncomingMessageFromSystem("<br>File transfer of " + mFileName +
                                        " complete [" + SSize + " " + SizeName +
                                        "]");

    mFileForRecive.close();
    mCore.getFileTransferManager()->removeFileRecive(mStreamID);
    mConnectionManager->doDestroyStreamObjectByID(mStreamID);

    emit signFileReceivedFinishedOK();
  }
}

void CFileTransferRecive::slotAbbortFileRecive() {

  mFileForRecive.close();
  mTimerForActAverageTransferSpeed.stop();
  mFileForRecive.remove();

  mConnectionManager->doDestroyStreamObjectByID(mStreamID);
  mCore.getFileTransferManager()->removeFileRecive(mStreamID);
}

void CFileTransferRecive::start(QString FilePath, bool Accepted) {
  if (Accepted == true) {
    // mFileForRecive= new QFile(FilePath);
    mFileName = FilePath.mid(FilePath.lastIndexOf("/") + 1);

    mFileForRecive.setFileName(FilePath);
    mFileForRecive.open(QIODevice::WriteOnly);
    mTimer.start();
    mTimerForActAverageTransferSpeed.start(
        TIMERCOUNTFORAVERAGETRANSFERSPEED_READ);

    if (mUsingProtocolVersionD <= 0.2) {
      mStream.operator<<(QString("0")); // true
    } else if (mUsingProtocolVersionD == 0.3) {
      mStream.operator<<(QString("0:\t0") + '\n');
    }

    mRequestAccepted = true;
    emit signFileNameChanged();
  } else {

    // emit signFileReciveAborted();
    if (mUsingProtocolVersionD <= 0.2) {
      mStream.operator<<(QString("1")); // false
    } else if (mUsingProtocolVersionD == 0.3) {
      mStream.operator<<(QString("1:\t") + '\n');
    }
    mCore.getConnectionManager()->doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileRecive(mStreamID);

    mRequestAccepted = false;
  }
}

void CFileTransferRecive::slotCalcAverageTransferSpeed() {
  int departedtime = (mTimer.elapsed() / 1000);
  if (departedtime <= 0)
    departedtime = 1;
  int speed = mAllreadyReceivedSize / departedtime;

  QString speedSize;
  QString speedType;

  mCore.doConvertNumberToTransferSize(speed, speedSize, speedType);

  emit signAverageReciveSpeed(speedSize, speedType);
  CalcETA(speed);
}

void CFileTransferRecive::doConvertNumberToTransferSize(quint64 inNumber,
                                                        QString &outNumber,
                                                        QString &outType,
                                                        bool addStoOutType) {
  return mCore.doConvertNumberToTransferSize(inNumber, outNumber, outType,
                                             addStoOutType);
}

void CFileTransferRecive::CalcETA(int speed) {
  QString EmitString;
  int secLeft;

  if (speed > 0) {
    secLeft = (mFileSize - mAllreadyReceivedSize) / speed;
  } else {
    secLeft = mFileSize - mAllreadyReceivedSize;
  }

  if (secLeft > 86400) {
    //> 24h
    emit signETA(tr("never or (>24 h)"));
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
