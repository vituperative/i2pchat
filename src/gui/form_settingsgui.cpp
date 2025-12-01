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
#include "form_settingsgui.h"
#include "Base.cpp"
#include "Core.h"
#include "UserBlockManager.h"
#include <QFileDialog>

form_settingsgui::form_settingsgui(CCore &Core, QWidget *parent,
                                   Qt::WindowFlags flags)
    : QDialog(parent, flags), mCore(Core), mConfigPath(Core.getConfigPath()) {
  setupUi(this);
  this->setAttribute(Qt::WA_DeleteOnClose, true);

  settings =
      new QSettings(mConfigPath + "/application.ini", QSettings::IniFormat);

  loadqss();
  styleCombo->addItems(QStyleFactory::keys());
  loadSettings();
  showUserBlockList();

  connect(ok_Button, SIGNAL(clicked(bool)), this, SLOT(saveSettings()));

  connect(cancel_Button, SIGNAL(clicked(bool)), this, SLOT(close()));

  connect(cmd_openFile, SIGNAL(clicked(bool)), this, SLOT(clicked_openFile()));

  connect(cmd_openFile_2, SIGNAL(clicked(bool)), this,
          SLOT(clicked_openFile2()));

  connect(cmd_openFile_3, SIGNAL(clicked(bool)), this,
           SLOT(clicked_openFile3()));

  connect(cmd_openFile_4, SIGNAL(clicked(bool)), this,
           SLOT(clicked_openFile4()));

  connect(checkBox_5, SIGNAL(clicked(bool)), this,
           SLOT(clicked_sortingEnabled(bool)));
  connect(radioButton_2, SIGNAL(clicked(bool)), this,
           SLOT(clicked_sortAlphabetically(bool)));
  connect(radioButton_3, SIGNAL(clicked(bool)), this,
           SLOT(clicked_sortByDateAdded(bool)));
  connect(radioButton_4, SIGNAL(clicked(bool)), this,
           SLOT(clicked_sortByLastCommunication(bool)));
  connect(radioButton_5, SIGNAL(clicked(bool)), this,
           SLOT(clicked_sortByLastOnline(bool)));

  connect(cmd_openFile_5, SIGNAL(clicked(bool)), this,
          SLOT(clicked_openFile5()));

  connect(cmd_openFile_6, SIGNAL(clicked(bool)), this,
          SLOT(clicked_openFile6()));

  connect(cmd_DestGenerate, SIGNAL(clicked(bool)), this,
          SLOT(clicked_DestinationGenerate()));

  connect(cmd_Downloads, SIGNAL(clicked(bool)), this,
          SLOT(clicked_IncomingFileFolder()));

  connect(checkGender_Male, SIGNAL(clicked(bool)), this,
          SLOT(clicked_Gender_Male(bool)));

  connect(checkGender_Female, SIGNAL(clicked(bool)), this,
          SLOT(clicked_Gender_Female(bool)));

  connect(cmdBoldChat, SIGNAL(clicked(bool)), this,
          SLOT(clicked_ChatMessageBold(bool)));

  connect(cmdUnderChat, SIGNAL(clicked(bool)), this,
          SLOT(clicked_ChatMessageUnderline(bool)));

  connect(cmdItalicChat, SIGNAL(clicked(bool)), this,
          SLOT(clicked_ChatMessageItalic(bool)));

  connect(cmdColorChat, SIGNAL(clicked(bool)), this,
          SLOT(clicked_ChatMessageTextColor()));

  connect(cmdFontChat, SIGNAL(clicked(bool)), this,
          SLOT(clicked_ChatMessageFont()));

  connect(cmdOverwriteBoldChat, SIGNAL(clicked(bool)), this,
          SLOT(clicked_OverWriteChatMessageBold(bool)));

  connect(cmdOverwriteUnderChat, SIGNAL(clicked(bool)), this,
          SLOT(clicked_OverWriteChatMessageUnderline(bool)));

  connect(cmdOverwriteItalicChat, SIGNAL(clicked(bool)), this,
          SLOT(clicked_OverWriteChatMessageItalic(bool)));

  connect(cmdOverwriteColorChat, SIGNAL(clicked(bool)), this,
          SLOT(clicked_OverWriteChatMessageTextColor()));

  connect(cmdOverwriteFontChat, SIGNAL(clicked(bool)), this,
          SLOT(clicked_OverWriteChatMessageFont()));

  connect(cmd_deleteUserFromBlockList, SIGNAL(clicked()), this,
          SLOT(clicked_BlockListDelete()));

  connect(cmd_unblockUserFromBlockList, SIGNAL(clicked()), this,
          SLOT(clicked_BlockListUnblock()));

  connect(check_UserSearchEnable, SIGNAL(clicked(bool)), this,
          SLOT(clicked_EnableUserSearch(bool)));

  connect(checkBox_AutoAcceptFiles, SIGNAL(clicked(bool)), checkBox_Subfolders,
          SLOT(setChecked(bool)));

  connect(checkBox_AutoAcceptFiles, SIGNAL(toggled(bool)), checkBox_Subfolders,
          SLOT(setEnabled(bool)));

  connect(cmd_selectAvatarImage, SIGNAL(clicked()), this,
          SLOT(clicked_SelectAvatarImage()));

  connect(cmd_clearAvatarImage, SIGNAL(clicked()), this,
          SLOT(clicked_ClearAvatarImage()));
  
  connect(blockallcheckBox, SIGNAL(clicked(bool)), this,
          SLOT(clicked_BlockAllUnknownUsers(bool)));
  
  connect(requestAuthcheckBox, SIGNAL(clicked(bool)), this,
          SLOT(clicked_RequestAuthorization(bool)));
  
  // TODO: Add this button from ui file. (QtCreator, i dont have it)
  // connect(setCustomSheetButton, SIGNAL( clicked(bool) ), this, SLOT(
  // setCustomStyleSheet() ) );
}

