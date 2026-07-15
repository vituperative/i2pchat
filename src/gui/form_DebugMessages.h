// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FORM_DEBUGMESSAGES_H
#define FORM_DEBUGMESSAGES_H

#include "Core.h"
#include "DebugMessageManager.h"
#include "ui_form_DebugMessages.h"

#include <QtGlobal>
#include <QtGui>

class form_DebugMessages : public QDialog, private Ui::form_DebugMessages {
  Q_OBJECT
public:
  form_DebugMessages(CCore &core, QDialog *parent = 0);
  ~form_DebugMessages();

  // forbid some operators
  form_DebugMessages(const form_DebugMessages &) = delete;
  form_DebugMessages &operator=(const form_DebugMessages &) = delete;

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
