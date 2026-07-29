// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FORM_CHATWIDGET_h
#define FORM_CHATWIDGET_h

#include "ChatDelegate.h"
#include "Core.h"
#include "FileTransferReceive.h"
#include "FileTransferSend.h"
#include "TextEmotionChanger.h"
#include "gui_icons.h"
#include "ui_ChatWidget.h"

#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QColorDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QKeyEvent>
#include <QListView>
#include <QMainWindow>
#include <QMimeData>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QTimer>
#include <QUrl>
#include <Qt>

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
class ChatWidget : public QMainWindow, public Ui::ChatWidget {
  Q_OBJECT
public:
  ChatWidget(CUser &user, CCore &Core, QDialog *parent = 0);
  ~ChatWidget();

  // forbid some operators
  ChatWidget(const ChatWidget &) = delete;
  ChatWidget &operator=(const ChatWidget &) = delete;

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
  void slotFileTransferCreated(qint32 streamID,
                               const QString &fileName,
                               quint64 fileSize,
                               bool isSend,
                               const QString &destination);
  void slotTransferUpdate();
  void slotTransferETA(const QString &eta);
  void slotTransferSpeed(const QString &speed, const QString &type);
  void slotTransferCompleted();
  void slotTransferAborted();

private:
  static const int TransferStreamIdRole = Qt::UserRole + 6;
  static const int TransferETARole = Qt::UserRole + 9;
  static const int TransferSpeedRole = Qt::UserRole + 10;

  void closeEvent(QCloseEvent *e);
  void centerDialog();
  static QString transferIconHtml(bool isSend);
  static QString transferProgressHtml(const QString &timePart,
                                      const QString &fileName,
                                      quint64 transferred,
                                      quint64 total,
                                      const QString &speed,
                                      const QString &eta,
                                      qint32 streamID,
                                      bool isSend);
  void updateTransferItem(QObject *s);

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
