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

#include <QSystemTrayIcon>

#include "UserManager.h"
#include "form_Main.h"
#include <QIcon>

form_MainWindow::form_MainWindow(QString configDir, QWidget *parent)
    : QMainWindow(parent) {
  setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint |
                 Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
  setupUi(this); // this sets up GUI

  QApplication::setQuitOnLastWindowClosed(false);
  Core = new CCore(configDir);
  connect(Core, SIGNAL(signUserStatusChanged()), this,
          SLOT(eventUserChanged()));
  connect(this, SIGNAL(changeAllowIncoming(bool)), Core,
          SLOT(changeAccessIncomingUsers(bool)));
  connect(Core, SIGNAL(signOnlineStatusChanged()), this,
          SLOT(OnlineStateChanged()));

  connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
          SLOT(openUserListeClicked()));

  connect(listWidget, SIGNAL(customContextMenuRequested(QPoint)), this,
          SLOT(connecttreeWidgetCostumPopupMenu(QPoint)));

  connect(comboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onlineComboBoxChanged()));

  connect(Core, SIGNAL(signOwnAvatarImageChanged()), this,
          SLOT(eventAvatarImageChanged()));

  connect(Core, SIGNAL(signNicknameChanged()), this,
          SLOT(eventNicknameChanged()));

  mUserSearchWindow = NULL;
  mTopicSubscribeWindow = NULL;
  mAboutWindow = NULL;
  mDebugWindow = NULL;

  Mute = false;
  applicationIsClosing = false;

  initStyle();
  initTryIconMenu();
  initTryIcon();
  initToolBars();

  Core->setOnlineStatus(USEROFFLINE);
  eventUserChanged();
  eventNicknameChanged(); //
  eventAvatarImageChanged();
}

form_MainWindow::~form_MainWindow() {
  applicationIsClosing = true;

  delete Core;
  delete trayIcon;
  this->close();
}

void form_MainWindow::onlineComboBoxChanged() {
  QComboBox *comboBox = this->comboBox;
  QString text = comboBox->currentText();

  if (text.contains(tr("Online"), Qt::CaseInsensitive) == true) {
    if (Core->getOnlineStatus() != User::USERONLINE)
      Core->setOnlineStatus(User::USERONLINE);
  } else if (text.contains(tr("Want to chat"), Qt::CaseInsensitive) == true) {
    if (Core->getOnlineStatus() != User::USERWANTTOCHAT)
      Core->setOnlineStatus(User::USERWANTTOCHAT);
  } else if (text.contains(tr("Away"), Qt::CaseInsensitive) == true) {
    if (Core->getOnlineStatus() != User::USERAWAY)
      Core->setOnlineStatus(User::USERAWAY);
  } else if (text.contains(tr("disturb"), Qt::CaseInsensitive) == true) {
    if (Core->getOnlineStatus() != User::USERDONT_DISTURB)
      Core->setOnlineStatus(User::USERDONT_DISTURB);
  } else if (text.contains(tr("Invisible"), Qt::CaseInsensitive) == true) {
    if (Core->getOnlineStatus() != User::USERINVISIBLE)
      Core->setOnlineStatus(User::USERINVISIBLE);
  } else if (text.contains(tr("Offline"), Qt::CaseInsensitive) == true) {
    if (Core->getFileTransferManager()
            ->checkIfAFileTransferOrReceiveisActive() == false) {
      if (Core->getOnlineStatus() != User::USEROFFLINE)
        Core->setOnlineStatus(User::USEROFFLINE);
    } else {
      QMessageBox *msgBox = new QMessageBox(NULL);
      msgBox->setIcon(QMessageBox::Information);
      msgBox->setText(tr("I2PChat"));
      msgBox->setInformativeText(tr("File transfer is in progress - cannot "
                                    "quit.\nAbort the transfer first."));
      msgBox->setStandardButtons(QMessageBox::Ok);
      msgBox->setDefaultButton(QMessageBox::Ok);
      msgBox->setWindowModality(Qt::NonModal);
      msgBox->show();
      OnlineStateChanged();
    }
  } else if (text.contains(tr("Connecting"), Qt::CaseInsensitive) == true) {
  }
}

