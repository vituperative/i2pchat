// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef RENAMEWINDOW_H
#define RENAMEWINDOW_H

#include "Core.h"
#include "ui_RenameWindow.h"

#include <QCloseEvent>
#include <QDialog>

class RenameWindow : public QDialog, private Ui::RenameWindow {
  Q_OBJECT
public:
  RenameWindow(CCore &Core, const QString &OldNickname, QString Destination);
  ~RenameWindow();

  // forbid some operators
  RenameWindow(const RenameWindow &) = delete;
  RenameWindow &operator=(const RenameWindow &) = delete;

private slots:
  void OK();

private:
  CCore &Core;
  QString Destination;
  void closeEvent(QCloseEvent *e);
};
#endif