form_settingsgui::~form_settingsgui() {
  settings->sync();
  delete (settings);
}

void form_settingsgui::loadSettings() {
  settings->beginGroup("General");
  spinBox->setValue(settings->value("Debug_Max_Message_count", "20").toInt());
  OfflineChkSpinBox->setValue(
      settings->value("Waittime_between_rechecking_offline_users", "1000")
          .toInt() /
      1000);
  checkBox_DebugLog->setChecked(
      settings->value(("DebugLogging"), "true").toBool());

  if (settings->value("current_Style", "").toString().isEmpty() == false)
    styleCombo->setCurrentIndex(
        styleCombo->findText(settings->value("current_Style", "").toString()));
  else {
    // find default Style for this System
    QRegExp regExp("Q(.*)Style");
    QString defaultStyle = QApplication::style()->metaObject()->className();

    if (defaultStyle == QLatin1String("QMacStyle"))
      defaultStyle = QLatin1String("Macintosh (Aqua)");
    else if (regExp.exactMatch(defaultStyle))
      defaultStyle = regExp.cap(1);

    styleCombo->setCurrentIndex(styleCombo->findText(defaultStyle));
  }

  settings->beginGroup("UserList");
  checkBox_5->setChecked(settings->value("SortingEnabled", false).toBool());
  int sortType = settings->value("SortType", 0).toInt();
  
  // Set radio button states
  radioButton_2->setChecked(sortType == 0);
  radioButton_3->setChecked(sortType == 1);
  radioButton_4->setChecked(sortType == 2);
  radioButton_5->setChecked(sortType == 3);
  
  // Enable/disable radio buttons based on checkbox state
  bool sortingEnabled = settings->value("SortingEnabled", false).toBool();
  radioButton_2->setEnabled(sortingEnabled);
  radioButton_3->setEnabled(sortingEnabled);
  radioButton_4->setEnabled(sortingEnabled);
  radioButton_5->setEnabled(sortingEnabled);
  
  settings->endGroup();

  settings->beginGroup("Network");

  checkBox_AutoAcceptFiles->setChecked(
      settings->value("AutoAcceptFileReceive", false).toBool());
  if (checkBox_AutoAcceptFiles->isChecked() == true) {
    cmd_Downloads->setEnabled(true);
    checkBox_Subfolders->setChecked(
        settings->value("UseIncomingSubFolderForEveryUser", false).toBool());
    txt_IncomingFileFolder->setReadOnly(false);
    txt_IncomingFileFolder->setEnabled(true);
  } else {
    checkBox_Subfolders->setDisabled(true);
    txt_IncomingFileFolder->setReadOnly(true); // no workee :(
  }

  txt_IncomingFileFolder->setText(
      settings->value("IncomingFileFolder", mConfigPath + "/Incoming")
          .toString());

  styleSheetCombo->setCurrentIndex(styleSheetCombo->findText(
      settings->value("current_Style_sheet", "Default").toString()));
  settings->endGroup();

  settings->beginGroup("Network");
  lineEdit_3->setText(settings->value("SamHost", "127.0.0.1").toString());
  lineEdit->setText(settings->value("TunnelName", "I2PChat").toString());
  spinBox_10->setValue(settings->value("SamPort", "7656").toInt());

  spinBox_4->setMinimum(1);
  spinBox_4->setValue(settings->value("inbound.length", "3").toInt());
  spinBox_4->setMaximum(7);

  spinBox_5->setMinimum(0);
  spinBox_5->setValue(settings->value("inbound.quantity", "1").toInt());
  spinBox_5->setMaximum(3);

  spinBox_6->setMinimum(0);
  spinBox_6->setValue(settings->value("inbound.backupQuantity", "1").toInt());
  spinBox_6->setMaximum(3);

  spinBox_7->setMinimum(0);
  spinBox_7->setValue(settings->value("outbound.backupQuantity", "1").toInt());
  spinBox_7->setMaximum(3);

  spinBox_8->setMinimum(1);
  spinBox_8->setValue(settings->value("outbound.length", "3").toInt());
  spinBox_8->setMaximum(7);

  spinBox_9->setMinimum(0);
  spinBox_9->setValue(settings->value("outbound.quantity", "1").toInt());
  spinBox_9->setMaximum(3);

  comboBox_SigType->setEditable(false);
  comboBox_SigType->setCurrentIndex(comboBox_SigType->findText(
      settings->value("SIGNATURE_TYPE", "EdDSA_SHA512_Ed25519").toString()));
  settings->endGroup();

  settings->beginGroup("Sound");
  settings->beginGroup("SoundFilePath");
  txt_SoundFile->setText(
      settings->value("User_go_Online", "./sounds/Startup.wav").toString());
  txt_SoundFile2->setText(settings->value("User_go_Offline", "").toString());
  txt_SoundFile3->setText(settings->value("FileSend_Finished", "").toString());
  txt_SoundFile4->setText(
      settings->value("FileReceive_Incoming", "./sounds/File.wav").toString());
  txt_SoundFile5->setText(
      settings->value("FileReceive_Finished", "").toString());
  txt_SoundFile6->setText(
      settings->value("NewChatMessage", "./sounds/Notify.wav").toString());

  if (!txt_SoundFile->text().isEmpty())
    checkBoxSound->setEnabled(true);
  if (!txt_SoundFile2->text().isEmpty())
    checkBoxSound_2->setEnabled(true);
  if (!txt_SoundFile3->text().isEmpty())
    checkBoxSound_3->setEnabled(true);
  if (!txt_SoundFile4->text().isEmpty())
    checkBoxSound_4->setEnabled(true);
  if (!txt_SoundFile5->text().isEmpty())
    checkBoxSound_5->setEnabled(true);
  if (!txt_SoundFile6->text().isEmpty())
    checkBoxSound_6->setEnabled(true);

  settings->endGroup();

  settings->beginGroup("Enable");
  checkBoxSound->setChecked(settings->value("User_go_Online", true).toBool());
  checkBoxSound_2->setChecked(
      settings->value("User_go_Offline", false).toBool());
  checkBoxSound_3->setChecked(
      settings->value("FileSend_Finished", false).toBool());
  checkBoxSound_4->setChecked(
      settings->value("FileReceive_Incoming", true).toBool());
  checkBoxSound_5->setChecked(
      settings->value("FileReceive_Finished", false).toBool());
  checkBoxSound_6->setChecked(settings->value("NewChatMessage", true).toBool());
  settings->endGroup();

  settings->endGroup();

  settings->beginGroup("User-Infos");
  txt_Nickname->setMaxLength(12);
  txt_Nickname->setText(settings->value("Nickname", "").toString());
  spinAge->setValue(settings->value("Age", "0").toInt());

  if (settings->value("Gender", "").toString() == "Male") {
    checkGender_Male->setChecked(true);
    checkGender_Female->setChecked(false);
  } else if (settings->value("Gender", "").toString() == "Female") {
    checkGender_Male->setChecked(false);
    checkGender_Female->setChecked(true);
  } else {
    checkGender_Male->setChecked(false);
    checkGender_Female->setChecked(false);
  }

  txt_Interests->setPlainText(settings->value("Interests", "").toString());

  QPixmap tmpPixmap;
  avatarImageByteArray = settings->value("AvatarBinaryImage", "").toByteArray();
  tmpPixmap.loadFromData(avatarImageByteArray);
  ownavatar_label->setAlignment(Qt::AlignCenter);
  ownavatar_label->setPixmap(tmpPixmap);
  ownavatar_label_2->setAlignment(Qt::AlignCenter);
  ownavatar_label_2->setPixmap(tmpPixmap);
  
  // Clear any default pixmap that might be set in UI
  if (avatarImageByteArray.isEmpty()) {
    ownavatar_label_2->clear();
  }

  settings->endGroup();

  settings->beginGroup("Chat");
  txtShowCurrentChatStyle->setText("Local settings preview");
  txtOverrideRemote->setText("Remote override preview");

  txtShowCurrentChatStyle->selectAll();
  QFont font;
  QColor color;

  font.fromString(settings->value("DefaultFont", "SansSerif,10").toString());
  color.setNamedColor(settings->value("DefaultColor", "#000000").toString());

  txtShowCurrentChatStyle->setFont(font);
  txtShowCurrentChatStyle->setTextColor(color);
  txtShowCurrentChatStyle->moveCursor(QTextCursor::End,
                                      QTextCursor::MoveAnchor);
  txtShowCurrentChatStyle->textCursor().clearSelection();

  // override remote chatmessage settings -> font/color
  chatOverrideBox->setChecked(settings->value("Override", false).toBool());
  if (chatOverrideBox->isChecked() == false) {
    txtOverrideRemote->selectAll();
    QFont font;
    QColor color;

    font.fromString(
        settings->value("FontForOverwrite", "SansSerif,10").toString());
    color.setNamedColor(
        settings->value("ColorForOverwrite", "#000000").toString());

    txtOverrideRemote->setFont(font);
    txtOverrideRemote->setTextColor(color);

    txtOverrideRemote->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    txtOverrideRemote->textCursor().clearSelection();
  } else {
    txtOverrideRemote->selectAll();
    QFont font;
    font.fromString(
        settings->value("FontForOverwrite", "SansSerif,10").toString());
    QColor color(settings->value("ColorForOverwrite", "#000000").toString());

    txtOverrideRemote->setFont(font);
    txtOverrideRemote->setTextColor(color);
    txtOverrideRemote->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    txtOverrideRemote->textCursor().clearSelection();
  }

  checkboxUserEvents->setChecked(
      settings->value("LogOnlineStatesOfUsers", true).toBool());

  // TODO: Connection timeout from I2PStream.cpp or nuke ?
  // spinBox_maxACK->setMinimum(20);
  // spinBox_maxACK->setMaximum(180);
  // spinBox_maxACK->setValue(settings->value("MaxChatmessageACKTimeInSec","120").toInt());

  settings->endGroup();

  settings->beginGroup("Security");
  if (settings->value("BlockStyle", "Normal").toString() == "Normal") {
    check_BlockNormal->setChecked(true);
  } else {

    check_BlockInvisible->setChecked(true);
  }

  if (settings->value("WebProfile", "Enabled").toString() == "Enabled") {
    WebProfileCheckbox->setChecked(true);
  } else {
    WebProfileCheckbox->setChecked(false);
  }
  if (settings->value("HideWebProfileWhenInvisible", "True").toString() ==
      "True") {
    HideWebCheckbox->setChecked(true);
  } else {
    HideWebCheckbox->setChecked(false);
  }

  if (!mCore.getMyDestination().isEmpty()) {
    size_t buffersize = 2048;
    uint8_t *outputbuffer = (uint8_t *)malloc(buffersize);
    char *b32buffer = (char *)malloc(buffersize);
    QByteArray sha256hash;
    int outputcount = i2p::data::Base64ToByteStream(
        mCore.getMyDestination().toUtf8().constData(),
        mCore.getMyDestination().size(), outputbuffer, buffersize);
    QByteArray qarraysha256hash = QByteArray((char *)outputbuffer, outputcount);
    while (outputcount > qarraysha256hash.size()) {
      qarraysha256hash.append((char)0);
    }
    sha256hash =
        QCryptographicHash::hash(qarraysha256hash, QCryptographicHash::Sha256);
    outputcount = i2p::data::ByteStreamToBase32(
        (uint8_t *)sha256hash.data(), sha256hash.size(), b32buffer, 52);
    b32buffer[52] = '\0';
    QString strb32address = "http://" + QString(b32buffer) + ".b32.i2p";
    b32address->setPlainText(QApplication::translate(
        "form_settingsgui", strb32address.toUtf8().constData(), Q_NULLPTR));
    free(outputbuffer);
    free(b32buffer);
  } else {
    b32address->setPlainText(QApplication::translate(
        "form_settingsgui", "b32 address will be displayed when online",
        Q_NULLPTR));
  }
  settings->endGroup();

  settings->beginGroup("Usersearch");
  if (txt_Nickname->text().isEmpty() == false &&
      (settings->value("Enabled", true).toBool()) == true) {
    check_UserSearchEnable->setChecked(true);
  } else {
    check_UserSearchEnable->setChecked(false);
  }

  spinBox_MaxLogMsgUserSearch->setMinimum(0);
  spinBox_MaxLogMsgUserSearch->setMaximum(200);
  spinBox_MaxLogMsgUserSearch->setValue(
      settings->value("Debug_Max_Message_count", 100).toInt());
  spinBox_ReAnnounceUserSearch->setMinimum(0);
  spinBox_ReAnnounceUserSearch->setMaximum(23);
  spinBox_ReAnnounceUserSearch->setValue(
      settings->value("ReAnnounceTimerInHours", 1).toInt());
  settings->endGroup();
  settings->sync();
}
void form_settingsgui::saveSettings() {
  settings->beginGroup("General");
  settings->setValue("Debug_Max_Message_count", spinBox->value());
  settings->setValue("Waittime_between_rechecking_offline_users",
                     OfflineChkSpinBox->value() * 1000);
  settings->setValue("current_Style", styleCombo->currentText());
  settings->setValue("current_Style_sheet", styleSheetCombo->currentText());
  settings->setValue("AutoAcceptFileReceive",
                     checkBox_AutoAcceptFiles->isChecked());
  settings->setValue("IncomingFileFolder", txt_IncomingFileFolder->text());
  settings->setValue("UseIncomingSubFolderForEveryUser",
                     checkBox_Subfolders->isChecked());
  settings->setValue("DebugLogging", checkBox_DebugLog->isChecked());
  settings->endGroup();

  settings->beginGroup("Network");
  settings->setValue("SamHost", lineEdit_3->text());
  settings->setValue("TunnelName", lineEdit->text());
  settings->setValue("SamPort", spinBox_10->value());
  // Inbound options
  settings->setValue("inbound.quantity", spinBox_5->value());
  settings->setValue("inbound.backupQuantity", spinBox_6->value());
  settings->setValue("inbound.length", spinBox_4->value());
  // Outpound options
  settings->setValue("outbound.quantity", spinBox_9->value());
  settings->setValue("outbound.backupQuantity", spinBox_7->value());
  settings->setValue("outbound.length", spinBox_8->value());

  // Signature_type
  settings->setValue("SIGNATURE_TYPE", comboBox_SigType->currentText());
  settings->endGroup();

  settings->beginGroup("Sound");
  settings->beginGroup("Enable");
  settings->setValue("User_go_Online", checkBoxSound->isChecked());
  settings->setValue("User_go_Offline", checkBoxSound_2->isChecked());
  settings->setValue("FileSend_Finished", checkBoxSound_3->isChecked());
  settings->setValue("FileReceive_Incoming", checkBoxSound_4->isChecked());
  settings->setValue("FileReceive_Finished", checkBoxSound_5->isChecked());
  settings->setValue("NewChatMessage", checkBoxSound_6->isChecked());
  settings->endGroup();
  settings->beginGroup("SoundFilePath");
  settings->setValue("User_go_Online", txt_SoundFile->text());
  settings->setValue("User_go_Offline", txt_SoundFile2->text());
  settings->setValue("FileSend_Finished", txt_SoundFile3->text());
  settings->setValue("FileReceive_Incoming", txt_SoundFile4->text());
  settings->setValue("FileReceive_Finished", txt_SoundFile5->text());
  settings->setValue("NewChatMessage", txt_SoundFile6->text());
  settings->endGroup();
  settings->endGroup();

  settings->beginGroup("Style"); // application.ini
  // settings->setValue("CurrentStyle", "Fusion");
  settings->setValue("CustomStyleSheet", "");
  settings->endGroup();

  settings->beginGroup("User-Infos");
  settings->setValue("Nickname", txt_Nickname->text());
  settings->setValue("Age", spinAge->value());
  if (checkGender_Male->isChecked() == true) {
    settings->setValue("Gender", "Male");
  } else if (checkGender_Female->isChecked() == true) {
    settings->setValue("Gender", "Female");
  } else {
    settings->setValue("Gender", "");
  }
  settings->setValue("Interests", txt_Interests->toPlainText());
  settings->setValue("AvatarBinaryImage", avatarImageByteArray);
  settings->endGroup();

  settings->beginGroup("Chat");
  settings->setValue("DefaultFont",
                     txtShowCurrentChatStyle->currentFont().toString());
  settings->setValue("DefaultColor",
                     txtShowCurrentChatStyle->textColor().name());

  // override remoute chatmessageSettings Font/Color
  settings->setValue("Override", chatOverrideBox->isChecked());
  settings->setValue("FontForOverwrite",
                     txtOverrideRemote->currentFont().toString());
  settings->setValue("ColorForOverwrite",
                     txtOverrideRemote->textColor().name());
  settings->setValue("LogOnlineStatesOfUsers", checkboxUserEvents->isChecked());
  settings->setValue("MaxChatmessageACKTimeInSec", spinBox_maxACK->value());
  settings->endGroup();

  settings->beginGroup("Security");
  if (check_BlockInvisible->isChecked() == true) {
    settings->setValue("BlockStyle", "Invisible");
  } else {
    settings->setValue("BlockStyle", "Normal");
  }
  if (WebProfileCheckbox->isChecked() == true) {
    settings->setValue("WebProfile", "Enabled");
  } else {
    settings->setValue("WebProfile", "Disabled");
  }
  if (HideWebCheckbox->isChecked() == true) {
    settings->setValue("HideWebProfileWhenInvisible", "True");
  } else {
    settings->setValue("HideWebProfileWhenInvisible", "False");
  }

  settings->endGroup();

  // TODO: DHT implementation
  settings->beginGroup("Usersearch");
  
  // Load security settings
  settings->beginGroup("Security");
  blockallcheckBox->setChecked(settings->value("BlockAllUnknownUsers", false).toBool());
  requestAuthcheckBox->setChecked(settings->value("RequestAuthorization", false).toBool());
  requestAuthcheckBox->setEnabled(true);  // Ensure checkbox is enabled
  settings->endGroup();
  settings->setValue("Enabled", check_UserSearchEnable->isChecked());
  settings->setValue("Debug_Max_Message_count",
                     spinBox_MaxLogMsgUserSearch->value());
  settings->setValue("ReAnnounceTimerInHours",
                     spinBox_ReAnnounceUserSearch->value());
  settings->endGroup();

  settings->sync();
  mCore.loadUserInfos();
  mCore.getUserManager()->avatarImageChanged();
  
  // Refresh avatar display in config panel AFTER avatar update signals have been sent
  QPixmap tmpPixmap;
  avatarImageByteArray = settings->value("AvatarBinaryImage", "").toByteArray();
  
  // Decode B64 string to raw binary data if needed
  if (!avatarImageByteArray.isEmpty()) {
    // Check if data is B64 encoded by trying to decode
    QByteArray decodedData = QByteArray::fromBase64(avatarImageByteArray);
    if (!decodedData.isEmpty()) {
      // Use decoded data if successful
      tmpPixmap.loadFromData(decodedData);
    } else {
      // Fall back to original data if decoding fails
      tmpPixmap.loadFromData(avatarImageByteArray);
    }
  } else {
    // Use original data if empty
    tmpPixmap.loadFromData(avatarImageByteArray);
  }
  
  ownavatar_label->setAlignment(Qt::AlignCenter);
  ownavatar_label->setPixmap(tmpPixmap);
  ownavatar_label_2->setAlignment(Qt::AlignCenter);
  ownavatar_label_2->setPixmap(tmpPixmap);
  if (avatarImageByteArray.isEmpty()) {
    ownavatar_label_2->clear();
  }
  
  this->close();
}