void form_MainWindow::initToolBars() {
  // toolBar->setIconSize(QSize(24, 24));
  QSettings settings(Core->getConfigPath() + "/application.ini",
                     QSettings::IniFormat);
  QToolBar *toolBar = this->toolBar;

  toolBar->setFixedHeight(32);
  toolBar->setContextMenuPolicy(Qt::CustomContextMenu);
  toolBar->addAction(QIcon(ICON_NEWUSER), tr("Add User"), this,
                     SLOT(openAdduserWindow()));

  /* User search disabled.. can we re-enable this?
      {
          settings.beginGroup("Usersearch");
          if((settings.value("Enabled",true).toBool()) ==true){
              if(Core->getUserInfos().Nickname.isEmpty()==false){
                  toolBar->addAction(QIcon(ICON_USERSEARCH),tr("Users Search")
     ,this,SLOT(openUserSearchWindow())); }else{ QMessageBox* msgBox= new
     QMessageBox(NULL); msgBox->setIcon(QMessageBox::Information);
                  msgBox->setText(tr("I2PChat"));
                  msgBox->setInformativeText(tr("You have to enter a Nickname
     (at User Details) to use the User Search. User Search deactivated"));
                  msgBox->setStandardButtons(QMessageBox::Ok);
                  msgBox->setDefaultButton(QMessageBox::Ok);
                  msgBox->setWindowModality(Qt::NonModal);
                  msgBox->exec();
              }
          }
          settings.endGroup();
      }
  */

  /* Topics appears broken.. disabling for now */
  /*
      {
          settings.beginGroup("Topics");
          if((settings.value("Enabled",true).toBool())){
              toolBar->addAction(QIcon(ICON_USERSEARCH), tr("Subscribe to
     Topic"), this, SLOT(openTopicSubscribeWindow()));
          }
          settings.endGroup();
          settings.sync();
      }
  */

  toolBar->addAction(QIcon(ICON_MYDESTINATION),
                     tr("Copy Destination to clipboard"), this,
                     SLOT(namingMe()));
  toolBar->addAction(QIcon(ICON_SETTINGS), tr("Settings"), this,
                     SLOT(openConfigWindow()));
  toolBar->addAction(QIcon(ICON_DEBUGMESSAGES), tr("Debug Messages"), this,
                     SLOT(openDebugMessagesWindow()));
  toolBar->addAction(QIcon(ICON_ABOUT), tr("About I2PChat"), this,
                     SLOT(openAboutDialog()));
  toolBar->addAction(QIcon(ICON_CLOSE), tr("Quit I2PChat"), this,
                     SLOT(closeApplication()));
}

void form_MainWindow::openConfigWindow() {

  form_settingsgui *dialog = new form_settingsgui(*Core);
  connect(this, SIGNAL(closeAllWindows()), dialog, SLOT(close()));

  dialog->show();
}
void form_MainWindow::openAdduserWindow() {
  form_newUserWindow *dialog = new form_newUserWindow(*Core);

  connect(this, SIGNAL(closeAllWindows()), dialog, SLOT(close()));

  dialog->show();
}

void form_MainWindow::openDebugMessagesWindow() {
  if (mDebugWindow == NULL) {
    mDebugWindow = new form_DebugMessages(*Core);

    connect(this, SIGNAL(closeAllWindows()), mDebugWindow, SLOT(close()));

    connect(mDebugWindow, SIGNAL(closingDebugWindow()), this,
            SLOT(eventDebugWindowClosed()));

    mDebugWindow->show();
  } else {
    mDebugWindow->getFocus();
  }
}

void form_MainWindow::namingMe() {
  QClipboard *clipboard = QApplication::clipboard();
  QString Destination = Core->getMyDestination();
  QPixmap pixmap = QPixmap(":/icons/avatar.svg");
  setWindowIcon(QIcon(pixmap));
  if (Destination != "") {
    clipboard->setText(Destination);
    QMessageBox::information(
        this, "", tr("\nYour Destination has been copied to the clipboard"),
        QMessageBox::Close);
  } else {
    QMessageBox::information(this, "",
                             tr("\nYou must be online to copy Destination"),
                             QMessageBox::Close);
  }
}
void form_MainWindow::closeApplication() {
  QPixmap pixmap = QPixmap(":/icons/avatar.svg");
  setWindowIcon(QIcon(pixmap));
  if (Core->getFileTransferManager()->checkIfAFileTransferOrReceiveisActive() ==
      false) {

    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setIcon(QMessageBox::Question);
    msgBox->setText(tr("\nAre you sure you wish to quit?"));
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::Yes);
    msgBox->setWindowModality(Qt::WindowModal);
    int ret = msgBox->exec();
    if (ret == QMessageBox::No) {
      return;
    }

    applicationIsClosing = true;
    emit closeAllWindows();

    delete Core;
    Core = 0;
    delete trayIcon;
    trayIcon = 0;

    this->close();
  } else {
    QMessageBox *msgBox = new QMessageBox(NULL);
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setText(
        tr("\nFile transfer in progress...\nCancel transfer first!"));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->exec();
  }
}

