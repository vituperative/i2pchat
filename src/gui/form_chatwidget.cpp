#include "form_chatwidget.h"

#include "User.h"

#include <QErrorMessage>
#include <QFile>
#include <QRegularExpression>
#include <QXmlStreamReader>

bool ChatEventEater::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (obj->objectName() == "message") {
      if (keyEvent->key() == Qt::Key_Return && keyEvent->modifiers() == Qt::NoModifier) {
        emit sendMessage();
        return true;
      }
      if (keyEvent->key() == Qt::Key_Return && keyEvent->modifiers() == Qt::ControlModifier) {
        emit sendMessage();
        return true;
      }
    }
    return QObject::eventFilter(obj, event);
  }

  if (event->type() == QEvent::FocusIn) {
    emit haveFocus(true);
    return true;
  }
  if (event->type() == QEvent::FocusOut) {
    emit haveFocus(false);
    return true;
  }

  return false;
}

form_ChatWidget::form_ChatWidget(CUser &user, CCore &Core, QDialog *parent /* = 0 */)
  : QMainWindow(parent, Qt::WindowFlags())
  , user(user)
  , Core(Core) {
  setupUi(this);

  QTextEdit *message = this->message;
  QTextBrowser *chat = this->chat;
  QToolButton *txtBold = this->txtBold;
  QToolButton *txtItalic = this->txtItalic;
  QToolButton *txtUnder = this->txtUnder;

  m_event_eater = new ChatEventEater(this);

  connect(m_event_eater, SIGNAL(sendMessage()), send, SLOT(click()));

  connect(m_event_eater, SIGNAL(haveFocus(bool)), this, SLOT(focusEvent(bool)));

  message->installEventFilter(m_event_eater);

  connect(&user, SIGNAL(signNewMessageReceived()), this, SLOT(newMessageReceived()));

  connect(&user, SIGNAL(signOnlineStateChanged()), this, SLOT(changeWindowsTitle()));

  connect(&user, SIGNAL(signUserDeleted()), this, SLOT(close()));

  connect(&user, SIGNAL(signNewAvatarImage()), this, SLOT(remoteAvatarImageChanged()));

  connect(&user, SIGNAL(signPendingCanceled()), this, SLOT(slotPendingCanceled()));

  connect(this, SIGNAL(sendChatMessage(QString)), &user, SLOT(slotSendChatMessage(QString)));

  connect(cmd_SendFile, SIGNAL(clicked()), this, SLOT(newFileTransfer()));

  connect(chat, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(anchorClicked(const QUrl &)));

  connect(avatarFrameButton, SIGNAL(toggled(bool)), this, SLOT(showAvatarFrame(bool)));

  connect(message, SIGNAL(textChanged()), this, SLOT(messageTextChanged()));

  setAcceptDrops(true);

  chat->setOpenLinks(false);

  mCurrentFont = user.getTextFont();
  textColor = user.getTextColor();
  mHaveFocus = false;

  QPixmap pxm(22, 22);
  pxm.fill(textColor);
  // txtColor->setIcon(pxm);

  connect(send, SIGNAL(clicked()), SLOT(sendMessageSignal()));
  connect(txtColor, SIGNAL(clicked()), SLOT(setTextColor()));
  connect(txtBold, SIGNAL(clicked(bool)), SLOT(setBold(bool)));
  connect(txtFont, SIGNAL(clicked()), SLOT(setFont()));
  connect(txtUnder, SIGNAL(clicked(bool)), SLOT(setUnderline(bool)));
  connect(txtItalic, SIGNAL(clicked(bool)), SLOT(setItalic(bool)));
  QPalette pal = message->palette();
  pal.setBrush(QPalette::Text, QBrush(textColor));
  message->setPalette(pal);

  message->setCurrentFont(mCurrentFont);
  message->setFont(mCurrentFont);

  txtBold->setChecked(mCurrentFont.bold());
  txtItalic->setChecked(mCurrentFont.italic());
  txtUnder->setChecked(mCurrentFont.underline());

  resize(480, 360); // maybe too from QSS?
  centerDialog();

  changeWindowsTitle();
  addAllMessages();
  QScrollBar *sb = chat->verticalScrollBar();
  sb->setValue(sb->maximum());

  slotLoadOwnAvatarImage();

  useravatar_label->setAlignment(Qt::AlignCenter);
  remoteAvatarImageChanged();

  // QTimer *updater_chat = new QTimer(this);
  // connect(timer, &QTimer::timeout, this, &form_ChatWidget::raise);
  // timer->start();
}

