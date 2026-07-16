// SPDX-License-Identifier: AGPL-3.0-or-later

#include "User.h"

#include "ChatmessageChanger.h"
#include "Core.h"
#include "FileTransferSend.h"
#include "Protocol.h"
#include "UserManager.h"

#include <utility>

CUser::CUser(CCore &Core, CProtocol &Protocol, QString Name, const QString &I2PDestination, qint32 I2PStream_ID)
  : mCore(Core)
  , mProtocol(Protocol)
  , mI2PDestination(I2PDestination)
  , mChatMessageChanger(*(CChatMessageChanger::exemplar(Core))) {
  QSettings settings(mCore.getConfigPath() + "/application.ini", QSettings::IniFormat);

  this->mName = std::move(Name);
  this->mI2PStream_ID = I2PStream_ID;
  this->mConnectionStatus = OFFLINE;
  this->mClientName = "";
  this->mClientVersion = "";
  this->mCurrentOnlineState = USEROFFLINE;
  this->mHaveNewUnreadMessages = false;
  this->mHaveNewUnreadChatmessage = false;

  settings.beginGroup("Chat");
  this->mTextFont.fromString(settings.value("DefaultFont", "SansSerif,10").toString());
  this->mTextColor.setNamedColor(settings.value("DefaultColor", "#000").toString());
  this->mLogOnlineStateOfUsers = (settings.value("LogOnlineStatesOfUsers", true).toBool());
  settings.endGroup();
  settings.sync();
  this->mInvisible = false;
  this->mReceivedNicknameToUserNickname = false;
  mNextCancelId = 0;
  this->mProtocolVersion = "0.2";
  this->mMaxProtocolVersionFiletransfer = "0.1";
  this->mMinProtocolVersionFiletransfer = "0.1";
  this->mReceivedUserInfos.Age = 0;
  mDateAdded = QDateTime::currentDateTime();
  mLastCommunication = QDateTime::currentDateTime();
  mLastOnline = QDateTime::currentDateTime();

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
CUser::~CUser() {
  emit signUserDeleted();
}

void CUser::setName(QString newName) {
  this->mName = std::move(newName);
  mCore.getUserManager()->saveUserList();
}
void CUser::setConnectionStatus(CONNECTIONTOUSER Status) {
  if (mConnectionStatus == Status) {
    return;
  }

  mConnectionStatus = Status;
  mLastOnline = QDateTime::currentDateTime();

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

void CUser::setI2PStreamID(qint32 ID) {
  this->mI2PStream_ID = ID;
}

void CUser::setProtocolVersion(QString Version) {
  this->mProtocolVersion = std::move(Version);
}

void CUser::slotIncomingNewChatMessage(QString newMessage) {

  newMessage = mChatMessageChanger.changeChatMessage(newMessage);
  auto myMessage = QDateTime::currentDateTime().toString("hh:mm:ss") + " ‣ " + mName + ":" + newMessage + "<br>";

  // TODO fix this in OOP way
  this->mAllMessages.push_back(myMessage);
  this->mNewMessages.push_back(myMessage);

  mHaveNewUnreadMessages = true;
  mHaveNewUnreadChatmessage = true;
  mLastCommunication = QDateTime::currentDateTime();

  emit signNewMessageReceived();
  emit signNewMessageSound();
  emit signOnlineStateChanged();
}

void CUser::slotSendFileOffer(const QString &fileName, quint64 fileSize, const QString &filePath) {
  QString sizeStr;
  QString sizeType;
  mCore.doConvertNumberToTransferSize(fileSize, sizeStr, sizeType);
  QString offerStr = fileName + "\t" + QString::number(fileSize) + "\t" + filePath;

  if (mConnectionStatus == ONLINE && mCurrentOnlineState != USEROFFLINE && mCurrentOnlineState != USERINVISIBLE) {
    // Send offer via protocol
    QByteArray payload = (fileName + "\t" + QString::number(fileSize)).toUtf8();
    mProtocol.send(FILE_OFFER, mI2PStream_ID, payload);

    auto msg = QDateTime::currentDateTime().toString("hh:mm:ss") + " ‣ " +
               tr("File offer: %1 (%2 %3) <i>(sent)</i><br>").arg(fileName, sizeStr, sizeType);
    mAllMessages.push_back(msg);
    mNewMessages.push_back(msg);
  } else {
    // Queue for later
    mUnsentedFileOffers.push_back(offerStr);

    qint32 cancelId = mNextCancelId++;
    mPendingFileIdx[cancelId] = mUnsentedFileOffers.size() - 1;
    auto msg = QDateTime::currentDateTime().toString("hh:mm:ss") + " ‣ " +
               tr("File offer: %1 (%2 %3) <i>(pending)</i>"
                  "<a href=\"cancelfile:%4\">✕</a><br>")
                 .arg(fileName, sizeStr, sizeType, QString::number(cancelId));
    mAllMessages.push_back(msg);
    mNewMessages.push_back(msg);
  }

  mHaveNewUnreadMessages = true;
  mLastCommunication = QDateTime::currentDateTime();
  emit signNewMessageReceived();
  emit signSaveUnsentMessages(mI2PDestination);
}

void CUser::slotSendAllFileOffers() {
  if (mUnsentedFileOffers.isEmpty())
    return;

  for (int i = 0; i < mUnsentedFileOffers.count(); i++) {
    QStringList parts = mUnsentedFileOffers.at(i).split("\t");
    if (parts.size() >= 2) {
      QByteArray payload = (parts.at(0) + "\t" + parts.at(1)).toUtf8();
      mProtocol.send(FILE_OFFER, mI2PStream_ID, payload);
    }
  }

  // Strip (pending) and cancel link from existing entries
  QString cancelPrefix = "<i>(" + tr("pending") + ")</i><a href=\"cancelfile:";
  for (int i = 0; i < mAllMessages.size(); i++) {
    int idx = mAllMessages[i].indexOf(cancelPrefix);
    if (idx != -1)
      mAllMessages[i] = mAllMessages[i].left(idx) + "<br>";
  }

  mUnsentedFileOffers.clear();
  mPendingFileIdx.clear();
  mHaveNewUnreadMessages = true;
  emit signPendingCanceled();
  emit signSaveUnsentMessages(mI2PDestination);
}

void CUser::slotIncomingFileOffer(const QString &data) {
  // Format: "filename\tfilesize"
  QStringList parts = data.split("\t");
  if (parts.size() < 2)
    return;

  QString fileName = parts.at(0);
  quint64 fileSize = parts.at(1).toULongLong();

  QString sizeStr, sizeType;
  mCore.doConvertNumberToTransferSize(fileSize, sizeStr, sizeType);

  QString msg = QDateTime::currentDateTime().toString("hh:mm:ss") + " ‣ " +
                tr("Offers file: %1 (%2 %3) "
                   "<a href=\"fileoffer:accept:%4\">[Accept]</a> "
                   "<a href=\"fileoffer:reject:%4\">[Reject]</a><br>")
                  .arg(fileName, sizeStr, sizeType, fileName.toHtmlEscaped());

  mAllMessages.push_back(msg);
  mNewMessages.push_back(msg);

  mHaveNewUnreadMessages = true;
  mHaveNewUnreadChatmessage = true;
  mLastCommunication = QDateTime::currentDateTime();
  emit signNewMessageReceived();
  emit signNewMessageSound();
  emit signOnlineStateChanged();
}

QString CUser::takeAcceptedFileOffer(const QString &fileName) {
  for (int i = 0; i < mUnsentedFileOffers.count(); i++) {
    QStringList parts = mUnsentedFileOffers.at(i).split("\t");
    if (parts.size() >= 3 && parts.at(0) == fileName) {
      QString filePath = parts.at(2);
      mUnsentedFileOffers.removeAt(i);
      emit signSaveUnsentMessages(mI2PDestination);
      return filePath;
    }
  }
  return QString();
}

void CUser::removeFileOffer(const QString &fileName) {
  for (int i = 0; i < mUnsentedFileOffers.count(); i++) {
    QStringList parts = mUnsentedFileOffers.at(i).split("\t");
    if (parts.size() >= 1 && parts.at(0) == fileName) {
      mUnsentedFileOffers.removeAt(i);
      emit signSaveUnsentMessages(mI2PDestination);
      break;
    }
  }
}

void CUser::setUnsentedFileOffers(const QStringList &newOffers) {
  mUnsentedFileOffers = newOffers;
  mPendingFileIdx.clear();
  for (int i = 0; i < mUnsentedFileOffers.count(); i++) {
    QStringList parts = mUnsentedFileOffers.at(i).split("\t");
    if (parts.size() < 2)
      continue;
    QString fileName = parts.at(0);
    quint64 fileSize = parts.at(1).toULongLong();
    QString sizeStr, sizeType;
    mCore.doConvertNumberToTransferSize(fileSize, sizeStr, sizeType);
    qint32 cancelId = mNextCancelId++;
    mPendingFileIdx[cancelId] = i;
    auto msg = QDateTime::currentDateTime().toString("hh:mm:ss") + " ‣ " +
               tr("File offer: %1 (%2 %3) <i>(pending)</i>"
                  "<a href=\"cancelfile:%4\">✕</a><br>")
                 .arg(fileName, sizeStr, sizeType, QString::number(cancelId));
    mAllMessages.push_back(msg);
    mNewMessages.push_back(msg);
  }
  emit signSaveUnsentMessages(mI2PDestination);
}

void CUser::cancelPendingMessage(qint32 id) {
  if (!mPendingMsgIdx.contains(id))
    return;
  int idx = mPendingMsgIdx[id];
  if (idx >= 0 && idx < mUnsentedMessages.size())
    mUnsentedMessages.removeAt(idx);

  QString cancelLink = QString("cancelmsg:%1").arg(id);
  for (int i = mAllMessages.size() - 1; i >= 0; i--) {
    if (mAllMessages[i].contains(cancelLink)) {
      mAllMessages.removeAt(i);
      break;
    }
  }
  for (int i = mNewMessages.size() - 1; i >= 0; i--) {
    if (mNewMessages[i].contains(cancelLink)) {
      mNewMessages.removeAt(i);
      break;
    }
  }
  mPendingMsgIdx.remove(id);
  // Shift indices for remaining entries
  QList<qint32> keys = mPendingMsgIdx.keys();
  for (qint32 k : keys) {
    if (mPendingMsgIdx[k] > idx)
      mPendingMsgIdx[k]--;
  }
  emit signSaveUnsentMessages(mI2PDestination);
  emit signPendingCanceled();
}

void CUser::cancelPendingFileOffer(qint32 id) {
  if (!mPendingFileIdx.contains(id))
    return;
  int idx = mPendingFileIdx[id];
  if (idx >= 0 && idx < mUnsentedFileOffers.size())
    mUnsentedFileOffers.removeAt(idx);

  QString cancelLink = QString("cancelfile:%1").arg(id);
  for (int i = mAllMessages.size() - 1; i >= 0; i--) {
    if (mAllMessages[i].contains(cancelLink)) {
      mAllMessages.removeAt(i);
      break;
    }
  }
  for (int i = mNewMessages.size() - 1; i >= 0; i--) {
    if (mNewMessages[i].contains(cancelLink)) {
      mNewMessages.removeAt(i);
      break;
    }
  }
  mPendingFileIdx.remove(id);
  QList<qint32> keys = mPendingFileIdx.keys();
  for (qint32 k : keys) {
    if (mPendingFileIdx[k] > idx)
      mPendingFileIdx[k]--;
  }
  emit signSaveUnsentMessages(mI2PDestination);
  emit signPendingCanceled();
}

void CUser::slotSendChatMessage(const QString &Message) {
  using namespace PROTOCOL_TAGS;
  QString Nickname;

  if (mConnectionStatus == ONLINE && mCurrentOnlineState != USEROFFLINE && mCurrentOnlineState != USERINVISIBLE) {
    QByteArray ByteMessage = Message.toUtf8();
    mProtocol.send(CHATMESSAGE, mI2PStream_ID, ByteMessage);

    if (mCore.getUserInfos().Nickname.isEmpty() == true) {
      Nickname = tr("Me ");
    } else {
      Nickname = mCore.getUserInfos().Nickname;
    }

    auto msg = QDateTime::currentDateTime().toString("hh:mm:ss") + " ‣ " + Nickname + ":" + Message + "<br>";

    this->mAllMessages.push_back(msg);
    this->mNewMessages.push_back(msg);

    mHaveNewUnreadMessages = true;
    mLastCommunication = QDateTime::currentDateTime();
    emit signNewMessageReceived();
  } else {
    mUnsentedMessages.push_back(Message);

    QString Nickname;
    if (mCore.getUserInfos().Nickname.isEmpty())
      Nickname = tr("Me ");
    else
      Nickname = mCore.getUserInfos().Nickname;

    qint32 cancelId = mNextCancelId++;
    auto msg = QDateTime::currentDateTime().toString("hh:mm:ss") + " ‣ " + Nickname + ":" + Message + " <i>(" +
               tr("pending") + ")</i><a href=\"cancelmsg:" + QString::number(cancelId) + "\">✕</a><br>";

    this->mAllMessages.push_back(msg);
    this->mNewMessages.push_back(msg);
    mPendingMsgIdx[cancelId] = mUnsentedMessages.size() - 1;
    mHaveNewUnreadMessages = true;
    mLastCommunication = QDateTime::currentDateTime();
    emit signNewMessageReceived();
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

  // Send queued text messages
  if (!mUnsentedMessages.empty()) {
    for (int i = 0; i < mUnsentedMessages.count(); i++)
      mProtocol.send(CHATMESSAGE, mI2PStream_ID, mUnsentedMessages.at(i));
    mUnsentedMessages.clear();
    mPendingMsgIdx.clear();
    // Strip (pending) and cancel link from chat entries
    for (int i = 0; i < mAllMessages.size(); i++) {
      int idx = mAllMessages[i].indexOf(" <i>(" + tr("pending") + ")</i><a href=\"cancelmsg:");
      if (idx != -1) {
        int end = mAllMessages[i].indexOf("\">✕</a>", idx);
        if (end != -1)
          mAllMessages[i] = mAllMessages[i].left(idx) + "<br>";
      }
    }
    mNewMessages.clear();
    slotIncomingMessageFromSystem("All previously unsent messages have been sent.", true);
    emit signPendingCanceled();
  }

  // Send queued file offers
  slotSendAllFileOffers();
}

void CUser::setClientName(QString Name) {
  mClientName = std::move(Name);
}

void CUser::setClientVersion(QString Version) {
  this->mClientVersion = std::move(Version);
  if (mClientName == "I2P-Messenger (QT)" && mClientVersion == "0.2.15 Beta") {
    setMaxProtocolVersionFiletransfer("0.2");
  }
}

void CUser::setOnlineState(const ONLINESTATE newState) {
  if (mCurrentOnlineState == newState)
    return;

  if (newState != USEROFFLINE && newState != USERINVISIBLE && newState != USERBLOCKEDYOU) {
    if (mCurrentOnlineState == USEROFFLINE || mCurrentOnlineState == USERINVISIBLE ||
        mCurrentOnlineState == USERBLOCKEDYOU) {
      if (mLogOnlineStateOfUsers == true) {
        slotIncomingMessageFromSystem(tr("%1 is online").arg(mName));
      }
      emit signConnectionOnline();
    }
    this->SendAllunsendedMessages();
  } else if (newState == USEROFFLINE || newState == USERINVISIBLE || newState == USERBLOCKEDYOU) {
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

void CUser::setTextColor(QColor textColor) {
  this->mTextColor = std::move(textColor);
}

void CUser::setTextFont(QFont textFont) {
  this->mTextFont = std::move(textFont);
}

void CUser::slotIncomingMessageFromSystem(const QString &newMessage, bool indicateWithSoundAndIcon) {
  this->mAllMessages.push_back(QDateTime::currentDateTime().toString("hh:mm:ss") + tr(" ‣ [System] ") + newMessage +
                               "<br><br>");
  this->mNewMessages.push_back(QDateTime::currentDateTime().toString("hh:mm:ss") + tr(" ‣ [System] ") + newMessage +
                               "<br><br>");

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
  double tmp = mProtocolVersion.toDouble(&OK);

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

void CUser::setReceivedUserInfos(RECEIVEDINFOS Tag, const QString &value) {
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
  return QString::number(getHighestUsableProtocolVersionFiletransfer_D(), 'g', 2);
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
    dest = std::move(b64Dest);
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
  mPendingMsgIdx.clear();
  for (int i = 0; i < mUnsentedMessages.count(); i++) {
    qint32 cancelId = mNextCancelId++;
    mPendingMsgIdx[cancelId] = i;
    QString Nickname;
    if (mCore.getUserInfos().Nickname.isEmpty())
      Nickname = tr("Me ");
    else
      Nickname = mCore.getUserInfos().Nickname;
    auto msg = QDateTime::currentDateTime().toString("hh:mm:ss") + " ‣ " + Nickname + ":" + mUnsentedMessages.at(i) +
               " <i>(" + tr("pending") + ")</i><a href=\"cancelmsg:" + QString::number(cancelId) + "\">✕</a><br>";
    mAllMessages.push_back(msg);
    mNewMessages.push_back(msg);
  }
  emit signSaveUnsentMessages(mI2PDestination);
}
