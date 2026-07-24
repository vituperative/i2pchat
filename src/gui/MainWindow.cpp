// SPDX-License-Identifier: AGPL-3.0-or-later

#include "MainWindow.h"

#include "Base.h"
#include "UserManager.h"

#include <QApplication>
#include <QCryptographicHash>
#include <QIcon>
#include <QMessageBox>
#include <QPixmap>

static const QPixmap &avatarPixmap() {
  static QPixmap pix(":/icons/avatar.svg");
  return pix;
}

// Scaled-down, alias-free version of newmail.svg for the taskbar tray icon.
// The SVG is rasterized at a small tray size and resampled with Lanczos to
// avoid the jaggies that appear when Qt scales the vector straight to 16-22px.
static const QPixmap &newmailTrayPixmap() {
  static QPixmap pix;
  if (pix.isNull()) {
    const int traySize = 22;
    QImage img = QIcon(":/icons/newmail.svg").pixmap(traySize, traySize).toImage();
    img = CCore::scaleImageLanczos(img, traySize, traySize);
    pix = QPixmap::fromImage(img);
  }
  return pix;
}

MainWindow::MainWindow(const QString &configDir, QWidget *parent)
  : QMainWindow(parent) {
  setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint |
                 Qt::WindowCloseButtonHint);
  setupUi(this); // this sets up GUI

  QApplication::setQuitOnLastWindowClosed(false);
  Core = new CCore(configDir);
  qApp->installEventFilter(this);
  connect(Core, SIGNAL(signUserStatusChanged()), this, SLOT(eventUserChanged()));
  connect(this, SIGNAL(changeAllowIncoming(bool)), Core, SLOT(changeAccessIncomingUsers(bool)));
  connect(Core, SIGNAL(signOnlineStatusChanged()), this, SLOT(OnlineStateChanged()));
  connect(Core,
          SIGNAL(signIncomingUserAuthorizationRequest(QString, qint32, QByteArray)),
          this,
          SLOT(incomingUserAuthorizationRequest(QString, qint32, QByteArray)));

  connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(openUserListeClicked()));

  connect(listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(connecttreeWidgetCostumPopupMenu(QPoint)));

  connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onlineComboBoxChanged()));

  connect(Core, SIGNAL(signOwnAvatarImageChanged()), this, SLOT(eventAvatarImageChanged()));

  connect(Core, SIGNAL(signNicknameChanged()), this, SLOT(eventNicknameChanged()));

  mAboutWindow = NULL;
  mDebugWindow = NULL;
  mAuthDialog = NULL;

  Mute = false;
  applicationIsClosing = false;

  initTryIconMenu();
  initTryIcon();
  initToolBars();

  Core->setOnlineStatus(USEROFFLINE);
  eventUserChanged();
  eventNicknameChanged(); //
  eventAvatarImageChanged();
}

MainWindow::~MainWindow() {
  applicationIsClosing = true;

  delete Core;
  this->close();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
  switch (event->type()) {
  case QEvent::MouseMove:
  case QEvent::MouseButtonPress:
  case QEvent::KeyPress:
  case QEvent::Wheel:
  case QEvent::TouchBegin:
    Core->resetAutoAway();
    break;
  default:
    break;
  }
  return QMainWindow::eventFilter(obj, event);
}

void MainWindow::onlineComboBoxChanged() {
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
    if (Core->getFileTransferManager()->checkActiveFileTransfer() == false) {
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
      msgBox->setAttribute(Qt::WA_DeleteOnClose);
      msgBox->show();
      OnlineStateChanged();
    }
  } else if (text.contains(tr("Connecting"), Qt::CaseInsensitive) == true) {
    if (Core->getOnlineStatus() != User::USERTRYTOCONNECT)
      Core->setOnlineStatus(User::USERTRYTOCONNECT);
  }
}

void MainWindow::initToolBars() {
  QSettings settings(Core->getConfigPath() + "/application.ini", QSettings::IniFormat);
  QToolBar *toolBar = this->toolBar;

  toolBar->setFixedHeight(32);
  toolBar->setContextMenuPolicy(Qt::CustomContextMenu);
  toolBar->addAction(QIcon(ICON_NEWUSER), tr("Add User"), this, SLOT(openAdduserWindow()));

  toolBar->addAction(QIcon(ICON_MYDESTINATION), tr("Copy Destination to clipboard"), this, SLOT(namingMe()));
  toolBar->addAction(QIcon(ICON_SETTINGS), tr("Settings"), this, SLOT(openConfigWindow()));
  toolBar->addAction(QIcon(ICON_DEBUGMESSAGES), tr("Debug Messages"), this, SLOT(openDebugMessagesWindow()));
  toolBar->addAction(QIcon(ICON_ABOUT), tr("About I2PChat"), this, SLOT(openAboutDialog()));
  toolBar->addAction(QIcon(ICON_CLOSE), tr("Quit I2PChat"), this, SLOT(closeApplication()));
}

