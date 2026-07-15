// SPDX-License-Identifier: AGPL-3.0-or-later

#include "form_about.h"

form_About::form_About(const QString &ApplicationVersion,
                       const QString &Protoclversion,
                       const QString &FiletransferversionMin,
                       const QString &FiletransferversionMax,
                       QWidget *parent)
  : QDialog(parent) {
  ui.setupUi(this);
  // mBoteDestination="mi8H8RkS1ckUrkS2JiFgojN7ExvoCJ45DAEj8y0Ac0Fu6kMS-vEvLerNa~a0WQN9hVhsUUlHJc2B47Ln6wscK5";

  // connect(ui.cmd_destCopy,SIGNAL(clicked()),this,
  //	SLOT(copyDestToClipboard()));

  ui.label_9->setText(ApplicationVersion);
  ui.label_10->setText(Protoclversion);
  if (FiletransferversionMin == FiletransferversionMax)
    ui.label_supportedVersion->setText(FiletransferversionMin);
  else
    ui.label_supportedVersion->setText(FiletransferversionMin + tr("…") + FiletransferversionMax);

  ui.gitCommitHashTextBrowser->setText(GIT_HASH);
  ui.label_dateCompile->setText(__DATE__);
}

form_About::~form_About() {}

void form_About::closeEvent(QCloseEvent *e) {
  e->ignore();
  emit closingAboutWindow();
}
/*
void form_About::copyDestToClipboard()
{
        QClipboard *clipboard = QApplication::clipboard();

        clipboard->setText(mBoteDestination);
        QMessageBox::information(this, "",
                tr("The Destination is in the clipboard"),QMessageBox::Close);
}
*/
void form_About::getFocus() {
  this->activateWindow();
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) | Qt::WindowActive);
  this->raise();
}
void form_About::keyPressEvent(QKeyEvent *event) {
  if (event->key() != Qt::Key_Escape) {
    QDialog::keyPressEvent(event);
  } else {
    event->accept();
    close();
  }
}