void form_MainWindow::eventUserChanged() {

  mLastDestinationWithUnreadMessages = "";
  bool showUnreadMessageAtTray = false;
  QList<CUser *> users = Core->getUserManager()->getUserList();
  QList<CFileTransferReceive *> FileReceives =
      Core->getFileTransferManager()->getFileTransferReceiveList();
  QList<CFileTransferSend *> FileSends =
      Core->getFileTransferManager()->getFileTransferSendsList();

  listWidget->clear();

  for (int i = 0; i < users.count(); i++) {
    // USERS
    QListWidgetItem *newItem = new QListWidgetItem(listWidget);
    QListWidgetItem *ChildWidthI2PDestinationAsText =
        new QListWidgetItem(listWidget);
    QListWidgetItem *ChildWidthTyp = new QListWidgetItem(listWidget);

    if (users.at(i)->getHaveNewUnreadChatmessages() == true) {
      newItem->setIcon(QIcon(ICON_NEWUNREADMESSAGE));
      showUnreadMessageAtTray = true;
      mLastDestinationWithUnreadMessages = users.at(i)->getI2PDestination();
    } else
      switch (users.at(i)->getOnlineState()) {

      case USERTRYTOCONNECT: {
        newItem->setIcon(QIcon(ICON_USER_OFFLINE));
        break;
      }
      case USERINVISIBLE:
      case USEROFFLINE: {
        newItem->setIcon(QIcon(ICON_USER_OFFLINE));
        break;
      }
      case USERONLINE: {
        newItem->setIcon(QIcon(ICON_USER_ONLINE));
        break;
      }
      case USERWANTTOCHAT: {
        newItem->setIcon(QIcon(ICON_USER_WANTTOCHAT));
        break;
      }
      case USERAWAY: {
        newItem->setIcon(QIcon(ICON_USER_AWAY));
        break;
      }
      case USERDONT_DISTURB: {
        newItem->setIcon(QIcon(ICON_USER_DONT_DISTURB));
        break;
      }
      case USERBLOCKEDYOU: {
        newItem->setIcon(QIcon(ICON_USER_BLOCKED_YOU));
        break;
      }
      }

    newItem->setTextAlignment(Qt::AlignLeft);
    QFont currentFont = newItem->font();
    newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    if (users.at(i)->getIsInvisible() == true) {
      currentFont.setItalic(true);
      newItem->setFont(currentFont);
    } else {
      currentFont.setItalic(false);
      newItem->setFont(currentFont);
    }

    newItem->setText(users.at(i)->getName());

    ChildWidthI2PDestinationAsText->setText(users.at(i)->getI2PDestination());
    ChildWidthI2PDestinationAsText->setHidden(true); // DEBUG
    ChildWidthTyp->setText("U");
    ChildWidthTyp->setHidden(true); // DEBUG
  }

  for (int i = 0; i < FileReceives.size(); i++) {
    // Filereceives
    QListWidgetItem *newItem = new QListWidgetItem(listWidget);
    QListWidgetItem *ChildWidthStreamIDAsText = new QListWidgetItem(listWidget);
    QListWidgetItem *ChildWidthTyp = new QListWidgetItem(listWidget);

    newItem->setIcon(QIcon(ICON_FILETRANSFER_RECEIVE));
    newItem->setText(FileReceives.at(i)->getFileName());
    newItem->setTextAlignment(Qt::AlignLeft);
    newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QString t;
    t.setNum(FileReceives.at(i)->getStreamID(), 10);

    ChildWidthStreamIDAsText->setText(t);
    ChildWidthStreamIDAsText->setHidden(true); // DEBUG
    ChildWidthTyp->setText("R");
    ChildWidthTyp->setHidden(true); // DEBUG
  }

  for (int i = 0; i < FileSends.size(); i++) {
    // Filesends
    QListWidgetItem *newItem = new QListWidgetItem(listWidget);
    QListWidgetItem *ChildWidthStreamIDAsText = new QListWidgetItem(listWidget);
    QListWidgetItem *ChildWidthTyp = new QListWidgetItem(listWidget);

    newItem->setIcon(QIcon(ICON_FILETRANSFER_SEND));
    newItem->setText(FileSends.at(i)->getFileName());
    newItem->setTextAlignment(Qt::AlignLeft);
    newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QString t;
    t.setNum(FileSends.at(i)->getStreamID(), 10);

    ChildWidthStreamIDAsText->setText(t);
    ChildWidthStreamIDAsText->setHidden(true); // DEBUG
    ChildWidthTyp->setText("S");
    ChildWidthTyp->setHidden(true); // DEBUG
  }

  if (showUnreadMessageAtTray == false) {
    OnlineStateChanged();
  } else {
    trayIcon->setIcon(QIcon(ICON_NEWUNREADMESSAGETRAY));
  }
}