void MainWindow::openConfigWindow() {

  SettingsGui *dialog = new SettingsGui(*Core);
  connect(this, SIGNAL(closeAllWindows()), dialog, SLOT(close()));

  dialog->show();
}
void MainWindow::openAdduserWindow() {
  NewUserWindow *dialog = new NewUserWindow(*Core);

  connect(this, SIGNAL(closeAllWindows()), dialog, SLOT(close()));

  dialog->show();
}

void MainWindow::openDebugMessagesWindow() {
  if (mDebugWindow == NULL) {
    mDebugWindow = new DebugMessages(*Core);

    connect(this, SIGNAL(closeAllWindows()), mDebugWindow, SLOT(close()));

    connect(mDebugWindow, SIGNAL(closingDebugWindow()), this, SLOT(eventDebugWindowClosed()));

    mDebugWindow->show();
  } else {
    mDebugWindow->getFocus();
  }
}

void MainWindow::namingMe() {
  QClipboard *clipboard = QApplication::clipboard();
  QString Destination = Core->getMyDestination();
  const QPixmap &pixmap = avatarPixmap();
  setWindowIcon(QIcon(pixmap));
  if (Destination != "") {
    clipboard->setText(Destination);
    QMessageBox::information(this, "", tr("\nYour Destination has been copied to the clipboard"), QMessageBox::Close);
  } else {
    QMessageBox::information(this, "", tr("\nYou must be online to copy Destination"), QMessageBox::Close);
  }
}
void MainWindow::closeApplication() {
  if (Core->getFileTransferManager()->checkActiveFileTransfer() == false) {

    QMessageBox msgBox(this);
    const QPixmap &pixmap = avatarPixmap();
    msgBox.setWindowIcon(QIcon(pixmap));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(tr("\nAre you sure you wish to quit?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setWindowModality(Qt::WindowModal);
    int ret = msgBox.exec();
    if (ret == QMessageBox::No) {
      return;
    }

    applicationIsClosing = true;
    emit closeAllWindows();

    delete Core;
    Core = 0;

    this->close();
  } else {
    QMessageBox msgBox(NULL);
    const QPixmap &pixmap = avatarPixmap();
    msgBox.setWindowIcon(QIcon(pixmap));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(tr("\nFile transfer in progress...\nCancel transfer first!"));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setWindowModality(Qt::NonModal);
    msgBox.exec();
  }
}

void MainWindow::eventUserChanged() {

  mLastDestinationWithUnreadMessages = "";
  bool showUnreadMessageAtTray = false;
  QList<CUser *> users = Core->getUserManager()->getUserList();
  QList<CFileTransferReceive *> FileReceives = Core->getFileTransferManager()->getFileTransferReceiveList();
  QList<CFileTransferSend *> FileSends = Core->getFileTransferManager()->getFileTransferSendsList();

  listWidget->clear();

  for (int i = 0; i < users.count(); i++) {
    // USERS
    QListWidgetItem *newItem = new QListWidgetItem(listWidget);
    QListWidgetItem *ChildWidthI2PDestinationAsText = new QListWidgetItem(listWidget);
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
      default:
        break;
      }

    newItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QFont currentFont = newItem->font();
    newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    /* Each user occupies 3 rows in the list widget:
       row+0: visible display name + status icon
       row+1: hidden destination string (for copyDestination / lookups)
       row+2: hidden type marker ("U"=user, "FS"=file-send, "FR"=file-receive) */
    if (users.at(i)->getIsInvisible() == true) {
      currentFont.setItalic(true);
      newItem->setFont(currentFont);
    } else {
      currentFont.setItalic(false);
      newItem->setFont(currentFont);
    }

    newItem->setText(users.at(i)->getName());

    ChildWidthI2PDestinationAsText->setText(users.at(i)->getI2PDestination());
    ChildWidthI2PDestinationAsText->setHidden(true);
    ChildWidthTyp->setText("U");
    ChildWidthTyp->setHidden(true);
  }

  for (int i = 0; i < FileReceives.size(); i++) {
    // Filereceives
    QListWidgetItem *newItem = new QListWidgetItem(listWidget);
    QListWidgetItem *ChildWidthStreamIDAsText = new QListWidgetItem(listWidget);
    QListWidgetItem *ChildWidthTyp = new QListWidgetItem(listWidget);

    newItem->setIcon(QIcon(ICON_FILETRANSFER_RECEIVE));
    newItem->setText(FileReceives.at(i)->getFileName());
    newItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
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
    newItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
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
    setTrayIcon(QIcon(newmailTrayPixmap()));
  }
}

void MainWindow::openUserListeClicked() {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 2);
  const QPixmap &pixmap = avatarPixmap();
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
      msgBox.setText(tr("MainWindow(openChat_or_FileReceive_Dialog))"));
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
      msgBox.setText(tr("MainWindow(openChat_or_FileReceive_Dialog)"));
      msgBox.setInformativeText(tr("can't parse value: %1").arg(t->text()));
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.exec();
    }
    openFileSendWindow(StreamID);
  }
}
void MainWindow::connecttreeWidgetCostumPopupMenu(QPoint point) {
  QListWidget *listWidget = this->listWidget;

  if (listWidget->count() == 0)
    return;

  QMenu contextMnu(this);
  QMenu contextMnuPos("Position", this);

  QMouseEvent mevent(QEvent::MouseButtonPress, point, Qt::RightButton, Qt::RightButton, Qt::NoModifier);

  QAction *UserChat = new QAction(QIcon(ICON_CHAT), tr("Chat"), this);
  connect(UserChat, SIGNAL(triggered()), this, SLOT(openUserListeClicked()));

  QAction *UserAutoDownload = new QAction(QIcon(ICON_USER_DOWNLOAD), tr("Auto-download"), this);
  UserAutoDownload->setCheckable(true);
  connect(UserAutoDownload, SIGNAL(triggered(bool)), this, SLOT(UserAutoDownload(bool)));
  UserAutoDownload->setEnabled(false);

  QAction *UserInvisible = new QAction(QIcon(ICON_USER_INVISIBLE), tr("Invisible"), this);
  UserInvisible->setCheckable(true);
  connect(UserInvisible, SIGNAL(triggered(bool)), this, SLOT(UserInvisible(bool)));

  QAction *UserDelete = new QAction(QIcon(ICON_USER_DELETE), tr("Delete"), this);
  connect(UserDelete, SIGNAL(triggered()), this, SLOT(deleteUserClicked()));

  QAction *UserRename = new QAction(QIcon(ICON_USER_RENAME), tr("Rename"), this);
  connect(UserRename, SIGNAL(triggered()), this, SLOT(renameUserCLicked()));

  QAction *CopyDestination = new QAction(QIcon(ICON_COPYBASE64), tr("Copy Destination"), this);
  connect(CopyDestination, SIGNAL(triggered()), this, SLOT(copyDestination()));

  QAction *CopyB32 = new QAction(QIcon(ICON_WEB), tr("Copy B32 Address"), this);
  connect(CopyB32, SIGNAL(triggered()), this, SLOT(copyB32()));
  CopyB32->setEnabled(false);

  QAction *ShowUserInfos = new QAction(QIcon(ICON_ABOUT), tr("User Info"), this);
  connect(ShowUserInfos, SIGNAL(triggered()), this, SLOT(showUserInfos()));

  QAction *UserToBlockList = new QAction(QIcon(ICON_BLOCK), tr("Block"), this);
  connect(UserToBlockList, SIGNAL(triggered()), this, SLOT(addUserToBlockList()));

  // for contextMnuPos
  QAction *UP = new QAction(tr("Up"), this);
  connect(UP, SIGNAL(triggered()), this, SLOT(UserPositionUP()));
  QAction *DOWN = new QAction(tr("Down"), this);
  connect(DOWN, SIGNAL(triggered()), this, SLOT(UserPositionDOWN()));
  QAction *TOP = new QAction(tr("Move to Top"), this);
  connect(TOP, SIGNAL(triggered()), this, SLOT(UserPositionTOP()));
  QAction *BOTTOM = new QAction(tr("Move to Bottom"), this);
  connect(BOTTOM, SIGNAL(triggered()), this, SLOT(UserPositionBOTTOM()));

  contextMnu.clear();
  contextMnu.addAction(UserChat);

  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 2);

  if (t->text() == "U") {
    QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 1);
    QString Destination = t->text();

    CUser *User;
    User = Core->getUserManager()->getUserByI2P_Destination(Destination);

    if (User->getIsInvisible() == true) {
      UserInvisible->setChecked(true);
    } else {
      UserInvisible->setChecked(false);
    }

    contextMnu.addAction(UserInvisible);
    contextMnu.addAction(UserAutoDownload);
    contextMnu.addSeparator();
    contextMnu.addAction(ShowUserInfos);
    contextMnu.addAction(CopyDestination);
    contextMnu.addAction(CopyB32);
    contextMnu.addAction(UserRename);
    contextMnu.addAction(UserDelete);
    contextMnu.addAction(UserToBlockList);

    // Enable Copy B32 only if user is online (can do naming lookup)
    if (User->getConnectionStatus() == ONLINE) {
      CopyB32->setEnabled(true);
    } else {
      CopyB32->setEnabled(false);
    }

    // Enable and set state of UserAutoDownload
    UserAutoDownload->setEnabled(true);
    UserAutoDownload->setChecked(User->getAutoDownloadEnabled());

    // Set icon based on auto-download state
    if (User->getAutoDownloadEnabled()) {
      UserAutoDownload->setIcon(QIcon(ICON_USER_DOWNLOAD));
    } else {
      UserAutoDownload->setIcon(QIcon(ICON_USER_DOWNLOAD_DISABLED));
    }

    contextMnuPos.addAction(UP);
    contextMnuPos.addAction(DOWN);
    contextMnuPos.addSeparator();
    contextMnuPos.addAction(TOP);
    contextMnuPos.addAction(BOTTOM);

    contextMnu.addMenu(&contextMnuPos);
    // TODO: Fix width of context menu and ensure sub-menu overlaps
    // contextMnu.setMaximumWidth(170);
    contextMnu.exec(mevent.globalPos());
  }
}

