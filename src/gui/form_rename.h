// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FORM_RENAME_H
#define FORM_RENAME_H

#include "Core.h"
#include "ui_form_rename.h"

#include <QtGui>

class form_RenameWindow : public QDialog, private Ui::form_renameWindow {
  Q_OBJECT
public:
  form_RenameWindow(CCore &Core, const QString &OldNickname, QString Destination);
  ~form_RenameWindow();

  // forbid some operators
  form_RenameWindow(const form_RenameWindow &) = delete;
  form_RenameWindow &operator=(const form_RenameWindow &) = delete;

private slots:
  void OK();

private:
  CCore &Core;
  QString Destination;
  void closeEvent(QCloseEvent *e);
};
#endif
