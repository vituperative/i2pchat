// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "About.h"
#include "ChatWidget.h"
#include "Core.h"
#include "DebugMessages.h"
#include "FileReceive.h"
#include "FileSend.h"
#include "FileTransferReceive.h"
#include "FileTransferSend.h"
#include "NewUserWindow.h"
#include "RenameWindow.h"
#include "SettingsGui.h"
#include "StatusNotifier.h"
#include "User.h"
#include "gui_icons.h"
#include "ui_MainWindow.h"

#include <QClipboard>
#include <QCloseEvent>
#include <QCursor>
#include <QEvent>
#include <QIcon>
#include <QMap>
#include <QMenu>
#include <QMessageBox>
#include <QPoint>
#include <QSettings>

class MainWindow : public QMainWindow, private Ui::MainWindow {
  Q_OBJECT

public:
  MainWindow(const QString &configDir, QWidget *parent = 0);
  ~MainWindow();

  // forbid some operators
  MainWindow(const MainWindow &) = delete;
  MainWindow &operator=(const MainWindow &) = delete;

protected:
  void closeEvent(QCloseEvent *);
  bool eventFilter(QObject *obj, QEvent *event);

signals:
  void closeAllWindows();

public slots:
  void eventAboutWindowClosed();
  void eventChatWindowClosed(const QString &Destination);
  void eventFileReceiveWindowClosed(qint32 StreamID);
  void eventFileSendWindowClosed(qint32 StreamID);
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
  void openChatWindow(const QString &Destination);
  void openFileReceiveWindow(qint32 StreamID);
  void openFileSendWindow(qint32 StreamID);

  // Windows end
  void namingMe();
  void copyDestination();
  void copyB32();
  void UserAutoDownload(bool enabled);
  void closeApplication();
  void eventUserChanged();
  void muteSound();
  void showUserInfos();
  void UserPositionUP();
  void UserPositionDOWN();
  void UserPositionTOP();
  void UserPositionBOTTOM();
  void UserInvisible(bool b);

  void connecttreeWidgetCostumPopupMenu(QPoint point);
  void slotLoadOwnAvatarImage();
  void deleteUserClicked();
  void renameUserCLicked();
  void addUserToBlockList();
  void updateMenu();
  void onlineComboBoxChanged();
  void toggleAllWindows();
  void showTrayMenu(int x, int y);
  void OnlineStateChanged();
  void incomingUserAuthorizationRequest(const QString &destination, int streamID, const QByteArray &data);
  //
signals:
  void changeAllowIncoming(bool);

private:
  void initTryIconMenu();
  void initTryIcon();
  void initToolBars();

  void setTrayIcon(const QIcon &icon);

  CCore *Core;
  bool applicationIsClosing;

  NewUserWindow *newUserWindow;

  CStatusNotifier *mStatusNotifier;
  QAction *toggleVisibilityAction, *toolAct;
  QAction *toggleMuteAction;
  QMenu *menu;
  QString mLastDestinationWithUnreadMessages;

  // windows
  QMap<QString, ChatWidget *> mAllOpenChatWindows;
  QMap<qint32, FileReceive *> mAllFileReceiveWindows;
  QMap<qint32, FileSend *> mAllFileSendWindows;

  About *mAboutWindow;
  DebugMessages *mDebugWindow;

  bool Mute;
  QMessageBox *mAuthDialog;
};
#endif
