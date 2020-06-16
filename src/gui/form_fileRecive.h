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

#ifndef FORM_FILERECIVE_H
#define FORM_FILERECIVE_H

#include <QFileDialog>
#include <QtGui>

#include "ui_form_fileRecive.h"

class CFileTransferRecive;
class form_fileRecive : public QDialog, public Ui::form_FileRecive {
  Q_OBJECT
public:
  form_fileRecive(CFileTransferRecive &FileRecive);
  ~form_fileRecive();

  // forbid some operators
  form_fileRecive(const form_fileRecive &) = delete;
  form_fileRecive &operator=(const form_fileRecive &) = delete;

  void getFocus();
  void start();

signals:
  void closingFileReciveWindow(qint32 StreamID);

private slots:
  void slot_Button();
  void slot_allreadyRecivedSizeChanged(quint64 value);
  void slot_FileRecivedFinishedOK();
  void slot_FileReciveError();
  void slot_SpeedChanged(QString SNumber, QString Type);

private:
  CFileTransferRecive &FileRecive;
  const qint32 mStreamID;

  void closeEvent(QCloseEvent *e);
  void keyPressEvent(QKeyEvent *event);
  void init();
  void askTheUser();
};
#endif
