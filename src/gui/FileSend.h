// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FILESEND_H
#define FILESEND_H

#include "FileTransferSend.h"
#include "ui_FileSend.h"

#include <QCloseEvent>
#include <QDialog>
#include <QKeyEvent>

class FileSend : public QDialog, public Ui::FileSend {
  Q_OBJECT
public:
  FileSend(CFileTransferSend &FileTransfer);
  ~FileSend();

  // forbid some operators
  FileSend(const FileSend &) = delete;
  FileSend &operator=(const FileSend &) = delete;

  void getFocus();
signals:
  void closingFileSendWindow(qint32 StreamID);

private slots:
  void slot_alreadySentSizeChanged(quint64 value);
  void slot_FileTransferFinishedOK();
  void slot_FileTransferError();
  void slot_FileTransferAccepted(bool t);
  void slot_FileTransferAborted();
  void slot_Button();
  void slot_SpeedChanged(const QString &SNumber, const QString &Type);

private:
  void closeEvent(QCloseEvent *e);
  void keyPressEvent(QKeyEvent *event);
  void init();
  CFileTransferSend &FileTransfer;
};
#endif
