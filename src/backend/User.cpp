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
#include "User.h"
#include "ChatmessageChanger.h"
#include "Core.h"
#include "FileTransferSend.h"
#include "Protocol.h"
#include "UserManager.h"

CUser::CUser(CCore &Core, CProtocol &Protocol, QString Name,
             QString I2PDestination, qint32 I2PStream_ID)
    : mCore(Core), mProtocol(Protocol), mI2PDestination(I2PDestination),
      mChatMessageChanger(*(CChatMessageChanger::exemplar(Core))) {
  QSettings settings(mCore.getConfigPath() + "/application.ini",
                     QSettings::IniFormat);

  this->mName = Name;
  this->mI2PStream_ID = I2PStream_ID;
  this->mConnectionStatus = OFFLINE;
  this->mClientName = "";
  this->mClientVersion = "";
  this->mCurrentOnlineState = USEROFFLINE;
  this->mHaveNewUnreadMessages = false;
  this->mHaveNewUnreadChatmessage = false;

  settings.beginGroup("Chat");
  this->mTextFont.fromString(
      settings.value("DefaultFont", "SansSerif,10").toString());
  this->mTextColor.setNamedColor(
      settings.value("DefaultColor", "#000").toString());
  this->mLogOnlineStateOfUsers =
      (settings.value("LogOnlineStatesOfUsers", true).toBool());
  settings.endGroup();
  settings.sync();
  this->mInvisible = false;
  this->mReceivedNicknameToUserNickname = false;
  this->mProtocolVersion = "0.2";
  this->mMaxProtocolVersionFiletransfer = "0.1";
  this->mMinProtocolVersionFiletransfer = "0.1";
  this->mReceivedUserInfos.Age = 0;

  if (mI2PDestination.length() == 60) {
    mUseB32Dest = true;
    mOriginalB32Address = mI2PDestination; // Store original B32 address
  } else {
    mUseB32Dest = false;
    mOriginalB32Address = ""; // No original B32 address
  }
  
  // Initialize per-user auto-download setting
  settings.beginGroup("UserSettings");
  settings.beginGroup(I2PDestination); // Use destination as unique identifier
  mAutoDownloadEnabled = settings.value("AutoDownload", false).toBool();
  settings.endGroup();
  settings.endGroup();
}
CUser::~CUser() { emit signUserDeleted(); }

void CUser::setName(QString newName) {
  this->mName = newName;
  mCore.getUserManager()->saveUserList();
}
void CUser::setConnectionStatus(CONNECTIONTOUSER Status) {
  if (mConnectionStatus == Status) {
    return;
  }

  mConnectionStatus = Status;

  if (Status == ONLINE) {
    if (getUsedB32Dest() == true) {
      mCore.doNamingLookUP(mI2PDestination);
    }
    // get some Infos from the CHATSYSTEM - client
    mProtocol.send(GET_CLIENTNAME, mI2PStream_ID);
    mProtocol.send(GET_CLIENTVERSION, mI2PStream_ID);
    mProtocol.send(GET_USER_ONLINESTATUS, mI2PStream_ID);

    if (getProtocolVersion_D() >= 0.3) {
      mProtocol.send(GET_MAX_PROTOCOLVERSION_FILETRANSFER, mI2PStream_ID);
      mProtocol.send(GET_USER_INFOS, mI2PStream_ID);
    }
    if (getProtocolVersion_D() >= 0.4) {
      mProtocol.send(GET_MIN_PROTOCOLVERSION_FILETRANSFER, mI2PStream_ID);
    }
    if (getProtocolVersion_D() >= 0.5) {
      if (mReceivedUserInfos.AvatarImage.isEmpty() == true) {
        mProtocol.send(GET_AVATARIMAGE, mI2PStream_ID);
      }
    }
  }

  if (Status == OFFLINE || Status == CONNECTERROR) {
    mI2PStream_ID = 0;

    if (mCurrentOnlineState != USERBLOCKEDYOU) {
      setOnlineState(USEROFFLINE);
    }
  }
  emit signOnlineStateChanged();
}

