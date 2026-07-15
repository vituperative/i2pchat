// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef UNSENTCHATMESSAGESTORAGE
#define UNSENTCHATMESSAGESTORAGE

#include <QSettings>
#include <QString>
#include <QStringList>

class CUnsentChatMessageStorage {
public:
  CUnsentChatMessageStorage(const QString &FileNameWithPath);
  ~CUnsentChatMessageStorage();

  // forbid some operators
  CUnsentChatMessageStorage(const CUnsentChatMessageStorage &) = delete;
  CUnsentChatMessageStorage &operator=(const CUnsentChatMessageStorage &) = delete;

  QStringList getMessagesForDest(const QString &I2PDest);
  void saveChatMessagesForDest(const QString &I2PDest, const QStringList &Messages);

  QStringList getFileOffersForDest(const QString &I2PDest);
  void saveFileOffersForDest(const QString &I2PDest, const QStringList &Offers);

  void clearStorage();

private:
  const QString mFileNameWithPath;
};
#endif