void form_MainWindow::openUserListeClicked() {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 2);
  QPixmap pixmap = QPixmap(":/icons/avatar.svg");
  setWindowIcon(QIcon(pixmap));

  if (t->text() == "U") {
    // open Chatwindow
    t = listWidget->item(listWidget->currentRow() + 1);
    QString Destination = t->text();

    openChatWindow(Destination);
  } else if (t->text() == "R") {
    // openFileReceiveWindow
    t = listWidget->item(listWidget->currentRow() + 1);

    bool OK = false;
    qint32 StreamID = t->text().toInt(&OK, 10);

    if (OK == false) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.setText(tr("form_Main(openChat_or_FileReceive_Dialog))"));
      msgBox.setInformativeText(tr("can't parse value: %1").arg(t->text()));
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.exec();
    }
    openFileReceiveWindow(StreamID);
  } else if (t->text() == "S") {
    // openFileSendWindow
    t = listWidget->item(listWidget->currentRow() + 1);

    bool OK = false;
    qint32 StreamID = t->text().toInt(&OK, 10);

    if (OK == false) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.setText(tr("form_Main(openChat_or_FileReceive_Dialog)"));
      msgBox.setInformativeText(tr("can't parse value: %1").arg(t->text()));
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.exec();
    }
    openFileSendWindow(StreamID);
  }
}
void form_MainWindow::connecttreeWidgetCostumPopupMenu(QPoint point) {
  QListWidget *listWidget = this->listWidget;

  if (listWidget->count() == 0)
    return;

  QMenu contextMnu(this);
  QMenu contextMnuPos("Position", this);

  QMouseEvent *mevent =
      new QMouseEvent(QEvent::MouseButtonPress, point, Qt::RightButton,
                      Qt::RightButton, Qt::NoModifier);

  QAction *UserChat = new QAction(QIcon(ICON_CHAT), tr("Chat"), this);
  connect(UserChat, SIGNAL(triggered()), this, SLOT(openUserListeClicked()));

  QAction *UserDelete =
      new QAction(QIcon(ICON_USER_DELETE), tr("Delete"), this);
  connect(UserDelete, SIGNAL(triggered()), this, SLOT(deleteUserClicked()));

  QAction *UserRename =
      new QAction(QIcon(ICON_USER_RENAME), tr("Rename"), this);
  connect(UserRename, SIGNAL(triggered()), this, SLOT(renameUserCLicked()));

  QAction *CopyDestination =
      new QAction(QIcon(ICON_COPYBASE64), tr("Copy Destination"), this);
  connect(CopyDestination, SIGNAL(triggered()), this, SLOT(copyDestination()));

  QAction *ShowUserInfos = new QAction(QIcon(ICON_ABOUT), tr("Userinfo"), this);
  connect(ShowUserInfos, SIGNAL(triggered()), this, SLOT(showUserInfos()));

  QAction *UserInvisible =
      new QAction(QIcon(ICON_USER_INVISIBLE), tr("Invisible"), this);
  UserInvisible->setCheckable(true);
  connect(UserInvisible, SIGNAL(triggered(bool)), this,
          SLOT(UserInvisible(bool)));

  QAction *UserToBlockList = new QAction(QIcon(ICON_BLOCK), tr("Block"), this);
  connect(UserToBlockList, SIGNAL(triggered()), this,
          SLOT(addUserToBlockList()));

  // for contextMnuPos
  QAction *UP = new QAction(tr("Up"), this);
  connect(UP, SIGNAL(triggered()), this, SLOT(UserPositionUP()));
  QAction *DOWN = new QAction(tr("Down"), this);
  connect(DOWN, SIGNAL(triggered()), this, SLOT(UserPositionDOWN()));

  contextMnu.clear();
  contextMnu.addAction(UserChat);

  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 2);

  if (t->text() == "U") {
    QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 1);
    QString Destination = t->text();

    CUser *User;
    User = Core->getUserManager()->getUserByI2P_Destination(Destination);

    if (User->getConnectionStatus() == ONLINE) {
      QAction *UserSendFile =
          new QAction(QIcon(ICON_FILETRANSFER_SEND), tr("SendFile"), this);
      connect(UserSendFile, SIGNAL(triggered()), this, SLOT(SendFile()));
      contextMnu.addAction(UserSendFile);
    }

    if (User->getIsInvisible() == true) {
      UserInvisible->setChecked(true);
    } else {
      UserInvisible->setChecked(false);
    }

    contextMnu.addAction(UserToBlockList);
    contextMnu.addSeparator();
    contextMnu.addAction(UserRename);
    contextMnu.addAction(UserDelete);
    contextMnu.addAction(CopyDestination);
    contextMnu.addAction(UserInvisible);
    contextMnu.addAction(ShowUserInfos);

    contextMnuPos.addAction(UP);
    contextMnuPos.addAction(DOWN);

    contextMnu.addMenu(&contextMnuPos);
    contextMnu.exec(mevent->globalPos());
  }
}

void form_MainWindow::deleteUserClicked() {

  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 1);
  QString Destination = t->text();
  QPixmap pixmap = QPixmap(":/icons/avatar.svg");
  setWindowIcon(QIcon(pixmap));

  QMessageBox *msgBox = new QMessageBox(this);
  msgBox->setIcon(QMessageBox::Question);
  msgBox->setText(tr("\nReally delete contact?"));
  msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox->setDefaultButton(QMessageBox::Yes);
  msgBox->setWindowModality(Qt::WindowModal);
  int ret = msgBox->exec();
  if (ret == QMessageBox::Yes) {
    Core->getUserManager()->deleteUserByI2PDestination(Destination);
  }
}

