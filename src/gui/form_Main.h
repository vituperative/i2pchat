// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FORM_MAIN_H
#define FORM_MAIN_H

#include "Core.h"
#include "FileTransferReceive.h"
#include "FileTransferSend.h"
#include "User.h"
#include "form_DebugMessages.h"
#include "form_TopicSubscribe.h"
#include "form_about.h"
#include "form_chatwidget.h"
#include "form_fileReceive.h"
#include "form_fileSend.h"
#include "form_newUser.h"
#include "form_rename.h"
#include "form_settingsgui.h"
#include "gui_icons.h"
#include "ui_form_Main.h"

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

class form_MainWindow : public QMainWindow, private Ui::form_MainWindow {
  Q_OBJECT

public:
  form_MainWindow(const QString &configDir, QWidget *parent = 0);
  ~form_MainWindow();

  // forbid some operators
  form_MainWindow(const form_MainWindow &) = delete;
  form_MainWindow &operator=(const form_MainWindow &) = delete;

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
  void eventTryIconDoubleClicked(enum QSystemTrayIcon::ActivationReason Reason);
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
  void openChatWindow(const QString &Destination);
  void openFileReceiveWindow(qint32 StreamID);
  void openFileSendWindow(qint32 StreamID);
  void openTopicSubscribeWindow();

  // Windows end
  void namingMe();
  void copyDestination();
  void copyB32();
  void UserAutoDownload(bool enabled);
  void SendFile();
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
  void toggleVisibility(QSystemTrayIcon::ActivationReason e);
  void toggleVisibilitycontextmenu();
  void OnlineStateChanged();
  void incomingUserAuthorizationRequest(const QString &destination, int streamID, const QByteArray &data);
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

  form_topicSubscribe *mTopicSubscribeWindow;
  form_About *mAboutWindow;
  form_DebugMessages *mDebugWindow;

  QByteArray avatarImageByteArray2;

  bool Mute;
};
#endif
