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

#ifndef CUSERMANAGER_H
#define CUSERMANAGER_H

#include "Core.h"
#include "User.h"
#include <QObject>
#include <QRegExp> 
#include "UnsentChatMessageStorage.h"
static const QRegExp nicknameRegExp("^([a-zA-Z0-9_ ]){1,256}$");

class CUserManager : public QObject {
  Q_OBJECT
public:
  CUserManager(CCore &Core, QString UserFileWithPath,
               CUnsentChatMessageStorage &UnsentChatMessageStorage);
  ~CUserManager();

  // forbid some operators
  CUserManager(const CUserManager &) = delete;
  CUserManager &operator=(const CUserManager &) = delete;

  void saveUserList() const;
  void loadUserList();
  bool addNewUser(QString Name, QString I2PDestination, qint32 I2PStream_ID = 0,
                  bool SaveUserList = true);
  bool checkIfUserExistsByI2PDestination(QString I2PDestination) const;
  void changeUserPositionInUserList(int oldPos, int newPos);
  bool renameUserByI2PDestination(const QString Destination,
                                  const QString newNickname);
  bool deleteUserByI2PDestination(const QString I2PDestination);
  void avatarImageChanged();

  bool deleteUserByI2P_Destination(QString Destination);
  CUser *getUserByI2P_ID(qint32 ID) const;
  CUser *getUserByI2P_Destination(QString Destination) const;
  QString getUserInfosByI2P_Destination(QString Destination) const;
  const QList<CUser *> getUserList() const;
  bool validateI2PDestination(const QString I2PDestination) const;

signals:
  void signUserStatusChanged();

private slots:
  void slotSaveUnsentMessageForDest(QString I2PDest);

private:
  CCore &mCore;
  const QString mUserFileWithPath;
  CUnsentChatMessageStorage &mUnsentMessageStorage;
  QList<CUser *> mUsers;
private:

};
#endif // CUSERMANAGER_H