void MainWindow::deleteUserClicked() {

  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 1);
  QString Destination = t->text();
  const QPixmap &pixmap = avatarPixmap();
  setWindowIcon(QIcon(pixmap));

  QMessageBox msgBox(this);
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setText(tr("\nReally delete contact?"));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::Yes);
  msgBox.setWindowModality(Qt::WindowModal);
  int ret = msgBox.exec();
  if (ret == QMessageBox::Yes) {
    Core->getUserManager()->deleteUserByI2PDestination(Destination);
  }
}

void MainWindow::renameUserCLicked() {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow());
  QString OldNickname = t->text();

  QListWidgetItem *t2 = listWidget->item(listWidget->currentRow() + 1);
  QString Destination = t2->text();

  RenameWindow *Dialog = new RenameWindow(*Core, OldNickname, Destination);
  Dialog->show();
}

void MainWindow::closeEvent(QCloseEvent *e) {
  if (mStatusNotifier != NULL) {
    hide();
    e->ignore();
  }

  if (applicationIsClosing == true) {
    e->accept();
    QApplication::exit(0);
  }
}

void MainWindow::updateMenu() {
  toggleVisibilityAction->setText(isVisible() ? tr("Hide") : tr("Show"));
}

void MainWindow::toggleAllWindows() {
  bool anyVisible = isVisible();
  for (auto it = mAllOpenChatWindows.begin(); it != mAllOpenChatWindows.end(); ++it)
    if (it.value()->isVisible())
      anyVisible = true;

  if (anyVisible) {
    hide();
    for (auto it = mAllOpenChatWindows.begin(); it != mAllOpenChatWindows.end(); ++it)
      if (it.value()->isVisible())
        it.value()->hide();
  } else {
    show();
    raise();
    activateWindow();
    for (auto it = mAllOpenChatWindows.begin(); it != mAllOpenChatWindows.end(); ++it)
      if (it.value()->isHidden() == false)
        it.value()->show();
  }
}

