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
