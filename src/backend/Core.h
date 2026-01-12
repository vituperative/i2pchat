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

#ifndef CORE_H
#define CORE_H

#include <QIODevice>
#include <QList>
#include <QSettings>
#include <QTextStream>
#include <QtGui>

#include "DebugMessageManager.h"
#include "FileTransferManager.h"
#include "I2PSamMessageAnalyser.h"
#include "Protocol.h"
#include "SoundManager.h"
#include "UnsentChatMessageStorage.h"
#include "User.h"
#include "UserBlockManager.h"

#define CLIENTVERSION "0.2.37"
#define CLIENTNAME "I2PChat"

using namespace SAM_Message_Types;
using namespace User;

class CUserManager;
class CConnectionManager;
class CFileTransferManager;
class CPacketManager;
class CCore : public QObject {
  Q_OBJECT
public:
  CCore(QString configPath);
  ~CCore();

  // forbid some operators
  CCore(const CCore &) = delete;
  CCore &operator=(const CCore &) = delete;

  QString getDestinationByID(qint32 ID) const;
  const QString getMyDestination() const;
  const QString getMyDestinationB32() const;
  ONLINESTATE getOnlineStatus() const;
  QString getClientName() const { return CLIENTNAME; };
  QString getClientVersion() const { return CLIENTVERSION; };
  QString getProtocolVersion() const {
    return mProtocol->getProtocolVersion();
  };
  CI2PStream *getI2PStreamObjectByID(qint32 ID) const;
  const CReceivedInfos getUserInfos() const;
  QString getConnectionDump() const;
  const QString getConfigPath() const { return mConfigPath; };

  //<SUBSYSTEMS>
  CDebugMessageManager *getDebugMessageHandler() const {
    return mDebugMessageHandler;
  };
  CConnectionManager *getConnectionManager() const {
    return mConnectionManager;
  };
  CUserBlockManager *getUserBlockManager() const { return mUserBlockManager; };
  CProtocol *getProtocol() const { return mProtocol; };
  CSoundManager *getSoundManager() const { return mSoundManager; };
  CUserManager *getUserManager() const { return mUserManager; };
  CFileTransferManager *getFileTransferManager() const {
    return mFileTransferManager;
  };
  //</SUBSYSTEMS>

  void setUserProtocolVersionByStreamID(qint32 ID, QString Version);
  void setOnlineStatus(const ONLINESTATE newStatus);
  void setStreamTypeToKnown(qint32 ID, const QByteArray Data,
                            bool isFileTransfer_Receive = false);
  void setMyDestinationB32(QString B32Dest);

  bool useThisChatConnection(const QString Destination, const qint32 ID);

  void doNamingLookUP(QString Name) const;
  void doConvertNumberToTransferSize(quint64 inNumber, QString &outNumber,
                                     QString &outType,
                                     bool addStoOutType = true) const;

  void deletePacketManagerByID(qint32 ID);
  void createStreamObjectsForAllUsers();
  void createStreamObjectForUser(CUser &User);
  void loadUserInfos();
  QString calcSessionOptionString() const;

  QString canonicalizeTopicId(QString topicIdNonCanonicalized);

private slots:
  // <SIGNALS FROM CONNECTIONMANAGER>
  void slotStreamStatusReceived(const SAM_Message_Types::RESULT result,
                                const qint32 ID, QString Message);
  void slotNamingReplyReceived(const SAM_Message_Types::RESULT result,
                               QString Name, QString Value = "",
                               QString Message = "");
  void slotStreamControllerStatusOK(bool Status);
  void slotIncomingStream(CI2PStream *stream);
  void slotNewSamPrivKeyGenerated(const QString SamPrivKey);
  // </SIGNALS FROM CONNECTIONMANAGER>

signals:
  void signUserStatusChanged();
  void signOnlineStatusChanged();
  void signOwnAvatarImageChanged();
  void signNicknameChanged();
  void signIncomingUserAuthorizationRequest(QString destination, qint32 streamID, QByteArray data);

private:
  CConnectionManager *mConnectionManager;
  CDebugMessageManager *mDebugMessageHandler;
  CSoundManager *mSoundManager;
  CProtocol *mProtocol;
  CUserBlockManager *mUserBlockManager;
  CUserManager *mUserManager;
  CFileTransferManager *mFileTransferManager;
  CUnsentChatMessageStorage *mUnsentChatMessageStorage;

  CReceivedInfos mUserInfos;
  QString mMyDestination;
  QString mMyDestinationB32;
  QString mConfigPath;
  QList<CPacketManager *> mDataPacketsManagers;
  ONLINESTATE mCurrentOnlineStatus;
  ONLINESTATE mNextOnlineStatus;

  void init();
  void stopCore();
  void restartCore();
  void closeAllActiveConnections();
public slots:
  void changeAccessIncomingUsers(bool);

protected:
  bool m_access_anyone_incoming; // new users.
public:
  bool getAccessAnyoneIncoming() { return m_access_anyone_incoming; }
};
#endif