void form_ChatWidget::newMessageReceived() { // TODO: qss add.
  QTextEdit *chat = this->chat;
  QScrollBar *sb = chat->verticalScrollBar();
  // chat->setStyleSheet("#IncomingMessages{color:rgb(100,200,254);}"); // does
  // not works;

  /*QPalette pal = chat->palette();
  pal.setBrush(QPalette::Window,QBrush(textColor));
  chat->setPalette(pal);*/ // works, but not for text which will be changes though QSS.
  // text+="<style>span{ background-color: yellow; }</style>"; // Too is NOT
  // WORKS, because it not supportd.

  int oldVerticalScrollBarValue = sb->value();
  int VerticalScrollBarMax = sb->maximum();
  bool restoreOldVerticalScrollBarValue = false;

  if (VerticalScrollBarMax - oldVerticalScrollBarValue > 2) {
    restoreOldVerticalScrollBarValue = true;
  }

  QStringList Messages = user.getNewMessages(mHaveFocus);
  int i = 0;
  QString temp;
  while (i < Messages.count()) {
    temp = Messages.at(i);
    this->addMessage(temp);
    i++;
  }

  if (restoreOldVerticalScrollBarValue == true) {
    sb->setValue(oldVerticalScrollBarValue);
  } else {
    sb->setValue(sb->maximum());
  }

  //  this->raise(); // no, do not steal focus!
}

void form_ChatWidget::addAllMessages() {
  QTextBrowser *chat = this->chat;
  QScrollBar *sb = chat->verticalScrollBar();
  chat->clear();

  QStringList Messages = user.getAllChatMessages();
  int i = 0;
  while (i < Messages.count()) {
    const QString &tmp = Messages.at(i);
    this->addMessage(tmp);
    i++;
  }

  sb->setValue(sb->maximum());
}

void form_ChatWidget::addMessage(QString text) {
  QTextBrowser *chat = this->chat;

  // Process emoticons in received messages
  CTextEmotionChanger::exemplar()->checkMessageForEmoticons(text);

  // append HTML
  {
    auto cursor = QTextCursor(chat->document());
    cursor.movePosition(QTextCursor::End);
    if (cursor.isNull()) {
      auto msg = "Error appending to chatLog: cursor is null";
      qDebug() << msg;
      QErrorMessage *box = new QErrorMessage(this);
      box->showMessage(msg);
      return;
    }
    // Optimize with edit block for better performance
    cursor.beginEditBlock();
    cursor.insertHtml(text);
    cursor.endEditBlock();
  }

  chat->update();
}

void form_ChatWidget::setTextColor() {
  QTextEdit *message = this->message;
  textColor = QColorDialog::getColor(user.getTextColor(), this);

  // textColor = QColorDialog::getColor(message->textColor(), this);
  QPixmap pxm(22, 22);
  pxm.fill(textColor);
  // txtColor->setIcon(pxm);
  user.setTextColor(textColor);

  QPalette pal = message->palette();
  pal.setBrush(QPalette::Text, QBrush(textColor));
  message->setPalette(pal);
}

void form_ChatWidget::setFont() {
  QTextEdit *message = this->message;
  bool ok;
  mCurrentFont = QFontDialog::getFont(&ok, mCurrentFont, this);

  user.setTextFont(mCurrentFont);
  message->setCurrentFont(mCurrentFont);
  message->setFont(mCurrentFont);
  message->setFocus();
}

void form_ChatWidget::setBold(bool t) {

  QTextEdit *message = this->message;

  mCurrentFont.setBold(t);
  user.setTextFont(mCurrentFont);

  message->setCurrentFont(mCurrentFont);
  message->setFont(mCurrentFont);
}

void form_ChatWidget::closeEvent(QCloseEvent *e) {
  disconnect(&user, SIGNAL(signNewMessageReceived()), this, SLOT(newMessageReceived()));

  emit closingChatWindow(user.getI2PDestination());
  e->ignore();
}

void form_ChatWidget::sendMessageSignal() {
  QTextEdit *message = this->message;
  if (message->toPlainText().length() == 0)
    return;

  mControllForChange.setHtml(message->toHtml());
  mControllForChange.selectAll();
  mControllForChange.setCurrentFont(mCurrentFont);
  mControllForChange.setFont(mCurrentFont);
  mControllForChange.setTextColor(user.getTextColor());

  mControllForChange.setFontUnderline(mCurrentFont.underline());
  mControllForChange.setFontItalic(mCurrentFont.italic());

  QString NewMessage = mControllForChange.toHtml();

  if (NewMessage.length() < 65535) {
    user.slotSendChatMessage(NewMessage);
    message->clear();
    newMessageReceived();
  } else {
    QMessageBox *msgBox = new QMessageBox(NULL);
    msgBox->setIcon(QMessageBox::Critical);
    msgBox->setText("Cannot send, message is too long!");
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->show();
  }
}

