// SPDX-License-Identifier: AGPL-3.0-or-later

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
