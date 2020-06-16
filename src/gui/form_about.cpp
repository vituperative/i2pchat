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

#include "form_about.h"

form_About::form_About(QString ApplicationVersion, QString Protoclversion,
                       QString FiletransferversionMin,
                       QString FiletransferversionMax, QWidget *parent)
    : QDialog(parent) {
  ui.setupUi(this);
  // mBoteDestination="mi8H8RkS1ckUrkS2JiFgojN7ExvoCJ45DAEj8y0Ac0Fu6kMS-vEvLerNa~a0WQN9hVhsUUlHJc2B47Ln6wscK5";

  // connect(ui.cmd_destCopy,SIGNAL(clicked()),this,
  //	SLOT(copyDestToClipboard()));

  ui.label_9->setText(ApplicationVersion);
  ui.label_10->setText(Protoclversion);
  ui.label_supportedVersion->setText(FiletransferversionMin + tr("…") +
                                     FiletransferversionMax);

  ui.labelGitCommitCount->setText(GIT_COMMIT_COUNT);
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
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) |
                       Qt::WindowActive);
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
