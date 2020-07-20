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
#ifndef FORM_MAIN_H
#define FORM_MAIN_H

#include <QClipboard>
#include <QContextMenuEvent>
#include <QCursor>
#include <QFileDialog>
#include <QMap>
#include <QMenu>
#include <QMouseEvent>
#include <QMutex>
#include <QPoint>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QtGui>

#include "form_DebugMessages.h"
#include "form_TopicSubscribe.h"
#include "form_UserSearch.h"
#include "form_about.h"
#include "form_chatwidget.h"
#include "form_fileReceive.h"
#include "form_fileSend.h"
#include "form_newUser.h"
#include "form_rename.h"
#include "form_settingsgui.h"
#include "gui_icons.h"
#include "ui_form_Main.h"

#include "Core.h"
#include "User.h"

#include "FileTransferReceive.h"
#include "FileTransferSend.h"

class form_MainWindow : public QMainWindow, private Ui::form_MainWindow {
  Q_OBJECT

public:
  form_MainWindow(QString configDir, QWidget *parent = 0);
  ~form_MainWindow();

  // forbid some operators
  form_MainWindow(const form_MainWindow &) = delete;
  form_MainWindow &operator=(const form_MainWindow &) = delete;

protected:
  void closeEvent(QCloseEvent *);

signals:
  void closeAllWindows();

public slots:
  void eventAboutWindowClosed();
  void eventChatWindowClosed(QString Destination);
  void eventFileReceiveWindowClosed(qint32 StreamID);
  void eventFileSendWindowClosed(qint32 StreamID);
  void eventTryIconDoubleClicked(enum QSystemTrayIcon::ActivationReason Reason);
  void eventUserSearchWindowClosed();
  void eventTopicSubscribeWindowClosed();
  void eventDebugWindowClosed();
  void eventAvatarImageChanged();
  void eventNicknameChanged();

private slots:
  // Windows
  void openConfigWindow();
  void openAdduserWindow();
  void openDebugMessagesWindow();
  void openAboutDialog();
  void openUserListeClicked();
  void openChatWindow(QString Destination);
  void openFileReceiveWindow(qint32 StreamID);
  void openFileSendWindow(qint32 StreamID);
  // void openUserSearchWindow();
  void openTopicSubscribeWindow();

  // Windows end
  void namingMe();
  void copyDestination();
  void SendFile();
  void closeApplication();
  void eventUserChanged();
  void muteSound();
  void showUserInfos();
  void UserPositionUP();
  void UserPositionDOWN();
  void UserInvisible(bool b);

  void connecttreeWidgetCostumPopupMenu(QPoint point);
  void deleteUserClicked();
  void renameUserCLicked();
  void addUserToBlockList();
  void updateMenu();
  void onlineComboBoxChanged();
  void toggleVisibility(QSystemTrayIcon::ActivationReason e);
  void toggleVisibilitycontextmenu();
  void OnlineStateChanged();
  //
signals:
  void changeAllowIncoming(bool);

private:
  void initStyle();
  void initTryIconMenu();
  void initTryIcon();
  void initToolBars();

  CCore *Core;
  bool applicationIsClosing;

  form_newUserWindow *newUserWindow;

  QSystemTrayIcon *trayIcon;
  QAction *toggleVisibilityAction, *toolAct;
  QAction *toggleMuteAction;
  QMenu *menu;
  QString mLastDestinationWithUnreadMessages;

  // windows
  QMap<QString, form_ChatWidget *> mAllOpenChatWindows;
  QMap<qint32, form_fileReceive *> mAllFileReceiveWindows;
  QMap<qint32, form_fileSend *> mAllFileSendWindows;

  form_userSearch *mUserSearchWindow;
  form_topicSubscribe *mTopicSubscribeWindow;
  form_About *mAboutWindow;
  form_DebugMessages *mDebugWindow;

  QByteArray avatarImageByteArray2;

  bool Mute;
};
#endif
