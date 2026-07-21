// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef USER_H
#define USER_H

#include "I2PStream.h"

#include <QByteArray>
#include <QDateTime>
#include <QElapsedTimer>
#include <QMap>
#include <QStringList>
#include <QtGlobal>
#include <QtGui>

namespace User {
enum CONNECTIONTOUSER { OFFLINE, ONLINE, TRYTOCONNECT, CONNECTERROR };

enum ONLINESTATE {
  USERONLINE,
  USEROFFLINE,
  USERINVISIBLE,
  USERWANTTOCHAT,
  USERAWAY,
  USERDONT_DISTURB,
  USERTRYTOCONNECT,
  USERBLOCKEDYOU
};

enum RECEIVEDINFOS { NICKNAME, GENDER, AGE, INTERESTS };

struct CReceivedInfos {
  QString Nickname;
  QString Gender;
  qint32 Age;
  QString Interests;
  QByteArray AvatarImage;
};

} // namespace User

using namespace User;
class CCore;
class CProtocol;
class CChatMessageChanger;
class CUser : public QObject {
  Q_OBJECT
public:
  CUser(CCore &Core, CProtocol &Protocol, QString Name, const QString &I2PDestination, qint32 I2PStream_ID);
  ~CUser();

  // forbid some operators
  CUser(const CUser &) = delete;
  CUser &operator=(const CUser &) = delete;

  const QString getName() const { return mName; }
  const QString getI2PDestination() const { return mI2PDestination; }
  qint32 getI2PStreamID() const { return mI2PStream_ID; }
  const QString getProtocolVersion() const { return mProtocolVersion; }
  double getProtocolVersion_D() const;
  const QString getClientName() const { return mClientName; }
  const QString getClientVersion() const { return mClientVersion; }
  const QDateTime getDateAdded() const { return mDateAdded; }
  const QDateTime getLastCommunication() const { return mLastCommunication; }
  const QDateTime getLastOnline() const { return mLastOnline; }
  QColor getTextColor() const { return mTextColor; }
  QFont getTextFont() const { return mTextFont; }
  CONNECTIONTOUSER getConnectionStatus() const { return mConnectionStatus; }
  ONLINESTATE getOnlineState() const { return mCurrentOnlineState; }
  const QStringList &getAllChatMessages();
  const QStringList getNewMessages(bool haveFocus);
  const CReceivedInfos getReceivedUserInfos() const { return mReceivedUserInfos; }

  const QString getHighestUsableProtocolVersionFiletransfer() const;
  double getHighestUsableProtocolVersionFiletransfer_D() const;
  const QString getMaxProtocolVersionFiletransfer() const { return mMaxProtocolVersionFiletransfer; };
  double getMaxProtocolVersionFiletransfer_D() const;
  const QString getMinProtocolVersionFiletransfer() const { return mMinProtocolVersionFiletransfer; };
  double getMinProtocolVersionFiletransfer_D() const;
  bool getHaveNewUnreadMessages() const { return mHaveNewUnreadMessages; }
  bool getHaveNewUnreadChatmessages() const { return mHaveNewUnreadChatmessage; }
  bool getIsInvisible() const { return mInvisible; };
  bool getUsedB32Dest() const { return mUseB32Dest; };
  bool getAutoDownloadEnabled() const { return mAutoDownloadEnabled; };
  const QString getOriginalB32Address() const { return mOriginalB32Address; };
  const QStringList getUnsentedMessages() const { return mUnsentedMessages; };
  const QStringList getUnsentedFileOffers() const { return mUnsentedFileOffers; };