void form_settingsgui::on_styleCombo_activated(const QString &styleName) {
  qApp->setStyle(styleName);
}

void form_settingsgui::on_styleSheetCombo_activated(const QString &sheetName) {
  loadStyleSheet(sheetName);
}

void form_settingsgui::loadStyleSheet(const QString &sheetName) {
  //
  // external Stylesheets
  QFile file(mConfigPath + "/qss/" + sheetName.toLower() + ".qss");
  if (file.exists()) {
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());

    qApp->setStyleSheet(styleSheet);
  } else
    qWarning() << "WARNING: stylesheet file is broken";
}

void form_settingsgui::loadqss() {

  QFileInfoList slist = QDir(mConfigPath + "/qss/").entryInfoList();
  foreach (QFileInfo st, slist) {
    if (st.fileName() != "." && st.fileName() != ".." && st.isFile())
      styleSheetCombo->addItem(st.fileName().remove(".qss"));
  }
}

void form_settingsgui::clicked_openFile() {
  txt_SoundFile->setText(
      QFileDialog::getOpenFileName(this, tr("Open File"), ".", "wav (*.wav)"));
  if (txt_SoundFile->text().isEmpty()) {
    checkBoxSound->setChecked(false);
    checkBoxSound->setEnabled(false);
  } else
    checkBoxSound->setEnabled(true);
}

