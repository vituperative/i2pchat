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
#ifndef CHATMESSAGECHANGER_H
#define CHATMESSAGECHANGER_H

#include <QTextEdit>
#include <QtGui>

class CCore;
class CChatMessageChanger {
public:
  static CChatMessageChanger *exemplar(CCore &Core);

  ~CChatMessageChanger();

  // forbid some operators
  CChatMessageChanger &operator=(const CChatMessageChanger &) = delete;

  QString changeChatMessage(QString Chatmessage);

private:
  static CChatMessageChanger *instanz;
  CChatMessageChanger(QString &ConfigPath);
  CChatMessageChanger(const CChatMessageChanger &);

  QFont mFont;
  QColor mColor;
  bool mDoChange;
  QTextEdit *mControllForChange;
};
#endif
