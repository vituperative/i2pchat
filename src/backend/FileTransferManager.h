// SPDX-License-Identifier: AGPL-3.0-or-later

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

  void addNewFileTransfer(const QString &FilePath, const QString &Destination);
  void addNewFileReceive(qint32 ID,
                         QString FileName,
                         const QString &FileSize,
                         const QString &Destination,
                         const QString &ProtocolVersion);

  void removeFileTransfer(const qint32 ID) /*noexcept*/;
  void removeFileReceive(const qint32 ID);

  CFileTransferReceive *getFileReceiveByID(qint32 ID) const;
  CFileTransferSend *getFileSendByID(qint32 ID) const;
  const QList<CFileTransferReceive *> getFileTransferReceiveList() const;
  const QList<CFileTransferSend *> getFileTransferSendsList() const;
  // template <typename T, typename where, typename what, typename fun>
  //	 T* getFileTransferBy(where wh, what wat,fun f);
  bool isThisID_a_FileSendID(qint32 ID) const;
  bool isThisID_a_FileReceiveID(qint32 ID) const;

  bool checkActiveFileTransfer() const;

signals:
  void signUserStatusChanged();

private:
  CCore &mCore;
  QList<CFileTransferSend *> mFileSends;
  QList<CFileTransferReceive *> mFileReceives;

  const QString FilterForFileName(QString FileName) const;
};
#endif // FILETRANSFERMANAGER_H
