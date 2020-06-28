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
#include "form_fileRecive.h"
#include "FileTransferRecive.h"

form_fileRecive::form_fileRecive(CFileTransferRecive &FileRecive)
    : FileRecive(FileRecive), mStreamID(FileRecive.getStreamID()) {
  setupUi(this);

  connect(&FileRecive, SIGNAL(signFileReceivedFinishedOK()), this,
          SLOT(slot_FileReceivedFinishedOK()));

  connect(&FileRecive, SIGNAL(signAllreadyReceivedSizeChanged(quint64)), this,
          SLOT(slot_allreadyReceivedSizeChanged(quint64)));

  connect(&FileRecive, SIGNAL(signFileReciveError()), this,
          SLOT(slot_FileReciveError()));

  connect(&FileRecive, SIGNAL(signFileReciveAborted()), this, SLOT(close()));

  connect(pushButton, SIGNAL(pressed()), this, SLOT(slot_Button()));

  connect(&FileRecive, SIGNAL(signAverageReciveSpeed(QString, QString)), this,
          SLOT(slot_SpeedChanged(QString, QString)));

  connect(&FileRecive, SIGNAL(signETA(QString)), label_15,
          SLOT(setText(QString)));

  init();
}

void form_fileRecive::init() {
  QString SSize;
  QString SType;
  QLabel *label_4 = this->label_4;
  QLabel *label_6 = this->label_6;
  QLabel *label_7 = this->label_7;
  QProgressBar *progressBar = this->progressBar;

  label_4->setText(FileRecive.getFileName());
  quint64 FileSize = FileRecive.getFileSize();

  FileRecive.doConvertNumberToTransferSize(FileSize, SSize, SType, false);
  label_6->setText(SSize);
  label_7->setText(SType);

  checkBox_3->setChecked(true);
  progressBar->setMinimum(0);
  progressBar->setMaximum(FileRecive.getFileSize());
  progressBar->setValue(FileRecive.getAllreadyReceivedSize());
  label_10->setText(FileRecive.getUsingProtocolVersion());
  label_11->setText("0");
  label_12->setText("");
}

void form_fileRecive::slot_Button() {
  FileRecive.slotAbbortFileRecive();
  this->close();
}

void form_fileRecive::slot_allreadyReceivedSizeChanged(quint64 value) {
  progressBar->setValue(value);
}

void form_fileRecive::slot_FileReceivedFinishedOK() {
  QCheckBox *checkBox_4 = this->checkBox_4;
  checkBox_4->setChecked(true);

  this->close();
}

void form_fileRecive::slot_FileReciveError() { this->close(); }

form_fileRecive::~form_fileRecive() {}

void form_fileRecive::askTheUser() {
  quint64 FileSize = FileRecive.getFileSize();
  QString FileName = FileRecive.getFileName();
  QString SizeName;
  QString SSize;

  FileRecive.doConvertNumberToTransferSize(FileSize, SSize, SizeName, false);

  QMessageBox *msgBox = new QMessageBox(NULL);
  msgBox->setText(tr("Incoming FileTransfer"));
  // msgBox->setInformativeText("Do you want to accept it ?\nFileName:
  // "+FileName+"\nFileSize: " +SSize+" "+SizeName);
  msgBox->setInformativeText(
      tr("Do you want to accept it ?\nFileName: %1 \nFileSize: %2 %3")
          .arg(FileName)
          .arg(SSize)
          .arg(SizeName));
  msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox->setDefaultButton(QMessageBox::Yes);
  msgBox->setWindowModality(Qt::WindowModal);
  int ret = msgBox->exec();

  if (ret == QMessageBox::Yes) {
    QString FilePath =
        QFileDialog::getSaveFileName(NULL, tr("File Save"), FileName);

    if (!FilePath.isEmpty()) {
      FileRecive.start(FilePath, true);
      label_4->setText(FileRecive.getFileName());
    } else {
      FileRecive.start("", false);
      this->close();
    }
  } else {
    FileRecive.start("", false);
    this->close();
  }
}

void form_fileRecive::getFocus() {
  this->activateWindow();
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) |
                       Qt::WindowActive);
  this->raise();
}

void form_fileRecive::closeEvent(QCloseEvent *e) {
  emit closingFileReciveWindow(mStreamID);
  e->ignore();
}

void form_fileRecive::slot_SpeedChanged(QString SNumber, QString Type) {
  label_11->setText(SNumber);
  label_12->setText(Type);
}

void form_fileRecive::start() {
  if (FileRecive.checkIfAllreadyAcceptTheRequest() == false) {
    askTheUser();
  }
}
void form_fileRecive::keyPressEvent(QKeyEvent *event) {
  if (event->key() != Qt::Key_Escape) {
    QDialog::keyPressEvent(event);
  } else {
    event->accept();
    close();
  }
}