void form_settingsgui::clicked_openFile2() {
  txt_SoundFile2->setText(
      QFileDialog::getOpenFileName(this, tr("Open File"), ".", "wav (*.wav)"));
  if (txt_SoundFile2->text().isEmpty()) {
    checkBoxSound_2->setChecked(false);
    checkBoxSound_2->setEnabled(false);
  } else
    checkBoxSound_2->setEnabled(true);
}
void form_settingsgui::clicked_openFile3() {
  txt_SoundFile3->setText(
      QFileDialog::getOpenFileName(this, tr("Open File"), ".", "wav (*.wav)"));
  if (txt_SoundFile3->text().isEmpty()) {
    checkBoxSound_3->setChecked(false);
    checkBoxSound_3->setEnabled(false);
  } else
    checkBoxSound_3->setEnabled(true);
}
void form_settingsgui::clicked_openFile4() {
  txt_SoundFile4->setText(
      QFileDialog::getOpenFileName(this, tr("Open File"), ".", "wav (*.wav)"));
  if (txt_SoundFile4->text().isEmpty()) {
    checkBoxSound_4->setChecked(false);
    checkBoxSound_4->setEnabled(false);
  } else
    checkBoxSound_4->setEnabled(true);
}
void form_settingsgui::clicked_openFile5() {
  txt_SoundFile5->setText(
      QFileDialog::getOpenFileName(this, tr("Open File"), ".", "wav (*.wav)"));
  if (txt_SoundFile5->text().isEmpty()) {
    checkBoxSound_5->setChecked(false);
    checkBoxSound_5->setEnabled(false);
  } else
    checkBoxSound_5->setEnabled(true);
}
void form_settingsgui::clicked_openFile6() {
  txt_SoundFile6->setText(
      QFileDialog::getOpenFileName(this, tr("Open File"), ".", "wav (*.wav)"));
  if (txt_SoundFile6->text().isEmpty()) {
    checkBoxSound_6->setChecked(false);
    checkBoxSound_6->setEnabled(false);
  } else
    checkBoxSound_6->setEnabled(true);
}

