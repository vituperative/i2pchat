// SPDX-License-Identifier: AGPL-3.0-or-later

#include "FileReceive.h"

#include "FileTransferReceive.h"

#include <QFileDialog>
#include <QMessageBox>

FileReceive::FileReceive(CFileTransferReceive &transfer)
  : mFileTransfer(transfer)
  , mStreamID(transfer.getStreamID()) {
  setupUi(this);

  connect(&mFileTransfer, SIGNAL(signFileReceivedFinishedOK()), this, SLOT(slot_FileReceivedFinishedOK()));

  connect(&mFileTransfer,
          SIGNAL(signgetTransferredSizeChanged(quint64)),
          this,
          SLOT(slot_allreadyReceivedSizeChanged(quint64)));

  connect(&mFileTransfer, SIGNAL(signFileReceiveError()), this, SLOT(slot_FileReceiveError()));

  connect(&mFileTransfer, SIGNAL(signFileReceiveAborted()), this, SLOT(close()));

  connect(pushButton, SIGNAL(pressed()), this, SLOT(slot_Button()));

  connect(
    &mFileTransfer, SIGNAL(signAverageReceiveSpeed(QString, QString)), this, SLOT(slot_SpeedChanged(QString, QString)));

  connect(&mFileTransfer, SIGNAL(signETA(QString)), labelETA, SLOT(setText(QString)));

  init();
}

static void ElideLabel(QLabel *label, const QString &text) {
  QFontMetrics metrics(label->font());
  int width = label->width() - 6;
  QString clippedText = metrics.elidedText(text, Qt::ElideMiddle, width);
  label->setText(clippedText);
}

void FileReceive::init() {
  QString SSize;
  QString SType;
  QLabel *labelFilename = this->labelFilename;
  QLabel *labelFilesize = this->labelFilesize;
  QProgressBar *progressBar = this->progressBar;

  QString file = mFileTransfer.getFileName();
  ElideLabel(labelFilename, file);

  quint64 FileSize = mFileTransfer.getFileSize();

  mFileTransfer.doConvertNumberToTransferSize(FileSize, SSize, SType, false);
  labelFilesize->setText(SSize + " " + SType);

  checkBox_3->setChecked(true);
  progressBar->setMinimum(0);
  progressBar->setMaximum(mFileTransfer.getFileSize());
  progressBar->setValue(mFileTransfer.getTransferredSize());
  labelSpeed->setText("waiting...");
}

void FileReceive::slot_Button() {
  mFileTransfer.slotAbbortFileReceive();
  this->close();
}

void FileReceive::slot_allreadyReceivedSizeChanged(quint64 value) {
  progressBar->setValue(value);
}

void FileReceive::slot_FileReceivedFinishedOK() {
  QCheckBox *checkBox_4 = this->checkBox_4;
  checkBox_4->setChecked(true);

  this->close();
}

void FileReceive::slot_FileReceiveError() {
  this->close();
}

FileReceive::~FileReceive() {}

void FileReceive::askTheUser() {
  quint64 FileSize = mFileTransfer.getFileSize();
  QString FileName = mFileTransfer.getFileName();
  QString SizeName;
  QString SSize;

  mFileTransfer.doConvertNumberToTransferSize(FileSize, SSize, SizeName, false);

  QMessageBox msgBox(NULL);
  QPixmap pixmap = QPixmap(":/icons/avatar.svg");
  msgBox.setWindowIcon(QIcon(pixmap));
  msgBox.setText(tr("Incoming File Transfer: %1 [%2%3]    ").arg(FileName).arg(SSize).arg(SizeName));
  msgBox.setInformativeText(tr("Do you wish to download this file?"));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::Yes);
  msgBox.setWindowModality(Qt::WindowModal);
  int ret = msgBox.exec();

  if (ret == QMessageBox::Yes) {
    QString FilePath = QFileDialog::getSaveFileName(NULL, tr("File Save"), FileName);

    if (!FilePath.isEmpty()) {
      mFileTransfer.start(FilePath, true);
      labelFilename->setText(mFileTransfer.getFileName());
    } else {
      mFileTransfer.start("", false);
      this->close();
    }
  } else {
    mFileTransfer.start("", false);
    this->close();
  }
}

void FileReceive::getFocus() {
  this->activateWindow();
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) | Qt::WindowActive);
  this->raise();
}

void FileReceive::closeEvent(QCloseEvent *e) {
  emit closingFileReceiveWindow(mStreamID);
  e->ignore();
}

void FileReceive::slot_SpeedChanged(const QString &SNumber, const QString &Type) {
  labelSpeed->setText(SNumber + " " + Type);
}

void FileReceive::start() {
  if (mFileTransfer.checkIfRequestAccepted() == false) {
    askTheUser();
  }
}
void FileReceive::keyPressEvent(QKeyEvent *event) {
  if (event->key() != Qt::Key_Escape) {
    QDialog::keyPressEvent(event);
  } else {
    event->accept();
    close();
  }
}
