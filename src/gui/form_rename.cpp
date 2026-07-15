// SPDX-License-Identifier: AGPL-3.0-or-later

#include "form_rename.h"

#include "UserManager.h"

#include <utility>

form_RenameWindow::form_RenameWindow(CCore &Core, const QString &OldNickname, QString Destination)
  : Core(Core) {
  setupUi(this);

  this->setAttribute(Qt::WA_DeleteOnClose, true);
  this->Destination = std::move(Destination);

  QLineEdit *lineEdit = this->lineEdit;
  lineEdit->setText(OldNickname);

  connect(okButton, SIGNAL(clicked()), this, SLOT(OK()));
}

form_RenameWindow::~form_RenameWindow() {}

void form_RenameWindow::OK() {
  QLineEdit *lineEdit_2 = this->lineEdit_2;
  Core.getUserManager()->renameUserByI2PDestination(Destination, lineEdit_2->text());
  this->close();
}

void form_RenameWindow::closeEvent(QCloseEvent *e) {
  e->ignore();
  this->deleteLater();
}
