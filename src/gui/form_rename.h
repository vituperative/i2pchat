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
#ifndef FORM_RENAME_H
#define FORM_RENAME_H

#include "Core.h"
#include "ui_form_rename.h"
#include <QtGui>

class form_RenameWindow : public QDialog, private Ui::form_renameWindow {
  Q_OBJECT
public:
  form_RenameWindow(CCore &Core, QString OldNickname, QString Destination);
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
