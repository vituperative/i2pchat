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
#ifndef USER_H
#define USER_H

#include <QByteArray>
#include <QStringList>
#include <QElapsedTimer>
#include <QtGlobal>
#include <QtGui>
#include <QDateTime>

#include "I2PStream.h"

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
class CCMessageAckManager;
class CUser : public QObject {
  Q_OBJECT
public:
  CUser(CCore &Core, CProtocol &Protocol, QString Name, QString I2PDestination,
        qint32 I2PStream_ID);
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
  QColor getTextColor() const { return mTextColor; }
  QFont getTextFont() const { return mTextFont; }
  CONNECTIONTOUSER getConnectionStatus() const { return mConnectionStatus; }
  ONLINESTATE getOnlineState() const { return mCurrentOnlineState; }
  const QStringList &getAllChatMessages();
  const QStringList getNewMessages(bool haveFocus);
  const CReceivedInfos getReceivedUserInfos() const {
    return mReceivedUserInfos;
  }

  const QString getHighestUsableProtocolVersionFiletransfer() const;
  double getHighestUsableProtocolVersionFiletransfer_D() const;
  const QString getMaxProtocolVersionFiletransfer() const {
    return mMaxProtocolVersionFiletransfer;
  };
  double getMaxProtocolVersionFiletransfer_D() const;
  const QString getMinProtocolVersionFiletransfer() const {
    return mMinProtocolVersionFiletransfer;
  };
  double getMinProtocolVersionFiletransfer_D() const;
  bool getHaveNewUnreadMessages() const { return mHaveNewUnreadMessages; }
  bool getHaveNewUnreadChatmessages() const {
    return mHaveNewUnreadChatmessage;
  }
  bool getIsInvisible() const { return mInvisible; };
  bool getUsedB32Dest() const { return mUseB32Dest; };
  const QStringList getUnsentedMessages() const { return mUnsentedMessages; };

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
  void setMaxProtocolVersionFiletransfer(QString Version) {
    mMaxProtocolVersionFiletransfer = Version;
  };
  void setMinProtocolVersionFiletransfer(QString Version) {
    mMinProtocolVersionFiletransfer = Version;
  };
  void setReceivedUserInfos(RECEIVEDINFOS Tag, QString value);
  void setReceivedNicknameToUserNickname();
  void setReplaceB32WithB64(QString b64Dest);
  void setAvatarImage(QByteArray &avatarImage);
  void setUnsentedMessages(QStringList &newMessages);

public slots:
  void slotSendChatMessage(QString Message);
  void slotIncomingNewChatMessage(QString newMessage);
  void slotIncomingMessageFromSystem(QString newMessage,
                                     bool indicateWithSoundAndIcon = false);

signals:
  void signOnlineStateChanged();
  void signNewMessageReceived();
  void signNewMessageSound();
  void signConnectionOnline();
  void signConnectionOffline();
  void signUserDeleted();
  void signNewAvatarImage();
  void signSaveUnsentMessages(QString I2PDest);

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

  CReceivedInfos mReceivedUserInfos;
  bool mUseB32Dest;

  //<Settings for the chatwindow>
  QColor mTextColor;
  QFont mTextFont;
  CChatMessageChanger &mChatMessageChanger;
  //</Settings for the chatwindow>
  void SendAllunsendedMessages();
};
#endif
