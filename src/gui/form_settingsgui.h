// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef SETTINGSGUI_H
#define SETTINGSGUI_H

#include "UserManager.h"
#include "ui_form_settingsgui.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QFontDialog>
#include <QMap>
#include <QMapIterator>
#include <QSettings>
#include <QStyleFactory>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtGui>

class CCore;
class form_settingsgui : public QDialog, private Ui::form_settingsgui {
  Q_OBJECT

public:
  form_settingsgui(CCore &Core, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
  ~form_settingsgui();

  // forbid some operators
  form_settingsgui(const form_settingsgui &) = delete;
  form_settingsgui &operator=(const form_settingsgui &) = delete;

private slots:
  void loadSettings();
  void saveSettings();
  void on_styleCombo_activated(const QString &styleName);
  void on_styleSheetCombo_activated(const QString &styleSheetName);
  void showUserBlockList();

  void clicked_openFile();
  void clicked_openFile2();
  void clicked_openFile3();
  void clicked_openFile4();
  void clicked_openFile5();
  void clicked_openFile6();
  void clicked_DestinationGenerate();
  void clicked_IncomingFileFolder();

  void clicked_Gender_Male(bool state);
  void clicked_Gender_Female(bool state);

  void clicked_sortingEnabled(bool enabled);
  void clicked_sortAlphabetically(bool checked);
  void clicked_sortByDateAdded(bool checked);
  void clicked_sortByLastCommunication(bool checked);
  void clicked_sortByLastOnline(bool checked);

  void clicked_ChatMessageTextColor();
  void clicked_ChatMessageBold(bool t);
  void clicked_ChatMessageFont();
  void clicked_ChatMessageItalic(bool t);
  void clicked_ChatMessageUnderline(bool t);

  void clicked_OverWriteChatMessageTextColor();
  void clicked_OverWriteChatMessageBold(bool t);
  void clicked_OverWriteChatMessageFont();
  void clicked_OverWriteChatMessageItalic(bool t);
  void clicked_OverWriteChatMessageUnderline(bool t);

  void clicked_BlockListDelete();
  void clicked_BlockListUnblock();
  void clicked_EnableUserSearch(bool t);
  void clicked_BlockAllUnknownUsers(bool checked);
  void clicked_RequestAuthorization(bool checked);

  void clicked_SelectAvatarImage();
  void clicked_ClearAvatarImage();
  void clicked_AutoAwayEnabled(bool enabled);
  void clicked_nonPersistDest(bool checked);
  void setCustomStyleSheet();
  void populateChatStyleCombo();
  void slotThemeDirChanged();

private:
  CCore &mCore;
  const QString mConfigPath;
  QSettings *settings;
  QByteArray avatarImageByteArray;
  QFileSystemWatcher *mThemeWatcher;

  void loadStyleSheet(const QString &sheetName);
  void loadqss();
};

#endif
