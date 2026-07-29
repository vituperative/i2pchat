// SPDX-License-Identifier: AGPL-3.0-or-later

#include "RenameWindow.h"

#include "UserManager.h"

#include <utility>

RenameWindow::RenameWindow(CCore &Core, const QString &OldNickname, QString Destination)
  : Core(Core) {
  setupUi(this);

  this->setAttribute(Qt::WA_DeleteOnClose, true);
  this->Destination = std::move(Destination);

  QLineEdit *lineEdit = this->lineEdit;
  lineEdit->setText(OldNickname);

  connect(okButton, SIGNAL(clicked()), this, SLOT(OK()));
}

RenameWindow::~RenameWindow() {}

void RenameWindow::OK() {
  QLineEdit *lineEdit_2 = this->lineEdit_2;
  Core.getUserManager()->renameUserByI2PDestination(Destination, lineEdit_2->text());
  this->close();
}

void RenameWindow::closeEvent(QCloseEvent *e) {
  e->ignore();
  this->deleteLater();
}
