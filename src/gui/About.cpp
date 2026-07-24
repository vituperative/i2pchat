// SPDX-License-Identifier: AGPL-3.0-or-later

#include "About.h"

About::About(const QString &ApplicationVersion,
             const QString &Protoclversion,
             const QString &FiletransferversionMin,
             const QString &FiletransferversionMax,
             QWidget *parent)
  : QDialog(parent) {
  ui.setupUi(this);

  ui.label_9->setText(ApplicationVersion);
  ui.label_10->setText(Protoclversion);
  if (FiletransferversionMin == FiletransferversionMax)
    ui.label_supportedVersion->setText(FiletransferversionMin);
  else
    ui.label_supportedVersion->setText(FiletransferversionMin + tr("…") + FiletransferversionMax);

  ui.gitCommitHashTextBrowser->setText(GIT_HASH);
  ui.label_dateCompile->setText(__DATE__);
}

About::~About() {}

void About::closeEvent(QCloseEvent *e) {
  e->ignore();
  emit closingAboutWindow();
}

void About::getFocus() {
  this->activateWindow();
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) | Qt::WindowActive);
  this->raise();
}

void About::keyPressEvent(QKeyEvent *event) {
  if (event->key() != Qt::Key_Escape) {
    QDialog::keyPressEvent(event);
  } else {
    event->accept();
    close();
  }
}
