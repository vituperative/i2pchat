// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FORM_USERSEARCH_H
#define FORM_USERSEARCH_H

#include "gui_icons.h"
#include "ui_form_userSearch.h"

#include <QMap>
#include <QMenu>
#include <QtGlobal>
#include <QtGui>

class CCore;
class form_userSearch : public QDialog, private Ui::form_userSearch {

  Q_OBJECT

public:
  form_userSearch(CCore &Core);
  ~form_userSearch();

  // forbid some operators
  form_userSearch(const form_userSearch &) = delete;
  form_userSearch &operator=(const form_userSearch &) = delete;

  void getFocus();

signals:
  void signClosingUserSearchWindow();

private slots:
  void slot_cmdSearch();
  void slot_showContextMenu(const QPoint &);
  void slot_addUser();
  void slot_onlineStateChanged();
  void slot_genderMale();
  void slot_genderFemale();

private:
  CCore &mCore;
  void init();
  void closeEvent(QCloseEvent *e);
  void keyPressEvent(QKeyEvent *event);
};
#endif
