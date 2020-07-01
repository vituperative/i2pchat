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

#ifndef FORM_FILERECEIVE_H
#define FORM_FILERECEIVE_H

#include <QFileDialog>
#include <QtGui>

#include "ui_form_fileReceive.h"

class CFileTransferReceive;
class form_fileReceive : public QDialog, public Ui::form_FileReceive {
  Q_OBJECT
public:
  form_fileReceive(CFileTransferReceive &FileReceive);
  ~form_fileReceive();

  // forbid some operators
  form_fileReceive(const form_fileReceive &) = delete;
  form_fileReceive &operator=(const form_fileReceive &) = delete;

  void getFocus();
  void start();

signals:
  void closingFileReceiveWindow(qint32 StreamID);

private slots:
  void slot_Button();
  void slot_allreadyReceivedSizeChanged(quint64 value);
  void slot_FileReceivedFinishedOK();
  void slot_FileReceiveError();
  void slot_SpeedChanged(QString SNumber, QString Type);

private:
  CFileTransferReceive &FileReceive;
  const qint32 mStreamID;

  void closeEvent(QCloseEvent *e);
  void keyPressEvent(QKeyEvent *event);
  void init();
  void askTheUser();
};
#endif
