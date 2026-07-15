#include "UnsentChatMessageStorage.h"

CUnsentChatMessageStorage::CUnsentChatMessageStorage(const QString &FileNameWithPath)
  : mFileNameWithPath(FileNameWithPath) {}

CUnsentChatMessageStorage::~CUnsentChatMessageStorage() {}

QStringList CUnsentChatMessageStorage::getMessagesForDest(const QString &I2PDest) {
  QSettings settings(mFileNameWithPath, QSettings::IniFormat);

  QStringList message;

  int size = settings.beginReadArray("Messages");
  for (int i = 0; i < size; i++) {
    settings.setArrayIndex(i);
    QString currentMessage = settings.value(I2PDest).toString();
    message.append(currentMessage);
  }
  settings.endArray();

  return message;
}

void CUnsentChatMessageStorage::saveChatMessagesForDest(const QString &I2PDest, const QStringList &Messages) {

  if (Messages.isEmpty() == true)
    return;

  QSettings settings(mFileNameWithPath, QSettings::IniFormat);

  while (settings.contains(I2PDest) == true) {
    // remove all old messages for this dest
    settings.remove(I2PDest);
  }

  settings.beginWriteArray("Messages");
  for (int i = 0; i < Messages.count(); i++) {
    settings.setArrayIndex(i);
    settings.setValue(I2PDest, Messages.at(i));
  }
  settings.endArray();
}

QStringList CUnsentChatMessageStorage::getFileOffersForDest(const QString &I2PDest) {
  QSettings settings(mFileNameWithPath, QSettings::IniFormat);

  QStringList offers;
  int size = settings.beginReadArray("FileOffers");
  for (int i = 0; i < size; i++) {
    settings.setArrayIndex(i);
    QString currentOffer = settings.value(I2PDest).toString();
    if (!currentOffer.isEmpty())
      offers.append(currentOffer);
  }
  settings.endArray();

  return offers;
}

void CUnsentChatMessageStorage::saveFileOffersForDest(const QString &I2PDest, const QStringList &Offers) {
  if (Offers.isEmpty())
    return;

  QSettings settings(mFileNameWithPath, QSettings::IniFormat);

  settings.beginWriteArray("FileOffers");
  for (int i = 0; i < Offers.count(); i++) {
    settings.setArrayIndex(i);
    settings.setValue(I2PDest, Offers.at(i));
  }
  settings.endArray();
}

void CUnsentChatMessageStorage::clearStorage() {
  QSettings settings(mFileNameWithPath, QSettings::IniFormat);
  settings.clear();
}