void MainWindow::OnlineStateChanged() {
  disconnect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onlineComboBoxChanged()));

  QComboBox *comboBox = this->comboBox;
  ONLINESTATE onlinestatus = Core->getOnlineStatus();

  if (onlinestatus == User::USERTRYTOCONNECT) {
    comboBox->clear();
    comboBox->addItem(QIcon(ICON_USER_TRYTOCONNECT),
                      tr("Connecting..."));                     // index 0
    comboBox->addItem(QIcon(ICON_USER_OFFLINE), tr("Offline")); // 1
    comboBox->setCurrentIndex(0);
    setTrayIcon(QIcon(ICON_USER_TRYTOCONNECT));
  } else {
    if (comboBox->count() < 6) {
      comboBox->clear();

      comboBox->addItem(QIcon(ICON_USER_ONLINE), tr("Online"));            // index 0
      comboBox->addItem(QIcon(ICON_USER_WANTTOCHAT), tr("Want to chat"));  // 1
      comboBox->addItem(QIcon(ICON_USER_AWAY), tr("Away"));                // 2
      comboBox->addItem(QIcon(ICON_USER_DONT_DISTURB), tr("No disturbo")); // 3
      comboBox->addItem(QIcon(ICON_USER_INVISIBLE), tr("Invisible"));      // 4
      comboBox->addItem(QIcon(ICON_USER_OFFLINE), tr("Offline"));          // 5
    }

    if (onlinestatus == User::USERONLINE) {
      comboBox->setCurrentIndex(0);
      setTrayIcon(QIcon(ICON_USER_ONLINE));
    } else if (onlinestatus == User::USERWANTTOCHAT) {
      comboBox->setCurrentIndex(1);
      setTrayIcon(QIcon(ICON_USER_WANTTOCHAT));
    } else if (onlinestatus == User::USERAWAY) {
      comboBox->setCurrentIndex(2);
      setTrayIcon(QIcon(ICON_USER_AWAY));
    } else if (onlinestatus == User::USERDONT_DISTURB) {
      comboBox->setCurrentIndex(3);
      setTrayIcon(QIcon(ICON_USER_DONT_DISTURB));
    } else if (onlinestatus == User::USERINVISIBLE) {
      comboBox->setCurrentIndex(4);
      setTrayIcon(QIcon(ICON_USER_INVISIBLE));
    } else if (onlinestatus == User::USEROFFLINE) {
      comboBox->setCurrentIndex(5);
      setTrayIcon(QIcon(ICON_USER_OFFLINE));
    }
  }

  // Refresh contact list icons when online status changes
  slotLoadOwnAvatarImage();

  connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onlineComboBoxChanged()));
}

