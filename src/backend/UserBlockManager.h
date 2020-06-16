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

#ifndef USERBLOCKMANAGER_H
#define USERBLOCKMANAGER_H

#include <QDate>
#include <QFile>
#include <QMap>
#include <QMapIterator>
#include <QStringList>
#include <QTextStream>
#include <QTime>
#include <QtDebug>
#include <QtGlobal>

class CCore;
class CUserBlockManager : public QObject {
public:
  struct CUserBlockEntity {

    CUserBlockEntity(QString NickName, QString Destination)
        : mNickName(NickName), mDestination(Destination),
          mBlockDate(QDate::currentDate().toString("dd.MM.yyyy")){};

    CUserBlockEntity(QString NickName, QString Destination, QString BlockDate)
        : mNickName(NickName), mDestination(Destination),
          mBlockDate(BlockDate){};

    const QString mNickName;
    const QString mDestination;
    const QString mBlockDate;
  };

public:
  CUserBlockManager(CCore &Core, const QString FilePathToBlockFile);
  ~CUserBlockManager();

  // forbid some operators
  CUserBlockManager(const CUserBlockManager &) = delete;
  CUserBlockManager &operator=(const CUserBlockManager &) = delete;

  void addNewBlockEntity(const QString NickName, const QString Destination,
                         QString BlockDate = "");
  void removeBlockEntity(const QString Destination, bool CreateUser = false);
  bool isDestinationInBlockList(const QString Destination) const;

  void readBlockListe();

  const QMap<QString, CUserBlockEntity *> getBlockList() const {
    return mUserBlockMap;
  };

private:
  CCore &mCore;
  const QString mFilePathToBlockFile;
  QMap<QString, CUserBlockEntity *> mUserBlockMap;

  void saveBlockListe();
};
#endif