void form_MainWindow::renameUserCLicked() {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow());
  QString OldNickname = t->text();

  QListWidgetItem *t2 = listWidget->item(listWidget->currentRow() + 1);
  QString Destination = t2->text();

  form_RenameWindow *Dialog =
      new form_RenameWindow(*Core, OldNickname, Destination);
  Dialog->show();
}

void form_MainWindow::closeEvent(QCloseEvent *e) {
  if ((trayIcon != 0) && (trayIcon->isVisible())) {
    static bool firstTime = true;
    if (firstTime) {

      //            QMessageBox::information(this, tr("I2PChat System tray"),
      //            tr("Application will continue running. Quit using context
      //            menu in the system tray"));
      firstTime = false;
    }
    hide();
    e->ignore();
  }

  if (applicationIsClosing == true) {
    e->accept();
    QApplication::exit(0);
  }
}

void form_MainWindow::updateMenu() {
  toggleVisibilityAction->setText(isVisible() ? tr("Hide") : tr("Show"));
}

void form_MainWindow::toggleVisibility(QSystemTrayIcon::ActivationReason e) {
  if (mLastDestinationWithUnreadMessages.isEmpty() == false)
    return;

  static QPoint MainFormPosition = this->pos();

  if (e == QSystemTrayIcon::Trigger || e == QSystemTrayIcon::DoubleClick) {
    if (isHidden()) {
      this->move(MainFormPosition);
      show();

      if (isMinimized()) {
        if (isMaximized()) {
          showMaximized();
        } else {
          showNormal();
        }
      }
      raise();
      activateWindow();
    } else {
      MainFormPosition = this->pos();
      hide();
    }
  }
}

void form_MainWindow::toggleVisibilitycontextmenu() {
  if (isVisible())
    hide();
  else
    show();
}

void form_MainWindow::OnlineStateChanged() {
  disconnect(comboBox, SIGNAL(currentIndexChanged(int)), this,
             SLOT(onlineComboBoxChanged()));

  QComboBox *comboBox = this->comboBox;
  ONLINESTATE onlinestatus = Core->getOnlineStatus();

  if (onlinestatus == User::USERTRYTOCONNECT) {
    comboBox->clear();
    comboBox->addItem(QIcon(ICON_USER_TRYTOCONNECT),
                      tr("Connecting..."));                     // index 0
    comboBox->addItem(QIcon(ICON_USER_OFFLINE), tr("Offline")); // 1
    comboBox->setCurrentIndex(0);
    trayIcon->setIcon(QIcon(ICON_USER_TRYTOCONNECT));
  } else {
    if (comboBox->count() < 6) {
      comboBox->clear();

      comboBox->addItem(QIcon(ICON_USER_ONLINE), tr("Online")); // index 0
      comboBox->addItem(QIcon(ICON_USER_WANTTOCHAT), tr("Want to chat")); // 1
      comboBox->addItem(QIcon(ICON_USER_AWAY), tr("Away"));               // 2
      comboBox->addItem(QIcon(ICON_USER_DONT_DISTURB),
                        tr("No disturbo"));                           // 3
      comboBox->addItem(QIcon(ICON_USER_INVISIBLE), tr("Invisible")); // 4
      comboBox->addItem(QIcon(ICON_USER_OFFLINE), tr("Offline"));     // 5
    }

    if (onlinestatus == User::USERONLINE) {
      comboBox->setCurrentIndex(0);
      trayIcon->setIcon(QIcon(ICON_USER_ONLINE));
    } else if (onlinestatus == User::USERWANTTOCHAT) {
      comboBox->setCurrentIndex(1);
      trayIcon->setIcon(QIcon(ICON_USER_WANTTOCHAT));
    } else if (onlinestatus == User::USERAWAY) {
      comboBox->setCurrentIndex(2);
      trayIcon->setIcon(QIcon(ICON_USER_AWAY));
    } else if (onlinestatus == User::USERDONT_DISTURB) {
      comboBox->setCurrentIndex(3);
      trayIcon->setIcon(QIcon(ICON_USER_DONT_DISTURB));
    } else if (onlinestatus == User::USERINVISIBLE) {
      comboBox->setCurrentIndex(4);
      trayIcon->setIcon(QIcon(ICON_USER_INVISIBLE));
    } else if (onlinestatus == User::USEROFFLINE) {
      comboBox->setCurrentIndex(5);
      trayIcon->setIcon(QIcon(ICON_USER_OFFLINE));
    }
  }
  connect(comboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onlineComboBoxChanged()));
}