void MainWindow::openAboutDialog() {
  if (mAboutWindow == NULL) {
    mAboutWindow = new About(Core->getClientVersion(),
                             Core->getProtocolVersion(),
                             FileTransferProtocol::MINPROTOCOLVERSION,
                             FileTransferProtocol::MAXPROTOCOLVERSION);
    connect(this, SIGNAL(closeAllWindows()), mAboutWindow, SLOT(close()));

    connect(mAboutWindow, SIGNAL(closingAboutWindow()), this, SLOT(eventAboutWindowClosed()));

    mAboutWindow->show();
  } else {
    mAboutWindow->getFocus();
  }
}

void MainWindow::initTryIconMenu() {
  // Tray icon Menu
  menu = new QMenu(this);
  QObject::connect(menu, SIGNAL(aboutToShow()), this, SLOT(updateMenu()));
  toggleVisibilityAction = menu->addAction(QIcon(ICON_CHAT), tr("Show/Hide"), this, SLOT(toggleAllWindows()));

  toggleMuteAction = menu->addAction(QIcon(ICON_SOUND_ON), tr("Sound on"), this, SLOT(muteSound()));
  menu->addSeparator();
  menu->addSeparator();
  menu->addAction(QIcon(ICON_CLOSE), tr("&Quit"), this, SLOT(closeApplication()));
}

void MainWindow::setTrayIcon(const QIcon &icon) {
  if (mStatusNotifier != NULL)
    mStatusNotifier->setIcon(icon.pixmap(22, 22));
}

void MainWindow::initTryIcon() {
  // Use a single StatusNotifierItem (D-Bus) for the tray. Modern desktops
  // (XFCE4 4.14+, KDE, etc.) only host the StatusNotifier protocol, not the
  // legacy XEmbed QSystemTrayIcon, so we register one item and nothing else.
  mStatusNotifier = new CStatusNotifier(this);
  mStatusNotifier->setIconFromSvg(QStringLiteral(":/icons/avatar.svg"), 22);
  mStatusNotifier->setToolTip(tr("I2PChat"), tr("I2PChat"));
  if (mStatusNotifier->registerNotifier() == false) {
    delete mStatusNotifier;
    mStatusNotifier = NULL;
  } else {
    // A left click opens our menu. sntray's mapping of left-click to either
    // Activate or ContextMenu varies, so bind both to the menu popup to be safe.
    connect(mStatusNotifier, SIGNAL(activate(int, int)), this, SLOT(showTrayMenu(int, int)));
    connect(mStatusNotifier, SIGNAL(contextMenu(int, int)), this, SLOT(showTrayMenu(int, int)));
  }
}

void MainWindow::showTrayMenu(int x, int y) {
  if (menu != NULL)
    menu->popup(QPoint(x, y));
}

void MainWindow::copyDestination() {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 1);
  if (t == NULL)
    return;
  QString Destination = t->text();

  QClipboard *clipboard = QApplication::clipboard();

  clipboard->setText(Destination);
  QMessageBox::information(this, "", tr("\nContact's Destination copied to clipboard"), QMessageBox::Close);
}

void MainWindow::copyB32() {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 1);
  if (t == NULL)
    return;
  QString Destination = t->text();

  size_t buffersize = 2048;
  uint8_t *outputbuffer = (uint8_t *)malloc(buffersize);
  char *b32buffer = (char *)malloc(buffersize);

  int outputcount =
    i2p::data::Base64ToByteStream(Destination.toUtf8().constData(), Destination.size(), outputbuffer, buffersize);
  QByteArray qarraysha256hash = QByteArray((char *)outputbuffer, outputcount);
  while (outputcount > qarraysha256hash.size()) {
    qarraysha256hash.append((char)0);
  }
  QByteArray sha256hash = QCryptographicHash::hash(qarraysha256hash, QCryptographicHash::Sha256);
  i2p::data::ByteStreamToBase32((uint8_t *)sha256hash.data(), sha256hash.size(), b32buffer, 52);
  b32buffer[52] = '\0';
  QString Address = "http://" + QString(b32buffer) + ".b32.i2p";

  free(outputbuffer);
  free(b32buffer);

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(Address);
  QMessageBox::information(this, "", tr("\nContact's profile address copied to clipboard"), QMessageBox::Close);
}

void MainWindow::muteSound() {
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

void MainWindow::showUserInfos() {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 1);
  QString Destination = t->text();
  QString UserInfos;
  QPixmap avatar;
  CUser *user;

  user = Core->getUserManager()->getUserByI2P_Destination(Destination);
  UserInfos = Core->getUserManager()->getUserInfosByI2P_Destination(Destination);

  avatar.loadFromData(user->getReceivedUserInfos().AvatarImage);

  QMessageBox msgBox;
  if (avatar.isNull() == true) {
    msgBox.setIcon(QMessageBox::Information);
  } else {
    msgBox.setIconPixmap(avatar);
  }
  const QPixmap &pixmap = avatarPixmap();
  msgBox.setWindowIcon(QIcon(pixmap));
  msgBox.setText("\n" + UserInfos);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  msgBox.setWindowModality(Qt::NonModal);
  msgBox.exec();
}