void form_ChatWidget::changeWindowsTitle() {
  QString OnlineStatus;
  switch (user.getOnlineState()) {

  case USERTRYTOCONNECT:
  case USERINVISIBLE:
  case USEROFFLINE: {
    OnlineStatus = tr("Offline");
    this->setWindowIcon(QIcon(ICON_USER_OFFLINE));
    break;
  }
  case USERONLINE: {
    OnlineStatus = tr("Online");
    this->setWindowIcon(QIcon(ICON_USER_ONLINE));
    break;
  }
  case USERWANTTOCHAT: {
    OnlineStatus = tr("Want to chat");
    this->setWindowIcon(QIcon(ICON_USER_WANTTOCHAT));
    break;
  }
  case USERAWAY: {
    OnlineStatus = tr("Away");
    this->setWindowIcon(QIcon(ICON_USER_AWAY));
    break;
  }
  case USERDONT_DISTURB: {
    OnlineStatus = tr("Do not disturb");
    this->setWindowIcon(QIcon(ICON_USER_DONT_DISTURB));
    break;
  }
  case USERBLOCKEDYOU: {
    OnlineStatus = tr("You have been blocked");
    this->setWindowIcon(QIcon(ICON_USER_BLOCKED_YOU));
    break;
  }
  default:
    break;
  }
  this->setWindowTitle(user.getName() + " [" + OnlineStatus + "]");
}

void form_ChatWidget::newFileTransfer() {
  if (user.getConnectionStatus() == ONLINE) {
    QString FilePath = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("all Files (*)"));

    if (FilePath.endsWith("/") == true) {
      return;
    }

    if (!FilePath.isEmpty())
      startFileTransfer(FilePath);

  } else {
    // Queue file offer for later delivery
    QString FilePath = QFileDialog::getOpenFileName(
      this, tr("Open File (will be sent when contact comes online)"), ".", tr("all Files (*)"));
    if (FilePath.isEmpty() || FilePath.endsWith("/"))
      return;

    QFileInfo fi(FilePath);
    if (!fi.exists() || !fi.isFile())
      return;

    user.slotSendFileOffer(fi.fileName(), fi.size(), FilePath);
  }
}

void form_ChatWidget::anchorClicked(const QUrl &link) {
  if (link.scheme() == "cancelmsg") {
    bool ok = false;
    qint32 id = link.toString().mid(link.scheme().length() + 1).toInt(&ok);
    if (ok)
      user.cancelPendingMessage(id);
    return;
  }
  if (link.scheme() == "cancelfile") {
    bool ok = false;
    qint32 id = link.toString().mid(link.scheme().length() + 1).toInt(&ok);
    if (ok)
      user.cancelPendingFileOffer(id);
    return;
  }

  if (link.scheme() == "fileoffer") {
    // Format: fileoffer:accept:filename or fileoffer:reject:filename
    QStringList parts = link.toString().split(":");
    if (parts.size() < 3)
      return;

    QString action = parts.at(1);
    QString fileName = parts.mid(2).join(":"); // Restore filename if it contained colons

    if (action == "accept") {
      // Send acceptance over chat protocol
      Core.getProtocol()->send(FILE_OFFER_ACCEPTED, user.getI2PStreamID(), fileName.toUtf8());
      // Show confirmation in chat
      user.slotIncomingMessageFromSystem(
        tr("You accepted the file \"%1\". Waiting for sender to start transfer...").arg(fileName), true);
    } else if (action == "reject") {
      Core.getProtocol()->send(FILE_OFFER_REJECTED, user.getI2PStreamID(), fileName.toUtf8());
      user.slotIncomingMessageFromSystem(tr("You rejected the file \"%1\".").arg(fileName), true);
    }
    return;
  }

  // Open browser, after clicking to link? TODO: add WARNING MESSAGE!!!
  if (link.scheme() == "http" || link.scheme() == "https")
    QDesktopServices::openUrl(link);
  else if (link.scheme() == "") {
    // it's probably a web adress, let's add http:// at the beginning of the
    // link
    QString newAddress = link.toString();
    newAddress.prepend("http://");
    QDesktopServices::openUrl(QUrl(newAddress));
  }
}

void form_ChatWidget::focusEvent(bool b) {
  mHaveFocus = b;

  if (user.getHaveNewUnreadMessages() == true) {
    newMessageReceived();
  }
}

void form_ChatWidget::slotPendingCanceled() {
  addAllMessages();
}

void form_ChatWidget::getFocus() {
  this->activateWindow();
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) | Qt::WindowActive);
  this->raise();
  //  this->setFocus(); // don't automatically focus chat window
}