void CUser::setI2PStreamID(qint32 ID) { this->mI2PStream_ID = ID; }

void CUser::setProtocolVersion(QString Version) {
  this->mProtocolVersion = Version;
}

void CUser::slotIncomingNewChatMessage(QString newMessage) {

  newMessage = mChatMessageChanger.changeChatMessage(newMessage);
  auto myMessage = QDateTime::currentDateTime().toString("hh:mm:ss") + " ‣ " + mName +
                   ":" + newMessage + "<br>";

  // TODO fix this in OOP way
  this->mAllMessages.push_back(myMessage);
  this->mNewMessages.push_back(myMessage);

  mHaveNewUnreadMessages = true;
  mHaveNewUnreadChatmessage = true;

  emit signNewMessageReceived();
  emit signNewMessageSound();
  emit signOnlineStateChanged();
}

void CUser::slotSendChatMessage(QString Message) {
  using namespace PROTOCOL_TAGS;
  QString Nickname;

  if (mConnectionStatus == ONLINE && mCurrentOnlineState != USEROFFLINE &&
      mCurrentOnlineState != USERINVISIBLE) {
    QByteArray ByteMessage = Message.toUtf8();
    mProtocol.send(CHATMESSAGE, mI2PStream_ID, ByteMessage);

    if (mCore.getUserInfos().Nickname.isEmpty() == true) {
      Nickname = tr("Me ");
    } else {
      Nickname = mCore.getUserInfos().Nickname;
    }

    auto msg = QDateTime::currentDateTime().toString("hh:mm:ss") + " ‣ " + Nickname +
               ":" + Message + "<br>";

    this->mAllMessages.push_back(msg);
    this->mNewMessages.push_back(msg);

    mHaveNewUnreadMessages = true;
    emit signNewMessageReceived();
  } else {
    mUnsentedMessages.push_back(Message + "<br>");
    slotIncomingMessageFromSystem(
        tr("Sending the message when the user comes online.<br>If you close "
           "the client, the message will be lost."));
  }
}

const QStringList &CUser::getAllChatMessages() {
  mNewMessages.clear();
  mHaveNewUnreadMessages = false;
  mHaveNewUnreadChatmessage = false;

  emit signOnlineStateChanged();
  return mAllMessages;
}
void CUser::SendAllunsendedMessages() {
  using namespace PROTOCOL_TAGS;
  if (mUnsentedMessages.empty())
    return;

  for (int i = 0; i < mUnsentedMessages.count(); i++)
    mProtocol.send(CHATMESSAGE, mI2PStream_ID, mUnsentedMessages.at(i));

  mUnsentedMessages.clear();
  slotIncomingMessageFromSystem(
      "All previously unsent messages have been sent.", true);
}

void CUser::setClientName(QString Name) { mClientName = Name; }

void CUser::setClientVersion(QString Version) {
  this->mClientVersion = Version;
  if (mClientName == "I2P-Messenger (QT)" && mClientVersion == "0.2.15 Beta") {
    setMaxProtocolVersionFiletransfer("0.2");
  }
}

void CUser::setOnlineState(const ONLINESTATE newState) {
  if (mCurrentOnlineState == newState)
    return;

  if (newState != USEROFFLINE && newState != USERINVISIBLE &&
      newState != USERBLOCKEDYOU) {
    if (mCurrentOnlineState == USEROFFLINE ||
        mCurrentOnlineState == USERINVISIBLE ||
        mCurrentOnlineState == USERBLOCKEDYOU) {
      if (mLogOnlineStateOfUsers == true) {
        slotIncomingMessageFromSystem(tr("%1 is online").arg(mName));
      }
      emit signConnectionOnline();
    }
    this->SendAllunsendedMessages();
  } else if (newState == USEROFFLINE || newState == USERINVISIBLE ||
             newState == USERBLOCKEDYOU) {
    if (newState != mCurrentOnlineState) {
      if (mLogOnlineStateOfUsers == true) {
        slotIncomingMessageFromSystem(tr("%1 is offline").arg(mName));
      }
      emit signConnectionOffline();
    }
  }

  this->mCurrentOnlineState = newState;
  emit signOnlineStateChanged();
}

