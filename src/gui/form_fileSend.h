// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FORM_FILESEND_H
#define FORM_FILESEND_H

#include "FileTransferSend.h"
#include "ui_form_fileSend.h"

#include <QtGlobal>
#include <QtGui>

class form_fileSend : public QDialog, public Ui::form_FileSend {
  Q_OBJECT
public:
  form_fileSend(CFileTransferSend &FileTransfer);
  ~form_fileSend();

  // forbid some operators
  form_fileSend(const form_fileSend &) = delete;
  form_fileSend &operator=(const form_fileSend &) = delete;

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
