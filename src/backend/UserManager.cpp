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

#include "UserManager.h"

#include "Core.h"
#include "Protocol.h"
#include "UserBlockManager.h"

CUserManager::CUserManager(CCore &Core, QString UserFileWithPath,
                           CUnsentChatMessageStorage &UnsentChatMessageStorage)
    : mCore(Core), mUserFileWithPath(UserFileWithPath),
      mUnsentMessageStorage(UnsentChatMessageStorage) {}

CUserManager::~CUserManager() {

  for (int i = 0; i < this->mUsers.count(); i++) {
    // delete mUsers.at(i);
    mUsers.at(i)->deleteLater();
  }

  mUsers.clear();
}

void CUserManager::loadUserList() {
  QFile file(mUserFileWithPath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;

  QString NickName;
  QString I2PDest;

  QString line;
  QStringList temp;

  QByteArray bUserList = file.readAll();
  QTextStream in(bUserList);
  in.skipWhiteSpace();

  while (!in.atEnd()) {
    line = in.readLine(550);
    temp = line.split("\t");

    if (temp[0] == "Nick:") {
      NickName = temp[1];
    } else if (temp[0] == "I2PDest:") {
      I2PDest = temp[1];
      this->addNewUser(NickName, I2PDest, 0, false);

      // load unsent ChatMessages
      QStringList message = mUnsentMessageStorage.getMessagesForDest(I2PDest);
      for (int i = 0; i < message.count(); i++) {
        if (message.at(i).isEmpty() == false) {
          getUserByI2P_Destination(I2PDest)->slotSendChatMessage(message.at(i));
        }
      }
    } else if (temp[0] == "Invisible:") {
      if (temp[1] == "true") {
        getUserByI2P_Destination(I2PDest)->setInvisible(true);
      }
    } else if (temp[0] == "TorDest:") {
      // ignore it
    }
  }
  file.close();
}

void CUserManager::saveUserList() const {
  QFile file(mCore.getConfigPath() + "/users.config");
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  QTextStream out(&file);
  QString InvisibleText;

  mUnsentMessageStorage.clearStorage();

  for (int i = 0; i < this->mUsers.count(); i++) {
    if (mUsers.at(i)->getIsInvisible() == true) {
      InvisibleText = "true";
    } else {
      InvisibleText = "false";
    }

    out << "Nick:\t" << (mUsers.at(i)->getName()) << endl
        << "I2PDest:\t" << (mUsers.at(i)->getI2PDestination()) << endl
        << "Invisible:\t" << InvisibleText << endl;

    // save unsent ChatMessages for this users
    const QString Dest = mUsers.at(i)->getI2PDestination();
    const QStringList Messages = mUsers.at(i)->getUnsentedMessages();
    mUnsentMessageStorage.saveChatMessagesForDest(Dest, Messages);
  }
  out.flush();
  file.close();
}

bool CUserManager::deleteUserByI2P_Destination(QString Destination) {
  for (int i = 0; i < mUsers.size(); i++) {
    if (mUsers.at(i)->getI2PDestination() == Destination) {
      mUsers.removeAt(i);
      return true;
    }
  }
  return false;
}

CUser *CUserManager::getUserByI2P_ID(qint32 ID) const {

  /*for(int i=0;i<mUsers.size();i++){
          if(mUsers.at(i)->getI2PStreamID()==ID){

                  return mUsers.at(i);
          }
  }*/
  for (auto it : mUsers)
    if (it->getI2PStreamID() == ID)
      return it;

  return NULL;
}
CUser *CUserManager::getUserByI2P_Destination(QString Destination) const {
  /*for(int i=0;i<mUsers.size();i++){
          if(mUsers.at(i)->getI2PDestination()==Destination){
                  return mUsers.at(i);
          }
  }*/
  for (auto it : mUsers)
    if (it->getI2PDestination() == Destination)
      return it;

  return NULL;
}

QString CUserManager::getUserInfosByI2P_Destination(QString Destination) const {
  QString Infos = "No Information received";
  for (int i = 0; i < mUsers.size(); i++) {
    if (mUsers.at(i)->getI2PDestination() == Destination) {
      CUser *theUser = mUsers.at(i);

      if (theUser->getClientName() != nullptr) {
        Infos = "Nickname:\t\t" + theUser->getName() + "\n";
        Infos += "Client:\t\t" + theUser->getClientName() + " " +
                 theUser->getClientVersion() + "\n";
        Infos += "Protocol version:\t" + theUser->getProtocolVersion() + "\n";
        Infos += "File transfer support:\t" +
                 theUser->getMinProtocolVersionFiletransfer() + " - " +
                 theUser->getMaxProtocolVersionFiletransfer() + "\n";
      } else {
        Infos = "Nickname:\t" + theUser->getName() + "\n";
        Infos += "Status:\tOffline\n";
      }

      if (theUser->getProtocolVersion_D() >= 0.3) {
        CReceivedInfos receivedInfos = theUser->getReceivedUserInfos();
        QString sAge;
        sAge.setNum(receivedInfos.Age, 10);

        if (receivedInfos.Gender != nullptr || sAge != "0" ||
            receivedInfos.Interests != nullptr) {
          Infos += "\nUser Information:\n";
          if (receivedInfos.Gender != nullptr) {
            Infos += "Gender:\t\t" + receivedInfos.Gender + "\n";
          }
          if (sAge != nullptr && sAge != "0") {
            Infos += "Age:\t\t" + sAge + "\n";
          }
          if (receivedInfos.Interests != nullptr) {
            Infos += "Interests:\t\t" + receivedInfos.Interests;
          }
        }
      }
    }
  }
  return Infos;
}

const QList<CUser *> CUserManager::getUserList() const { return mUsers; }

bool CUserManager::validateI2PDestination(const QString I2PDestination) const {
  auto validateB64 = [](QString Dest) {
    if (Dest.length() == 516 &&
        Dest.right(4).contains("AAAA", Qt::CaseInsensitive))
      return true;
    else
      return false;
  };

  auto validateB32 = [](QString Dest) {
    if (Dest.length() == 60 &&
        Dest.right(8).contains(".b32.i2p", Qt::CaseInsensitive))
      return true;
    else
      return false;
  };

  auto validateECDSA_SHA256_P256 = [](QString Dest) {
    if (Dest.length() == 524 &&
        Dest.right(10).contains("AEAAEAAA==", Qt::CaseInsensitive))
      return true;
    else
      return false;
  };

  auto validateECDSA_SHA384_P384 = [](QString Dest) {
    if (Dest.length() == 524 &&
        Dest.right(10).contains("AEAAIAAA==", Qt::CaseInsensitive))
      return true;
    else
      return false;
  };

  auto validateECDSA_SHA512_P512 = [](QString Dest) {
    if (Dest.length() == 528 &&
        Dest.right(1).contains("=", Qt::CaseInsensitive) &&
        Dest.mid(512, 9).contains("BQAIAAMAA", Qt::CaseInsensitive))
      return true;
    else
      return false;
  };

  if (I2PDestination.right(4).contains("AAAA", Qt::CaseInsensitive)) {
    return validateB64(I2PDestination);
  } else if (I2PDestination.right(8).contains(".b32.i2p",
                                              Qt::CaseInsensitive)) {
    return validateB32(I2PDestination);
  } else if (I2PDestination.right(10).contains("AEAAEAAA==",
                                               Qt::CaseInsensitive)) {
    return validateECDSA_SHA256_P256(I2PDestination);
  } else if (I2PDestination.right(10).contains("AEAAIAAA==",
                                               Qt::CaseInsensitive)) {
    return validateECDSA_SHA384_P384(I2PDestination);
  } else if (I2PDestination.length() == 528 &&
             I2PDestination.mid(512, 9).contains("BQAIAAMAA",
                                                 Qt::CaseInsensitive)) {
    return validateECDSA_SHA512_P512(I2PDestination);
  } else
    return false;
}

bool CUserManager::addNewUser(QString Name, QString I2PDestination,
                              qint32 I2PStream_ID, bool SaveUserList) {
  CUserBlockManager &UserBlockManager = *(mCore.getUserBlockManager());
  CProtocol &Protocol = *(mCore.getProtocol());
  if (!mCore.getAccessAnyoneIncoming())
    return false;
  if (!nicknameRegExp.exactMatch(Name))
    Name = "Unallowed nickname";

  bool isValid = validateI2PDestination(I2PDestination);

  auto critical = [&I2PDestination](QString why = "undefined") {
    qCritical() << "File\t" << __FILE__ << endl
                << "Line:\t" << __LINE__ << endl
                << "Function:\t"
                << "CUserManager::addNewUser" << endl
                << "Message:\t"
                << "Destination is not valid" << endl
                << "Destination:\t" << I2PDestination << endl
                << why << endl;
    return false;
  };
  /*if(	getUserByI2P_Destination(I2PDestination)	!=	NULL	){
          return critical("Already exists user");
  }*/
  if (isValid == false) {
/*
    qCritical() << "File\t" << __FILE__ << endl
                << "Line:\t" << __LINE__ << endl
                << "Function:\t"
                << "CUserManager::addNewUser" << endl
                << "Message:\t"
                << "Destination is not valid" << endl
                << "Destination:\t" << I2PDestination << endl
                << "Action:\tAdd New User ignored" << endl;
    return false;
*/
    return critical("Not a valid user");
  }

  if (UserBlockManager.isDestinationInBlockList(I2PDestination) == true) {
    return critical("Destination is in blockList");
  }

  if (this->checkIfUserExistsByI2PDestination(I2PDestination) == true) {
    return critical("Destination already exists for user");
  }

  if (I2PDestination == mCore.getMyDestination()) {
    return critical("Cannot add our own destination");
  }

  // add newuser
  CSoundManager &SoundManager = *(mCore.getSoundManager());
  CUser *newuser =
      new CUser(mCore, Protocol, Name, I2PDestination, I2PStream_ID);
  connect(newuser, SIGNAL(signNewMessageSound()), &SoundManager,
          SLOT(slotNewChatMessage()));

  connect(newuser, SIGNAL(signConnectionOnline()), &SoundManager,
          SLOT(slotUserGoOnline()));

  connect(newuser, SIGNAL(signConnectionOffline()), &SoundManager,
          SLOT(slotUserGoOffline()));

  connect(newuser, SIGNAL(signOnlineStateChanged()), this,
          SIGNAL(signUserStatusChanged()));

  connect(newuser, SIGNAL(signSaveUnsentMessages(QString)), this,
          SLOT(slotSaveUnsentMessageForDest(QString)));

  this->mUsers.append(newuser);
  if (SaveUserList == true) {
    saveUserList();
  }

  if (mCore.getOnlineStatus() != USEROFFLINE &&
      mCore.getOnlineStatus() != USERTRYTOCONNECT) {
    if (I2PStream_ID == 0) {
      mCore.createStreamObjectForUser(*newuser);
    }
  }
  emit signUserStatusChanged();
  return true;
}

bool CUserManager::checkIfUserExistsByI2PDestination(
    const QString I2PDestination) const {
  if (I2PDestination == mCore.getMyDestination())
    return true;

  /*for(int i=0;i<mUsers.count();i++){
          if(mUsers.at(i)->getI2PDestination()==I2PDestination){
                  return true;
          }
  }*/
  if (this->getUserByI2P_Destination(I2PDestination) != NULL)
    return true;
  return false;
}
void CUserManager::changeUserPositionInUserList(int oldPos, int newPos) {
  mUsers.swap(oldPos, newPos);
  saveUserList();
  emit signUserStatusChanged();
}

bool CUserManager::deleteUserByI2PDestination(QString I2PDestination) {
  auto Him = this->getUserByI2P_Destination(I2PDestination);
  if (Him == NULL)
    return false;
  /*for(int i=0;i<mUsers.count();i++){
          if(mUsers.at(i)->getI2PDestination()==I2PDestination){
                  if(mUsers.at(i)->getConnectionStatus()==ONLINE
  ||mUsers.at(i)->getConnectionStatus()==TRYTOCONNECT){
                          mCore.deletePacketManagerByID(mUsers.at(i)->getI2PStreamID());
                          mCore.getConnectionManager()->doDestroyStreamObjectByID(mUsers.at(i)->getI2PStreamID());
                  }

                  if(mCore.getConnectionManager()->isComponentStopped()==false){
                          mCore.getConnectionManager()->doDestroyStreamObjectByID(mUsers.at(i)->getI2PStreamID());
                  }
                  mUsers.at(i)->deleteLater();
                  mUsers.removeAt(i);
                  saveUserList();
                  emit signUserStatusChanged();
                  return true;
          }
  }*/

  if (Him->getConnectionStatus() == ONLINE ||
      Him->getConnectionStatus() == TRYTOCONNECT) {
    mCore.deletePacketManagerByID(Him->getI2PStreamID());
    mCore.getConnectionManager()->doDestroyStreamObjectByID(
        Him->getI2PStreamID());
  }

  if (mCore.getConnectionManager()->isComponentStopped() == false) {
    mCore.getConnectionManager()->doDestroyStreamObjectByID(
        Him->getI2PStreamID());
  }
  Him->deleteLater();
  // mUsers.removeAt(i);
  this->deleteUserByI2P_Destination(I2PDestination);
  saveUserList();
  emit signUserStatusChanged();
  return true;
}

bool CUserManager::renameUserByI2PDestination(const QString Destination,
                                              const QString newNickname) {
  for (int i = 0; i < mUsers.size(); i++) {
    if (mUsers.at(i)->getI2PDestination() == Destination) {
      mUsers.at(i)->setName(newNickname);
      saveUserList();
      emit signUserStatusChanged();
      return true;
    }
  }
  return false;
}

void CUserManager::avatarImageChanged() {
  for (int i = 0; i < mUsers.count(); i++) {
    CUser *User = mUsers.at(i);

    if (User->getOnlineState() != USEROFFLINE &&
        User->getOnlineState() != USERTRYTOCONNECT &&
        User->getOnlineState() != USERBLOCKEDYOU &&
        User->getProtocolVersion_D() >= 0.6) {
      CProtocol &Protocol = *(mCore.getProtocol());
      Protocol.send(AVATARIMAGE_CHANGED, mUsers.at(i)->getI2PStreamID(),
                    QString());
    }
  }
}

void CUserManager::slotSaveUnsentMessageForDest(QString I2PDest) {
  CUser *theUser = getUserByI2P_Destination(I2PDest);
  if (theUser != NULL) {
    const QStringList Messages = theUser->getUnsentedMessages();
    mUnsentMessageStorage.saveChatMessagesForDest(I2PDest, Messages);
  } else {
    qWarning() << "File\t" << __FILE__ << endl
               << "Line:\t" << __LINE__ << endl
               << "Function:\t"
               << "CUserManager::slotSaveUnsentMessageForDest" << endl
               << "Message:\t"
               << "No User found with this destination" << endl
               << "I2PDest.:\t" << I2PDest << endl;
  }
}