void CUser::setTextColor(QColor textColor) { this->mTextColor = textColor; }

void CUser::setTextFont(QFont textFont) { this->mTextFont = textFont; }

void CUser::slotIncomingMessageFromSystem(QString newMessage,
                                          bool indicateWithSoundAndIcon) {
  this->mAllMessages.push_back(QDateTime::currentDateTime().toString("hh:mm:ss") +
                               tr(" ‣ [System] ") + newMessage + "<br><br>");
  this->mNewMessages.push_back(QDateTime::currentDateTime().toString("hh:mm:ss") +
                               tr(" ‣ [System] ") + newMessage + "<br><br>");

  mHaveNewUnreadMessages = true;

  emit signNewMessageReceived();

  if (indicateWithSoundAndIcon == true) {
    emit signNewMessageSound();
    mHaveNewUnreadChatmessage = true;
    emit signNewMessageReceived();
  }

  emit signOnlineStateChanged();
}

void CUser::setInvisible(bool b) {
  mInvisible = b;
}

void CUser::setAutoDownloadEnabled(bool enabled) {
  mAutoDownloadEnabled = enabled;
  
  // Save the setting
  QSettings settings(mCore.getConfigPath() + "/application.ini", QSettings::IniFormat);
  settings.beginGroup("UserSettings");
  settings.beginGroup(mI2PDestination);
  settings.setValue("AutoDownload", enabled);
  settings.endGroup();
  settings.endGroup();
  settings.sync();
}

const QStringList CUser::getNewMessages(bool haveFocus) {
  QStringList tmp(mNewMessages);
  mNewMessages.clear();

  if (haveFocus == true) {
    mHaveNewUnreadMessages = false;
    mHaveNewUnreadChatmessage = false;
    emit signOnlineStateChanged();
  }

  return tmp;
}

double CUser::getProtocolVersion_D() const {
  bool OK = false;
  bool tmp = mProtocolVersion.toDouble(&OK);

  if (OK == false) {
    qCritical() << "File\t" << __FILE__ << Qt::endl
                << "Line:\t" << __LINE__ << Qt::endl
                << "Function:\t"
                << "CUser::getProtocolVersion_D" << Qt::endl
                << "Message:\t"
                << "Can't convert QString to double" << Qt::endl
                << "QString:\t" << mProtocolVersion << Qt::endl;
  }
  return tmp;
}

double CUser::getMaxProtocolVersionFiletransfer_D() const {
  bool OK = false;
  double tmp = mMaxProtocolVersionFiletransfer.toDouble(&OK);

  if (OK == false) {
    qCritical() << "File\t" << __FILE__ << Qt::endl
                << "Line:\t" << __LINE__ << Qt::endl
                << "Function:\t"
                << "CUser::getMaxProtocolVersionFiletransfer_D" << Qt::endl
                << "Message\t"
                << "Can't convert QString to double" << Qt::endl
                << "QString:\t" << mMaxProtocolVersionFiletransfer << Qt::endl;
  }
  return tmp;
}

double CUser::getMinProtocolVersionFiletransfer_D() const {
  bool OK = false;
  double tmp = mMinProtocolVersionFiletransfer.toDouble(&OK);

  if (OK == false) {
    qCritical() << "File\t" << __FILE__ << Qt::endl
                << "Line:\t" << __LINE__ << Qt::endl
                << "Function:\t"
                << "CUser::getMinProtocolVersionFiletransfer_D" << Qt::endl
                << "Message:\t"
                << "Can't convert QString to double" << Qt::endl
                << "QString:\t" << mMinProtocolVersionFiletransfer << Qt::endl;
  }
  return tmp;
}

