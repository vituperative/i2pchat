// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef _HELPDIALOG_H
#define _HELPDIALOG_H

#include "ui_form_HelpDialog.h"

#include <QFileDialog>

class form_HelpDialog : public QDialog {
  Q_OBJECT

public:
  form_HelpDialog(QString ProgrammVersion, QString ProtocolVersion, QString ConfigPath, QWidget *parent = 0);

  // forbid some operators
  form_HelpDialog(const form_HelpDialog &) = delete;
  form_HelpDialog &operator=(const form_HelpDialog &) = delete;

private slots:

private:
  Ui::form_HelpDialog ui;
  const QString mConfigPath;
};

#endif