void form_MainWindow::openAboutDialog() {
  if (mAboutWindow == NULL) {
    mAboutWindow =
        new form_About(Core->getClientVersion(), Core->getProtocolVersion(),
                       FileTransferProtocol::MINPROTOCOLVERSION,
                       FileTransferProtocol::MAXPROTOCOLVERSION);
    connect(this, SIGNAL(closeAllWindows()), mAboutWindow, SLOT(close()));

    connect(mAboutWindow, SIGNAL(closingAboutWindow()), this,
            SLOT(eventAboutWindowClosed()));

    mAboutWindow->show();
  } else {
    mAboutWindow->getFocus();
    ;
  }
}

void form_MainWindow::initStyle() {
  // commented out the style stuff as styles on "ubuntu 14.04 mate" make the
  // tray icon not appear
  /*
  QSettings * settings=new
  QSettings(Core->getConfigPath()+"/application.ini",QSettings::IniFormat);
  settings->beginGroup("General");
  //Load Style
  QString Style=(settings->value("current_Style","")).toString();
  if(Style.isEmpty()==true)
  {
      //find default Style for this System
      QRegExp regExp("Q(.*)Style");
      Style = QApplication::style()->metaObject()->className();

      if (Style == QLatin1String("QMacStyle"))
          Style = QLatin1String("Macintosh (Aqua)");
      else if (regExp.exactMatch(Style))
          Style = regExp.cap(1);

      //styleCombo->addItems(QStyleFactory::keys());
  }

  qApp->setStyle(Style);
  //Load Style end

  //Load Stylesheet
  QFile file(Core->getConfigPath() + "/qss/" +
             settings->value("current_Style_sheet","Default").toString() +
  ".qss");

  file.open(QFile::ReadOnly);
  QString styleSheet = QLatin1String(file.readAll());
  qApp->setStyleSheet(styleSheet);
  //load Stylesheet end
  settings->endGroup();
      settings->sync();
  delete settings;
  */
}

void form_MainWindow::initTryIconMenu() {
  // Tray icon Menu
  menu = new QMenu(this);
  QObject::connect(menu, SIGNAL(aboutToShow()), this, SLOT(updateMenu()));
  toggleVisibilityAction =
      menu->addAction(QIcon(ICON_CHAT), tr("Show/Hide"), this,
                      SLOT(toggleVisibilitycontextmenu()));

  toggleMuteAction = menu->addAction(QIcon(ICON_SOUND_ON), tr("Sound on"), this,
                                     SLOT(muteSound()));
  menu->addSeparator();
  // menu->addAction(QIcon(ICON_MINIMIZE), tr("Minimize"), this,
  // SLOT(showMinimized())); menu->addAction(QIcon(ICON_MAXIMIZE),
  // tr("Maximize"), this, SLOT(showMaximized()));
  menu->addSeparator();
  menu->addAction(QIcon(ICON_CLOSE), tr("&Quit"), this,
                  SLOT(closeApplication()));
}

void form_MainWindow::initTryIcon() {
  // Create the tray icon
  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setToolTip(tr("I2PChat"));
  trayIcon->setContextMenu(menu);
  trayIcon->setIcon(QIcon(ICON_CHAT));

  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
          SLOT(toggleVisibility(QSystemTrayIcon::ActivationReason)));

  connect(
      trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
      SLOT(eventTryIconDoubleClicked(enum QSystemTrayIcon::ActivationReason)));

  trayIcon->show();
}

void form_MainWindow::SendFile() {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 1);
  QString Destination = t->text();
  QString FilePath = QFileDialog::getOpenFileName(this, tr("Open File"), ".",
                                                  tr("all Files (*)"));

  if (FilePath.endsWith("/") == true) {
    // only a directory ,- dont send it
    return;
  }

  if (Destination.length() == 516) {
    if (!FilePath.isEmpty())
      try {
        Core->getFileTransferManager()->addNewFileTransfer(FilePath,
                                                           Destination);
      } catch (std::exception &e) {
        qWarning() << "\nform_MainWindow::SendFile()\n"
                   << e.what() << "Destination: " << Destination << "\n"
                   << "\naction aported";
      }
  } else {
    qWarning() << "\nform_MainWindow::SendFile()\n"
               << "Destination.legth!=516\n"
               << "Destination: " << Destination << "\n"
               << "\naction aported";
  }
}

void form_MainWindow::copyDestination() {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 1);
  QString Destination = t->text();

  QClipboard *clipboard = QApplication::clipboard();

  clipboard->setText(Destination);
  QMessageBox::information(this, "",
                           tr("\nContact's Destination copied to clipboard"),
                           QMessageBox::Close);
}

void form_MainWindow::muteSound() {
  if (this->Mute == false) {
    toggleMuteAction->setIcon(QIcon(ICON_SOUND_OFF));
    toggleMuteAction->setText(tr("Sound off"));
    Mute = true;
  } else {
    toggleMuteAction->setIcon(QIcon(ICON_SOUND_ON));
    toggleMuteAction->setText(tr("Sound on"));
    Mute = false;
  }
  Core->getSoundManager()->doMute(Mute);
}

