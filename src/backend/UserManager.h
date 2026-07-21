// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef CUSERMANAGER_H
#define CUSERMANAGER_H

#include "Core.h"
#include "UnsentChatMessageStorage.h"
#include "User.h"

#include <QObject>
#include <QRegularExpression>
static const QRegularExpression nicknameRegExp("^[a-zA-Z0-9_ ]{1,12}$");

class CUserManager : public QObject {
  Q_OBJECT
public:
  CUserManager(CCore &Core, QString UserFileWithPath, CUnsentChatMessageStorage &UnsentChatMessageStorage);
  ~CUserManager();

  // forbid some operators
  CUserManager(const CUserManager &) = delete;
  CUserManager &operator=(const CUserManager &) = delete;

  void saveUserList();
  void loadUserList();
  bool addNewUser(QString Name,
                  QString I2PDestination,
                  qint32 I2PStream_ID = 0,
                  bool SaveUserList = true,
                  bool skipGlobalGate = false);
  bool checkIfUserExistsByI2PDestination(const QString &I2PDestination) const;
  void changeUserPositionInUserList(int oldPos, int newPos);
  void sortUserList(int sortType);
  void setSortingEnabled(bool enabled);
  bool renameUserByI2PDestination(const QString &Destination, const QString &newNickname);
  bool deleteUserByI2PDestination(const QString &I2PDestination);
  void avatarImageChanged();

  bool deleteUserByI2P_Destination(const QString &Destination);
  CUser *getUserByI2P_ID(qint32 ID) const;
  CUser *getUserByI2P_Destination(const QString &Destination) const;
  QString getUserInfosByI2P_Destination(const QString &Destination) const;
  const QList<CUser *> getUserList() const;
  bool validateI2PDestination(const QString &I2PDestination) const;

signals:
  void signUserStatusChanged();

private slots:
  void slotSaveUnsentMessageForDest(const QString &I2PDest);
  void slotResort();

private:
  CCore &mCore;
  const QString mUserFileWithPath;
  CUnsentChatMessageStorage &mUnsentMessageStorage;
  QList<CUser *> mUsers;
  bool mSortingEnabled;
  int mSortType;

private:
};
#endif // CUSERMANAGER_H
