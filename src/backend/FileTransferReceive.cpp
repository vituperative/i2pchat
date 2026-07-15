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

#include "FileTransferReceive.h"

#include "Core.h"
#include "User.h"
#include "UserManager.h"

#include <QFileInfo>
#include <QSettings>
#include <QUrl>

#include <cmath>
#include <utility>

static bool isImageExtension(const QString &fileName);
static void tryDisplayImageInline(const QString &filePath, CUser *user);

CFileTransferReceive::CFileTransferReceive(CCore &Core,
                                           CI2PStream &Stream,
                                           qint32 StreamID,
                                           QString FileName,
                                           quint64 FileSize,
                                           const QString &Destination,
                                           QString Protocolversion,
                                           double ProtocolversionD)

  : mCore(Core)
  , mStream(Stream)
  , mStreamID(StreamID)
  , mFileSize(FileSize)
  , mDestination(Destination)
  , mUsingProtocolVersion(std::move(Protocolversion))
  , mUsingProtocolVersionD(ProtocolversionD) {

  mFileName = std::move(FileName);

  mConnectionManager = mCore.getConnectionManager();

  QSettings settings(mCore.getConfigPath() + "/application.ini", QSettings::IniFormat);
  bool AutoAcceptFileReceive;
  QString AutoAcceptedFilePath;
  mStream.setUsedFor("FileTransferReceive");

  connect(&Stream,
          SIGNAL(signStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)),
          this,
          SLOT(slotStreamStatusReceived(const SAM_Message_Types::RESULT, const qint32, const QString)));

  connect(&Stream,
          SIGNAL(signDataReceived(const qint32, const QByteArray)),
          this,
          SLOT(slotDataReceived(const qint32, QByteArray)));

  connect(&mTimerForActAverageTransferSpeed, SIGNAL(timeout()), this, SLOT(slotCalcAverageTransferSpeed()));

  mAlreadyReceivedSize = 0;
  mRequestAccepted = false;

  // Initialize write buffer for performance
  mWriteBuffer.reserve(1024 * 1024); // 1MB buffer

  settings.beginGroup("General");
  AutoAcceptFileReceive = (settings.value("AutoAcceptFileReceive", false).toBool());
  AutoAcceptedFilePath = (settings.value("IncomingFileFolder", mCore.getConfigPath() + "/Incoming").toString());
  if (settings.value("UseIncomingSubFolderForEveryUser", false).toBool() == true) {
    CUser *theUser = mCore.getUserManager()->getUserByI2P_Destination(mStream.getDestination());
    if (theUser != NULL) {
      AutoAcceptedFilePath += "/" + theUser->getName();
    }
  }

  settings.endGroup();
  settings.sync();

  // Check per-user auto-download setting first, then global setting
  CUser *theUser = mCore.getUserManager()->getUserByI2P_Destination(Destination);
  bool shouldAutoAccept = false;

  if (theUser != NULL) {
    shouldAutoAccept = theUser->getAutoDownloadEnabled();
  }

  // If per-user setting is not enabled, fall back to global setting
  if (!shouldAutoAccept) {
    shouldAutoAccept = AutoAcceptFileReceive;
  }

  if (shouldAutoAccept == true && theUser != NULL) {
    theUser->slotIncomingMessageFromSystem(tr(" Auto-accepted download [%1]").arg(mFileName), true);

    QDir dir(AutoAcceptedFilePath);
    if (dir.exists() == false) {
      dir.mkpath(AutoAcceptedFilePath);
    }

    start(AutoAcceptedFilePath + "/" + mFileName, true);
  } else {
    if (auto *u = mCore.getUserManager()->getUserByI2P_Destination(Destination))
      u->slotIncomingMessageFromSystem(tr(" Incoming file transfer [%1]"
                                          "<br>Accept or reject from the userlist")
                                         .arg(mFileName));
  }
}

CFileTransferReceive::~CFileTransferReceive() {
  mTimerForActAverageTransferSpeed.stop();

  // Flush any remaining buffer data
  if (!mWriteBuffer.isEmpty() && mFileForReceive.isOpen()) {
    mFileForReceive.write(mWriteBuffer);
    mFileForReceive.flush();
  }
}

