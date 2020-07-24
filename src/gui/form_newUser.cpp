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
#include "form_newUser.h"
#include "UserManager.h"

form_newUserWindow::form_newUserWindow(CCore &Core, QDialog *parent)
    : QDialog(parent), Core(Core) {
  setupUi(this);
  // this->setAttribute(Qt::WA_DeleteOnClose,true);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(addnewUser()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

form_newUserWindow::~form_newUserWindow() {}

void form_newUserWindow::addnewUser() {
  QString Name = lineEdit->text();
  QString I2PDestination = textEdit->toPlainText();

  if (Name.isEmpty()) {
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setText(tr("\nPlease supply a nickname for the contact"));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->show();
    return;
  }

  if (Core.getUserManager()->validateI2PDestination(I2PDestination) == false) {

    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setText(tr("\nAdd failed: Destination is invalid"));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->show();

    return;
  }

  if (I2PDestination == Core.getMyDestination()) {
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setText(tr("\nCannot add your own destination to contact list"));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->show();
    return;
  }

  if (Core.getUserBlockManager()->isDestinationInBlockList(I2PDestination) ==
      true) {
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setText(tr("\nAdd failed: Destination is blocklisted"));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->show();
    return;
  }

  if (Core.getUserManager()->addNewUser(Name, I2PDestination, 0, true) ==
      false) {

    QMessageBox *msgBox = new QMessageBox(NULL);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setText(tr("\nAdd failed: Destination already in contacts"));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->show();

    this->close();
  } else {
    this->close();
  }
}

void form_newUserWindow::closeEvent(QCloseEvent *e) {
  e->ignore();
  this->deleteLater();
  // delete this;
}
