// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FORM_TOPICSUBSCRIBE_H
#define FORM_TOPICSUBSCRIBE_H

#include "gui_icons.h"
#include "ui_form_topicSubscribe.h"

#include <QMap>
#include <QtGlobal>
#include <QtGui>

class CCore;
class form_topicSubscribe : public QDialog, private Ui::form_topicSubscribe {

  Q_OBJECT

public:
  form_topicSubscribe(CCore &Core);
  ~form_topicSubscribe();

  // forbid some operators
  form_topicSubscribe(const form_topicSubscribe &) = delete;
  form_topicSubscribe &operator=(const form_topicSubscribe &) = delete;

  void requestFocus();

signals:
  void signClosingTopicSubscribeWindow();

private slots:
  void slot_cmdSubscribe();
  void slot_showContextMenu(const QPoint &);
  void slot_openTopic();
  void slot_onlineStateChanged();

private:
  CCore &mCore;
  void init();
  void closeEvent(QCloseEvent *e);
  void keyPressEvent(QKeyEvent *event);
};
#endif /* of FORM_TOPICSUBSCRIBE_H */