void CFileTransferReceive::slotStreamStatusReceived(const SAM_Message_Types::RESULT result,
                                                    const qint32 ID,
                                                    const QString &Message) {
  if (mStreamID != ID) {
    qDebug() << "CFileTransferReceive::slotStreamStatusReceived\n"
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
    if (mAlreadyReceivedSize == mFileSize) {
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
      if (auto *u = mCore.getUserManager()->getUserByI2P_Destination(mDestination)) {
        u->slotIncomingMessageFromSystem(tr("Download complete [%1 %2 %3]").arg(mFileName).arg(SSize).arg(SizeName));

        QSettings settings(mCore.getConfigPath() + "/application.ini", QSettings::IniFormat);
        if (settings.value("Chat/DisplayImagesInline", false).toBool())
          tryDisplayImageInline(mFileForReceive.fileName(), u);
      }
    } else {
      emit signFileReceiveAborted();
      if (mRequestAccepted == true) {
        mFileForReceive.remove();
        if (auto *u = mCore.getUserManager()->getUserByI2P_Destination(mDestination))
          u->slotIncomingMessageFromSystem(tr("Sender aborted file transfer [%1]").arg(mFileName));

      } else {
        mFileForReceive.remove();
        if (auto *u = mCore.getUserManager()->getUserByI2P_Destination(mDestination))
          u->slotIncomingMessageFromSystem(tr("Download aborted [%1]").arg(mFileName));
      }
    }

    mFileForReceive.close();

    mConnectionManager->doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileReceive(mStreamID);
    break;
  }
  case (SAM_Message_Types::I2P_ERROR): {
    emit signFileReceiveAborted();
    if (auto *u = mCore.getUserManager()->getUserByI2P_Destination(mDestination))
      u->slotIncomingMessageFromSystem(tr("I2P Stream Error: Download failed [%1]<br>%2").arg(mFileName).arg(Message));
    mFileForReceive.close();

    mConnectionManager->doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileReceive(mStreamID);
    break;
  }
  case (SAM_Message_Types::INVALID_KEY): {
    emit signFileReceiveAborted();
    if (auto *u = mCore.getUserManager()->getUserByI2P_Destination(mDestination))
      u->slotIncomingMessageFromSystem(
        tr("I2P Stream Error (Invalid Key): Download failed [%1]<br>%2").arg(mFileName).arg(Message));

    mFileForReceive.close();

    mConnectionManager->doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileReceive(mStreamID);
    break;
  }
  case (SAM_Message_Types::INVALID_ID): {
    emit signFileReceiveAborted();
    if (auto *u = mCore.getUserManager()->getUserByI2P_Destination(mDestination))
      u->slotIncomingMessageFromSystem(
        tr("I2P Stream Error (Invalid ID): Download failed [%1]<br>%2").arg(mFileName).arg(Message));

    mFileForReceive.close();

    mConnectionManager->doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileReceive(mStreamID);
    break;
  }
  default: {
    break;
  }
  }
}

void CFileTransferReceive::slotDataReceived(const qint32 ID, const QByteArray &t) {

  if (mStreamID != ID) {
    qDebug() << "CFileTransferReceive::slotDataReceived\n"
             << "mStreamID!=ID WTF";
  }

  mAlreadyReceivedSize += t.length();

  // Use buffering for better performance
  mWriteBuffer.append(t);

  // Flush buffer when it reaches threshold or transfer is complete
  if (mWriteBuffer.size() >= 1024 * 1024 || mAlreadyReceivedSize == mFileSize) {
    mFileForReceive.write(mWriteBuffer);
    mFileForReceive.flush();
    mWriteBuffer.clear();
  }

  emit signgetTransferredSizeChanged(mAlreadyReceivedSize);

  if (mUsingProtocolVersionD == 0.2) {
    mStream.operator<<(QString("2")); // next block
  } else if (mUsingProtocolVersionD == 0.3) {
    // next block & length of received data
    mStream.operator<<(QString("2:\t" + QString::number(t.length(), 10) + '\n'));
  }

  if (mAlreadyReceivedSize == mFileSize) {

    QString SizeName;
    QString SSize;

    // Flush any remaining buffer data before closing
    if (!mWriteBuffer.isEmpty()) {
      mFileForReceive.write(mWriteBuffer);
      mFileForReceive.flush();
      mWriteBuffer.clear();
    }

    mFileForReceive.close();

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
    if (auto *u = mCore.getUserManager()->getUserByI2P_Destination(mDestination)) {
      u->slotIncomingMessageFromSystem("<br>Download complete [" + mFileName + " " + SSize + " " + SizeName + "]");

      QSettings settings(mCore.getConfigPath() + "/application.ini", QSettings::IniFormat);
      if (settings.value("Chat/DisplayImagesInline", false).toBool())
        tryDisplayImageInline(mFileForReceive.fileName(), u);
    }

    mFileForReceive.close();
    mCore.getFileTransferManager()->removeFileReceive(mStreamID);
    mConnectionManager->doDestroyStreamObjectByID(mStreamID);

    emit signFileReceivedFinishedOK();
  }
}

