/***************************************************************************
 *   Copyright (C) 2008 by I2P-Messenger   				   *
 *   Messenger-Dev@I2P-Messenger   					   *
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

#include "Core.h"
#include "UserBlockManager.h"
#include "form_settingsgui.h"

form_settingsgui::form_settingsgui(CCore& Core,QWidget *parent, Qt::WindowFlags flags)
	: QDialog(parent, flags),mCore(Core),mConfigPath(Core.getConfigPath())
{
    	setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose,true);
	
	settings= new QSettings(mConfigPath+"/application.ini",QSettings::IniFormat);

    	loadqss();
	styleCombo->addItems(QStyleFactory::keys());
	loadSettings();
	showUserBlockList();

	connect(ok_Button,SIGNAL(clicked(bool)),
		this,SLOT(saveSettings()));

	connect(cancel_Button,SIGNAL(clicked(bool)),
		this,SLOT(close()));

	connect(cmd_openFile,SIGNAL(clicked(bool)),
		this,SLOT(clicked_openFile()));

	connect(cmd_openFile_2,SIGNAL(clicked(bool)),
		this,SLOT(clicked_openFile2()));

	connect(cmd_openFile_3,SIGNAL(clicked(bool)),
		this,SLOT(clicked_openFile3()));

	connect(cmd_openFile_4,SIGNAL(clicked(bool)),
		this,SLOT(clicked_openFile4()));

	connect(cmd_openFile_5,SIGNAL(clicked(bool)),
		this,SLOT(clicked_openFile5()));

	connect(cmd_openFile_6,SIGNAL(clicked(bool)),
		this,SLOT(clicked_openFile6()));

	connect(cmd_DestinationGenerate,SIGNAL(clicked(bool)),
		this,SLOT(clicked_DestinationGenerate()));

	connect(cmd_IncomingFileFolder, SIGNAL(clicked(bool) ),
		this,SLOT(clicked_IncomingFileFolder()));

	connect(checkGender_Male,SIGNAL(clicked(bool)),
		this,SLOT(clicked_Gender_Male(bool)));

	connect(checkGender_Female,SIGNAL(clicked(bool)),
		this,SLOT(clicked_Gender_Female(bool)));

	connect(cmdBoldChat,SIGNAL(clicked(bool)),
		this,SLOT(clicked_ChatMessageBold(bool)));

	connect(cmdUnderChat,SIGNAL(clicked(bool)),
		this,SLOT(clicked_ChatMessageUnderline(bool)));

	connect(cmdItalicChat,SIGNAL(clicked(bool)),
		this,SLOT(clicked_ChatMessageItalic(bool)));
	
	connect(cmdColorChat,SIGNAL(clicked(bool)),
		this,SLOT(clicked_ChatMessageTextColor()));

	connect(cmdFontChat,SIGNAL(clicked(bool)),
		this,SLOT(clicked_ChatMessageFont()));

	connect(cmdOverwriteBoldChat,SIGNAL(clicked(bool)),
		this,SLOT(clicked_OverWriteChatMessageBold(bool)));

	connect(cmdOverwriteUnderChat,SIGNAL(clicked(bool)),
		this,SLOT(clicked_OverWriteChatMessageUnderline(bool)));

	connect(cmdOverwriteItalicChat,SIGNAL(clicked(bool)),
		this,SLOT(clicked_OverWriteChatMessageItalic(bool)));

	connect(cmdOverwriteColorChat,SIGNAL(clicked(bool)),
		this,SLOT(clicked_OverWriteChatMessageTextColor()));

	connect(cmdOverwriteFontChat,SIGNAL(clicked(bool)),
		this,SLOT(clicked_OverWriteChatMessageFont()));
		
	connect(cmd_deleteUserFromBlockList,SIGNAL(clicked()),
		this,SLOT(clicked_BlockListDelete()));
	
	connect(cmd_unblockUserFromBlockList,SIGNAL(clicked()),
		this,SLOT(clicked_BlockListUnblock()));
		
	connect(check_UserSearchEnable,SIGNAL(clicked(bool)),
		this,SLOT(clicked_EnableUserSearch(bool)));
	
	connect(checkBox_AutoAcceptFileRecive,SIGNAL(clicked(bool)),
		checkBox_IncomingSubFolders,SLOT(setChecked(bool)));
		
	connect(checkBox_AutoAcceptFileRecive,SIGNAL(toggled(bool)),
		checkBox_IncomingSubFolders,SLOT(setEnabled(bool)));

	connect(cmd_selectAvatarImage,SIGNAL(clicked()),
		this,SLOT(clicked_SelectAvatarImage()));
		
	connect(cmd_clearAvatarImage,SIGNAL(clicked()),
		this,SLOT(clicked_ClearAvatarImage()));
}

form_settingsgui::~form_settingsgui()
{
	settings->sync();
	delete (settings);
}


void form_settingsgui::loadSettings()
{
	settings->beginGroup("General");
		spinBox->setValue(settings->value("Debug_Max_Message_count","20").toInt());
        waitTimeBetweenCheckingForOfflineUsersSecondsSpinBox->setValue(settings->value("Waittime_between_rechecking_offline_users","1000").toInt()/1000);
		checkBox_DebugLog->setChecked(settings->value(("DebugLogging"),"true").toBool());

		if(settings->value("current_Style","").toString().isEmpty()==false)
			styleCombo->setCurrentIndex(styleCombo->findText(settings->value("current_Style","").toString()));
		else
		{
			//find default Style for this System
			QRegExp regExp("Q(.*)Style");
			QString defaultStyle = QApplication::style()->metaObject()->className();
			
			if (defaultStyle == QLatin1String("QMacStyle"))
				defaultStyle = QLatin1String("Macintosh (Aqua)");
			else if (regExp.exactMatch(defaultStyle))
				defaultStyle = regExp.cap(1);
			
			styleCombo->setCurrentIndex(styleCombo->findText(defaultStyle));

		}

		checkBox_AutoAcceptFileRecive->setChecked(settings->value("AutoAcceptFileRecive",false).toBool());
		if(checkBox_AutoAcceptFileRecive->isChecked()==true){
			cmd_IncomingFileFolder->setEnabled(true);
			checkBox_IncomingSubFolders->setChecked(settings->value("UseIncomingSubFolderForEveryUser",false).toBool());
		}

		txt_IncomingFileFolder->setText(settings->value("IncomingFileFolder",mConfigPath+"/Incoming").toString());


		styleSheetCombo->setCurrentIndex(styleSheetCombo->findText(settings->value("current_Style_sheet","Default").toString()));
	settings->endGroup();

	settings->beginGroup("Network");
		lineEdit_3->setText(settings->value("SamHost","127.0.0.1").toString());
		lineEdit->setText(settings->value("TunnelName","I2PChat").toString());
		spinBox_10->setValue(settings->value("SamPort","7656").toInt());
		
		spinBox_4->setMinimum(1);
		spinBox_4->setValue(settings->value("inbound.length","3").toInt());
		spinBox_4->setMaximum(3);
	
		spinBox_5->setMinimum(0);
		spinBox_5->setValue(settings->value("inbound.quantity","1").toInt());
		spinBox_5->setMaximum(3);
		
		spinBox_6->setMinimum(0);
		spinBox_6->setValue(settings->value("inbound.backupQuantity","1").toInt());
		spinBox_6->setMaximum(3);
	
		spinBox_7->setMinimum(0);
		spinBox_7->setValue(settings->value("outbound.backupQuantity","1").toInt());
		spinBox_7->setMaximum(3);
	
		spinBox_8->setMinimum(1);
		spinBox_8->setValue(settings->value("outbound.length","3").toInt());
		spinBox_8->setMaximum(3);
		
		spinBox_9->setMinimum(0);
		spinBox_9->setValue(settings->value("outbound.quantity","1").toInt());
		spinBox_9->setMaximum(3);
		
		comboBox_Signature->setEditable(false);
		comboBox_Signature->setCurrentIndex(comboBox_Signature->findText(settings->value("SIGNATURE_TYPE","DSA_SHA1").toString()));
	settings->endGroup();


	settings->beginGroup("Sound");
		settings->beginGroup("SoundFilePath");
			txt_SoundFile->setText(settings->value("User_go_Online","./sounds/Startup.wav").toString());
			txt_SoundFile2->setText(settings->value("User_go_Offline","").toString());
			txt_SoundFile3->setText(settings->value("FileSend_Finished","").toString());
			txt_SoundFile4->setText(settings->value("FileRecive_Incoming","./sounds/File.wav").toString());
			txt_SoundFile5->setText(settings->value("FileRecive_Finished","").toString());
			txt_SoundFile6->setText(settings->value("NewChatMessage","./sounds/Notify.wav").toString());

			if(!txt_SoundFile->text().isEmpty())checkBoxSound->setEnabled(true);
			if(!txt_SoundFile2->text().isEmpty())checkBoxSound_2->setEnabled(true);
			if(!txt_SoundFile3->text().isEmpty())checkBoxSound_3->setEnabled(true);
			if(!txt_SoundFile4->text().isEmpty())checkBoxSound_4->setEnabled(true);
			if(!txt_SoundFile5->text().isEmpty())checkBoxSound_5->setEnabled(true);
			if(!txt_SoundFile6->text().isEmpty())checkBoxSound_6->setEnabled(true);

		settings->endGroup();

		settings->beginGroup("Enable");
			checkBoxSound->setChecked(settings->value("User_go_Online",true).toBool());
			checkBoxSound_2->setChecked(settings->value("User_go_Offline",false).toBool());
			checkBoxSound_3->setChecked(settings->value("FileSend_Finished",false).toBool());
			checkBoxSound_4->setChecked(settings->value("FileRecive_Incoming",true).toBool());
			checkBoxSound_5->setChecked(settings->value("FileRecive_Finished",false).toBool());
			checkBoxSound_6->setChecked(settings->value("NewChatMessage",true).toBool());
		settings->endGroup();	
		
	settings->endGroup();


	settings->beginGroup("User-Infos");
		txt_Nickname->setText(settings->value("Nickname","").toString());
		spinAge->setValue(settings->value("Age","0").toInt());
		
		if(settings->value("Gender","").toString()=="Male"){
			checkGender_Male->setChecked(true);
			checkGender_Female->setChecked(false);
		}
		else if(settings->value("Gender","").toString()=="Female"){
			checkGender_Male->setChecked(false);
			checkGender_Female->setChecked(true);
		}
		else{
			checkGender_Male->setChecked(false);
			checkGender_Female->setChecked(false);
		}

		txt_Interests->setPlainText (settings->value("Interests","").toString());
		
		QPixmap tmpPixmap;
		avatarImageByteArray=settings->value("AvatarBinaryImage","").toByteArray();
		tmpPixmap.loadFromData(avatarImageByteArray);
		ownavatar_label->setAlignment(Qt::AlignCenter);
		ownavatar_label->setPixmap(tmpPixmap);
		
	settings->endGroup();

	settings->beginGroup("Chat");
		txtShowCurrentChatStyle->setText("Local settings preview");
		txtShowCurrentChatStyleOverride->setText("Remote override preview");

		txtShowCurrentChatStyle->selectAll();
			QFont  font;				
			QColor color;

			font.fromString(settings->value("DefaultFont" ,"SansSerif,10").toString());			
			color.setNamedColor(settings->value("DefaultColor","#000000").toString());

			txtShowCurrentChatStyle->setFont(font);
			txtShowCurrentChatStyle->setTextColor(color);
		txtShowCurrentChatStyle->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
		txtShowCurrentChatStyle->textCursor().clearSelection();

		//override remote chatmessage settings -> font/color
		chatOverrideBox->setChecked(settings->value("Override",false).toBool());
		if(chatOverrideBox->isChecked()==false){
			txtShowCurrentChatStyleOverride->selectAll();
				QFont  font;
				QColor color;

				font.fromString(settings->value("FontForOverwrite" ,"SansSerif,10").toString());
				color.setNamedColor(settings->value("ColorForOverwrite","#000000").toString());

				txtShowCurrentChatStyleOverride->setFont(font);
				txtShowCurrentChatStyleOverride->setTextColor(color);
			
			txtShowCurrentChatStyleOverride->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
			txtShowCurrentChatStyleOverride->textCursor().clearSelection();
		}
		else{
			txtShowCurrentChatStyleOverride->selectAll();
				QFont  font;
				font.fromString(settings->value("FontForOverwrite","SansSerif,10").toString());
				QColor color(settings->value("ColorForOverwrite","#000000").toString());

				txtShowCurrentChatStyleOverride->setFont(font);
				txtShowCurrentChatStyleOverride->setTextColor(color);
			txtShowCurrentChatStyleOverride->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);	
			txtShowCurrentChatStyleOverride->textCursor().clearSelection();		
		}
		
		LogOnlineStatusBox->setChecked(settings->value("LogOnlineStatesOfUsers",true).toBool());
		
// 		spinBox_maxACK->setMaximum(2);
// 		spinBox_maxACK->setMaximum(60);
// 		spinBox_maxACK->setValue(settings->value("MaxChatmessageACKTimeInSec","10").toInt());
		
		

	settings->endGroup();
	
	settings->beginGroup("Security");
		 if(settings->value("BlockStyle","Normal").toString()=="Normal"){
			check_BlockNormal->setChecked(true);
		 }
		 else{
			
			check_BlockInvisible->setChecked(true);
		 }
	settings->endGroup();
	
	settings->beginGroup("Usersearch");
		if(txt_Nickname->text().isEmpty()==false && (settings->value("Enabled",true).toBool())==true){
		  check_UserSearchEnable->setChecked(true);
		}else{
		  check_UserSearchEnable->setChecked(false);
		}
		spinBox_MaxLogMessagesUserSearch->setMinimum(0);
		spinBox_MaxLogMessagesUserSearch->setMaximum(200);
		spinBox_MaxLogMessagesUserSearch->setValue(settings->value("Debug_Max_Message_count",20).toInt());
		spinBox_ReAnnouncingUserSearch->setMinimum(0);
		spinBox_ReAnnouncingUserSearch->setMaximum(23);
		spinBox_ReAnnouncingUserSearch->setValue(settings->value("ReAnnounceTimerInHours",1).toInt());
	settings->endGroup();
	settings->sync();
}
void form_settingsgui::saveSettings()
{	
	settings->beginGroup("General");
		settings->setValue("Debug_Max_Message_count",spinBox->value());
        settings->setValue("Waittime_between_rechecking_offline_users",
                           waitTimeBetweenCheckingForOfflineUsersSecondsSpinBox->value()*1000);
		settings->setValue("current_Style",styleCombo->currentText());
		settings->setValue("current_Style_sheet",styleSheetCombo->currentText());
		settings->setValue("AutoAcceptFileRecive",checkBox_AutoAcceptFileRecive->isChecked());
		settings->setValue("IncomingFileFolder",txt_IncomingFileFolder->text());
		settings->setValue("UseIncomingSubFolderForEveryUser",checkBox_IncomingSubFolders->isChecked());
		settings->setValue("DebugLogging",checkBox_DebugLog->isChecked());
	settings->endGroup();

	settings->beginGroup("Network");
		settings->setValue("SamHost",lineEdit_3->text());
		settings->setValue("TunnelName",lineEdit->text());
		settings->setValue("SamPort",spinBox_10->value());
		//Inbound options
		settings->setValue("inbound.quantity",spinBox_5->value());
		settings->setValue("inbound.backupQuantity",spinBox_6->value());
		settings->setValue("inbound.length",spinBox_4->value());
		//Outpound options
		settings->setValue("outbound.quantity",spinBox_9->value());
		settings->setValue("outbound.backupQuantity",spinBox_7->value());
		settings->setValue("outbound.length",spinBox_8->value());
		
		//Signature_type
		settings->setValue("SIGNATURE_TYPE",comboBox_Signature->currentText());
	settings->endGroup();
	
	settings->beginGroup("Sound");
		settings->beginGroup("Enable");
			settings->setValue("User_go_Online",checkBoxSound->isChecked());
			settings->setValue("User_go_Offline",checkBoxSound_2->isChecked());
			settings->setValue("FileSend_Finished",checkBoxSound_3->isChecked());
			settings->setValue("FileRecive_Incoming",checkBoxSound_4->isChecked());
			settings->setValue("FileRecive_Finished",checkBoxSound_5->isChecked());
			settings->setValue("NewChatMessage",checkBoxSound_6->isChecked());
			settings->endGroup();
		settings->beginGroup("SoundFilePath");
			settings->setValue("User_go_Online",txt_SoundFile->text());
			settings->setValue("User_go_Offline",txt_SoundFile2->text());
			settings->setValue("FileSend_Finished",txt_SoundFile3->text());
			settings->setValue("FileRecive_Incoming",txt_SoundFile4->text());
			settings->setValue("FileRecive_Finished",txt_SoundFile5->text());
			settings->setValue("NewChatMessage",txt_SoundFile6->text());
		settings->endGroup();
	settings->endGroup();
	
	settings->beginGroup("Style"); // application.ini
		//settings->setValue("CurrentStyle", "Fusion");
		settings->setValue("CustomStyleSheet", "");
	settings->endGroup();

	settings->beginGroup("User-Infos");
		settings->setValue("Nickname",txt_Nickname->text());
		settings->setValue("Age",spinAge->value());
		if(checkGender_Male->isChecked()==true){
			settings->setValue("Gender","Male");
		}
		else if(checkGender_Female->isChecked()==true){
			settings->setValue("Gender","Female");
		}
		else{
			settings->setValue("Gender","");
		}
		settings->setValue("Interests",txt_Interests->toPlainText());
		settings->setValue("AvatarBinaryImage",avatarImageByteArray);
	settings->endGroup();

	settings->beginGroup("Chat");
		settings->setValue("DefaultFont" ,txtShowCurrentChatStyle->currentFont().toString());
		settings->setValue("DefaultColor",txtShowCurrentChatStyle->textColor().name());
		
		//override remoute chatmessageSettings Font/Color
		settings->setValue("Override",chatOverrideBox->isChecked());
		settings->setValue("FontForOverwrite" ,txtShowCurrentChatStyleOverride->currentFont().toString());
		settings->setValue("ColorForOverwrite",txtShowCurrentChatStyleOverride->textColor().name());
		settings->setValue("LogOnlineStatesOfUsers",LogOnlineStatusBox->isChecked());
// 		settings->setValue("MaxChatmessageACKTimeInSec",spinBox_maxACK->value());
	settings->endGroup();
	
	settings->beginGroup("Security");
		if(check_BlockInvisible->isChecked()==true){
		      settings->setValue("BlockStyle","Invisible");
		}
		else{
		       settings->setValue("BlockStyle","Normal");
		}
	settings->endGroup();
	
	settings->beginGroup("Usersearch");
		settings->setValue("Enabled", check_UserSearchEnable->isChecked());
		settings->setValue("Debug_Max_Message_count",spinBox_MaxLogMessagesUserSearch->value());
		settings->setValue("ReAnnounceTimerInHours",spinBox_ReAnnouncingUserSearch->value());
	settings->endGroup();
	settings->sync();
	mCore.loadUserInfos();
	mCore.getUserManager()->avatarImageChanged();
	this->close();
}


void form_settingsgui::on_styleCombo_activated(const QString &styleName)
{
	qApp->setStyle(styleName);

}

void form_settingsgui::on_styleSheetCombo_activated(const QString &sheetName)
{
	loadStyleSheet(sheetName);
}

void form_settingsgui::loadStyleSheet(const QString &sheetName)
{
	// 
	// external Stylesheets
	QFile file(mConfigPath + "/qss/" + sheetName.toLower() + ".qss");
	
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
	
	
	qApp->setStyleSheet(styleSheet); 
}

void form_settingsgui::loadqss()
{

	QFileInfoList slist = QDir(mConfigPath + "/qss/").entryInfoList();
	foreach(QFileInfo st, slist)
	{
	if(st.fileName() != "." && st.fileName() != ".." && st.isFile())
	styleSheetCombo->addItem(st.fileName().remove(".qss"));
	}
}

void form_settingsgui::clicked_openFile()
{
	txt_SoundFile->setText(QFileDialog::getOpenFileName(this,tr("Open File"), ".", "wav (*.wav)"));
	if(txt_SoundFile->text().isEmpty()){
		checkBoxSound->setChecked(false);
		checkBoxSound->setEnabled(false);
	}
	else
		checkBoxSound->setEnabled(true);
}

void form_settingsgui::clicked_openFile2()
{
	txt_SoundFile2->setText(QFileDialog::getOpenFileName(this,tr("Open File"), ".", "wav (*.wav)"));
	if(txt_SoundFile2->text().isEmpty()){	
		checkBoxSound_2->setChecked(false);
		checkBoxSound_2->setEnabled(false);
	}	
	else
		checkBoxSound_2->setEnabled(true);

}
void form_settingsgui::clicked_openFile3()
{
	txt_SoundFile3->setText(QFileDialog::getOpenFileName(this,tr("Open File"), ".", "wav (*.wav)"));
	if(txt_SoundFile3->text().isEmpty()){
		checkBoxSound_3->setChecked(false);
		checkBoxSound_3->setEnabled(false);
	}
	else
		checkBoxSound_3->setEnabled(true);
}
void form_settingsgui::clicked_openFile4()
{
	txt_SoundFile4->setText(QFileDialog::getOpenFileName(this,tr("Open File"), ".", "wav (*.wav)"));
	if(txt_SoundFile4->text().isEmpty()){
		checkBoxSound_4->setChecked(false);
		checkBoxSound_4->setEnabled(false);
	}
	else
		checkBoxSound_4->setEnabled(true);
}
void form_settingsgui::clicked_openFile5()
{
	txt_SoundFile5->setText(QFileDialog::getOpenFileName(this,tr("Open File"), ".", "wav (*.wav)"));
	if(txt_SoundFile5->text().isEmpty()){
		checkBoxSound_5->setChecked(false);
		checkBoxSound_5->setEnabled(false);
	}
	else
		checkBoxSound_5->setEnabled(true);
}
void form_settingsgui::clicked_openFile6()
{
	txt_SoundFile6->setText(QFileDialog::getOpenFileName(this,tr("Open File"), ".", "wav (*.wav)"));
	if(txt_SoundFile6->text().isEmpty()){
		checkBoxSound_6->setChecked(false);
		checkBoxSound_6->setEnabled(false);
	}
	else
		checkBoxSound_6->setEnabled(true);
	
}

void form_settingsgui::clicked_DestinationGenerate()
{

	QSettings* settings= new QSettings(mConfigPath+"/application.ini",QSettings::IniFormat);
	settings->beginGroup("Network");
		settings->setValue("SamPrivKey","");
	settings->endGroup();
	settings->sync();
	delete settings;

	QMessageBox* msgBox= new QMessageBox(NULL);
	msgBox->setIcon(QMessageBox::Information);
	msgBox->setText(tr("Please restart I2PChat"));
	msgBox->setStandardButtons(QMessageBox::Ok);
	msgBox->setDefaultButton(QMessageBox::Ok);
	msgBox->setWindowModality(Qt::NonModal);
	msgBox->exec();
	
}

void form_settingsgui::clicked_IncomingFileFolder()
{
	txt_IncomingFileFolder->setText(QFileDialog::getExistingDirectory(this,
		tr("Open Folder"),mConfigPath+"/Incoming",
		QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks));

	if(txt_IncomingFileFolder->text().isEmpty()){
		checkBox_AutoAcceptFileRecive->setChecked(false);
	}
}

void form_settingsgui::clicked_Gender_Male(bool state)
{
	if(state==true){
		checkGender_Female->setChecked(false);
	}
	
	
}

void form_settingsgui::clicked_Gender_Female(bool state)
{
	if(state==true){
		checkGender_Male->setChecked(false);
	}
}

void form_settingsgui::clicked_ChatMessageTextColor()
{
	txtShowCurrentChatStyle->selectAll();
		txtShowCurrentChatStyle->setTextColor(QColorDialog::getColor(Qt::black, this));

	txtShowCurrentChatStyle->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
	txtShowCurrentChatStyle->textCursor().clearSelection();
}

void form_settingsgui::clicked_ChatMessageBold(bool t)
{
	QFont font = txtShowCurrentChatStyle->currentFont();
	font.setBold(t);
	txtShowCurrentChatStyle->selectAll();
		txtShowCurrentChatStyle->setCurrentFont(font);
	txtShowCurrentChatStyle->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
	txtShowCurrentChatStyle->textCursor().clearSelection();
}

void form_settingsgui::clicked_ChatMessageFont()
{
	bool ok;
	QFont newFont=QFontDialog::getFont(&ok, txtShowCurrentChatStyle->currentFont(), this);
	if(ok==true){
		txtShowCurrentChatStyle->selectAll();
			txtShowCurrentChatStyle->setCurrentFont(newFont);
		txtShowCurrentChatStyle->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
		txtShowCurrentChatStyle->textCursor().clearSelection();
	}
}

void form_settingsgui::clicked_OverWriteChatMessageTextColor()
{
	txtShowCurrentChatStyleOverride->selectAll();
		txtShowCurrentChatStyleOverride->setTextColor(QColorDialog::getColor(Qt::black, this));
	txtShowCurrentChatStyleOverride->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
	txtShowCurrentChatStyleOverride->textCursor().clearSelection();
}

void form_settingsgui::clicked_OverWriteChatMessageBold(bool t)
{
	QFont font = txtShowCurrentChatStyleOverride->currentFont();
	font.setBold(t);
	txtShowCurrentChatStyleOverride->selectAll();
		txtShowCurrentChatStyleOverride->setCurrentFont(font);
	txtShowCurrentChatStyleOverride->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
	txtShowCurrentChatStyleOverride->textCursor().clearSelection();
}

void form_settingsgui::clicked_OverWriteChatMessageFont()
{
	bool ok;
	QFont newFont=QFontDialog::getFont(&ok, txtShowCurrentChatStyle->currentFont(), this);
	if(ok==true){ 
		txtShowCurrentChatStyleOverride->selectAll();
			txtShowCurrentChatStyleOverride->setCurrentFont(newFont);
		txtShowCurrentChatStyleOverride->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
		txtShowCurrentChatStyleOverride->textCursor().clearSelection();
	}
}

void form_settingsgui::clicked_ChatMessageItalic(bool t)
{
	txtShowCurrentChatStyle->selectAll();
		txtShowCurrentChatStyle->setFontItalic(t);
	txtShowCurrentChatStyle->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
	txtShowCurrentChatStyle->textCursor().clearSelection();
}

void form_settingsgui::clicked_ChatMessageUnderline(bool t)
{
	txtShowCurrentChatStyle->selectAll();
		txtShowCurrentChatStyle->setFontUnderline (t);
	txtShowCurrentChatStyle->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
	txtShowCurrentChatStyle->textCursor().clearSelection();
}

void form_settingsgui::clicked_OverWriteChatMessageItalic(bool t)
{
	txtShowCurrentChatStyleOverride->selectAll();
		txtShowCurrentChatStyleOverride->setFontItalic(t);
	txtShowCurrentChatStyleOverride->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
	txtShowCurrentChatStyleOverride->textCursor().clearSelection();
}

void form_settingsgui::clicked_OverWriteChatMessageUnderline(bool t)
{
	txtShowCurrentChatStyleOverride->selectAll();
		txtShowCurrentChatStyleOverride->setFontUnderline(t);
	txtShowCurrentChatStyleOverride->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
	txtShowCurrentChatStyleOverride->textCursor().clearSelection();
}

void form_settingsgui::showUserBlockList()
{
	QTreeWidget* UserBlockTreeWidget= this->UserBlockTreeWidget;
	
	QMap<QString,CUserBlockManager::CUserBlockEntity*> UserBlockMap;
	UserBlockMap=mCore.getUserBlockManager()->getBlockList();
	QMapIterator<QString,CUserBlockManager::CUserBlockEntity*> i(UserBlockMap);

	UserBlockTreeWidget->setColumnCount(2);
	UserBlockTreeWidget->setHeaderLabels(QStringList()<<tr("Nicknames")<<tr("values"));
	
	UserBlockTreeWidget->clear();

	while(i.hasNext()){
	  	i.next();
		CUserBlockManager::CUserBlockEntity* currentEntity=i.value();
		
		QTreeWidgetItem* itemTopLevel= new QTreeWidgetItem();
		QTreeWidgetItem* itemNickname= new QTreeWidgetItem(); 
		QTreeWidgetItem* itemBlockDate= new QTreeWidgetItem();
		QTreeWidgetItem* itemDestination= new QTreeWidgetItem();
		
		itemTopLevel->setText(0,currentEntity->mNickName);
		
		
		itemNickname->setText(0,tr("Nickname"));
		itemNickname->setText(1,currentEntity->mNickName);
		
		
		itemBlockDate->setText(0,tr("BlockDate"));
		itemBlockDate->setText(1,currentEntity->mBlockDate);
		
		
		itemDestination->setText(0,tr("Destination"));
		itemDestination->setText(1,currentEntity->mDestination);
		
		itemTopLevel->addChild(itemNickname);
		itemTopLevel->addChild(itemBlockDate);
		itemTopLevel->addChild(itemDestination);
		
		UserBlockTreeWidget->addTopLevelItem(itemTopLevel);
		
	}
	UserBlockTreeWidget->sortByColumn(0);
	
}

void form_settingsgui::clicked_BlockListDelete()
{
    QTreeWidget* UserBlockTreeWidget= this->UserBlockTreeWidget;
    QTreeWidgetItem *item;
    QTreeWidgetItem *parent;
    QString Destination;
    
    item=UserBlockTreeWidget->currentItem();
    
    if(item!=NULL){
      parent=item->parent();
      if(parent==NULL){
	   parent=item;
      }
      
      Destination=parent->child(1)->text(1); 
      mCore.getUserBlockManager()->removeBlockEntity(Destination,false);
      showUserBlockList();
    }
}

void form_settingsgui::clicked_BlockListUnblock()
{
    QTreeWidget* UserBlockTreeWidget= this->UserBlockTreeWidget;
    QTreeWidgetItem *item;
    QTreeWidgetItem *parent;
    QString Nickname;
    QString Destination;
    
    item=UserBlockTreeWidget->currentItem();
    if(item!=NULL){
      parent=item->parent();
      if(parent==NULL){
	   parent=item;
      }
      

      Nickname=parent->text(0);
      Destination=parent->child(1)->text(1);
      
      mCore.getUserBlockManager()->removeBlockEntity(Destination,true);
      showUserBlockList();
    }
}

void form_settingsgui::clicked_EnableUserSearch(bool t)
{
    if(t==true){ 
	if(txt_Nickname->text().isEmpty()==true){
	    QMessageBox* msgBox= new QMessageBox(NULL);
	    msgBox->setIcon(QMessageBox::Information);
	    msgBox->setText(tr("I2PChat"));
	    msgBox->setInformativeText(tr("Sorry you have to enter a Nickname(at User-details) for UserSearch"));
	    msgBox->setStandardButtons(QMessageBox::Ok);
	    msgBox->setDefaultButton(QMessageBox::Ok);
	    msgBox->setWindowModality(Qt::NonModal);
	    msgBox->exec();
	    check_UserSearchEnable->setChecked(false);
	    return;
	}
    }
}

void form_settingsgui::clicked_SelectAvatarImage()
{
	QPixmap tmpPixmap;
	
	QString tmp=QFileDialog::getOpenFileName(this,tr("Open File"), ".", tr("Images (*.png *.svg *.jpg *.gif)"));
	if(tmp.isEmpty()==false && QFile::exists(tmp)==true){
	  tmpPixmap.load(tmp);
	  tmpPixmap=tmpPixmap.scaled(90,90,Qt::KeepAspectRatio);
	  avatarImageByteArray.clear();
	  
	   QBuffer buffer(&avatarImageByteArray);
	   buffer.open(QIODevice::WriteOnly);
	   tmpPixmap.save(&buffer, "PNG");
	   ownavatar_label->setAlignment(Qt::AlignCenter);
	   ownavatar_label->setPixmap(tmpPixmap);
	}
}
void form_settingsgui::clicked_ClearAvatarImage()
{
      avatarImageByteArray.clear();
      ownavatar_label->clear();
}



