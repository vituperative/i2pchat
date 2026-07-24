// SPDX-License-Identifier: AGPL-3.0-or-later

#include "NewUserWindow.h"

#include "UserManager.h"

#include <QMessageBox>

NewUserWindow::NewUserWindow(CCore &Core, QDialog *parent)
  : QDialog(parent)
  , Core(Core) {
  setupUi(this);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(addnewUser()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

NewUserWindow::~NewUserWindow() {}

void NewUserWindow::addnewUser() {
  QString Name = lineEdit->text();
  QString I2PDestination = textEdit->toPlainText();

  if (Name.isEmpty()) {
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setText(tr("\nPlease supply a nickname for the contact"));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
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
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
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
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->show();
    return;
  }

  if (Core.getUserBlockManager()->isDestinationInBlockList(I2PDestination) == true) {
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setText(tr("\nAdd failed: Destination is blocklisted"));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->show();
    return;
  }

  if (Core.getUserManager()->addNewUser(Name, I2PDestination, 0, true) == false) {

    QMessageBox *msgBox = new QMessageBox(NULL);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setText(tr("\nAdd failed: Destination already in contacts"));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->show();

    this->close();
  } else {
    this->close();
  }
}

void NewUserWindow::closeEvent(QCloseEvent *e) {
  e->ignore();
  this->deleteLater();
}
