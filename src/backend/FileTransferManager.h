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

#ifndef FILETRANSFERMANAGER_H
#define FILETRANSFERMANAGER_H

#include "Core.h"
#include "FileTransferReceive.h"
#include "FileTransferSend.h"
#include <QObject>

class CFileTransferManager : public QObject {

  Q_OBJECT
public:
  CFileTransferManager(CCore &Core);
  ~CFileTransferManager();

  // forbid some operators
  CFileTransferManager(const CFileTransferManager &) = delete;
  CFileTransferManager &operator=(const CFileTransferManager &) = delete;

  void addNewFileTransfer(QString FilePath, QString Destination);
  void addNewFileReceive(qint32 ID, QString FileName, QString FileSize,
                        QString Destination, QString ProtocolVersion);

  void removeFileTransfer(const qint32 ID) /*noexcept*/;
  void removeFileReceive(const qint32 ID);

  CFileTransferReceive *getFileTransferReceiveByID(qint32 ID) const;
  CFileTransferSend *getFileTransferSendsByID(qint32 ID) const;
  const QList<CFileTransferReceive *> getFileTransferReceiveList() const;
  const QList<CFileTransferSend *> getFileTransferSendsList() const;
  // template <typename T, typename where, typename what, typename fun>
  //	 T* getFileTransferBy(where wh, what wat,fun f);
  bool isThisID_a_FileSendID(qint32 ID) const;
  bool isThisID_a_FileReceiveID(qint32 ID) const;

  bool checkIfAFileTransferOrReceiveisActive() const;

signals:
  void signUserStatusChanged();

private:
  CCore &mCore;
  QList<CFileTransferSend *> mFileSends;
  QList<CFileTransferReceive *> mFileReceives;

  const QString FilterForFileName(QString FileName) const;
};
#endif // FILETRANSFERMANAGER_H
