// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FILERECEIVE_H
#define FILERECEIVE_H

#include "ui_FileReceive.h"

#include <QCloseEvent>
#include <QDialog>
#include <QKeyEvent>

class CFileTransferReceive;
class FileReceive : public QDialog, public Ui::FileReceive {
  Q_OBJECT
public:
  FileReceive(CFileTransferReceive &transfer);
  ~FileReceive();

  // forbid some operators
  FileReceive(const FileReceive &) = delete;
  FileReceive &operator=(const FileReceive &) = delete;

  void getFocus();
  void start();

signals:
  void closingFileReceiveWindow(qint32 StreamID);

private slots:
  void slot_Button();
  void slot_allreadyReceivedSizeChanged(quint64 value);
  void slot_FileReceivedFinishedOK();
  void slot_FileReceiveError();
  void slot_SpeedChanged(const QString &SNumber, const QString &Type);

private:
  CFileTransferReceive &mFileTransfer;
  const qint32 mStreamID;

  void closeEvent(QCloseEvent *e);
  void keyPressEvent(QKeyEvent *event);
  void init();
  void askTheUser();
};
#endif