void form_MainWindow::showUserInfos() {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 1);
  QString Destination = t->text();
  QString UserInfos;
  QPixmap avatar;
  CUser *user;

  user = Core->getUserManager()->getUserByI2P_Destination(Destination);
  UserInfos =
      Core->getUserManager()->getUserInfosByI2P_Destination(Destination);

  avatar.loadFromData(user->getReceivedUserInfos().AvatarImage);

  QMessageBox msgBox;
  if (avatar.isNull() == true) {
    msgBox.setIcon(QMessageBox::Information);
  } else {
    msgBox.setIconPixmap(avatar);
  }
  msgBox.setText("");
  msgBox.setInformativeText(UserInfos);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  msgBox.setWindowModality(Qt::NonModal);
  msgBox.exec();
}

void form_MainWindow::UserPositionUP() {
  QListWidget *listWidget = this->listWidget;
  if (listWidget->currentRow() >= 1)
    Core->getUserManager()->changeUserPositionInUserList(
        listWidget->currentRow() / 3, listWidget->currentRow() / 3 - 1);
}

void form_MainWindow::UserPositionDOWN() {
  QListWidget *listWidget = this->listWidget;
  if (listWidget->currentRow() < (listWidget->count() / 3) - 1)
    Core->getUserManager()->changeUserPositionInUserList(
        listWidget->currentRow() / 3, listWidget->currentRow() / 3 + 1);
}

void form_MainWindow::UserInvisible(bool b) {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 1);
  QString Destination = t->text();

  CUser *User;
  User = Core->getUserManager()->getUserByI2P_Destination(Destination);
  if (User != NULL) {
    User->setInvisible(b);
  }
}

void form_MainWindow::eventChatWindowClosed(QString Destination) {
  if (mAllOpenChatWindows.contains(Destination) == true) {
    // delete (mAllOpenChatWindows.value(Destination)); // TODO: What for is
    // what added? mAllOpenChatWindows.remove(Destination);
    mAllOpenChatWindows[Destination]->hide();
  } else {
    qCritical() << "form_MainWindow::eventChatWindowClosed\n"
                << "Closing a unknown ChatWindow";
  }
}

void form_MainWindow::eventTryIconDoubleClicked(
    enum QSystemTrayIcon::ActivationReason Reason) {
  if (Reason == QSystemTrayIcon::DoubleClick &&
      mLastDestinationWithUnreadMessages.isEmpty() == false) {
    openChatWindow(mLastDestinationWithUnreadMessages);
  }
}

void form_MainWindow::openChatWindow(QString Destination) {
  CUser *User;
  User = Core->getUserManager()->getUserByI2P_Destination(Destination);
  if (User == NULL) {
    qCritical() << "form_MainWindow::openChatWindow"
                << "try to open a Chatwindow, but the user doesn't exist";
    return;
  }

  if (mAllOpenChatWindows.contains(Destination) == false) {
    // create new chatWindow
    form_ChatWidget *tmp = new form_ChatWidget(*User, *Core);
    connect(this, SIGNAL(closeAllWindows()), tmp, SLOT(close()));

    connect(tmp, SIGNAL(closingChatWindow(QString)), this,
            SLOT(eventChatWindowClosed(QString)));

    connect(Core, SIGNAL(signOwnAvatarImageChanged()), tmp,
            SLOT(slotLoadOwnAvatarImage()));

    mAllOpenChatWindows.insert(Destination, tmp);
    tmp->show();
  } else {
    // open the existing chatwindow
    mAllOpenChatWindows.value(Destination)->show();
    mAllOpenChatWindows.value(Destination)->getFocus();
  }
}

void form_MainWindow::eventFileReceiveWindowClosed(qint32 StreamID) {
  if (mAllFileReceiveWindows.contains(StreamID) == true) {
    delete (mAllFileReceiveWindows.value(StreamID));
    mAllFileReceiveWindows.remove(StreamID);
  } else {
    qCritical() << "form_MainWindow::eventFileReceiveWindowClose\n"
                << "Closing a unknown FileReceiveWindow";
  }
}

void form_MainWindow::eventFileSendWindowClosed(qint32 StreamID) {
  if (mAllFileSendWindows.contains(StreamID) == true) {
    delete mAllFileSendWindows.value(StreamID);
    mAllFileSendWindows.remove(StreamID);
  } else {
    qCritical() << "form_MainWindow::eventFileSendWindowClosed\n"
                << "Closing a unknown FileSendWindow";
  }
}

