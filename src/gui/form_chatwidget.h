// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FORM_CHATWIDGET_h
#define FORM_CHATWIDGET_h

#include "ChatDelegate.h"
#include "Core.h"
#include "TextEmotionChanger.h"
#include "gui_icons.h"
#include "ui_form_chatwidget.h"

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QFocusEvent>
#include <QFontDialog>
#include <QKeyEvent>
#include <QListView>
#include <QMimeData>
#include <QMouseEvent>
#include <QPalette>
#include <QPushButton>
#include <QScreen>
#include <QScrollBar>
#include <QSettings>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QTextBrowser>
#include <QTimer>
#include <QUrl>
#include <Qt>
#include <QtGui>

class ChatEventEater : public QObject {
  Q_OBJECT

public:
  ChatEventEater(QWidget *parent = 0)
    : QObject(parent) {}

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

  struct ChatBubbleStyle {
    QString sentBg, sentColor;
    QString receivedBg, receivedColor;
    QString systemColor;
    int radius;
    int padV, padH;
    int padVInner;
    bool alignRight;
  };

  void startFileTransfer(const QString &filePath);
  void getFocus();
  void loadChatStyle();

protected:
  void applyThemeCss(const QString &style, ChatBubbleStyle &bs, ChatDelegate::BubbleColors &dc);
  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);

private slots:
  void sendMessageSignal();
  void addAllMessages();
  void addAllMessagesClassic();
  void addMessage(QString text);
  void setTextColor();
  void newMessageReceived();
  void setBold(bool t);
  void setFont();
  void setUnderline(bool t);
  void setItalic(bool t);

  void changeWindowsTitle();
  void newFileTransfer();

  void anchorClicked(const QUrl &);
  void focusEvent(bool b);
  void showContextMenu(const QPoint &pos);
  void showAvatarFrame(bool show);
  void remoteAvatarImageChanged();
  void messageTextChanged();

signals:
  void sendChatMessage(QString chatMessage);
  void closingChatWindow(QString);

public slots:
  void slotLoadOwnAvatarImage();
  void slotPendingCanceled();

private:
  void closeEvent(QCloseEvent *e);
  void centerDialog();

  ChatBubbleStyle mBubbleStyle;

  QColor textColor;

  CUser &user;
  QFont mCurrentFont;

  ChatEventEater *m_event_eater;
  CCore &Core;
  bool mHaveFocus;
  QTextEdit mControllForChange;
  void keyPressEvent(QKeyEvent *event);
  QPixmap mOwnAvatar;
  QPixmap mUserAvatar;

  QStandardItemModel *mChatModel;
  ChatDelegate *mChatDelegate;
  QListView *mChatListView;
  QStackedWidget *mChatStack;
  QString mChatStyle;
  QFileSystemWatcher *mFileWatcher;
  QTimer *mReloadTimer;
};
#endif