void form_settingsgui::clicked_DestinationGenerate() {

  QSettings *settings =
      new QSettings(mConfigPath + "/application.ini", QSettings::IniFormat);
  settings->beginGroup("Network");
  settings->setValue("SamPrivKey", "");
  settings->endGroup();
  settings->sync();
  delete settings;

  QMessageBox *msgBox = new QMessageBox(NULL);
  msgBox->setIcon(QMessageBox::Information);
  msgBox->setText(tr("Please restart I2PChat"));
  msgBox->setStandardButtons(QMessageBox::Ok);
  msgBox->setDefaultButton(QMessageBox::Ok);
  msgBox->setWindowModality(Qt::NonModal);
  msgBox->exec();
}

void form_settingsgui::clicked_IncomingFileFolder() {
  txt_IncomingFileFolder->setText(QFileDialog::getExistingDirectory(
      this, tr("Open Folder"), mConfigPath + "/Incoming",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));

  if (txt_IncomingFileFolder->text().isEmpty()) {
    checkBox_AutoAcceptFiles->setChecked(false);
  }
}

void form_settingsgui::clicked_Gender_Male(bool state) {
  if (state == true) {
    checkGender_Female->setChecked(false);
  }
}

void form_settingsgui::clicked_Gender_Female(bool state) {
  if (state == true) {
    checkGender_Male->setChecked(false);
  }
}