void CUser::setReceivedUserInfos(RECEIVEDINFOS Tag, QString value) {
  switch (Tag) {
  case NICKNAME: {
    mReceivedUserInfos.Nickname = value;
    if (mReceivedNicknameToUserNickname == true) {
      if (value.isEmpty() == true) {
        setName(tr("No Nickname"));
      } else {
        setName(value);
      }
      mReceivedNicknameToUserNickname = false;
      emit signOnlineStateChanged();
    }
    break;
  }
  case GENDER: {
    mReceivedUserInfos.Gender = value;
    break;
  }
  case AGE: {
    bool OK = false;
    mReceivedUserInfos.Age = value.toInt(&OK);
    if (OK == false) {
      qCritical() << "File\t" << __FILE__ << Qt::endl
                  << "Line:\t" << __LINE__ << Qt::endl
                  << "Function:\t"
                  << "Can't convert QString to qint32" << Qt::endl
                  << "QString:\t" << value << Qt::endl;
    }
    break;
  }
  case INTERESTS: {
    mReceivedUserInfos.Interests = value;
    break;
  }
  default: {
    qWarning() << "File\t" << __FILE__ << Qt::endl
               << "Line:\t" << __LINE__ << Qt::endl
               << "Function:\t"
               << "CUser::setReceivedUserInfos" << Qt::endl
               << "Message:\t"
               << "unknown Tag" << Qt::endl;
    break;
  }
  }
}

void CUser::setReceivedNicknameToUserNickname() {
  if (getProtocolVersion_D() >= 0.3) {
    mReceivedNicknameToUserNickname = true;
  } else {
    qWarning() << "File\t" << __FILE__ << Qt::endl
               << "Line:\t" << __LINE__ << Qt::endl
               << "Function:\t"
               << "setReceivedNicknameToUserNickname" << Qt::endl
               << "Message:\t"
               << "Protocolversion <0.3, action ignored" << Qt::endl;
  }
}

const QString CUser::getHighestUsableProtocolVersionFiletransfer() const {
  return QString::number(getHighestUsableProtocolVersionFiletransfer_D(), 'g',
                         2);
}

double CUser::getHighestUsableProtocolVersionFiletransfer_D() const {
  double maxVersion = getMaxProtocolVersionFiletransfer_D();

  while (maxVersion > FileTransferProtocol::MAXPROTOCOLVERSION_D) {
    maxVersion -= 0.1;
  }
  return maxVersion;
}
void CUser::setReplaceB32WithB64(QString b64Dest) {
  if (mUseB32Dest == true) {
    QString &dest = const_cast<QString &>(mI2PDestination);
    dest = b64Dest;
  } else {
    qCritical() << "File\t" << __FILE__ << Qt::endl
                << "Line:\t" << __LINE__ << Qt::endl
                << "Function:\t"
                << "CUser::setReplaceB32WithB64" << Qt::endl
                << "Message:\t"
                << "Current Destination is not a b32 dest" << Qt::endl;
  }
}

void CUser::setAvatarImage(QByteArray &avatarImage) {
  mReceivedUserInfos.AvatarImage.clear();

  QPixmap tmpPixmap;
  tmpPixmap.loadFromData(avatarImage);
  tmpPixmap = tmpPixmap.scaled(90, 90, Qt::KeepAspectRatio);

  QBuffer buffer(&mReceivedUserInfos.AvatarImage);
  buffer.open(QIODevice::WriteOnly);
  tmpPixmap.save(&buffer, "PNG");

  emit signNewAvatarImage();
}

void CUser::setUnsentedMessages(QStringList &newMessages) {
  mUnsentedMessages = newMessages;
  emit signSaveUnsentMessages(mI2PDestination);
}
