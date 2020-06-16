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

#ifndef FORM_CHATWIDGET_h
#define FORM_CHATWIDGET_h

#include "Core.h"
#include "TextEmotionChanger.h"
#include "gui_icons.h"
#include "ui_form_chatwidget.h"

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QFocusEvent>
#include <QFontDialog>
#include <QKeyEvent>
#include <QPalette>
#include <QPushButton>
#include <QScrollBar>
#include <QTextBrowser>
#include <Qt>
#include <QtGui>

class ChatEventEater : public QObject {
  Q_OBJECT

public:
  ChatEventEater(QWidget *parent = 0) : QObject(parent) {}

signals:
  void sendMessage();
  void haveFocus(bool b);

protected:
  bool eventFilter(QObject *obj, QEvent *event);
};

class CUser;
class form_ChatWidget : public QMainWindow, public Ui::form_chatwidget {
  Q_OBJECT
public:
  form_ChatWidget(CUser &user, CCore &Core, QDialog *parent = 0);
  ~form_ChatWidget();

  // forbid some operators
  form_ChatWidget(const form_ChatWidget &) = delete;
  form_ChatWidget &operator=(const form_ChatWidget &) = delete;

  void getFocus();

private slots:
  void sendMessageSignal();
  void addAllMessages();
  void addMessage(QString text);
  void setTextColor();
  void setTextColor_2();
  void newMessageRecived();
  void setBold(bool t);
  void setBold_2(bool t);
  void setFont();
  void setFont_2();
  void setUnderline(bool t);
  void setUnderline_2(bool t);
  void setItalic(bool t);
  void setItalic_2(bool t);

  void changeWindowsTitle();
  void newFileTransfer();

  void anchorClicked(const QUrl &);
  void focusEvent(bool b);
  void showAvatarFrame(bool show);
  void remoteAvatarImageChanged();
  void messageTextChanged();

  void tabIndexChanged(int tabIndex);

  void saveChangedOfflineMessages();

signals:
  void sendChatMessage(QString chatMessage);
  void closingChatWindow(QString);

public slots:
  void slotLoadOwnAvatarImage();
private slots:
  void displayOfflineMessages(int index);
  void cmd_back();
  void cmd_next();
  void cmd_delete();
  void cmd_new();
  void reloadOfflineMessages();

private:
  void closeEvent(QCloseEvent *e);
  void centerDialog();

  QColor textColor;
  QColor textColor2;

  CUser &user;
  QFont mCurrentFont;
  QFont mCurrentFont2;

  ChatEventEater *m_event_eater;
  CCore &Core;
  bool mHaveFocus;
  QTextEdit mControllForChange;
  void keyPressEvent(QKeyEvent *event);
  QPixmap mOwnAvatar;
  QPixmap mUserAvatar;
  int currentOfflineMessageIndex;
  int OfflineMessageCount;
  QStringList offlineMessages;
};
#endif
