// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FORM_NEWUSER_H
#define FORM_NEWUSER_H

#include "Core.h"
#include "ui_form_newUser.h"

#include <QtGui>

class form_newUserWindow : public QDialog, private Ui::form_newUserWindow {
  Q_OBJECT
public:
  form_newUserWindow(CCore &Core, QDialog *parent = 0);
  ~form_newUserWindow();

  // forbid some operators
  form_newUserWindow(const form_newUserWindow &) = delete;
  form_newUserWindow &operator=(const form_newUserWindow &) = delete;

private slots:
  void addnewUser();

private:
  CCore &Core;

  void closeEvent(QCloseEvent *e);
};

#endif