void MainWindow::UserPositionUP() {
  QListWidget *listWidget = this->listWidget;
  if (listWidget->currentRow() >= 1)
    Core->getUserManager()->changeUserPositionInUserList(listWidget->currentRow() / 3,
                                                         (listWidget->currentRow() / 3) - 1);
}

void MainWindow::UserPositionDOWN() {
  QListWidget *listWidget = this->listWidget;
  if (listWidget->currentRow() < (listWidget->count() / 3) - 1)
    Core->getUserManager()->changeUserPositionInUserList(listWidget->currentRow() / 3,
                                                         (listWidget->currentRow() / 3) + 1);
}

void MainWindow::UserPositionTOP() {
  QListWidget *listWidget = this->listWidget;
  int currentUserIndex = listWidget->currentRow() / 3;
  if (currentUserIndex > 0) {
    Core->getUserManager()->changeUserPositionInUserList(currentUserIndex, 0);
  }
}

void MainWindow::UserPositionBOTTOM() {
  QListWidget *listWidget = this->listWidget;
  int currentUserIndex = listWidget->currentRow() / 3;
  int totalUsers = listWidget->count() / 3;
  if (currentUserIndex < totalUsers - 1) {
    Core->getUserManager()->changeUserPositionInUserList(currentUserIndex, totalUsers - 1);
  }
}

void MainWindow::UserInvisible(bool b) {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 1);
  QString Destination = t->text();

  CUser *User;
  User = Core->getUserManager()->getUserByI2P_Destination(Destination);
  if (User != NULL) {
    User->setInvisible(b);
  }
}

void MainWindow::eventChatWindowClosed(const QString &Destination) {
  if (mAllOpenChatWindows.contains(Destination) == true) {
    mAllOpenChatWindows[Destination]->deleteLater();
    mAllOpenChatWindows.remove(Destination);
  } else {
    qCritical() << "MainWindow::eventChatWindowClosed\n"
                << "Closing a unknown chat window";
  }
}

void MainWindow::openChatWindow(const QString &Destination) {
  CUser *User;
  User = Core->getUserManager()->getUserByI2P_Destination(Destination);
  if (User == NULL) {
    qCritical() << "MainWindow::openChatWindow"
                << "Cannot open chat window for non-existent user!";
    return;
  }

  if (mAllOpenChatWindows.contains(Destination) == false) {
    // create new chatWindow
    ChatWidget *tmp = new ChatWidget(*User, *Core);
    connect(this, SIGNAL(closeAllWindows()), tmp, SLOT(close()));

    connect(tmp, SIGNAL(closingChatWindow(QString)), this, SLOT(eventChatWindowClosed(QString)));

    connect(Core, SIGNAL(signOwnAvatarImageChanged()), this, SLOT(slotLoadOwnAvatarImage()));

    mAllOpenChatWindows.insert(Destination, tmp);
    tmp->show();
  } else {
    // open the existing chatwindow
    mAllOpenChatWindows.value(Destination)->show();
    mAllOpenChatWindows.value(Destination)->getFocus();
  }
}

void MainWindow::eventFileReceiveWindowClosed(qint32 StreamID) {
  if (mAllFileReceiveWindows.contains(StreamID) == true) {
    mAllFileReceiveWindows.value(StreamID)->deleteLater();
    mAllFileReceiveWindows.remove(StreamID);
  } else {
    qCritical() << "MainWindow::eventFileReceiveWindowClose\n"
                << "Closing a unknown FileReceive window";
  }
}

void MainWindow::eventFileSendWindowClosed(qint32 StreamID) {
  if (mAllFileSendWindows.contains(StreamID) == true) {
    mAllFileSendWindows.value(StreamID)->deleteLater();
    mAllFileSendWindows.remove(StreamID);
  } else {
    qCritical() << "MainWindow::eventFileSendWindowClosed\n"
                << "Closing a unknown FileSend window";
  }
}

void MainWindow::openFileSendWindow(qint32 StreamID) {
  CFileTransferSend *TransferSend = Core->getFileTransferManager()->getFileSendByID(StreamID);

  if (TransferSend == NULL) {
    qCritical() << "MainWindow::openFileSendWindow\n"
                << "Can't find FileSend Object with ID: " << StreamID << "\nFile transfer failed!";
    return;
  }

  if (mAllFileSendWindows.contains(StreamID) == false) {
    // open new FileSendWindow
    FileSend *Dialog = new FileSend(*TransferSend);
    connect(Dialog, SIGNAL(closingFileSendWindow(qint32)), this, SLOT(eventFileSendWindowClosed(qint32)));

    mAllFileSendWindows.insert(StreamID, Dialog);
    Dialog->show();

  } else {
    mAllFileSendWindows.value(StreamID)->getFocus();
  }
}

