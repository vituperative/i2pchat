/***************************************************************************
 *   Copyright (C) 2008 by I2P-Messenger                                   *
 *   Messenger-Dev@I2P-Messenger                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef FORM_ABOUT_H
#define FORM_ABOUT_H

#include <QClipboard>
#include <QMessageBox>
#include <QtGlobal>
#include <QtGui>

#include "ui_form_about.h"

class form_About : public QDialog {

  Q_OBJECT

public:
  form_About(QString ApplicationVersion, QString Protoclversion,
             QString FiletransferversionMin, QString FiletransferversionMax,
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
