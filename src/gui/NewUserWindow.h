// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef NEWUSERWINDOW_H
#define NEWUSERWINDOW_H

#include "Core.h"
#include "ui_NewUserWindow.h"

#include <QCloseEvent>
#include <QDialog>

class NewUserWindow : public QDialog, private Ui::NewUserWindow {
  Q_OBJECT
public:
  NewUserWindow(CCore &Core, QDialog *parent = 0);
  ~NewUserWindow();

  // forbid some operators
  NewUserWindow(const NewUserWindow &) = delete;
  NewUserWindow &operator=(const NewUserWindow &) = delete;

private slots:
  void addnewUser();

private:
  CCore &Core;

  void closeEvent(QCloseEvent *e);
};

#endif