void form_settingsgui::clicked_ChatMessageTextColor() {
  txtShowCurrentChatStyle->selectAll();
  txtShowCurrentChatStyle->setTextColor(
      QColorDialog::getColor(Qt::black, this));

  txtShowCurrentChatStyle->moveCursor(QTextCursor::End,
                                      QTextCursor::MoveAnchor);
  txtShowCurrentChatStyle->textCursor().clearSelection();
}

void form_settingsgui::clicked_ChatMessageBold(bool t) {
  QFont font = txtShowCurrentChatStyle->currentFont();
  font.setBold(t);
  txtShowCurrentChatStyle->selectAll();
  txtShowCurrentChatStyle->setCurrentFont(font);
  txtShowCurrentChatStyle->moveCursor(QTextCursor::End,
                                      QTextCursor::MoveAnchor);
  txtShowCurrentChatStyle->textCursor().clearSelection();
}

void form_settingsgui::clicked_ChatMessageFont() {
  bool ok;
  QFont newFont =
      QFontDialog::getFont(&ok, txtShowCurrentChatStyle->currentFont(), this);
  if (ok == true) {
    txtShowCurrentChatStyle->selectAll();
    txtShowCurrentChatStyle->setCurrentFont(newFont);
    txtShowCurrentChatStyle->moveCursor(QTextCursor::End,
                                        QTextCursor::MoveAnchor);
    txtShowCurrentChatStyle->textCursor().clearSelection();
  }
}

