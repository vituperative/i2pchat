/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "FileTransferManager.h"

#include "UserManager.h"

CFileTransferManager::CFileTransferManager(CCore &Core) : mCore(Core) {}

CFileTransferManager::~CFileTransferManager() {}

CFileTransferRecive *
CFileTransferManager::getFileTransferReciveByID(qint32 ID) const {
  /*for(int i=0;i<mFileRecives.size();i++){
          if(mFileRecives.at(i)->getStreamID()==ID){
                  return mFileRecives.at(i);
          }
  }*/
  for (auto it : mFileRecives)
    if (it->getStreamID() == ID)
      return it;

  return NULL;
}

CFileTransferSend *
CFileTransferManager::getFileTransferSendsByID(qint32 ID) const {
  for (auto it : mFileSends)
    if (it->getStreamID() == ID)
      return it;

  /*for(int i=0;i<mFileSends.size();i++){
          if(mFileSends.at(i)->getStreamID()==ID){
                  return mFileSends.at(i);
          }
  }*/
  return NULL;
}

const QList<CFileTransferSend *>
CFileTransferManager::getFileTransferSendsList() const {

  return mFileSends;
}

const QList<CFileTransferRecive *>
CFileTransferManager::getFileTransferReciveList() const {
  return mFileRecives;
}

void CFileTransferManager::addNewFileTransfer(QString FilePath,
                                              QString Destination) {

  QString Protocolversion;
  double ProtoVersionD = 0.0;

  CUser *User = mCore.getUserManager()->getUserByI2P_Destination(Destination);
  if (User != NULL) {
    Protocolversion = User->getHighestUsableProtocolVersionFiletransfer();
    ProtoVersionD = User->getHighestUsableProtocolVersionFiletransfer_D();
  } else {
    qCritical() << "Undefined user for file transfer";
    return;
  }
  if (this->getFileTransferSendsByID(User->getI2PStreamID()) != NULL ||
      this->getFileTransferReciveByID(User->getI2PStreamID()) != NULL) {
    qCritical() << "Already exists transfer for user";
    throw new std::runtime_error("Already exists transfer for user");
    return;
  }

  CFileTransferSend *t =
      new CFileTransferSend(mCore, *(mCore.getConnectionManager()), FilePath,
                            Destination, Protocolversion, ProtoVersionD);
  connect(t, SIGNAL(signFileTransferFinishedOK()), mCore.getSoundManager(),
          SLOT(slotFileSendFinished()));

  mFileSends.append(t);
  emit signUserStatusChanged();
}

