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
#include <QSettings>

#include "Core.h"
#include "I2PStream.h"
#include "Protocol.h"
#include "User.h"
#include "UserBlockManager.h"
#include "UserManager.h"

CUserBlockManager::CUserBlockManager(CCore &Core,
                                     const QString FilePathToBlockFile)
    : mCore(Core), mFilePathToBlockFile(FilePathToBlockFile) {}

CUserBlockManager::~CUserBlockManager() { saveBlockListe(); }

void CUserBlockManager::readBlockListe() {
  QFile file(mFilePathToBlockFile);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;

  QByteArray bUserList = file.readAll();
  QTextStream in(bUserList);
  in.skipWhiteSpace();

  QString Nickname;
  QString Destination;
  QString BlockDate;
  QString line;
  QStringList temp;

  while (!in.atEnd()) {
    line = in.readLine(550);
    temp = line.split("\t");

    if (temp[0] == "NickName:") {
      Nickname = temp[1];
    } else if (temp[0] == "Destination:") {
      Destination = temp[1];
    } else if (temp[0] == "BlockDate:") {
      BlockDate = temp[1];
      addNewBlockEntity(Nickname, Destination, BlockDate);
    }
  }
  file.close();
}

void CUserBlockManager::saveBlockListe() {
  QFile file(mFilePathToBlockFile);
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  QTextStream out(&file);

  QMapIterator<QString, CUserBlockEntity *> i(mUserBlockMap);

  while (i.hasNext()) {
    i.next();
    CUserBlockEntity *currentEntity = i.value();

    out << "NickName:\t" << currentEntity->mNickName << Qt::endl
        << "Destination:\t" << currentEntity->mDestination << Qt::endl
        << "BlockDate:\t" << currentEntity->mBlockDate << Qt::endl;
  }
  out.flush();
  file.close();
}

void CUserBlockManager::addNewBlockEntity(const QString NickName,
                                          const QString Destination,
                                          QString BlockDate) {
  using namespace User;
  CUser *User;
  User = mCore.getUserManager()->getUserByI2P_Destination(Destination);

  if (mUserBlockMap.contains(Destination) == true) {
    // do nothing allready blocked ?
    qDebug() << "File\t" << __FILE__ << Qt::endl
             << "Line:\t" << __LINE__ << Qt::endl
             << "Function:\t"
             << "cUserBlockManager::addNewBlockEntity" << Qt::endl
             << "Message:\t"
             << "Destination: " << Destination << Qt::endl
             << "is allready in the Blocklist" << Qt::endl
             << "Action Aborted" << Qt::endl;
  } else {
    if (BlockDate.isEmpty()) {
      if (BlockDate.isEmpty()) {
        CUserBlockEntity *tmp = new CUserBlockEntity(NickName, Destination);
        mUserBlockMap.insert(Destination, tmp);
      }
    } else {
      CUserBlockEntity *tmp =
          new CUserBlockEntity(NickName, Destination, BlockDate);
      mUserBlockMap.insert(Destination, tmp);
    }
  }
  if (User != NULL) {
    if (User->getConnectionStatus() == ONLINE ||
        User->getConnectionStatus() == TRYTOCONNECT) {
      if (User->getProtocolVersion_D() < 0.4) {
        mCore.getProtocol()->send(
            CHATMESSAGE, User->getI2PStreamID(),
            QString("You were blocked,all Packets will be ignored !"));
      } else {
        QSettings settings(mCore.getConfigPath() + "/application.ini",
                           QSettings::IniFormat);
        settings.beginGroup("Security");
        if (settings.value("BlockStyle", "Normal").toString() == "Normal") {
          mCore.getProtocol()->send(CHATMESSAGE, User->getI2PStreamID(),
                                    QString("You were blocked !"));
          mCore.getProtocol()->send(USER_BLOCK_NORMAL, User->getI2PStreamID(),
                                    QString(""));

        } else {
          // Block-Style Invisible
          mCore.getProtocol()->send(USER_BLOCK_INVISIBLE,
                                    User->getI2PStreamID(), QString(""));
        }
        settings.endGroup();
        settings.sync();
      }
    }
    mCore.getUserManager()->deleteUserByI2PDestination(
        User->getI2PDestination());
  }
  saveBlockListe();
}

void CUserBlockManager::removeBlockEntity(const QString Destination,
                                          bool CreateUser) {
  QString Nickname;

  if (mUserBlockMap.contains(Destination)) {
    Nickname = mUserBlockMap.value(Destination)->mNickName;
    mUserBlockMap.remove(Destination);
    if (CreateUser == true) {
      mCore.getUserManager()->addNewUser(Nickname, Destination, true);
    }

  } else {
    qDebug() << "File\t" << __FILE__ << Qt::endl
             << "Line:\t" << __LINE__ << Qt::endl
             << "Function:\t"
             << "CUserBlockManager::removeBlockEntity" << Qt::endl
             << "Message:\t"
             << "Destination: " << Destination << Qt::endl
             << "isn't in the Blocklist" << Qt::endl
             << "Action Aborted" << Qt::endl;
  }
  saveBlockListe();
}

bool CUserBlockManager::isDestinationInBlockList(
    const QString Destination) const {
  return mUserBlockMap.contains(Destination);
}
