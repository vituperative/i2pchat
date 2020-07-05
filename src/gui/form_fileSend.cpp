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

#include "form_fileSend.h"

form_fileSend::form_fileSend(CFileTransferSend &FileTransfer)
    : FileTransfer(FileTransfer) {
  setupUi(this);
  // this->setAttribute(Qt::WA_DeleteOnClose,true);

  QPushButton *pushButton = this->pushButton;

  slot_allreadySendedSizeChanged(FileTransfer.getAllreadySendedSize());
  connect(&FileTransfer, SIGNAL(signAllreadySendedSizeChanged(quint64)), this,
          SLOT(slot_allreadySendedSizeChanged(quint64)));

  connect(&FileTransfer, SIGNAL(signFileTransferFinishedOK()), this,
          SLOT(slot_FileTransferFinishedOK()));

  connect(&FileTransfer, SIGNAL(signFileTransferAccepted(bool)), this,
          SLOT(slot_FileTransferAccepted(bool)));

  connect(&FileTransfer, SIGNAL(signFileTransferAborted()), this,
          SLOT(slot_FileTransferAborted()));

  connect(&FileTransfer, SIGNAL(signFileTransferError()), this,
          SLOT(slot_FileTransferError()));

  connect(pushButton, SIGNAL(pressed()), this, SLOT(slot_Button()));

  connect(&FileTransfer, SIGNAL(signAverageTransferSpeed(QString, QString)),
          this, SLOT(slot_SpeedChanged(QString, QString)));

  connect(&FileTransfer, SIGNAL(signETA(QString)), label_15,
          SLOT(setText(QString)));

  init();
}

void form_fileSend::init() {
  QString SSize;
  QLabel *label_4 = this->label_4;
  QLabel *label_6 = this->label_6;
  QLabel *label_7 = this->label_7;
  QProgressBar *progressBar = this->progressBar;
  QString sType;

  label_4->setText(FileTransfer.getFileName());

  quint64 FileSize = FileTransfer.getFileSize();

  FileTransfer.doConvertNumberToTransferSize(FileSize, SSize, sType, false);
  label_6->setText(SSize);
  label_7->setText(sType);

  progressBar->setMinimum(0);
  progressBar->setMaximum(FileTransfer.getFileSize());
  progressBar->setValue(FileTransfer.getAllreadySendedSize());

//  label_10->setText(FileTransfer.getUsingProtocolVersion());

  slot_FileTransferAccepted(FileTransfer.getAllreadyTransferAccepted());

  if (FileTransfer.getIsAllreadyFinished() == true) {
    slot_FileTransferFinishedOK();
  }

  label_13->setText("0");
  label_11->setText("");
  label_15->setText("");
}

void form_fileSend::slot_allreadySendedSizeChanged(quint64 value) {
  progressBar->setValue(value);
}

void form_fileSend::slot_FileTransferFinishedOK() {
  QCheckBox *checkBox_4 = this->checkBox_4;
  checkBox_4->setChecked(true);

  this->close();
}

void form_fileSend::slot_FileTransferAccepted(bool t) {
  if (t == true) {
    checkBox_2->setChecked(true);
    checkBox_3->setChecked(true);
  }
}

void form_fileSend::slot_Button() {

  FileTransfer.slotAbbortFileSend();
  this->close();
}

void form_fileSend::slot_FileTransferError() { this->close(); }

void form_fileSend::slot_FileTransferAborted() { this->close(); }

form_fileSend::~form_fileSend() {}

void form_fileSend::closeEvent(QCloseEvent *e) {
  emit closingFileSendWindow(FileTransfer.getStreamID());
  e->ignore();
}

void form_fileSend::getFocus() {
  this->activateWindow();
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) |
                       Qt::WindowActive);
  this->raise();
}

void form_fileSend::slot_SpeedChanged(QString SNumber, QString Type) {
  label_13->setText(SNumber);
  label_11->setText(Type);
}
void form_fileSend::keyPressEvent(QKeyEvent *event) {
  if (event->key() != Qt::Key_Escape) {
    QDialog::keyPressEvent(event);
  } else {
    event->accept();
    close();
  }
}