void form_MainWindow::openFileSendWindow(qint32 StreamID) {
  CFileTransferSend *TransferSend =
      Core->getFileTransferManager()->getFileTransferSendsByID(StreamID);

  if (TransferSend == NULL) {
    qCritical() << "form_MainWindow::openFileSendWindow\n"
                << "Can't find FileSend Object with ID: " << StreamID
                << "\nAction Ignored";
    return;
  }

  if (mAllFileSendWindows.contains(StreamID) == false) {
    // open new FileSendWindow
    form_fileSend *Dialog = new form_fileSend(*TransferSend);
    connect(Dialog, SIGNAL(closingFileSendWindow(qint32)), this,
            SLOT(eventFileSendWindowClosed(qint32)));

    mAllFileSendWindows.insert(StreamID, Dialog);
    Dialog->show();

  } else {
    mAllFileSendWindows.value(StreamID)->getFocus();
  }
}

void form_MainWindow::openFileReceiveWindow(qint32 StreamID) {
  CFileTransferReceive *receive =
      Core->getFileTransferManager()->getFileTransferReceiveByID(StreamID);
  if (receive == NULL) {
    qCritical() << "form_MainWindow::openFileReceiveWindow\n"
                << "Can't find FileReceive Object with ID: " << StreamID
                << "\nAction Ignored";
    return;
  }

  if (mAllFileReceiveWindows.contains(StreamID) == false) {
    // create new FileReceiveWindow
    form_fileReceive *Dialog = new form_fileReceive(*receive);

    connect(Dialog, SIGNAL(closingFileReceiveWindow(qint32)), this,
            SLOT(eventFileReceiveWindowClosed(qint32)));

    mAllFileReceiveWindows.insert(StreamID, Dialog);
    Dialog->show();
    Dialog->start();

  } else {
    // open the existing FileReceiveWindow
    mAllFileReceiveWindows.value(StreamID)->getFocus();
  }
}

void form_MainWindow::addUserToBlockList() {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 2);

  if (t->text() == "U") {
    // open Chatwindow
    t = listWidget->item(listWidget->currentRow() + 1);
    QString Destination = t->text();

    CUser *User = Core->getUserManager()->getUserByI2P_Destination(Destination);
    CUserBlockManager *BlockManager = Core->getUserBlockManager();

    if (User != NULL && BlockManager != NULL) {
      BlockManager->addNewBlockEntity(User->getName(), Destination);
    }
  }
}
/*
void form_MainWindow::openUserSearchWindow()
{
    ONLINESTATE currentState=Core->getOnlineStatus();

    if(currentState==USEROFFLINE || currentState==USERTRYTOCONNECT){
        QMessageBox::information(this, "",
                                 tr("Your Client must be Online for
that"),QMessageBox::Close); return;
    }

    if(mUserSearchWindow==NULL){

        mUserSearchWindow= new
form_userSearch(*Core,*(Core->getSeedlessManager()));
        connect(this,SIGNAL(closeAllWindows()),mUserSearchWindow,
                SLOT(close()));

        connect(mUserSearchWindow,SIGNAL(signClosingUserSearchWindow()),this,
                SLOT(eventUserSearchWindowClosed()));
        mUserSearchWindow->show();
    }else {
        mUserSearchWindow->getFocus();
    }
}
*/
void form_MainWindow::eventUserSearchWindowClosed() {
  delete mUserSearchWindow;
  mUserSearchWindow = NULL;
}

void form_MainWindow::eventTopicSubscribeWindowClosed() {
  delete mTopicSubscribeWindow;
  mTopicSubscribeWindow = NULL;
}

void form_MainWindow::eventAboutWindowClosed() {
  delete mAboutWindow;
  mAboutWindow = NULL;
}
void form_MainWindow::eventDebugWindowClosed() {
  delete mDebugWindow;
  mDebugWindow = NULL;
}

void form_MainWindow::eventNicknameChanged() {
  nicknamelabel->setText(Core->getUserInfos().Nickname);
}

void form_MainWindow::eventAvatarImageChanged() {
  if (Core->getUserInfos().AvatarImage.isEmpty() == false) {
    QPixmap avatar;
    avatar.loadFromData(Core->getUserInfos().AvatarImage);
    avatar = avatar.scaled(avatarlabel->width(), avatarlabel->height(),
                           Qt::KeepAspectRatio);
    avatarlabel->setAlignment(Qt::AlignCenter);
    avatarlabel->setPixmap(avatar);
  }
}

void form_MainWindow::openTopicSubscribeWindow() {
  ONLINESTATE currentState = Core->getOnlineStatus();

  if (currentState == USEROFFLINE || currentState == USERTRYTOCONNECT) {
    QMessageBox::information(this, "", tr("You must be connected for this"),
                             QMessageBox::Close);
    return;
  }

  if (mTopicSubscribeWindow == NULL) {

    mTopicSubscribeWindow = new form_topicSubscribe(*Core);

    connect(this, SIGNAL(closeAllWindows()), mTopicSubscribeWindow,
            SLOT(close()));

    connect(mTopicSubscribeWindow, SIGNAL(signClosingTopicSubscribeWindow()),
            this, SLOT(eventTopicSubscribeWindowClosed()));
    mTopicSubscribeWindow->show();
  } else {
    mTopicSubscribeWindow->requestFocus();
  }
}