void MainWindow::openFileReceiveWindow(qint32 StreamID) {
  CFileTransferReceive *receive = Core->getFileTransferManager()->getFileReceiveByID(StreamID);
  if (receive == NULL) {
    qCritical() << "MainWindow::openFileReceiveWindow\n"
                << "Can't find FileReceive Object with ID: " << StreamID << "\nFile transfer failed!";
    return;
  }

  if (mAllFileReceiveWindows.contains(StreamID) == false) {
    // create new FileReceiveWindow
    FileReceive *Dialog = new FileReceive(*receive);

    connect(Dialog, SIGNAL(closingFileReceiveWindow(qint32)), this, SLOT(eventFileReceiveWindowClosed(qint32)));

    mAllFileReceiveWindows.insert(StreamID, Dialog);
    Dialog->show();
    Dialog->start();

  } else {
    // open the existing FileReceiveWindow
    mAllFileReceiveWindows.value(StreamID)->getFocus();
  }
}

void MainWindow::addUserToBlockList() {
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

void MainWindow::incomingUserAuthorizationRequest(const QString &destination, int streamID, const QByteArray &data) {
  // Only one auth dialog at a time — deny new requests while one is pending
  if (mAuthDialog != NULL) {
    Core->getConnectionManager()->doDestroyStreamObjectByID(streamID);
    mAuthDialog->raise();
    mAuthDialog->activateWindow();
    return;
  }

  // Extract caller nickname from handshake: CHATSYSTEM\tVersion[\tNickname]\n
  QString callerNickname;
  int firstTab = data.indexOf('\t');
  int secondTab = data.indexOf('\t', firstTab + 1);
  int newlinePos = data.indexOf('\n', secondTab);
  if (secondTab > firstTab && secondTab < newlinePos) {
    callerNickname = QString::fromUtf8(data.mid(secondTab + 1, newlinePos - secondTab - 1));
  }

  QString displayName = callerNickname.isEmpty() ? destination : callerNickname;

  mAuthDialog = new QMessageBox(this);
  mAuthDialog->setAttribute(Qt::WA_DeleteOnClose);
  mAuthDialog->setIcon(QMessageBox::Question);
  mAuthDialog->setWindowTitle(tr("Incoming connection"));
  mAuthDialog->setText(tr("Incoming connection from unauthorized user"));
  mAuthDialog->setInformativeText(tr("Allow connection from %1?").arg(displayName));
  auto *btnBlock = mAuthDialog->addButton(tr("Block"), QMessageBox::DestructiveRole);
  mAuthDialog->addButton(QMessageBox::Yes);
  mAuthDialog->addButton(QMessageBox::No);
  mAuthDialog->setDefaultButton(QMessageBox::No);
  connect(mAuthDialog,
          &QDialog::finished,
          this,
          [this, data, destination, streamID, displayName, callerNickname, btnBlock](int ret) {
            QAbstractButton *clicked = mAuthDialog ? mAuthDialog->clickedButton() : nullptr;
            mAuthDialog = NULL;
            if (ret == QMessageBox::Yes) {
              // Extract version
              QByteArray temp = data.mid(data.indexOf("\t") + 1, data.indexOf("\n") - data.indexOf("\t") - 1);
              QString version(temp);
              bool OK = false;
              double versiond = version.toDouble(&OK);
              if (!OK)
                versiond = 0.0;

              // Remove first packet
              QByteArray Data2 = data;
              Data2 = Data2.remove(0, data.indexOf("\n") + 1);

              // Check for existing user first (covers edge cases where the
              // userlist guard in handleChatProtocolPacket didn't fire)
              CUser *User = Core->getUserManager()->getUserByI2P_Destination(destination);
              if (User) {
                User->setI2PStreamID(streamID);
                User->setProtocolVersion(version);
                User->setConnectionStatus(ONLINE);
                Core->setStreamTypeToKnown(streamID, Data2, false);
                if (!callerNickname.isEmpty())
                  User->setReceivedUserInfos(NICKNAME, callerNickname);
                else if (versiond >= 0.3)
                  User->setReceivedNicknameToUserNickname();
                return;
              }

              QString userName = callerNickname.isEmpty() ? "Unknown" : callerNickname;
              bool added = Core->getUserManager()->addNewUser(userName, destination, streamID, true, true);

              if (!added) {
                emit Core->getConnectionManager()->signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") +
                                                                     " • AddNewUser FAILED — stream " +
                                                                     QString::number(streamID) + " destroyed");
                Core->getConnectionManager()->doDestroyStreamObjectByID(streamID);
                return;
              }
              emit Core->getConnectionManager()->signDebugMessages(QDateTime::currentDateTime().toString("hh:mm:ss") +
                                                                   " • AddNewUser OK — stream " +
                                                                   QString::number(streamID) + " authorized");
              CUser *NewUser = Core->getUserManager()->getUserByI2P_Destination(destination);
              if (NewUser) {
                NewUser->setI2PStreamID(streamID);
                NewUser->setProtocolVersion(version);
                NewUser->setConnectionStatus(ONLINE);
                Core->setStreamTypeToKnown(streamID, Data2, false);
                if (!callerNickname.isEmpty())
                  NewUser->setReceivedUserInfos(NICKNAME, callerNickname);
                else if (versiond >= 0.3)
                  NewUser->setReceivedNicknameToUserNickname();
              }
            } else if (clicked == btnBlock) {
              Core->getUserBlockManager()->addNewBlockEntity(displayName, destination);
              Core->getConnectionManager()->doDestroyStreamObjectByID(streamID);
            } else {
              // Deny, close the connection
              Core->getConnectionManager()->doDestroyStreamObjectByID(streamID);
            }
          });
  mAuthDialog->show();
}