void form_settingsgui::clicked_OverWriteChatMessageTextColor() {
  txtOverrideRemote->selectAll();
  txtOverrideRemote->setTextColor(QColorDialog::getColor(Qt::black, this));
  txtOverrideRemote->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
  txtOverrideRemote->textCursor().clearSelection();
}

void form_settingsgui::clicked_OverWriteChatMessageBold(bool t) {
  QFont font = txtOverrideRemote->currentFont();
  font.setBold(t);
  txtOverrideRemote->selectAll();
  txtOverrideRemote->setCurrentFont(font);
  txtOverrideRemote->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
  txtOverrideRemote->textCursor().clearSelection();
}

void form_settingsgui::clicked_OverWriteChatMessageFont() {
  bool ok;
  QFont newFont =
      QFontDialog::getFont(&ok, txtShowCurrentChatStyle->currentFont(), this);
  if (ok == true) {
    txtOverrideRemote->selectAll();
    txtOverrideRemote->setCurrentFont(newFont);
    txtOverrideRemote->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    txtOverrideRemote->textCursor().clearSelection();
  }
}

void form_settingsgui::clicked_ChatMessageItalic(bool t) {
  txtShowCurrentChatStyle->selectAll();
  txtShowCurrentChatStyle->setFontItalic(t);
  txtShowCurrentChatStyle->moveCursor(QTextCursor::End,
                                      QTextCursor::MoveAnchor);
  txtShowCurrentChatStyle->textCursor().clearSelection();
}

void form_settingsgui::clicked_ChatMessageUnderline(bool t) {
  txtShowCurrentChatStyle->selectAll();
  txtShowCurrentChatStyle->setFontUnderline(t);
  txtShowCurrentChatStyle->moveCursor(QTextCursor::End,
                                      QTextCursor::MoveAnchor);
  txtShowCurrentChatStyle->textCursor().clearSelection();
}

void form_settingsgui::clicked_OverWriteChatMessageItalic(bool t) {
  txtOverrideRemote->selectAll();
  txtOverrideRemote->setFontItalic(t);
  txtOverrideRemote->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
  txtOverrideRemote->textCursor().clearSelection();
}

void form_settingsgui::clicked_OverWriteChatMessageUnderline(bool t) {
  txtOverrideRemote->selectAll();
  txtOverrideRemote->setFontUnderline(t);
  txtOverrideRemote->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
  txtOverrideRemote->textCursor().clearSelection();
}

void form_settingsgui::showUserBlockList() {
  QTreeWidget *UserBlockTreeWidget = this->UserBlockTreeWidget;

  QMap<QString, CUserBlockManager::CUserBlockEntity *> UserBlockMap;
  UserBlockMap = mCore.getUserBlockManager()->getBlockList();
  QMapIterator<QString, CUserBlockManager::CUserBlockEntity *> i(UserBlockMap);

  UserBlockTreeWidget->setColumnCount(2);
  UserBlockTreeWidget->setHeaderLabels(QStringList() << tr("User") << tr(""));

  UserBlockTreeWidget->clear();

  while (i.hasNext()) {
    i.next();
    CUserBlockManager::CUserBlockEntity *currentEntity = i.value();

    QTreeWidgetItem *itemTopLevel = new QTreeWidgetItem();
    QTreeWidgetItem *itemNickname = new QTreeWidgetItem();
    QTreeWidgetItem *itemBlockDate = new QTreeWidgetItem();
    QTreeWidgetItem *itemDestination = new QTreeWidgetItem();

    itemTopLevel->setText(0, currentEntity->mNickName);

    itemNickname->setText(0, tr("Nickname"));
    itemNickname->setText(1, currentEntity->mNickName);

    itemBlockDate->setText(0, tr("Blocked on"));
    itemBlockDate->setText(1, currentEntity->mBlockDate);

    itemDestination->setText(0, tr("Destination"));
    itemDestination->setText(1, currentEntity->mDestination);

    itemTopLevel->addChild(itemNickname);
    itemTopLevel->addChild(itemBlockDate);
    itemTopLevel->addChild(itemDestination);

    UserBlockTreeWidget->addTopLevelItem(itemTopLevel);
  }
  UserBlockTreeWidget->sortByColumn(0, Qt::AscendingOrder);
}

