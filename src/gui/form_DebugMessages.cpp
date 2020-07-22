/***************************************************************************
 *   Copyright (C) 2008 by I2P-Messenger                                   *
 *   Messenger-Dev@I2P-Messenger					   *
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
#include "form_DebugMessages.h"

form_DebugMessages::form_DebugMessages(CCore &core, QDialog *parent)
    : QDialog(parent), core(core) {
  setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
  setupUi(this);
  // this->setAttribute(Qt::WA_DeleteOnClose,true);

  DebugMessageManager = core.getDebugMessageHandler();

  if (DebugMessageManager == NULL) {
    return;
  }

  connect(cmd_clear_sam, SIGNAL(clicked()), this, SLOT(clearDebugMessages()));
  connect(DebugMessageManager, SIGNAL(signNewDebugMessage(QString)), this,
          SLOT(newDebugMessage()));
  connect(cmd_Connection_snapshot, SIGNAL(clicked()), this,
          SLOT(connectionDump()));

  connect(cmd_close, SIGNAL(clicked()), this, SLOT(close()));
  connect(cmd_close_3, SIGNAL(clicked()), this, SLOT(close()));

  newDebugMessage();
}

form_DebugMessages::~form_DebugMessages() {}

void form_DebugMessages::connectionDump() {
  QString Message = core.getConnectionDump();
  connection_txt->clear();
  connection_txt->setText(Message);
}

void form_DebugMessages::newDebugMessage() {
  Sam_txt->clear();

  QStringList temp = DebugMessageManager->getAllMessages();
  for (int i = 0; i < temp.count(); i++) {
    this->Sam_txt->append(temp[i]);
  }

  QTextCursor cursor = Sam_txt->textCursor();
  cursor.movePosition(QTextCursor::Start);
  Sam_txt->setTextCursor(cursor);
}

void form_DebugMessages::clearDebugMessages() {
  DebugMessageManager->doClearAllMessages();
  Sam_txt->clear();
}

void form_DebugMessages::closeEvent(QCloseEvent *e) {
  // e->ignore();
  e->accept();
  emit closingDebugWindow();
}
void form_DebugMessages::getFocus() {
  this->activateWindow();
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) |
                       Qt::WindowActive);
  this->raise();
}
void form_DebugMessages::keyPressEvent(QKeyEvent *event) {
  if (event->key() != Qt::Key_Escape) {
    QDialog::keyPressEvent(event);
  } else {
    event->accept();
    close();
  }
}
