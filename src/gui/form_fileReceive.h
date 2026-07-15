// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FORM_FILERECEIVE_H
#define FORM_FILERECEIVE_H

#include "ui_form_fileReceive.h"

#include <QFileDialog>
#include <QtGui>

class CFileTransferReceive;
class form_fileReceive : public QDialog, public Ui::form_FileReceive {
  Q_OBJECT
public:
  form_fileReceive(CFileTransferReceive &FileReceive);
  ~form_fileReceive();

  // forbid some operators
  form_fileReceive(const form_fileReceive &) = delete;
  form_fileReceive &operator=(const form_fileReceive &) = delete;

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
  CFileTransferReceive &FileReceive;
  const qint32 mStreamID;

  void closeEvent(QCloseEvent *e);
  void keyPressEvent(QKeyEvent *event);
  void init();
  void askTheUser();
};
#endif
