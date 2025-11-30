#include <QErrorMessage>

#include "User.h"
#include "form_chatwidget.h"

bool ChatEventEater::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (obj->objectName() == "message") {
      if (keyEvent->key() == Qt::Key_Return &&
          keyEvent->modifiers() == Qt::NoModifier) {
        emit sendMessage();
        return true;
      } else if (keyEvent->key() == Qt::Key_Return &&
                 keyEvent->modifiers() == Qt::ControlModifier) {
        emit sendMessage();
        return true;
      }
    }
    return QObject::eventFilter(obj, event);
  }

  if (event->type() == QEvent::FocusIn) {
    emit haveFocus(true);
    return true;
  } else if (event->type() == QEvent::FocusOut) {
    emit haveFocus(false);
    return true;
  }

  return false;
}

form_ChatWidget::form_ChatWidget(CUser &user, CCore &Core,
                                 QDialog *parent /* = 0 */)
    : QMainWindow(parent, 0), user(user), Core(Core) {
  setupUi(this);

  QTextEdit *message = this->message;
  QTextBrowser *chat = this->chat;
  QToolButton *txtBold = this->txtBold;
  QToolButton *txtItalic = this->txtItalic;
  QToolButton *txtUnder = this->txtUnder;

  m_event_eater = new ChatEventEater(this);

  tabWidget->setCurrentIndex(0);

  connect(m_event_eater, SIGNAL(sendMessage()), send, SLOT(click()));

  connect(m_event_eater, SIGNAL(haveFocus(bool)), this, SLOT(focusEvent(bool)));

  message->installEventFilter(m_event_eater);

  connect(&user, SIGNAL(signNewMessageReceived()), this,
          SLOT(newMessageReceived()));

  connect(&user, SIGNAL(signOnlineStateChanged()), this,
          SLOT(changeWindowsTitle()));

  connect(&user, SIGNAL(signUserDeleted()), this, SLOT(close()));

  connect(&user, SIGNAL(signNewAvatarImage()), this,
          SLOT(remoteAvatarImageChanged()));

  connect(this, SIGNAL(sendChatMessage(QString)), &user,
          SLOT(slotSendChatMessage(QString)));

  connect(cmd_SendFile, SIGNAL(clicked()), this, SLOT(newFileTransfer()));

  connect(chat, SIGNAL(anchorClicked(const QUrl &)), this,
          SLOT(anchorClicked(const QUrl &)));

  connect(avatarFrameButton, SIGNAL(toggled(bool)), this,
          SLOT(showAvatarFrame(bool)));

  connect(message, SIGNAL(textChanged()), this, SLOT(messageTextChanged()));

  connect(tabWidget, SIGNAL(currentChanged(int)), this,
          SLOT(tabIndexChanged(int)));

  connect(cmd_Reaload, SIGNAL(clicked(bool)), this,
          SLOT(reloadOfflineMessages()));

  connect(cmd_Save, SIGNAL(clicked(bool)), this,
          SLOT(saveChangedOfflineMessages()));

  connect(Ui_form_chatwidget::cmd_delete, SIGNAL(clicked(bool)), this,
          SLOT(cmd_delete()));

  connect(Ui_form_chatwidget::cmd_new, SIGNAL(clicked(bool)), this,
          SLOT(cmd_new()));

  chat->setOpenLinks(false);

  mCurrentFont = user.getTextFont();
  mCurrentFont2 = user.getTextFont();
  textColor = user.getTextColor();
  textColor2 = user.getTextColor();
  mHaveFocus = false;

  QPixmap pxm(22, 22);
  pxm.fill(textColor);
  // txtColor->setIcon(pxm);

  connect(send, SIGNAL(clicked()), SLOT(sendMessageSignal()));
  connect(txtColor, SIGNAL(clicked()), SLOT(setTextColor()));
  connect(txtColor_2, SIGNAL(clicked()), SLOT(setTextColor_2()));
  connect(txtBold, SIGNAL(clicked(bool)), SLOT(setBold(bool)));
  connect(txtBold_2, SIGNAL(clicked(bool)), SLOT(setBold_2(bool)));
  connect(txtFont, SIGNAL(clicked()), SLOT(setFont()));
  connect(txtFont_2, SIGNAL(clicked()), SLOT(setFont_2()));
  connect(txtUnder, SIGNAL(clicked(bool)), SLOT(setUnderline(bool)));
  connect(txtUnder_2, SIGNAL(clicked(bool)), SLOT(setUnderline_2(bool)));
  connect(txtItalic, SIGNAL(clicked(bool)), SLOT(setItalic(bool)));
  connect(txtItalic_2, SIGNAL(clicked(bool)), SLOT(setItalic_2(bool)));

  currentOfflineMessageIndex = 0;
  OfflineMessageCount = 0;
  Ui_form_chatwidget::cmd_next->setEnabled(false);
  connect(Ui_form_chatwidget::cmd_next, SIGNAL(clicked(bool)),
          SLOT(cmd_next()));

  Ui_form_chatwidget::cmd_back->setEnabled(false);
  connect(Ui_form_chatwidget::cmd_back, SIGNAL(clicked(bool)),
          SLOT(cmd_back()));

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
    QString tmp = Messages.at(i);
    this->addMessage(tmp);
    i++;
  }

  sb->setValue(sb->maximum());
}