void form_ChatWidget::setUnderline(bool t) {
  mCurrentFont.setUnderline(t);
  user.setTextFont(mCurrentFont);

  message->setCurrentFont(mCurrentFont);
  message->setFont(mCurrentFont);
  message->setFocus();
}

void form_ChatWidget::setItalic(bool t) {
  mCurrentFont.setItalic(t);
  user.setTextFont(mCurrentFont);

  message->setCurrentFont(mCurrentFont);
  message->setFont(mCurrentFont);
  message->setFocus();
}

void form_ChatWidget::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData() && event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
  }
}

void form_ChatWidget::dropEvent(QDropEvent *event) {
  if (!event->mimeData() || !event->mimeData()->hasUrls())
    return;

  const auto urls = event->mimeData()->urls();
  if (urls.isEmpty())
    return;

  QString filePath = urls.first().toLocalFile();
  if (filePath.isEmpty())
    return;

  QFileInfo fi(filePath);
  if (!fi.exists() || !fi.isFile())
    return;

  startFileTransfer(filePath);
}

void form_ChatWidget::startFileTransfer(const QString &filePath) {
  if (user.getConnectionStatus() == ONLINE) {
    Core.getFileTransferManager()->addNewFileTransfer(filePath, user.getI2PDestination());
  } else {
    QFileInfo fi(filePath);
    if (fi.exists() && fi.isFile()) {
      user.slotSendFileOffer(fi.fileName(), fi.size(), filePath);
    }
  }
}

form_ChatWidget::~form_ChatWidget() {}

void form_ChatWidget::keyPressEvent(QKeyEvent *event) {
  if (event->key() != Qt::Key_Escape) {
    QMainWindow::keyPressEvent(event);
  } else {
    event->accept();
    close();
  }
}
void form_ChatWidget::showAvatarFrame(bool show) {
  if (show) {
    avatarframe->setVisible(false);
    avatarFrameButton->setChecked(true);
    avatarFrameButton->setToolTip(tr("Hide Avatar"));
    avatarFrameButton->setIcon(QIcon(tr(":icons/hide_frame.png")));
  } else {
    avatarframe->setVisible(true);
    avatarFrameButton->setChecked(false);
    avatarFrameButton->setToolTip(tr("Show Avatar"));
    avatarFrameButton->setIcon(QIcon(tr(":icons/show_frame.png")));
  }
}

void form_ChatWidget::remoteAvatarImageChanged() {
  if (user.getReceivedUserInfos().AvatarImage.size() > 0) {
    QPixmap pxm;
    pxm.loadFromData(user.getReceivedUserInfos().AvatarImage);
    int w = useravatar_label->width();
    int h = useravatar_label->height();
    if (pxm.width() != w || pxm.height() != h) {
      QImage img = pxm.toImage();
      img = CCore::scaleImageLanczos(img, w, h);
      mUserAvatar = QPixmap::fromImage(img);
    } else {
      mUserAvatar = pxm;
    }
    useravatar_label->setPixmap(mUserAvatar);
  }
}

void form_ChatWidget::messageTextChanged() {
  if (user.getProtocolVersion_D() < 0.5) {
    return;
  }

  QTextCursor tmpCursor = message->textCursor();
  int cursorPos = tmpCursor.position();

  QString messageString = message->toHtml();

  CTextEmotionChanger::exemplar()->checkMessageForEmoticons(messageString);

  disconnect(message, SIGNAL(textChanged()), this, SLOT(messageTextChanged()));
  message->setHtml(messageString);
  connect(message, SIGNAL(textChanged()), this, SLOT(messageTextChanged()));

  tmpCursor.movePosition(QTextCursor::End);
  int maxPos = tmpCursor.position();

  if (cursorPos <= maxPos) {
    tmpCursor.setPosition(cursorPos);
  } else {
    tmpCursor.setPosition(maxPos);
  }
  message->setTextCursor(tmpCursor);
}
void form_ChatWidget::centerDialog() {
  QRect scr = QGuiApplication::primaryScreen()->geometry();
  move(scr.center() - rect().center());
}

void form_ChatWidget::slotLoadOwnAvatarImage() {
  ownavatar_label->setAlignment(Qt::AlignCenter);
  QPixmap pxm;
  pxm.loadFromData(Core.getUserInfos().AvatarImage);
  int w = ownavatar_label->width();
  int h = ownavatar_label->height();
  if (pxm.width() != w || pxm.height() != h) {
    QImage img = pxm.toImage();
    img = CCore::scaleImageLanczos(img, w, h);
    mOwnAvatar = QPixmap::fromImage(img);
  } else {
    mOwnAvatar = pxm;
  }
  ownavatar_label->setPixmap(mOwnAvatar);
}