void CFileTransferReceive::slotAbbortFileReceive() {

  // Flush any remaining buffer data before closing
  if (!mWriteBuffer.isEmpty() && mFileForReceive.isOpen()) {
    mFileForReceive.write(mWriteBuffer);
    mFileForReceive.flush();
  }

  mFileForReceive.close();
  mTimerForActAverageTransferSpeed.stop();
  mFileForReceive.remove();

  mConnectionManager->doDestroyStreamObjectByID(mStreamID);
  mCore.getFileTransferManager()->removeFileReceive(mStreamID);
}

void CFileTransferReceive::start(const QString &FilePath, bool Accepted) {
  if (Accepted == true) {
    // mFileForReceive= new QFile(FilePath);
    mFileName = FilePath.mid(FilePath.lastIndexOf("/") + 1);

    mFileForReceive.setFileName(FilePath);
    mFileForReceive.open(QIODevice::WriteOnly);
    mTimer.start();
    mTimerForActAverageTransferSpeed.start(TIMERCOUNTFORAVERAGETRANSFERSPEED_READ);

    if (mUsingProtocolVersionD <= 0.2) {
      mStream.operator<<(QString("0")); // true
    } else if (mUsingProtocolVersionD == 0.3) {
      mStream.operator<<(QString("0:\t0") + '\n');
    }

    mRequestAccepted = true;
    emit signFileNameChanged();
  } else {

    // emit signFileReceiveAborted();
    if (mUsingProtocolVersionD <= 0.2) {
      mStream.operator<<(QString("1")); // false
    } else if (mUsingProtocolVersionD == 0.3) {
      mStream.operator<<(QString("1:\t") + '\n');
    }
    mCore.getConnectionManager()->doDestroyStreamObjectByID(mStreamID);
    mCore.getFileTransferManager()->removeFileReceive(mStreamID);

    mRequestAccepted = false;
  }
}

void CFileTransferReceive::slotCalcAverageTransferSpeed() {
  qint64 departedtime = (mTimer.elapsed() / 1000);
  if (departedtime <= 0)
    departedtime = 1;
  quint64 speed = mAlreadyReceivedSize / departedtime;

  QString speedSize;
  QString speedType;

  mCore.doConvertNumberToTransferSize(speed, speedSize, speedType);

  emit signAverageReceiveSpeed(speedSize, speedType);
  CalcETA(speed);
}

void CFileTransferReceive::doConvertNumberToTransferSize(quint64 inNumber,
                                                         QString &outNumber,
                                                         QString &outType,
                                                         bool addStoOutType) {
  mCore.doConvertNumberToTransferSize(inNumber, outNumber, outType, addStoOutType);
}

void CFileTransferReceive::CalcETA(quint64 speed) {
  QString EmitString;
  quint64 secLeft;

  if (speed > 0) {
    secLeft = (mFileSize - mAlreadyReceivedSize) / speed;
  } else {
    secLeft = mFileSize - mAlreadyReceivedSize;
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

static bool isImageExtension(const QString &fileName) {
  QString ext = QFileInfo(fileName).suffix().toLower();
  return ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "gif" || ext == "bmp" || ext == "webp";
}

static void tryDisplayImageInline(const QString &filePath, CUser *user) {
  if (!user || filePath.isEmpty())
    return;
  if (!isImageExtension(filePath))
    return;

  QImage img(filePath);
  if (img.isNull())
    return;

  const int MAX_CHAT_WIDTH = 400;
  QImage scaled = CCore::scaleImageLanczos(img, MAX_CHAT_WIDTH);

  QDir().mkpath("/tmp/i2pchat");
  QString previewPath = QString("/tmp/i2pchat/%1.jpg").arg(user->getName());
  if (!scaled.save(previewPath, "JPEG"))
    return;

  QUrl url = QUrl::fromLocalFile(previewPath);
  user->slotIncomingMessageFromSystem(QString("<br><img src=\"%1\" style=\"max-width:400px;\" />").arg(url.toString()));
}