void form_ChatWidget::addMessage(QString text) {
  QTextBrowser *chat = this->chat;
  //	QString& newMessage=text; // UDP(By Voron): DONT NEED, will be
  // deleted!!!

  /* UPD: (by Voron)  temporarly disabled by me, because there is infinite
  cycle!! is willbe fixed.
  //<Voron> i did even with itteration, and some another magick. so i did it for
  one link, but for 2-3 not yet. I will read it firstly
  //<Voron> https://doc.qt.io/archives/qt-4.8/qregexp.html
  //<Voron> i did with QStringList list = rx.capturedTexts();
  //<Voron> for (auto it = ...
  //<Voron> also i disable
  //<Voron> open the browser?!
          //replace http://, https:// and www. with <a href> links
          QRegExp rx("(https?://[^ <>]*)|(www\\.[^ <>]*)");
          int pos = 100; //ignore the first 100 char because of the standard DTD
  ref while ( (pos = rx.indexIn(text, pos)) != -1 ) {
                  //we need to look ahead to see if it's already a well formed
  link if (text.mid(pos - 6, 6) != "href=\"" && text.mid(pos - 6, 6) != "href='"
  && text.mid(pos - 6, 6) != "ttp://" && text.mid(pos - 6, 6) != "tps://"
                      )
                  {
                          if(rx.cap(1).isEmpty()==false)
                          {
                              QString linkBefore=text.mid(pos,
  rx.matchedLength()); QString linkAfter="<a href=\"" + rx.cap(1) + "\">" +
  rx.cap(1) + "</a>"; newMessage=newMessage.replace(linkBefore,linkAfter);

                          }
                          else{
                              QString linkBefore=text.mid(pos,
  rx.matchedLength()); QString linkAfter="<a href=\"" + rx.cap(2) + "\">" +
  rx.cap(2) + "</a>"; newMessage=newMessage.replace(linkBefore,linkAfter);
                          }
                  }
                  pos += rx.matchedLength();
          }
  */

  // append HTML (newMessage)
  {
    auto cursor = QTextCursor(chat->document());
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
  disconnect(&user, SIGNAL(signNewMessageReceived()), this,
             SLOT(newMessageReceived()));

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
    msgBox->show();
  }
}

void form_ChatWidget::changeWindowsTitle() {
  QString OnlineStatus;
  QString OnlineStatusIcon;
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
  }
  this->setWindowTitle(user.getName() + " [" + OnlineStatus + "]");
}

void form_ChatWidget::newFileTransfer() {
  if (user.getConnectionStatus() == ONLINE) {
    QString FilePath = QFileDialog::getOpenFileName(this, tr("Open File"), ".",
                                                    tr("all Files (*)"));
    QString Destination = user.getI2PDestination();

    if (FilePath.endsWith("/") == true) {
      // only a directory ,- dont send it
      return;
    }

    if (!FilePath.isEmpty())
      Core.getFileTransferManager()->addNewFileTransfer(FilePath, Destination);

  } else {
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setText(tr("\nCannot send files when contact is offline!"));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->show();
  }
}

void form_ChatWidget::anchorClicked(const QUrl &link) {
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

void form_ChatWidget::getFocus() {
  this->activateWindow();
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) |
                       Qt::WindowActive);
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
    mUserAvatar.loadFromData(user.getReceivedUserInfos().AvatarImage);
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
  mOwnAvatar.loadFromData(Core.getUserInfos().AvatarImage);
  ownavatar_label->setPixmap(mOwnAvatar);
}

void form_ChatWidget::setTextColor_2() {
  textColor2 = QColorDialog::getColor(textColor2, this);

  QPalette pal = textEdit->palette();
  pal.setBrush(QPalette::Text, QBrush(textColor2));
  textEdit->setPalette(pal);
}

void form_ChatWidget::setBold_2(bool t) {
  mCurrentFont2.setBold(t);

  textEdit->setCurrentFont(mCurrentFont2);
  textEdit->setFont(mCurrentFont2);
}

