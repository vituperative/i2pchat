// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef ABOUT_H
#define ABOUT_H

#include "ui_About.h"

#include <QCloseEvent>
#include <QDialog>
#include <QKeyEvent>

class About : public QDialog {

  Q_OBJECT

public:
  About(const QString &ApplicationVersion,
        const QString &Protoclversion,
        const QString &FiletransferversionMin,
        const QString &FiletransferversionMax,
        QWidget *parent = 0);
  ~About();

  About(const About &) = delete;
  About &operator=(const About &) = delete;

  void getFocus();

signals:
  void closingAboutWindow();

private:
  void closeEvent(QCloseEvent *e);
  void keyPressEvent(QKeyEvent *event);

  Ui::About ui;
};
#endif
