// SPDX-License-Identifier: AGPL-3.0-or-later

#include "FileSend.h"

FileSend::FileSend(CFileTransferSend &FileTransfer)
  : FileTransfer(FileTransfer) {
  setupUi(this);

  QPushButton *pushButton = this->pushButton;

  slot_alreadySentSizeChanged(FileTransfer.getAlreadySentSize());
  connect(&FileTransfer, SIGNAL(signAlreadySentSizeChanged(quint64)), this, SLOT(slot_alreadySentSizeChanged(quint64)));

  connect(&FileTransfer, SIGNAL(signFileTransferFinishedOK()), this, SLOT(slot_FileTransferFinishedOK()));

  connect(&FileTransfer, SIGNAL(signFileTransferAccepted(bool)), this, SLOT(slot_FileTransferAccepted(bool)));

  connect(&FileTransfer, SIGNAL(signFileTransferAborted()), this, SLOT(slot_FileTransferAborted()));

  connect(&FileTransfer, SIGNAL(signFileTransferError()), this, SLOT(slot_FileTransferError()));

  connect(pushButton, SIGNAL(pressed()), this, SLOT(slot_Button()));

  connect(
    &FileTransfer, SIGNAL(signAverageTransferSpeed(QString, QString)), this, SLOT(slot_SpeedChanged(QString, QString)));

  connect(&FileTransfer, SIGNAL(signETA(QString)), labelETA, SLOT(setText(QString)));

  init();
}

static void ElideLabel(QLabel *label, const QString &text) {
  QFontMetrics metrics(label->font());
  int width = label->width() - 6;
  QString clippedText = metrics.elidedText(text, Qt::ElideMiddle, width);
  label->setText(clippedText);
}

void FileSend::init() {
  QString SSize;
  QLabel *labelFilename = this->labelFilename;
  QLabel *labelFilesize = this->labelFilesize;
  QProgressBar *progressBar = this->progressBar;
  QString sType;

  QString file = FileTransfer.getFileName();
  ElideLabel(labelFilename, file);

  quint64 FileSize = FileTransfer.getFileSize();

  FileTransfer.doConvertNumberToTransferSize(FileSize, SSize, sType, false);
  labelFilesize->setText(SSize + " " + sType);

  progressBar->setMinimum(0);
  progressBar->setMaximum(FileTransfer.getFileSize());
  progressBar->setValue(FileTransfer.getAlreadySentSize());

  slot_FileTransferAccepted(FileTransfer.getAlreadyTransferAccepted());

  if (FileTransfer.getIsTransferComplete() == true) {
    slot_FileTransferFinishedOK();
  }

  labelSpeed->setText("waiting...");
  labelETA->setText("n/a");
}

void FileSend::slot_alreadySentSizeChanged(quint64 value) {
  progressBar->setValue(value);
}

void FileSend::slot_FileTransferFinishedOK() {
  QCheckBox *checkBox_4 = this->checkBox_4;
  checkBox_4->setChecked(true);

  this->close();
}

void FileSend::slot_FileTransferAccepted(bool t) {
  if (t == true) {
    checkBox_2->setChecked(true);
    checkBox_3->setChecked(true);
    Speed->setText("Speed:");
  } else {
    Speed->setText("Status:");
  }
}

void FileSend::slot_Button() {

  FileTransfer.slotAbbortFileSend();
  this->close();
}

void FileSend::slot_FileTransferError() {
  this->close();
}

void FileSend::slot_FileTransferAborted() {
  this->close();
}

FileSend::~FileSend() {}

void FileSend::closeEvent(QCloseEvent *e) {
  emit closingFileSendWindow(FileTransfer.getStreamID());
  e->ignore();
}

void FileSend::getFocus() {
  this->activateWindow();
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) | Qt::WindowActive);
  this->raise();
}

void FileSend::slot_SpeedChanged(const QString &SNumber, const QString &Type) {
  labelSpeed->setText(SNumber + " " + Type);
}

void FileSend::keyPressEvent(QKeyEvent *event) {
  if (event->key() != Qt::Key_Escape) {
    QDialog::keyPressEvent(event);
  } else {
    event->accept();
    close();
  }
}