void CFileTransferManager::addNewFileRecive(qint32 ID, QString FileName,
                                            QString FileSize,
                                            QString Destination,
                                            QString ProtocolVersion) {
  CI2PStream *Stream = mCore.getConnectionManager()->getStreamObjectByID(ID);
  FileName = FilterForFileName(FileName);

  double ProtocolVersionD;
  quint64 Size;
  bool OK;
  Size = FileSize.toULongLong(&OK, 10);
  if (OK == false) {
    QMessageBox *msgBox = new QMessageBox(NULL);
    msgBox->setIcon(QMessageBox::Critical);
    msgBox->setText(tr("CCore(addNewFileRecive)"));
    msgBox->setInformativeText(tr("Incoming file transfer rejected\nError "
                                  "converting QString to Quint64\nValue: %1")
                                   .arg(FileSize));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->show();

    // abort the Filerecive
    if (ProtocolVersion == "0.1" || ProtocolVersion == "0.2") {
      Stream->operator<<(QString("1")); // false
    } else if (ProtocolVersion == "0.3") {
      Stream->operator<<(QString(" 1:\t\n")); // false
    }

    mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
    removeFileRecive(ID);

    return;
  }
  ProtocolVersionD = ProtocolVersion.toDouble(&OK);
  if (OK == false) {
    qWarning() << "File\t" << __FILE__ << endl
               << "Line:\t" << __LINE__ << endl
               << "Function:\t"
               << " CFileTransferManager::addNewFileRecive" << endl
               << "Message:\t"
               << "Can't convert QString to double" << endl
               << "QString:\t" << ProtocolVersion << endl;

    // abort the Filerecive
    if (ProtocolVersion == "0.1" || ProtocolVersion == "0.2") {
      Stream->operator<<(QString("1")); // false
    } else if (ProtocolVersion == "0.3") {
      Stream->operator<<(QString(" 1:\t\n")); // false
    }

    mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
    removeFileRecive(ID);
    return;
  }

  if (ProtocolVersionD > FileTransferProtocol::MAXPROTOCOLVERSION_D) {
    // Show Info Message
    CUser *User = mCore.getUserManager()->getUserByI2P_Destination(Destination);
    if (User != NULL) {
      User->slotIncomingMessageFromSystem(
          tr("Incoming file transfer rejected: no protocol support\n"
             "Incoming protocol version: %1 \n"
             "Highest supported version: %2\n"
             "Filename: %3")
              .arg(ProtocolVersion)
              .arg(FileTransferProtocol::MAXPROTOCOLVERSION)
              .arg(FileName));
    }

    if (this->getFileTransferSendsByID(User->getI2PStreamID()) != NULL ||
        this->getFileTransferReciveByID(User->getI2PStreamID()) != NULL) {
      qCritical() << "File is already in the transfer queue";
      throw new std::runtime_error("File is already in the transfer queue");
      return;
    }

    // abort the Filerecive
    if (ProtocolVersion == "0.1" || ProtocolVersion == "0.2") {
      Stream->operator<<(QString("1")); // false
    } else if (ProtocolVersion == "0.3") {
      Stream->operator<<(QString(" 1:\t\n")); // false
    }

    mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
    removeFileRecive(ID);
    return;
  }

  mCore.getSoundManager()->slotFileReciveIncoming();

  disconnect(Stream,
             SIGNAL(signStreamStatusRecived(const SAM_Message_Types::RESULT,
                                            const qint32, const QString)),
             &mCore,
             SLOT(slotStreamStatusRecived(const SAM_Message_Types::RESULT,
                                          const qint32, QString)));

  CFileTransferRecive *t =
      new CFileTransferRecive(mCore, *Stream, ID, FileName, Size, Destination,
                              ProtocolVersion, ProtocolVersionD);
  connect(t, SIGNAL(signFileRecivedFinishedOK()), mCore.getSoundManager(),
          SLOT(slotFileReciveFinished()));

  connect(t, SIGNAL(signFileNameChanged()), this,
          SIGNAL(signUserStatusChanged()));

  mFileRecives.append(t);
  emit signUserStatusChanged();
}

bool CFileTransferManager::isThisID_a_FileSendID(qint32 ID) const {
  for (int i = 0; i < mFileSends.size(); i++) {
    if (mFileSends.at(i)->getStreamID() == ID) {
      return true;
    }
  }
  return false;
}

bool CFileTransferManager::isThisID_a_FileReciveID(qint32 ID) const {
  for (int i = 0; i < mFileRecives.size(); i++) {
    if (mFileRecives.at(i)->getStreamID() == ID) {
      return true;
    }
  }
  return false;
}

bool CFileTransferManager::checkIfAFileTransferOrReciveisActive() const {
  if (mFileSends.count() > 0)
    return true;
  if (mFileRecives.count() > 0)
    return true;

  return false;
}

void CFileTransferManager::removeFileTransfer(const qint32 ID) {
  if (mFileSends.count() > 0) {
    for (int i = 0; i < mFileSends.count(); i++) {
      if (mFileSends.at(i)->getStreamID() == ID) {
        mFileSends.at(i)->deleteLater();
        mFileSends.removeAt(i);
        emit signUserStatusChanged();
        break;
      }
    }
  }
}

void CFileTransferManager::removeFileRecive(const qint32 ID) {
  if (mFileRecives.count() > 0) {
    for (int i = 0; i < mFileRecives.count(); i++) {
      if (mFileRecives.at(i)->getStreamID() == ID) {
        mFileRecives.at(i)->deleteLater();
        mFileRecives.removeAt(i);
        emit signUserStatusChanged();
        break;
      }
    }
  }
}
const QString CFileTransferManager::FilterForFileName(QString FileName) const {
  FileName.replace("\\", "");
  FileName.replace("/", "");
  FileName.replace("..", "");
  return FileName;
}
