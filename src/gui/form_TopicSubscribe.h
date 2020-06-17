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

#ifndef FORM_TOPICSUBSCRIBE_H
#define FORM_TOPICSUBSCRIBE_H

#include <QMap>
#include <QtGlobal>
#include <QtGui>

#include "ui_form_topicSubscribe.h"

#include "gui_icons.h"

class CCore;
class form_topicSubscribe : public QDialog, private Ui::form_topicSubscribe {

  Q_OBJECT

public:
  form_topicSubscribe(CCore &Core);
  ~form_topicSubscribe();

  // forbid some operators
  form_topicSubscribe(const form_topicSubscribe &) = delete;
  form_topicSubscribe &operator=(const form_topicSubscribe &) = delete;

  void requestFocus();

signals:
  void signClosingTopicSubscribeWindow();

private slots:
  void slot_cmdSubscribe();
  void slot_showContextMenu(const QPoint &);
  void slot_openTopic();
  void slot_onlineStateChanged();

private:
  CCore &mCore;
  void init();
  void closeEvent(QCloseEvent *e);
  void keyPressEvent(QKeyEvent *event);
};
#endif /* of FORM_TOPICSUBSCRIBE_H */