void form_settingsgui::clicked_BlockListDelete() {
  QTreeWidget *UserBlockTreeWidget = this->UserBlockTreeWidget;
  QTreeWidgetItem *item;
  QTreeWidgetItem *parent;
  QString Destination;

  item = UserBlockTreeWidget->currentItem();

  if (item != NULL) {
    parent = item->parent();
    if (parent == NULL) {
      parent = item;
    }

    Destination = parent->child(1)->text(1);
    mCore.getUserBlockManager()->removeBlockEntity(Destination, false);
    showUserBlockList();
  }
}

void form_settingsgui::clicked_BlockListUnblock() {
  QTreeWidget *UserBlockTreeWidget = this->UserBlockTreeWidget;
  QTreeWidgetItem *item;
  QTreeWidgetItem *parent;
  QString Nickname;
  QString Destination;

  item = UserBlockTreeWidget->currentItem();
  if (item != NULL) {
    parent = item->parent();
    if (parent == NULL) {
      parent = item;
    }

    Nickname = parent->text(0);
    Destination = parent->child(1)->text(1);

    mCore.getUserBlockManager()->removeBlockEntity(Destination, true);
    showUserBlockList();
  }
}

void form_settingsgui::clicked_EnableUserSearch(bool t) {
  if (t == true) {
    if (txt_Nickname->text().isEmpty() == true) {
      QMessageBox *msgBox = new QMessageBox(NULL);
      msgBox->setIcon(QMessageBox::Information);
      msgBox->setText(tr("I2PChat"));
      msgBox->setInformativeText(tr("Sorry you have to enter a Nickname(at "
                                    "User-details) for UserSearch"));
      msgBox->setStandardButtons(QMessageBox::Ok);
      msgBox->setDefaultButton(QMessageBox::Ok);
      msgBox->setWindowModality(Qt::NonModal);
      msgBox->exec();
      check_UserSearchEnable->setChecked(false);
      return;
    }
  }
}

void form_settingsgui::clicked_SelectAvatarImage() {
  QPixmap tmpPixmap;

  QString tmp = QFileDialog::getOpenFileName(
      this, tr("Open File"), ".", tr("Images (*.png *.svg *.jpg *.gif)"));
  if (tmp.isEmpty() == false && QFile::exists(tmp) == true) {
    tmpPixmap.load(tmp);
    tmpPixmap = tmpPixmap.scaled(90, 90, Qt::KeepAspectRatio);
    avatarImageByteArray.clear();

    QBuffer buffer(&avatarImageByteArray);
    buffer.open(QIODevice::WriteOnly);
    tmpPixmap.save(&buffer, "PNG");
    ownavatar_label->setAlignment(Qt::AlignCenter);
    ownavatar_label->setPixmap(tmpPixmap);
    ownavatar_label_2->setAlignment(Qt::AlignCenter);
    ownavatar_label_2->setPixmap(tmpPixmap);
  }
}
void form_settingsgui::clicked_ClearAvatarImage() {
  avatarImageByteArray.clear();
  ownavatar_label->clear();
  ownavatar_label_2->clear();
}

void form_settingsgui::setCustomStyleSheet(void) {

  auto fileName =
      QFileDialog::getOpenFileName(this, tr("open your StyleSheet"), "",
                                   tr("StyleSheet(*.css *.qss *.txt)"));
  loadStyleSheet(fileName);
  settings->beginGroup("Style");
  settings->setValue("CustomStyleSheet", fileName);
  settings->endGroup();
  settings->sync();
}

void form_settingsgui::clicked_sortingEnabled(bool enabled) {
  settings->beginGroup("UserList");
  settings->setValue("SortingEnabled", enabled);
  settings->endGroup();
  settings->sync();
}

void form_settingsgui::clicked_BlockAllUnknownUsers(bool checked) {
  settings->beginGroup("Security");
  settings->setValue("BlockAllUnknownUsers", checked);
  settings->endGroup();
  settings->sync();
}

void form_settingsgui::clicked_RequestAuthorization(bool checked) {
  settings->beginGroup("Security");
  settings->setValue("RequestAuthorization", checked);
  settings->endGroup();
  settings->sync();
}

void form_settingsgui::clicked_sortAlphabetically(bool checked) {
  if (checked) {
    settings->beginGroup("UserList");
    settings->setValue("SortType", 0);
    settings->endGroup();
    settings->sync();
    
    mCore.getUserManager()->sortUserList(0);
  }
}

void form_settingsgui::clicked_sortByDateAdded(bool checked) {
  if (checked) {
    settings->beginGroup("UserList");
    settings->setValue("SortType", 1);
    settings->endGroup();
    settings->sync();
    
    mCore.getUserManager()->sortUserList(1);
  }
}

void form_settingsgui::clicked_sortByLastCommunication(bool checked) {
  if (checked) {
    settings->beginGroup("UserList");
    settings->setValue("SortType", 2);
    settings->endGroup();
    settings->sync();
    
    mCore.getUserManager()->sortUserList(2);
  }
}

void form_settingsgui::clicked_sortByLastOnline(bool checked) {
  if (checked) {
    settings->beginGroup("UserList");
    settings->setValue("SortType", 3);
    settings->endGroup();
    settings->sync();
    
    mCore.getUserManager()->sortUserList(3);
  }
}
