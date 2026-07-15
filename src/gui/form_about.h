// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FORM_ABOUT_H
#define FORM_ABOUT_H

#include "ui_form_about.h"

#include <QClipboard>
#include <QMessageBox>
#include <QtGlobal>
#include <QtGui>

class form_About : public QDialog {

  Q_OBJECT

public:
  form_About(const QString &ApplicationVersion,
             const QString &Protoclversion,
             const QString &FiletransferversionMin,
             const QString &FiletransferversionMax,
             QWidget *parent = 0);
  ~form_About();

  // forbid some operators
  form_About(const form_About &) = delete;
  form_About &operator=(const form_About &) = delete;

  void getFocus();
  // private slots:
  //	void copyDestToClipboard();

signals:
  void closingAboutWindow();

private:
  void closeEvent(QCloseEvent *e);
  void keyPressEvent(QKeyEvent *event);

  Ui::form_About ui;
  // QString mBoteDestination;
};
#endif
