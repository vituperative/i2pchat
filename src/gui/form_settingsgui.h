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

#ifndef SETTINGSGUI_H
#define SETTINGSGUI_H

#include "ui_form_settingsgui.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QFontDialog>
#include <QMap>
#include <QMapIterator>
#include <QSettings>
#include <QStyleFactory>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtGui>

#include "UserManager.h"

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
  void setCustomStyleSheet();

private:
  CCore &mCore;
  const QString mConfigPath;
  QSettings *settings;
  QByteArray avatarImageByteArray;

  void loadStyleSheet(const QString &sheetName);
  void loadqss();
};

#endif
