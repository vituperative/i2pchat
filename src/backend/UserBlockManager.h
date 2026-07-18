// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef USERBLOCKMANAGER_H
#define USERBLOCKMANAGER_H

#include <QDate>
#include <QElapsedTimer>
#include <QFile>
#include <QMap>
#include <QMapIterator>
#include <QStringList>
#include <QTextStream>
#include <QtDebug>
#include <QtGlobal>

class CCore;
class CUserBlockManager : public QObject {
public:
  struct CUserBlockEntity {

    CUserBlockEntity(QString NickName, QString Destination)
      : mNickName(NickName)
      , mDestination(Destination)
      , mBlockDate(QDate::currentDate().toString("dd.MM.yyyy")) {};

    CUserBlockEntity(QString NickName, QString Destination, QString BlockDate)
      : mNickName(NickName)
      , mDestination(Destination)
      , mBlockDate(BlockDate) {};

    const QString mNickName;
    const QString mDestination;
    const QString mBlockDate;
  };

public:
  CUserBlockManager(CCore &Core, const QString &FilePathToBlockFile);
  ~CUserBlockManager();

  // forbid some operators
  CUserBlockManager(const CUserBlockManager &) = delete;
  CUserBlockManager &operator=(const CUserBlockManager &) = delete;

  void addNewBlockEntity(const QString &NickName, const QString &Destination, const QString &BlockDate = "");
  void removeBlockEntity(const QString &Destination, bool CreateUser = false);
  bool isDestinationInBlockList(const QString &Destination) const;

  void readBlockListe();

  const QMap<QString, CUserBlockEntity *> &getBlockList() const { return mUserBlockMap; };

private:
  CCore &mCore;
  const QString mFilePathToBlockFile;
  QMap<QString, CUserBlockEntity *> mUserBlockMap;

  void saveBlockListe();
};
#endif
