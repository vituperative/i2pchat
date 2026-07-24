// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef DEBUGMESSAGES_H
#define DEBUGMESSAGES_H

#include "Core.h"
#include "DebugMessageManager.h"
#include "ui_DebugMessages.h"

#include <QCloseEvent>
#include <QDialog>
#include <QKeyEvent>

class DebugMessages : public QDialog, private Ui::DebugMessages {
  Q_OBJECT
public:
  DebugMessages(CCore &core, QDialog *parent = 0);
  ~DebugMessages();

  // forbid some operators
  DebugMessages(const DebugMessages &) = delete;
  DebugMessages &operator=(const DebugMessages &) = delete;

  void getFocus();

private slots:
  void newDebugMessage();
  void clearDebugMessages();
  void connectionDump();

signals:
  void closingDebugWindow();

private:
  CCore &core;
  CDebugMessageManager *DebugMessageManager;

  void closeEvent(QCloseEvent *e);
  void keyPressEvent(QKeyEvent *event);
};
#endif