  void setConnectionStatus(CONNECTIONTOUSER Status);
  void setOnlineState(const ONLINESTATE newState);
  void setName(QString newName);
  void setI2PStreamID(qint32 ID);
  void setProtocolVersion(QString Version);
  void setClientName(QString Name);
  void setClientVersion(QString Version);
  void setTextColor(QColor textColor);
  void setTextFont(QFont textFont);
  void setInvisible(bool b);
  void setAutoDownloadEnabled(bool enabled);
  void setMaxProtocolVersionFiletransfer(QString Version) { mMaxProtocolVersionFiletransfer = Version; };
  void setMinProtocolVersionFiletransfer(QString Version) { mMinProtocolVersionFiletransfer = Version; };
  void setReceivedUserInfos(RECEIVEDINFOS Tag, const QString &value);
  void setReceivedNicknameToUserNickname();
  void setReplaceB32WithB64(QString b64Dest);
  void setAvatarImage(QByteArray &avatarImage);
  void setUnsentedMessages(QStringList &newMessages);
  void setUnsentedFileOffers(const QStringList &newOffers);
  QString takeAcceptedFileOffer(const QString &fileName);
  void removeFileOffer(const QString &fileName);
  void cancelSentFileOffer(const QString &fileName);
  void setDateAdded(const QDateTime &dt) { mDateAdded = dt; }
  void setLastCommunication(const QDateTime &dt) { mLastCommunication = dt; }
  void setLastOnline(const QDateTime &dt) { mLastOnline = dt; }

public slots:
  void slotSendChatMessage(const QString &Message);
  void slotSendFileOffer(const QString &fileName, quint64 fileSize, const QString &filePath);
  void slotSendAllFileOffers();
  void slotIncomingFileOffer(const QString &data);
  void slotIncomingNewChatMessage(QString newMessage);
  void slotIncomingMessageFromSystem(const QString &newMessage, bool indicateWithSoundAndIcon = false);
  void cancelPendingMessage(qint32 id);
  void cancelPendingFileOffer(qint32 id);

signals:
  void signOnlineStateChanged();
  void signNewMessageReceived();
  void signNewMessageSound();
  void signConnectionOnline();
  void signConnectionOffline();
  void signUserDeleted();
  void signNewAvatarImage();
  void signSaveUnsentMessages(QString I2PDest);
  void signPendingCanceled();

private:
  CCore &mCore;
  CProtocol &mProtocol;
  QString mName;
  const QString mI2PDestination;
  qint32 mI2PStream_ID;

  bool mInvisible;
  bool mReceivedNicknameToUserNickname;
  bool mHaveNewUnreadMessages;
  bool mHaveNewUnreadChatmessage;
  bool mLogOnlineStateOfUsers;
  CONNECTIONTOUSER mConnectionStatus;
  ONLINESTATE mCurrentOnlineState;
  QString mProtocolVersion;
  QString mMaxProtocolVersionFiletransfer;
  QString mMinProtocolVersionFiletransfer;
  QString mClientName;
  QString mClientVersion;
  QStringList mAllMessages;
  QStringList mNewMessages;
  QStringList mUnsentedMessages;
  /* Cancel mechanism: mNextCancelId increments for each cancelP* call.
     mPendingMsgIdx / mPendingFileIdx map cancelId → index into mAllMessages
     so the caller can identify which pending msg/file-offer was cancelled. */
  qint32 mNextCancelId;
  QMap<qint32, int> mPendingMsgIdx;
  QMap<qint32, int> mPendingFileIdx;

  CReceivedInfos mReceivedUserInfos;
  bool mUseB32Dest;
  bool mAutoDownloadEnabled;
  QString mOriginalB32Address;
  QDateTime mDateAdded;
  QDateTime mLastCommunication;
  QDateTime mLastOnline;

  //<Settings for the chatwindow>
  QColor mTextColor;
  QFont mTextFont;
  CChatMessageChanger &mChatMessageChanger;
  //</Settings for the chatwindow>
  void SendAllunsendedMessages();
  void
  removePendingByCancelId(qint32 id, const QString &linkPrefix, QMap<qint32, int> &idxMap, QStringList &unsentList);

  QStringList mUnsentedFileOffers;
  QStringList mSentOfferStrs;
};
#endif
