// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef CORE_H
#define CORE_H

#include "DebugMessageManager.h"
#include "FileTransferManager.h"
#include "I2PSamMessageAnalyser.h"
#include "Protocol.h"
#include "SoundManager.h"
#include "UnsentChatMessageStorage.h"
#include "User.h"
#include "UserBlockManager.h"

#include <QIODevice>
#include <QList>
#include <QSettings>
#include <QTextStream>
#include <QTimer>
#include <QtGui>

#define CLIENTVERSION "0.2.43"
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
  CCore(const QString &configPath);
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
  QString getProtocolVersion() const { return mProtocol->getProtocolVersion(); };
  CI2PStream *getI2PStreamObjectByID(qint32 ID) const;
  const CReceivedInfos getUserInfos() const;
  QString getConnectionDump() const;
  const QString getConfigPath() const { return mConfigPath; };

  //<SUBSYSTEMS>
  CDebugMessageManager *getDebugMessageHandler() const { return mDebugMessageHandler; };
  CConnectionManager *getConnectionManager() const { return mConnectionManager; };
  CUserBlockManager *getUserBlockManager() const { return mUserBlockManager; };
  CProtocol *getProtocol() const { return mProtocol; };
  CSoundManager *getSoundManager() const { return mSoundManager; };
  CUserManager *getUserManager() const { return mUserManager; };
  CFileTransferManager *getFileTransferManager() const { return mFileTransferManager; };
  //</SUBSYSTEMS>

  void setUserProtocolVersionByStreamID(qint32 ID, QString Version);
  void setOnlineStatus(const ONLINESTATE newStatus);
  void setStreamTypeToKnown(qint32 ID, const QByteArray &Data, bool isFileTransfer_Receive = false);
  void setMyDestinationB32(const QString &B32Dest);

  bool useThisChatConnection(const QString &Destination, const qint32 ID);

  void doNamingLookUP(const QString &Name) const;
  void resetAutoAway();
  void applyAutoAwaySettings(int minutes);
  void doConvertNumberToTransferSize(quint64 inNumber,
                                     QString &outNumber,
                                     QString &outType,
                                     bool addStoOutType = true) const;
  static QString formatETA(quint64 secLeft);

  void deletePacketManagerByID(qint32 ID);
  void createStreamObjectsForAllUsers();
  void createStreamObjectForUser(CUser &User);
  void loadUserInfos();
  QString calcSessionOptionString() const;

  QString canonicalizeTopicId(QString topicIdNonCanonicalized);

private slots:
  // <SIGNALS FROM CONNECTIONMANAGER>
  void slotStreamStatusReceived(const SAM_Message_Types::RESULT result, const qint32 ID, const QString &Message);
  void slotNamingReplyReceived(const SAM_Message_Types::RESULT result,
                               const QString &Name,
                               const QString &Value = "",
                               const QString &Message = "");
  void slotStreamControllerStatusOK(bool Status);
  void slotReconnectAttempt();
  void slotIncomingStream(CI2PStream *stream);
  void slotNewSamPrivKeyGenerated(const QString &SamPrivKey);
  // </SIGNALS FROM CONNECTIONMANAGER>

signals:
  void signUserStatusChanged();
  void signOnlineStatusChanged();
  void signOwnAvatarImageChanged();
  void signNicknameChanged();
  void signIncomingUserAuthorizationRequest(QString destination, qint32 streamID, QByteArray data);
  void signChatStyleChanged();

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
  QTimer mKeepAliveTimer;
  QTimer mAutoAwayTimer;
  int mAutoAwayMinutes;

  void init();
  void stopCore();
  void restartCore();
  void closeAllActiveConnections();
public slots:
  void changeAccessIncomingUsers(bool);

private slots:
  void slotPingClients();
  void slotAutoAwayTimeout();

protected:
  bool m_access_anyone_incoming; // new users.
public:
  bool getAccessAnyoneIncoming() { return m_access_anyone_incoming; }

  static QImage scaleImageLanczos(const QImage &src, int maxWidth, int maxHeight = 0);
};
#endif
