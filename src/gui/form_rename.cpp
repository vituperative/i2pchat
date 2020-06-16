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

#include "form_rename.h"
#include "UserManager.h"

form_RenameWindow::form_RenameWindow(CCore &Core, QString OldNickname,
                                     QString Destination)
    : Core(Core) {
  setupUi(this);

  this->setAttribute(Qt::WA_DeleteOnClose, true);
  this->Destination = Destination;

  QLineEdit *lineEdit = this->lineEdit;
  lineEdit->setText(OldNickname);

  connect(okButton, SIGNAL(clicked()), this, SLOT(OK()));
}

form_RenameWindow::~form_RenameWindow() {}

void form_RenameWindow::OK() {
  QLineEdit *lineEdit_2 = this->lineEdit_2;
  Core.getUserManager()->renameUserByI2PDestination(Destination,
                                                    lineEdit_2->text());
  this->close();
}

void form_RenameWindow::closeEvent(QCloseEvent *e) {
  e->ignore();
  this->deleteLater();
}
