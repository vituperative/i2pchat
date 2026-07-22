// SPDX-License-Identifier: AGPL-3.0-or-later

#include "form_fileReceive.h"

#include "FileTransferReceive.h"

form_fileReceive::form_fileReceive(CFileTransferReceive &FileReceive)
  : FileReceive(FileReceive)
  , mStreamID(FileReceive.getStreamID()) {
  setupUi(this);

  connect(&FileReceive, SIGNAL(signFileReceivedFinishedOK()), this, SLOT(slot_FileReceivedFinishedOK()));

  connect(&FileReceive,
          SIGNAL(signgetTransferredSizeChanged(quint64)),
          this,
          SLOT(slot_allreadyReceivedSizeChanged(quint64)));

  connect(&FileReceive, SIGNAL(signFileReceiveError()), this, SLOT(slot_FileReceiveError()));

  connect(&FileReceive, SIGNAL(signFileReceiveAborted()), this, SLOT(close()));

  connect(pushButton, SIGNAL(pressed()), this, SLOT(slot_Button()));

  connect(
    &FileReceive, SIGNAL(signAverageReceiveSpeed(QString, QString)), this, SLOT(slot_SpeedChanged(QString, QString)));

  connect(&FileReceive, SIGNAL(signETA(QString)), labelETA, SLOT(setText(QString)));

  init();
}

static void ElideLabel(QLabel *label, const QString &text) {
  QFontMetrics metrics(label->font());
  int width = label->width() - 6;
  QString clippedText = metrics.elidedText(text, Qt::ElideMiddle, width);
  label->setText(clippedText);
}

void form_fileReceive::init() {
  QString SSize;
  QString SType;
  QLabel *labelFilename = this->labelFilename;
  QLabel *labelFilesize = this->labelFilesize;
  QProgressBar *progressBar = this->progressBar;

  // labelFilename->setText(FileReceive.getFileName());
  QString file = FileReceive.getFileName();
  ElideLabel(labelFilename, file);

  quint64 FileSize = FileReceive.getFileSize();

  FileReceive.doConvertNumberToTransferSize(FileSize, SSize, SType, false);
  labelFilesize->setText(SSize + " " + SType);

  checkBox_3->setChecked(true);
  progressBar->setMinimum(0);
  progressBar->setMaximum(FileReceive.getFileSize());
  progressBar->setValue(FileReceive.getTransferredSize());
  //  label_10->setText(FileReceive.getUsingProtocolVersion());
  labelSpeed->setText("waiting...");
}

void form_fileReceive::slot_Button() {
  FileReceive.slotAbbortFileReceive();
  this->close();
}

void form_fileReceive::slot_allreadyReceivedSizeChanged(quint64 value) {
  progressBar->setValue(value);
}

void form_fileReceive::slot_FileReceivedFinishedOK() {
  QCheckBox *checkBox_4 = this->checkBox_4;
  checkBox_4->setChecked(true);

  this->close();
}

void form_fileReceive::slot_FileReceiveError() {
  this->close();
}

form_fileReceive::~form_fileReceive() {}

void form_fileReceive::askTheUser() {
  quint64 FileSize = FileReceive.getFileSize();
  QString FileName = FileReceive.getFileName();
  QString SizeName;
  QString SSize;

  FileReceive.doConvertNumberToTransferSize(FileSize, SSize, SizeName, false);

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
      FileReceive.start(FilePath, true);
      labelFilename->setText(FileReceive.getFileName());
    } else {
      FileReceive.start("", false);
      this->close();
    }
  } else {
    FileReceive.start("", false);
    this->close();
  }
}

void form_fileReceive::getFocus() {
  this->activateWindow();
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) | Qt::WindowActive);
  this->raise();
}

void form_fileReceive::closeEvent(QCloseEvent *e) {
  emit closingFileReceiveWindow(mStreamID);
  e->ignore();
}

void form_fileReceive::slot_SpeedChanged(const QString &SNumber, const QString &Type) {
  labelSpeed->setText(SNumber + " " + Type);
}

void form_fileReceive::start() {
  if (FileReceive.checkIfRequestAccepted() == false) {
    askTheUser();
  }
}
void form_fileReceive::keyPressEvent(QKeyEvent *event) {
  if (event->key() != Qt::Key_Escape) {
    QDialog::keyPressEvent(event);
  } else {
    event->accept();
    close();
  }
}