void form_ChatWidget::setFont_2() {
  bool ok;
  mCurrentFont2 = QFontDialog::getFont(&ok, mCurrentFont2, this);

  textEdit->setCurrentFont(mCurrentFont2);
  textEdit->setFont(mCurrentFont2);
  textEdit->setFocus();
}
void form_ChatWidget::setUnderline_2(bool t) {
  mCurrentFont2.setUnderline(t);

  textEdit->setCurrentFont(mCurrentFont2);
  textEdit->setFont(mCurrentFont2);
  textEdit->setFocus();
}

void form_ChatWidget::setItalic_2(bool t) {
  mCurrentFont2.setItalic(t);

  textEdit->setCurrentFont(mCurrentFont2);
  textEdit->setFont(mCurrentFont2);
  textEdit->setFocus();
}

void form_ChatWidget::tabIndexChanged(int tabIndex) {
  if (tabIndex == 1) {
    reloadOfflineMessages();
  }
}

void form_ChatWidget::reloadOfflineMessages() {
  offlineMessages = offlineMessages = user.getUnsentedMessages();
  OfflineMessageCount = offlineMessages.count();
  if (currentOfflineMessageIndex == 0 && OfflineMessageCount > 0) {
    currentOfflineMessageIndex = 1;
    Ui_form_chatwidget::cmd_next->setEnabled(true);
  }
  displayOfflineMessages(currentOfflineMessageIndex);
}

void form_ChatWidget::displayOfflineMessages(int index) {
  textEdit->clear();
  if (index <= OfflineMessageCount) {
    currentOfflineMessageIndex = index;

    label_currentMessageCount->setText(
        QString::number(currentOfflineMessageIndex) + " / " +
        QString::number(OfflineMessageCount));

    if (currentOfflineMessageIndex == 0) {
      if (offlineMessages.isEmpty() == false) {
        textEdit->insertHtml(offlineMessages.at(currentOfflineMessageIndex));
      }
    } else {
      textEdit->insertHtml(offlineMessages.at(currentOfflineMessageIndex - 1));
    }

    // qDebug()<<textEdit->toHtml()<<endl;
  }
  if (currentOfflineMessageIndex < OfflineMessageCount) {
    Ui_form_chatwidget::cmd_next->setEnabled(true);
  } else {
    Ui_form_chatwidget::cmd_next->setEnabled(false);
  }

  if (currentOfflineMessageIndex > 1) {
    Ui_form_chatwidget::cmd_back->setEnabled(true);
  } else {
    Ui_form_chatwidget::cmd_back->setEnabled(false);
  }
}
// UDP: by voron like offline messages does not works correctly.
void form_ChatWidget::saveChangedOfflineMessages() {
  if (textEdit->toPlainText().length() == 0) {
    cmd_delete();
  } else {
    QString NewMessage = textEdit->toHtml();

    if (NewMessage.length() < 65535) {
      if (offlineMessages.size() > 1)
        offlineMessages.replace(currentOfflineMessageIndex - 1, NewMessage);
      // qDebug()<<textEdit->toHtml()<<endl;
      user.setUnsentedMessages(offlineMessages);

    } else {
      QMessageBox *msgBox = new QMessageBox(this);
      msgBox->setIcon(QMessageBox::Critical);
      msgBox->setText("I2PChat");
      msgBox->setInformativeText(tr("Sorry, the chatmessage is too long!"));
      msgBox->setStandardButtons(QMessageBox::Ok);
      msgBox->setDefaultButton(QMessageBox::Ok);
      msgBox->setWindowModality(Qt::NonModal);
      msgBox->show();
    }
  }
}

void form_ChatWidget::cmd_back() {
  if (currentOfflineMessageIndex >= 1) {
    displayOfflineMessages(currentOfflineMessageIndex - 1);
  }
}

void form_ChatWidget::cmd_next() {
  if (currentOfflineMessageIndex + 1 <= OfflineMessageCount) {
    displayOfflineMessages(currentOfflineMessageIndex + 1);
  }
}

void form_ChatWidget::cmd_delete() {
  if (offlineMessages.isEmpty() == false) {
    offlineMessages.removeAt(currentOfflineMessageIndex - 1);
    OfflineMessageCount = offlineMessages.count();
  }

  if (currentOfflineMessageIndex > OfflineMessageCount) {
    currentOfflineMessageIndex = OfflineMessageCount;
  }

  user.setUnsentedMessages(offlineMessages);
  reloadOfflineMessages();
}

void form_ChatWidget::cmd_new() {
  offlineMessages.push_back("");
  OfflineMessageCount = offlineMessages.count();

  displayOfflineMessages(OfflineMessageCount);
}