void MainWindow::eventAboutWindowClosed() {
  mAboutWindow->deleteLater();
  mAboutWindow = NULL;
}
void MainWindow::eventDebugWindowClosed() {
  mDebugWindow->deleteLater();
  mDebugWindow = NULL;
}

void MainWindow::eventNicknameChanged() {
  QString nick = Core->getUserInfos().Nickname;
  nicknamelabel->setText(nick);
}

void MainWindow::eventAvatarImageChanged() {
  if (Core->getUserInfos().AvatarImage.isEmpty() == false) {
    QPixmap avatar;
    avatar.loadFromData(Core->getUserInfos().AvatarImage);
    int w = avatarlabel->width();
    int h = avatarlabel->height();
    if (avatar.width() != w || avatar.height() != h) {
      QImage img = avatar.toImage();
      img = CCore::scaleImageLanczos(img, w, h);
      avatar = QPixmap::fromImage(img);
    }
    avatarlabel->setAlignment(Qt::AlignCenter);
    avatarlabel->setPixmap(avatar);
  }
  slotLoadOwnAvatarImage();
}

void MainWindow::slotLoadOwnAvatarImage() {
  ONLINESTATE onlineStatus = Core->getOnlineStatus();
  bool isOnline = (onlineStatus != User::USEROFFLINE && onlineStatus != User::USERTRYTOCONNECT);

  for (int i = 0; i < listWidget->count(); i++) {
    QListWidgetItem *typeItem = listWidget->item(i + 2);
    if (typeItem && typeItem->text() == "U") {
      QListWidgetItem *destItem = listWidget->item(i + 1);
      if (destItem) {
        QString Destination = destItem->text();
        CUser *User = Core->getUserManager()->getUserByI2P_Destination(Destination);
        if (User) {
          QListWidgetItem *iconItem = listWidget->item(i);
          if (iconItem) {
            if (isOnline) {
              // When online, show status icons instead of avatars
              switch (User->getOnlineState()) {
              case USERTRYTOCONNECT:
                iconItem->setIcon(QIcon(ICON_USER_OFFLINE));
                break;
              case USERINVISIBLE:
              case USEROFFLINE:
                iconItem->setIcon(QIcon(ICON_USER_OFFLINE));
                break;
              case USERONLINE:
                iconItem->setIcon(QIcon(ICON_USER_ONLINE));
                break;
              case USERWANTTOCHAT:
                iconItem->setIcon(QIcon(ICON_USER_WANTTOCHAT));
                break;
              case USERAWAY:
                iconItem->setIcon(QIcon(ICON_USER_AWAY));
                break;
              case USERDONT_DISTURB:
                iconItem->setIcon(QIcon(ICON_USER_DONT_DISTURB));
                break;
              case USERBLOCKEDYOU:
                iconItem->setIcon(QIcon(ICON_USER_BLOCKED_YOU));
                break;
              default:
                break;
              }
            } else {
              // When offline, show avatars
              QPixmap avatar;
              if (User->getReceivedUserInfos().AvatarImage.isEmpty() == false) {
                avatar.loadFromData(User->getReceivedUserInfos().AvatarImage);
              } else {
                avatar = avatarPixmap();
              }
              iconItem->setIcon(QIcon(avatar));
            }
          }
        }
      }
    }
    i += 2;
  }
}

void MainWindow::UserAutoDownload(bool enabled) {
  QListWidgetItem *t = listWidget->item(listWidget->currentRow() + 1);
  QString Destination = t->text();

  CUser *User = Core->getUserManager()->getUserByI2P_Destination(Destination);
  if (User) {
    User->setAutoDownloadEnabled(enabled);

    // Update the icon in the menu action
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
      if (enabled) {
        action->setIcon(QIcon(ICON_USER_DOWNLOAD));
      } else {
        action->setIcon(QIcon(ICON_USER_DOWNLOAD_DISABLED));
      }
    }
  }
}
