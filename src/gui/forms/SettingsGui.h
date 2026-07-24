/********************************************************************************
** Form generated from reading UI file 'SettingsGui.ui'
**
** Created by: Qt User Interface Compiler version 5.15.19
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef SETTINGSGUI_H
#define SETTINGSGUI_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsGui
{
public:
    QFrame *SettingsContainer;
    QStackedWidget *Settings;
    QWidget *general_page;
    QVBoxLayout *vlay_general_page;
    QGroupBox *FileXfer;
    QGridLayout *grid_FileXfer;
    QCheckBox *checkBox_AutoAcceptFiles;
    QLabel *label_autoDL;
    QLineEdit *txt_IncomingFileFolder;
    QPushButton *cmd_Downloads;
    QCheckBox *checkBox_Subfolders;
    QGroupBox *UserCheck;
    QGridLayout *grid_UserCheck;
    QSpinBox *OfflineChkSpinBox;
    QLabel *label_offlinechk;
    QGroupBox *AutoAway;
    QGridLayout *grid_AutoAway;
    QSpinBox *AutoAwaySpinBox;
    QLabel *NoActivityLabel;
    QGroupBox *SAMLog;
    QGridLayout *grid_SAMLog;
    QLabel *MaxLogLabel;
    QSpinBox *spinBox;
    QGroupBox *Debugging;
    QGridLayout *grid_Debugging;
    QCheckBox *checkBox_DebugLog;
    QWidget *network_Page;
    QVBoxLayout *vlay_network_Page;
    QGroupBox *SAM;
    QGridLayout *grid_SAM;
    QLabel *label_17;
    QLineEdit *lineEdit_3;
    QLabel *label_18;
    QSpinBox *spinBox_10;
    QGroupBox *SigType;
    QGridLayout *grid_SigType;
    QCheckBox *requestAuthcheckBox;
    QComboBox *comboBox_SigType;
    QLabel *label_26;
    QPushButton *cmd_DestGenerate;
    QCheckBox *nonpersistdest;
    QGroupBox *Crypto;
    QGridLayout *grid_Crypto;
    QLabel *Encryption_label;
    QComboBox *comboBox_EncType;
    QGroupBox *TunnelConfig;
    QGridLayout *grid_TunnelConfig;
    QLabel *label_9;
    QLineEdit *lineEdit;
    QLabel *label_14;
    QLabel *label_15;
    QSpinBox *spinBox_8;
    QSpinBox *spinBox_4;
    QLabel *label_10;
    QSpinBox *spinBox_5;
    QSpinBox *spinBox_9;
    QLabel *label_11;
    QSpinBox *spinBox_7;
    QLabel *label_12;
    QSpinBox *spinBox_6;
    QWidget *userdetails_Page;
    QVBoxLayout *vlay_userdetails_Page;
    QGroupBox *UserBio;
    QGridLayout *grid_UserBio;
    QLabel *label_19;
    QLineEdit *txt_Nickname;
    QGroupBox *UserSuppInfo;
    QGridLayout *grid_UserSuppInfo;
    QPlainTextEdit *txt_Interests;
    QGroupBox *SelectAvatar;
    QGridLayout *grid_SelectAvatar;
    QLabel *ownavatar_label;
    QLabel *label;
    QPushButton *cmd_selectAvatarImage;
    QPushButton *cmd_clearAvatarImage;
    QWidget *style_Page;
    QVBoxLayout *vlay_style_Page;
    QGroupBox *WindowStyle;
    QGridLayout *grid_WindowStyle;
    QLabel *label_7;
    QComboBox *styleCombo;
    QGroupBox *CustomStyle;
    QGridLayout *grid_CustomStyle;
    QLabel *label_8;
    QComboBox *styleSheetCombo;
    QGroupBox *ContactSort;
    QGridLayout *grid_ContactSort;
    QCheckBox *checkBox_5;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;
    QRadioButton *radioButton_4;
    QRadioButton *radioButton_5;
    QWidget *webserver_Page;
    QVBoxLayout *vlay_webserver_Page;
    QGroupBox *WebServer;
    QGridLayout *grid_WebServer;
    QCheckBox *WebServerAuthCheckbox;
    QCheckBox *WebProfileDirectoryListingCheckbox;
    QLabel *labelRealm;
    QLineEdit *webServerRealmEdit;
    QLabel *labelSessionTimeout;
    QSpinBox *webServerSessionTimeout;
    QTableWidget *webServerUserTable;
    QPushButton *webServerAddUserButton;
    QPushButton *webServerEditUserButton;
    QPushButton *webServerDeleteUserButton;
    QGroupBox *Docroot;
    QGridLayout *grid_Docroot;
    QLineEdit *WebProfileDocrootEdit;
    QPushButton *browseDocrootButton;
    QWidget *security_Page;
    QVBoxLayout *vlay_security_Page;
    QGroupBox *UserBlocking;
    QGridLayout *grid_UserBlocking;
    QRadioButton *check_BlockNormal;
    QRadioButton *check_BlockInvisible;
    QGroupBox *UserAuth;
    QGridLayout *grid_UserAuth;
    QCheckBox *blockallcheckBox;
    QGroupBox *WebPage;
    QGridLayout *grid_WebPage;
    QCheckBox *WebProfileCheckbox;
    QPlainTextEdit *b32address;
    QCheckBox *HideWebCheckbox;
    QWidget *sound_Page;
    QVBoxLayout *vlay_sound_Page;
    QGroupBox *SoundConnection;
    QGridLayout *grid_SoundConnection;
    QCheckBox *checkBoxSound_2;
    QLineEdit *txt_SoundFile2;
    QPushButton *cmd_openFile_2;
    QCheckBox *checkBoxSound;
    QLineEdit *txt_SoundFile;
    QPushButton *cmd_openFile;
    QGroupBox *SoundFilesend;
    QGridLayout *grid_SoundFilesend;
    QCheckBox *checkBoxSound_3;
    QLineEdit *txt_SoundFile3;
    QPushButton *cmd_openFile_3;
    QGroupBox *SoundFileReceive;
    QGridLayout *grid_SoundFileReceive;
    QCheckBox *checkBoxSound_5;
    QLineEdit *txt_SoundFile5;
    QPushButton *cmd_openFile_5;
    QCheckBox *checkBoxSound_4;
    QLineEdit *txt_SoundFile4;
    QPushButton *cmd_openFile_4;
    QGroupBox *SoundChatMessage;
    QGridLayout *grid_SoundChatMessage;
    QCheckBox *checkBoxSound_6;
    QLineEdit *txt_SoundFile6;
    QPushButton *cmd_openFile_6;
    QWidget *chat_Page;
    QVBoxLayout *vlay_chat_Page;
    QFrame *frame_18;
    QVBoxLayout *vlay_frame18;
    QGroupBox *LocalStyle;
    QGridLayout *grid_LocalStyle;
    QTextEdit *txtShowCurrentChatStyle;
    QToolButton *cmdBoldChat;
    QToolButton *cmdUnderChat;
    QToolButton *cmdItalicChat;
    QToolButton *cmdColorChat;
    QToolButton *cmdFontChat;
    QLabel *label_4;
    QGroupBox *RemoteStyle;
    QGridLayout *grid_RemoteStyle;
    QCheckBox *chatOverrideBox;
    QTextEdit *txtOverrideRemote;
    QToolButton *cmdOverwriteBoldChat;
    QToolButton *cmdOverwriteUnderChat;
    QToolButton *cmdOverwriteItalicChat;
    QToolButton *cmdOverwriteColorChat;
    QToolButton *cmdOverwriteFontChat;
    QGroupBox *Emoticons;
    QGridLayout *grid_Emoticons;
    QCheckBox *checkBoxEmoticons;
    QGroupBox *groupChatStyle;
    QGridLayout *grid_groupChatStyle;
    QLabel *labelChatStyle;
    QComboBox *comboBoxChatStyle;
    QCheckBox *checkBox_DisplayImagesInline;
    QGroupBox *UserEvents;
    QGridLayout *grid_UserEvents;
    QCheckBox *checkboxUserEvents;
    QGroupBox *groupBox;
    QGridLayout *grid_groupBox;
    QLabel *label_6;
    QSpinBox *spinBox_maxACK;
    QWidget *blocklist_page;
    QVBoxLayout *vlay_blocklist;
    QTreeWidget *UserBlockTreeWidget;
    QHBoxLayout *hlay_blocklist_buttons;
    QSpacerItem *blocklist_spacer;
    QPushButton *cmd_unblockUserFromBlockList;
    QPushButton *cmd_deleteUserFromBlockList;
    QListWidget *Sections;
    QFrame *line_2;
    QFrame *buttonFrame;
    QPushButton *cancel_Button;
    QPushButton *ok_Button;

    void setupUi(QDialog *SettingsGui)
    {
        if (SettingsGui->objectName().isEmpty())
            SettingsGui->setObjectName(QString::fromUtf8("SettingsGui"));
        SettingsGui->resize(534, 452);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SettingsGui->sizePolicy().hasHeightForWidth());
        SettingsGui->setSizePolicy(sizePolicy);
        SettingsGui->setMinimumSize(QSize(534, 452));
        SettingsGui->setMaximumSize(QSize(534, 452));
        QFont font;
        font.setPointSize(9);
        SettingsGui->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/settings.svg"), QSize(), QIcon::Normal, QIcon::Off);
        SettingsGui->setWindowIcon(icon);
        SettingsGui->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        SettingsGui->setSizeGripEnabled(false);
        SettingsGui->setModal(true);
        SettingsContainer = new QFrame(SettingsGui);
        SettingsContainer->setObjectName(QString::fromUtf8("SettingsContainer"));
        SettingsContainer->setGeometry(QRect(4, 6, 540, 448));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(SettingsContainer->sizePolicy().hasHeightForWidth());
        SettingsContainer->setSizePolicy(sizePolicy1);
        SettingsContainer->setMinimumSize(QSize(540, 448));
        SettingsContainer->setMaximumSize(QSize(540, 448));
        SettingsContainer->setFrameShape(QFrame::Shape::NoFrame);
        SettingsContainer->setFrameShadow(QFrame::Shadow::Plain);
        Settings = new QStackedWidget(SettingsContainer);
        Settings->setObjectName(QString::fromUtf8("Settings"));
        general_page = new QWidget();
        general_page->setObjectName(QString::fromUtf8("general_page"));
        general_page->setMinimumSize(QSize(394, 408));
        general_page->setMaximumSize(QSize(394, 408));
        general_page->setAutoFillBackground(true);
        vlay_general_page = new QVBoxLayout(general_page);
        vlay_general_page->setSpacing(0);
        vlay_general_page->setObjectName(QString::fromUtf8("vlay_general_page"));
        vlay_general_page->setContentsMargins(0, 0, 0, 0);
        FileXfer = new QGroupBox(general_page);
        FileXfer->setObjectName(QString::fromUtf8("FileXfer"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(FileXfer->sizePolicy().hasHeightForWidth());
        FileXfer->setSizePolicy(sizePolicy2);
        FileXfer->setMinimumSize(QSize(0, 142));
        FileXfer->setMaximumSize(QSize(16777215, 142));
        grid_FileXfer = new QGridLayout(FileXfer);
        grid_FileXfer->setObjectName(QString::fromUtf8("grid_FileXfer"));
        grid_FileXfer->setHorizontalSpacing(10);
        grid_FileXfer->setVerticalSpacing(2);
        grid_FileXfer->setContentsMargins(6, 2, 6, 2);
        checkBox_AutoAcceptFiles = new QCheckBox(FileXfer);
        checkBox_AutoAcceptFiles->setObjectName(QString::fromUtf8("checkBox_AutoAcceptFiles"));
        checkBox_AutoAcceptFiles->setEnabled(true);
        checkBox_AutoAcceptFiles->setCheckable(true);
        checkBox_AutoAcceptFiles->setMinimumSize(QSize(0, 26));
        checkBox_AutoAcceptFiles->setMaximumSize(QSize(16777215, 26));

        grid_FileXfer->addWidget(checkBox_AutoAcceptFiles, 0, 0, 1, 2, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        label_autoDL = new QLabel(FileXfer);
        label_autoDL->setObjectName(QString::fromUtf8("label_autoDL"));
        label_autoDL->setBaseSize(QSize(0, 20));
        label_autoDL->setMinimumSize(QSize(0, 26));
        label_autoDL->setMaximumSize(QSize(16777215, 26));

        grid_FileXfer->addWidget(label_autoDL, 2, 0, 1, 2);

        txt_IncomingFileFolder = new QLineEdit(FileXfer);
        txt_IncomingFileFolder->setObjectName(QString::fromUtf8("txt_IncomingFileFolder"));
        txt_IncomingFileFolder->setEnabled(false);
        txt_IncomingFileFolder->setAutoFillBackground(true);
        txt_IncomingFileFolder->setClearButtonEnabled(true);
        txt_IncomingFileFolder->setMinimumSize(QSize(0, 26));
        txt_IncomingFileFolder->setMaximumSize(QSize(16777215, 26));

        grid_FileXfer->addWidget(txt_IncomingFileFolder, 3, 0, 1, 1);

        cmd_Downloads = new QPushButton(FileXfer);
        cmd_Downloads->setObjectName(QString::fromUtf8("cmd_Downloads"));
        cmd_Downloads->setEnabled(true);
        cmd_Downloads->setMinimumSize(QSize(0, 26));
        cmd_Downloads->setMaximumSize(QSize(16777215, 26));

        grid_FileXfer->addWidget(cmd_Downloads, 3, 1, 1, 1);

        checkBox_Subfolders = new QCheckBox(FileXfer);
        checkBox_Subfolders->setObjectName(QString::fromUtf8("checkBox_Subfolders"));
        checkBox_Subfolders->setEnabled(true);
        checkBox_Subfolders->setCheckable(true);
        checkBox_Subfolders->setMinimumSize(QSize(0, 26));
        checkBox_Subfolders->setMaximumSize(QSize(16777215, 26));

        grid_FileXfer->addWidget(checkBox_Subfolders, 1, 0, 1, 2, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);


        vlay_general_page->addWidget(FileXfer);

        UserCheck = new QGroupBox(general_page);
        UserCheck->setObjectName(QString::fromUtf8("UserCheck"));
        sizePolicy2.setHeightForWidth(UserCheck->sizePolicy().hasHeightForWidth());
        UserCheck->setSizePolicy(sizePolicy2);
        UserCheck->setMinimumSize(QSize(0, 58));
        UserCheck->setMaximumSize(QSize(16777215, 58));
        grid_UserCheck = new QGridLayout(UserCheck);
        grid_UserCheck->setObjectName(QString::fromUtf8("grid_UserCheck"));
        grid_UserCheck->setHorizontalSpacing(10);
        grid_UserCheck->setVerticalSpacing(2);
        grid_UserCheck->setContentsMargins(6, 2, 6, 2);
        OfflineChkSpinBox = new QSpinBox(UserCheck);
        OfflineChkSpinBox->setObjectName(QString::fromUtf8("OfflineChkSpinBox"));
        OfflineChkSpinBox->setMinimum(5);
        OfflineChkSpinBox->setMaximum(120);
        OfflineChkSpinBox->setValue(5);
        OfflineChkSpinBox->setMinimumSize(QSize(0, 26));
        OfflineChkSpinBox->setMaximumSize(QSize(16777215, 26));

        grid_UserCheck->addWidget(OfflineChkSpinBox, 0, 1, 1, 1);

        label_offlinechk = new QLabel(UserCheck);
        label_offlinechk->setObjectName(QString::fromUtf8("label_offlinechk"));
        label_offlinechk->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        label_offlinechk->setMinimumSize(QSize(0, 26));
        label_offlinechk->setMaximumSize(QSize(16777215, 26));

        grid_UserCheck->addWidget(label_offlinechk, 0, 0, 1, 1);


        vlay_general_page->addWidget(UserCheck);

        AutoAway = new QGroupBox(general_page);
        AutoAway->setObjectName(QString::fromUtf8("AutoAway"));
        AutoAway->setCheckable(true);
        sizePolicy2.setHeightForWidth(AutoAway->sizePolicy().hasHeightForWidth());
        AutoAway->setSizePolicy(sizePolicy2);
        AutoAway->setMinimumSize(QSize(0, 58));
        AutoAway->setMaximumSize(QSize(16777215, 58));
        grid_AutoAway = new QGridLayout(AutoAway);
        grid_AutoAway->setObjectName(QString::fromUtf8("grid_AutoAway"));
        grid_AutoAway->setHorizontalSpacing(10);
        grid_AutoAway->setVerticalSpacing(2);
        grid_AutoAway->setContentsMargins(6, 2, 6, 2);
        AutoAwaySpinBox = new QSpinBox(AutoAway);
        AutoAwaySpinBox->setObjectName(QString::fromUtf8("AutoAwaySpinBox"));
        AutoAwaySpinBox->setMinimum(0);
        AutoAwaySpinBox->setMaximum(720);
        AutoAwaySpinBox->setValue(0);
        AutoAwaySpinBox->setMinimumSize(QSize(0, 26));
        AutoAwaySpinBox->setMaximumSize(QSize(16777215, 26));

        grid_AutoAway->addWidget(AutoAwaySpinBox, 0, 1, 1, 1);

        NoActivityLabel = new QLabel(AutoAway);
        NoActivityLabel->setObjectName(QString::fromUtf8("NoActivityLabel"));
        NoActivityLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        NoActivityLabel->setMinimumSize(QSize(0, 26));
        NoActivityLabel->setMaximumSize(QSize(16777215, 26));

        grid_AutoAway->addWidget(NoActivityLabel, 0, 0, 1, 1);


        vlay_general_page->addWidget(AutoAway);

        SAMLog = new QGroupBox(general_page);
        SAMLog->setObjectName(QString::fromUtf8("SAMLog"));
        sizePolicy2.setHeightForWidth(SAMLog->sizePolicy().hasHeightForWidth());
        SAMLog->setSizePolicy(sizePolicy2);
        SAMLog->setMinimumSize(QSize(0, 58));
        SAMLog->setMaximumSize(QSize(16777215, 58));
        grid_SAMLog = new QGridLayout(SAMLog);
        grid_SAMLog->setObjectName(QString::fromUtf8("grid_SAMLog"));
        grid_SAMLog->setHorizontalSpacing(10);
        grid_SAMLog->setVerticalSpacing(2);
        grid_SAMLog->setContentsMargins(6, 2, 6, 2);
        MaxLogLabel = new QLabel(SAMLog);
        MaxLogLabel->setObjectName(QString::fromUtf8("MaxLogLabel"));
        MaxLogLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        MaxLogLabel->setMinimumSize(QSize(0, 26));
        MaxLogLabel->setMaximumSize(QSize(16777215, 26));

        grid_SAMLog->addWidget(MaxLogLabel, 0, 0, 1, 1);

        spinBox = new QSpinBox(SAMLog);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setMaximum(1000);
        spinBox->setValue(100);
        spinBox->setMinimumSize(QSize(0, 26));
        spinBox->setMaximumSize(QSize(16777215, 26));

        grid_SAMLog->addWidget(spinBox, 0, 1, 1, 1);


        vlay_general_page->addWidget(SAMLog);

        Debugging = new QGroupBox(general_page);
        Debugging->setObjectName(QString::fromUtf8("Debugging"));
        sizePolicy2.setHeightForWidth(Debugging->sizePolicy().hasHeightForWidth());
        Debugging->setSizePolicy(sizePolicy2);
        Debugging->setMinimumSize(QSize(0, 58));
        Debugging->setMaximumSize(QSize(16777215, 58));
        grid_Debugging = new QGridLayout(Debugging);
        grid_Debugging->setObjectName(QString::fromUtf8("grid_Debugging"));
        grid_Debugging->setHorizontalSpacing(10);
        grid_Debugging->setVerticalSpacing(2);
        grid_Debugging->setContentsMargins(6, 2, 6, 2);
        checkBox_DebugLog = new QCheckBox(Debugging);
        checkBox_DebugLog->setObjectName(QString::fromUtf8("checkBox_DebugLog"));
        checkBox_DebugLog->setEnabled(true);
        checkBox_DebugLog->setCheckable(true);
        checkBox_DebugLog->setMinimumSize(QSize(0, 26));
        checkBox_DebugLog->setMaximumSize(QSize(16777215, 26));

        grid_Debugging->addWidget(checkBox_DebugLog, 0, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);


        vlay_general_page->addWidget(Debugging);

        Settings->addWidget(general_page);
        network_Page = new QWidget();
        network_Page->setObjectName(QString::fromUtf8("network_Page"));
        network_Page->setMinimumSize(QSize(394, 408));
        network_Page->setMaximumSize(QSize(394, 408));
        network_Page->setAutoFillBackground(true);
        vlay_network_Page = new QVBoxLayout(network_Page);
        vlay_network_Page->setSpacing(0);
        vlay_network_Page->setObjectName(QString::fromUtf8("vlay_network_Page"));
        vlay_network_Page->setContentsMargins(0, 0, 0, 0);
        SAM = new QGroupBox(network_Page);
        SAM->setObjectName(QString::fromUtf8("SAM"));
        sizePolicy2.setHeightForWidth(SAM->sizePolicy().hasHeightForWidth());
        SAM->setSizePolicy(sizePolicy2);
        SAM->setMinimumSize(QSize(0, 58));
        SAM->setMaximumSize(QSize(16777215, 58));
        grid_SAM = new QGridLayout(SAM);
        grid_SAM->setObjectName(QString::fromUtf8("grid_SAM"));
        grid_SAM->setHorizontalSpacing(10);
        grid_SAM->setVerticalSpacing(2);
        grid_SAM->setContentsMargins(6, 2, 6, 2);
        label_17 = new QLabel(SAM);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setBaseSize(QSize(120, 0));
        label_17->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        label_17->setMinimumSize(QSize(0, 26));
        label_17->setMaximumSize(QSize(16777215, 26));

        grid_SAM->addWidget(label_17, 0, 0, 1, 1);

        lineEdit_3 = new QLineEdit(SAM);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));
        lineEdit_3->setClearButtonEnabled(true);
        lineEdit_3->setMinimumSize(QSize(0, 26));
        lineEdit_3->setMaximumSize(QSize(16777215, 26));

        grid_SAM->addWidget(lineEdit_3, 0, 1, 1, 1);

        label_18 = new QLabel(SAM);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setMinimumSize(QSize(0, 26));
        label_18->setMaximumSize(QSize(16777215, 26));

        grid_SAM->addWidget(label_18, 0, 2, 1, 1);

        spinBox_10 = new QSpinBox(SAM);
        spinBox_10->setObjectName(QString::fromUtf8("spinBox_10"));
        spinBox_10->setMaximum(65000);
        spinBox_10->setValue(7656);
        spinBox_10->setMinimumSize(QSize(0, 26));
        spinBox_10->setMaximumSize(QSize(16777215, 26));

        grid_SAM->addWidget(spinBox_10, 0, 3, 1, 1);


        vlay_network_Page->addWidget(SAM);

        SigType = new QGroupBox(network_Page);
        SigType->setObjectName(QString::fromUtf8("SigType"));
        sizePolicy2.setHeightForWidth(SigType->sizePolicy().hasHeightForWidth());
        SigType->setSizePolicy(sizePolicy2);
        SigType->setMinimumSize(QSize(0, 114));
        SigType->setMaximumSize(QSize(16777215, 114));
        grid_SigType = new QGridLayout(SigType);
        grid_SigType->setObjectName(QString::fromUtf8("grid_SigType"));
        grid_SigType->setHorizontalSpacing(10);
        grid_SigType->setVerticalSpacing(2);
        grid_SigType->setContentsMargins(6, 2, 6, 2);
        requestAuthcheckBox = new QCheckBox(SigType);
        requestAuthcheckBox->setObjectName(QString::fromUtf8("requestAuthcheckBox"));
        requestAuthcheckBox->setEnabled(true);
        requestAuthcheckBox->setMinimumSize(QSize(0, 26));
        requestAuthcheckBox->setMaximumSize(QSize(16777215, 26));

        grid_SigType->addWidget(requestAuthcheckBox, 0, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        comboBox_SigType = new QComboBox(SigType);
        comboBox_SigType->setObjectName(QString::fromUtf8("comboBox_SigType"));
        comboBox_SigType->setEnabled(true);
        comboBox_SigType->setMinimumSize(QSize(0, 26));
        comboBox_SigType->setMaximumSize(QSize(16777215, 26));

        grid_SigType->addWidget(comboBox_SigType, 0, 1, 1, 1);

        label_26 = new QLabel(SigType);
        label_26->setObjectName(QString::fromUtf8("label_26"));
        label_26->setBaseSize(QSize(160, 0));
        label_26->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        label_26->setMinimumSize(QSize(0, 26));
        label_26->setMaximumSize(QSize(16777215, 26));

        grid_SigType->addWidget(label_26, 1, 0, 1, 1);

        cmd_DestGenerate = new QPushButton(SigType);
        cmd_DestGenerate->setObjectName(QString::fromUtf8("cmd_DestGenerate"));
        cmd_DestGenerate->setMinimumSize(QSize(0, 26));
        cmd_DestGenerate->setMaximumSize(QSize(16777215, 26));

        grid_SigType->addWidget(cmd_DestGenerate, 1, 1, 1, 1);

        nonpersistdest = new QCheckBox(SigType);
        nonpersistdest->setObjectName(QString::fromUtf8("nonpersistdest"));
        nonpersistdest->setMinimumSize(QSize(0, 26));
        nonpersistdest->setMaximumSize(QSize(16777215, 26));

        grid_SigType->addWidget(nonpersistdest, 2, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);


        vlay_network_Page->addWidget(SigType);

        Crypto = new QGroupBox(network_Page);
        Crypto->setObjectName(QString::fromUtf8("Crypto"));
        sizePolicy2.setHeightForWidth(Crypto->sizePolicy().hasHeightForWidth());
        Crypto->setSizePolicy(sizePolicy2);
        Crypto->setMinimumSize(QSize(0, 58));
        Crypto->setMaximumSize(QSize(16777215, 58));
        grid_Crypto = new QGridLayout(Crypto);
        grid_Crypto->setObjectName(QString::fromUtf8("grid_Crypto"));
        grid_Crypto->setHorizontalSpacing(10);
        grid_Crypto->setVerticalSpacing(2);
        grid_Crypto->setContentsMargins(6, 2, 6, 2);
        Encryption_label = new QLabel(Crypto);
        Encryption_label->setObjectName(QString::fromUtf8("Encryption_label"));
        Encryption_label->setEnabled(true);
        Encryption_label->setBaseSize(QSize(120, 0));
        Encryption_label->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        Encryption_label->setMinimumSize(QSize(0, 26));
        Encryption_label->setMaximumSize(QSize(16777215, 26));

        grid_Crypto->addWidget(Encryption_label, 0, 0, 1, 1);

        comboBox_EncType = new QComboBox(Crypto);
        comboBox_EncType->setObjectName(QString::fromUtf8("comboBox_EncType"));
        comboBox_EncType->setEnabled(true);
        comboBox_EncType->setMinimumSize(QSize(0, 26));
        comboBox_EncType->setMaximumSize(QSize(16777215, 26));

        grid_Crypto->addWidget(comboBox_EncType, 0, 1, 1, 2);


        vlay_network_Page->addWidget(Crypto);

        TunnelConfig = new QGroupBox(network_Page);
        TunnelConfig->setObjectName(QString::fromUtf8("TunnelConfig"));
        sizePolicy2.setHeightForWidth(TunnelConfig->sizePolicy().hasHeightForWidth());
        TunnelConfig->setSizePolicy(sizePolicy2);
        TunnelConfig->setMinimumSize(QSize(0, 170));
        TunnelConfig->setMaximumSize(QSize(16777215, 170));
        grid_TunnelConfig = new QGridLayout(TunnelConfig);
        grid_TunnelConfig->setObjectName(QString::fromUtf8("grid_TunnelConfig"));
        grid_TunnelConfig->setHorizontalSpacing(10);
        grid_TunnelConfig->setVerticalSpacing(2);
        grid_TunnelConfig->setContentsMargins(6, 2, 6, 2);
        label_9 = new QLabel(TunnelConfig);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setBaseSize(QSize(160, 0));
        label_9->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        label_9->setMinimumSize(QSize(0, 26));
        label_9->setMaximumSize(QSize(16777215, 26));

        grid_TunnelConfig->addWidget(label_9, 0, 0, 1, 1);

        lineEdit = new QLineEdit(TunnelConfig);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setClearButtonEnabled(true);
        lineEdit->setMinimumSize(QSize(0, 26));
        lineEdit->setMaximumSize(QSize(16777215, 26));

        grid_TunnelConfig->addWidget(lineEdit, 0, 1, 1, 1);

        label_14 = new QLabel(TunnelConfig);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setMinimumSize(QSize(0, 26));
        label_14->setMaximumSize(QSize(16777215, 26));

        grid_TunnelConfig->addWidget(label_14, 1, 2, 1, 1);

        label_15 = new QLabel(TunnelConfig);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setMinimumSize(QSize(0, 26));
        label_15->setMaximumSize(QSize(16777215, 26));

        grid_TunnelConfig->addWidget(label_15, 1, 1, 1, 1);

        spinBox_8 = new QSpinBox(TunnelConfig);
        spinBox_8->setObjectName(QString::fromUtf8("spinBox_8"));
        spinBox_8->setMinimumSize(QSize(0, 26));
        spinBox_8->setMaximumSize(QSize(16777215, 26));

        grid_TunnelConfig->addWidget(spinBox_8, 2, 2, 1, 1);

        spinBox_4 = new QSpinBox(TunnelConfig);
        spinBox_4->setObjectName(QString::fromUtf8("spinBox_4"));
        spinBox_4->setMinimumSize(QSize(0, 26));
        spinBox_4->setMaximumSize(QSize(16777215, 26));

        grid_TunnelConfig->addWidget(spinBox_4, 2, 1, 1, 1);

        label_10 = new QLabel(TunnelConfig);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setBaseSize(QSize(160, 0));
        label_10->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        label_10->setMinimumSize(QSize(0, 26));
        label_10->setMaximumSize(QSize(16777215, 26));

        grid_TunnelConfig->addWidget(label_10, 2, 0, 1, 1);

        spinBox_5 = new QSpinBox(TunnelConfig);
        spinBox_5->setObjectName(QString::fromUtf8("spinBox_5"));
        spinBox_5->setMinimumSize(QSize(0, 26));
        spinBox_5->setMaximumSize(QSize(16777215, 26));

        grid_TunnelConfig->addWidget(spinBox_5, 3, 1, 1, 1);

        spinBox_9 = new QSpinBox(TunnelConfig);
        spinBox_9->setObjectName(QString::fromUtf8("spinBox_9"));
        spinBox_9->setMinimumSize(QSize(0, 26));
        spinBox_9->setMaximumSize(QSize(16777215, 26));

        grid_TunnelConfig->addWidget(spinBox_9, 3, 2, 1, 1);

        label_11 = new QLabel(TunnelConfig);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setBaseSize(QSize(160, 0));
        label_11->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        label_11->setMinimumSize(QSize(0, 26));
        label_11->setMaximumSize(QSize(16777215, 26));

        grid_TunnelConfig->addWidget(label_11, 3, 0, 1, 1);

        spinBox_7 = new QSpinBox(TunnelConfig);
        spinBox_7->setObjectName(QString::fromUtf8("spinBox_7"));
        spinBox_7->setMinimumSize(QSize(0, 26));
        spinBox_7->setMaximumSize(QSize(16777215, 26));

        grid_TunnelConfig->addWidget(spinBox_7, 4, 2, 1, 1);

        label_12 = new QLabel(TunnelConfig);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setBaseSize(QSize(160, 0));
        label_12->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        label_12->setMinimumSize(QSize(0, 26));
        label_12->setMaximumSize(QSize(16777215, 26));

        grid_TunnelConfig->addWidget(label_12, 4, 0, 1, 1);

        spinBox_6 = new QSpinBox(TunnelConfig);
        spinBox_6->setObjectName(QString::fromUtf8("spinBox_6"));
        spinBox_6->setMinimumSize(QSize(0, 26));
        spinBox_6->setMaximumSize(QSize(16777215, 26));

        grid_TunnelConfig->addWidget(spinBox_6, 4, 1, 1, 1);


        vlay_network_Page->addWidget(TunnelConfig);

        Settings->addWidget(network_Page);
        userdetails_Page = new QWidget();
        userdetails_Page->setObjectName(QString::fromUtf8("userdetails_Page"));
        userdetails_Page->setMinimumSize(QSize(394, 408));
        userdetails_Page->setMaximumSize(QSize(394, 408));
        userdetails_Page->setAutoFillBackground(true);
        vlay_userdetails_Page = new QVBoxLayout(userdetails_Page);
        vlay_userdetails_Page->setSpacing(0);
        vlay_userdetails_Page->setObjectName(QString::fromUtf8("vlay_userdetails_Page"));
        vlay_userdetails_Page->setContentsMargins(0, 0, 0, 0);
        UserBio = new QGroupBox(userdetails_Page);
        UserBio->setObjectName(QString::fromUtf8("UserBio"));
        sizePolicy2.setHeightForWidth(UserBio->sizePolicy().hasHeightForWidth());
        UserBio->setSizePolicy(sizePolicy2);
        UserBio->setMinimumSize(QSize(0, 58));
        UserBio->setMaximumSize(QSize(16777215, 58));
        grid_UserBio = new QGridLayout(UserBio);
        grid_UserBio->setObjectName(QString::fromUtf8("grid_UserBio"));
        grid_UserBio->setHorizontalSpacing(10);
        grid_UserBio->setVerticalSpacing(2);
        grid_UserBio->setContentsMargins(6, 2, 6, 2);
        label_19 = new QLabel(UserBio);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        label_19->setMinimumSize(QSize(0, 26));
        label_19->setMaximumSize(QSize(16777215, 26));

        grid_UserBio->addWidget(label_19, 0, 0, 1, 1);

        txt_Nickname = new QLineEdit(UserBio);
        txt_Nickname->setObjectName(QString::fromUtf8("txt_Nickname"));
        txt_Nickname->setEnabled(true);
        txt_Nickname->setClearButtonEnabled(true);
        txt_Nickname->setMinimumSize(QSize(0, 26));
        txt_Nickname->setMaximumSize(QSize(16777215, 26));

        grid_UserBio->addWidget(txt_Nickname, 0, 1, 1, 2);


        vlay_userdetails_Page->addWidget(UserBio);

        UserSuppInfo = new QGroupBox(userdetails_Page);
        UserSuppInfo->setObjectName(QString::fromUtf8("UserSuppInfo"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(UserSuppInfo->sizePolicy().hasHeightForWidth());
        UserSuppInfo->setSizePolicy(sizePolicy3);
        UserSuppInfo->setMinimumSize(QSize(0, 110));
        grid_UserSuppInfo = new QGridLayout(UserSuppInfo);
        grid_UserSuppInfo->setObjectName(QString::fromUtf8("grid_UserSuppInfo"));
        grid_UserSuppInfo->setHorizontalSpacing(10);
        grid_UserSuppInfo->setVerticalSpacing(2);
        grid_UserSuppInfo->setContentsMargins(6, 2, 6, 2);
        txt_Interests = new QPlainTextEdit(UserSuppInfo);
        txt_Interests->setObjectName(QString::fromUtf8("txt_Interests"));
        txt_Interests->setMinimumSize(QSize(0, 60));
        txt_Interests->setMaximumSize(QSize(16777215, 16777215));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(txt_Interests->sizePolicy().hasHeightForWidth());
        txt_Interests->setSizePolicy(sizePolicy4);

        grid_UserSuppInfo->addWidget(txt_Interests, 0, 0, 1, 1);


        vlay_userdetails_Page->addWidget(UserSuppInfo);

        SelectAvatar = new QGroupBox(userdetails_Page);
        SelectAvatar->setObjectName(QString::fromUtf8("SelectAvatar"));
        sizePolicy2.setHeightForWidth(SelectAvatar->sizePolicy().hasHeightForWidth());
        SelectAvatar->setSizePolicy(sizePolicy2);
        SelectAvatar->setMinimumSize(QSize(0, 114));
        SelectAvatar->setMaximumSize(QSize(16777215, 114));
        grid_SelectAvatar = new QGridLayout(SelectAvatar);
        grid_SelectAvatar->setObjectName(QString::fromUtf8("grid_SelectAvatar"));
        grid_SelectAvatar->setHorizontalSpacing(10);
        grid_SelectAvatar->setVerticalSpacing(2);
        grid_SelectAvatar->setContentsMargins(6, 2, 6, 2);
        ownavatar_label = new QLabel(SelectAvatar);
        ownavatar_label->setObjectName(QString::fromUtf8("ownavatar_label"));
        ownavatar_label->setSizeIncrement(QSize(80, 80));
        ownavatar_label->setAutoFillBackground(false);
        ownavatar_label->setStyleSheet(QString::fromUtf8("QLabel{\n"
"border: 1px solid #999;\n"
"border-radius: 2px;\n"
"}"));
        ownavatar_label->setPixmap(QPixmap(QString::fromUtf8(":/icons/silhouette.svg")));
        ownavatar_label->setScaledContents(true);
        ownavatar_label->setMinimumSize(QSize(0, 26));
        ownavatar_label->setMaximumSize(QSize(16777215, 26));

        grid_SelectAvatar->addWidget(ownavatar_label, 1, 0, 1, 2);

        label = new QLabel(SelectAvatar);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label->setMinimumSize(QSize(0, 26));
        label->setMaximumSize(QSize(16777215, 26));

        grid_SelectAvatar->addWidget(label, 0, 0, 1, 2);

        cmd_selectAvatarImage = new QPushButton(SelectAvatar);
        cmd_selectAvatarImage->setObjectName(QString::fromUtf8("cmd_selectAvatarImage"));
        cmd_selectAvatarImage->setMinimumSize(QSize(0, 26));
        cmd_selectAvatarImage->setMaximumSize(QSize(16777215, 26));

        grid_SelectAvatar->addWidget(cmd_selectAvatarImage, 2, 0, 1, 1);

        cmd_clearAvatarImage = new QPushButton(SelectAvatar);
        cmd_clearAvatarImage->setObjectName(QString::fromUtf8("cmd_clearAvatarImage"));
        cmd_clearAvatarImage->setMinimumSize(QSize(0, 26));
        cmd_clearAvatarImage->setMaximumSize(QSize(16777215, 26));

        grid_SelectAvatar->addWidget(cmd_clearAvatarImage, 2, 1, 1, 1);


        vlay_userdetails_Page->addWidget(SelectAvatar);

        Settings->addWidget(userdetails_Page);
        style_Page = new QWidget();
        style_Page->setObjectName(QString::fromUtf8("style_Page"));
        style_Page->setMinimumSize(QSize(394, 408));
        style_Page->setMaximumSize(QSize(394, 408));
        style_Page->setAutoFillBackground(true);
        vlay_style_Page = new QVBoxLayout(style_Page);
        vlay_style_Page->setSpacing(0);
        vlay_style_Page->setObjectName(QString::fromUtf8("vlay_style_Page"));
        vlay_style_Page->setContentsMargins(0, 0, 0, 0);
        WindowStyle = new QGroupBox(style_Page);
        WindowStyle->setObjectName(QString::fromUtf8("WindowStyle"));
        sizePolicy2.setHeightForWidth(WindowStyle->sizePolicy().hasHeightForWidth());
        WindowStyle->setSizePolicy(sizePolicy2);
        WindowStyle->setMinimumSize(QSize(0, 58));
        WindowStyle->setMaximumSize(QSize(16777215, 58));
        grid_WindowStyle = new QGridLayout(WindowStyle);
        grid_WindowStyle->setObjectName(QString::fromUtf8("grid_WindowStyle"));
        grid_WindowStyle->setHorizontalSpacing(10);
        grid_WindowStyle->setVerticalSpacing(2);
        grid_WindowStyle->setContentsMargins(6, 2, 6, 2);
        label_7 = new QLabel(WindowStyle);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setBaseSize(QSize(120, 92));
        label_7->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        label_7->setMinimumSize(QSize(0, 26));
        label_7->setMaximumSize(QSize(16777215, 26));

        grid_WindowStyle->addWidget(label_7, 0, 0, 1, 1);

        styleCombo = new QComboBox(WindowStyle);
        styleCombo->setObjectName(QString::fromUtf8("styleCombo"));
        styleCombo->setEnabled(true);
        styleCombo->setBaseSize(QSize(200, 92));
        styleCombo->setFrame(true);
        styleCombo->setMinimumSize(QSize(0, 26));
        styleCombo->setMaximumSize(QSize(16777215, 26));

        grid_WindowStyle->addWidget(styleCombo, 0, 1, 1, 1);


        vlay_style_Page->addWidget(WindowStyle);

        CustomStyle = new QGroupBox(style_Page);
        CustomStyle->setObjectName(QString::fromUtf8("CustomStyle"));
        CustomStyle->setSizeIncrement(QSize(0, 0));
        sizePolicy2.setHeightForWidth(CustomStyle->sizePolicy().hasHeightForWidth());
        CustomStyle->setSizePolicy(sizePolicy2);
        CustomStyle->setMinimumSize(QSize(0, 58));
        CustomStyle->setMaximumSize(QSize(16777215, 58));
        grid_CustomStyle = new QGridLayout(CustomStyle);
        grid_CustomStyle->setObjectName(QString::fromUtf8("grid_CustomStyle"));
        grid_CustomStyle->setHorizontalSpacing(10);
        grid_CustomStyle->setVerticalSpacing(2);
        grid_CustomStyle->setContentsMargins(6, 2, 6, 2);
        label_8 = new QLabel(CustomStyle);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setBaseSize(QSize(120, 92));
        label_8->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        label_8->setMinimumSize(QSize(0, 26));
        label_8->setMaximumSize(QSize(16777215, 26));

        grid_CustomStyle->addWidget(label_8, 0, 0, 1, 1);

        styleSheetCombo = new QComboBox(CustomStyle);
        styleSheetCombo->setObjectName(QString::fromUtf8("styleSheetCombo"));
        styleSheetCombo->setEnabled(true);
        QFont font1;
        font1.setPointSize(9);
        font1.setBold(false);
        styleSheetCombo->setFont(font1);
        styleSheetCombo->setAutoFillBackground(false);
        styleSheetCombo->setIconSize(QSize(200, 92));
        styleSheetCombo->setFrame(true);
        styleSheetCombo->setMinimumSize(QSize(0, 26));
        styleSheetCombo->setMaximumSize(QSize(16777215, 26));

        grid_CustomStyle->addWidget(styleSheetCombo, 0, 1, 1, 1);


        vlay_style_Page->addWidget(CustomStyle);

        ContactSort = new QGroupBox(style_Page);
        ContactSort->setObjectName(QString::fromUtf8("ContactSort"));
        sizePolicy2.setHeightForWidth(ContactSort->sizePolicy().hasHeightForWidth());
        ContactSort->setSizePolicy(sizePolicy2);
        ContactSort->setMinimumSize(QSize(0, 170));
        ContactSort->setMaximumSize(QSize(16777215, 170));
        grid_ContactSort = new QGridLayout(ContactSort);
        grid_ContactSort->setObjectName(QString::fromUtf8("grid_ContactSort"));
        grid_ContactSort->setHorizontalSpacing(10);
        grid_ContactSort->setVerticalSpacing(2);
        grid_ContactSort->setContentsMargins(6, 2, 6, 2);
        checkBox_5 = new QCheckBox(ContactSort);
        checkBox_5->setObjectName(QString::fromUtf8("checkBox_5"));
        checkBox_5->setEnabled(true);
        checkBox_5->setChecked(false);
        checkBox_5->setMinimumSize(QSize(0, 26));
        checkBox_5->setMaximumSize(QSize(16777215, 26));

        grid_ContactSort->addWidget(checkBox_5, 0, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        radioButton_2 = new QRadioButton(ContactSort);
        radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));
        radioButton_2->setEnabled(true);
        radioButton_2->setChecked(true);
        radioButton_2->setMinimumSize(QSize(0, 26));
        radioButton_2->setMaximumSize(QSize(16777215, 26));

        grid_ContactSort->addWidget(radioButton_2, 1, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        radioButton_3 = new QRadioButton(ContactSort);
        radioButton_3->setObjectName(QString::fromUtf8("radioButton_3"));
        radioButton_3->setEnabled(true);
        radioButton_3->setChecked(false);
        radioButton_3->setMinimumSize(QSize(0, 26));
        radioButton_3->setMaximumSize(QSize(16777215, 26));

        grid_ContactSort->addWidget(radioButton_3, 2, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        radioButton_4 = new QRadioButton(ContactSort);
        radioButton_4->setObjectName(QString::fromUtf8("radioButton_4"));
        radioButton_4->setEnabled(true);
        radioButton_4->setChecked(false);
        radioButton_4->setMinimumSize(QSize(0, 26));
        radioButton_4->setMaximumSize(QSize(16777215, 26));

        grid_ContactSort->addWidget(radioButton_4, 3, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        radioButton_5 = new QRadioButton(ContactSort);
        radioButton_5->setObjectName(QString::fromUtf8("radioButton_5"));
        radioButton_5->setEnabled(true);
        radioButton_5->setChecked(false);
        radioButton_5->setMinimumSize(QSize(0, 26));
        radioButton_5->setMaximumSize(QSize(16777215, 26));

        grid_ContactSort->addWidget(radioButton_5, 4, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);


        vlay_style_Page->addWidget(ContactSort);

        Settings->addWidget(style_Page);
        webserver_Page = new QWidget();
        webserver_Page->setObjectName(QString::fromUtf8("webserver_Page"));
        webserver_Page->setMinimumSize(QSize(394, 408));
        webserver_Page->setMaximumSize(QSize(394, 408));
        webserver_Page->setAutoFillBackground(true);
        vlay_webserver_Page = new QVBoxLayout(webserver_Page);
        vlay_webserver_Page->setSpacing(0);
        vlay_webserver_Page->setObjectName(QString::fromUtf8("vlay_webserver_Page"));
        vlay_webserver_Page->setContentsMargins(0, 0, 0, 0);
        WebServer = new QGroupBox(webserver_Page);
        WebServer->setObjectName(QString::fromUtf8("WebServer"));
        sizePolicy2.setHeightForWidth(WebServer->sizePolicy().hasHeightForWidth());
        WebServer->setSizePolicy(sizePolicy2);
        WebServer->setMinimumSize(QSize(0, 198));
        WebServer->setMaximumSize(QSize(16777215, 198));
        grid_WebServer = new QGridLayout(WebServer);
        grid_WebServer->setObjectName(QString::fromUtf8("grid_WebServer"));
        grid_WebServer->setHorizontalSpacing(10);
        grid_WebServer->setVerticalSpacing(2);
        grid_WebServer->setContentsMargins(6, 2, 6, 2);
        WebServerAuthCheckbox = new QCheckBox(WebServer);
        WebServerAuthCheckbox->setObjectName(QString::fromUtf8("WebServerAuthCheckbox"));
        WebServerAuthCheckbox->setChecked(false);
        WebServerAuthCheckbox->setMinimumSize(QSize(0, 26));
        WebServerAuthCheckbox->setMaximumSize(QSize(16777215, 26));

        grid_WebServer->addWidget(WebServerAuthCheckbox, 0, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        WebProfileDirectoryListingCheckbox = new QCheckBox(WebServer);
        WebProfileDirectoryListingCheckbox->setObjectName(QString::fromUtf8("WebProfileDirectoryListingCheckbox"));
        WebProfileDirectoryListingCheckbox->setChecked(false);
        WebProfileDirectoryListingCheckbox->setMinimumSize(QSize(0, 26));
        WebProfileDirectoryListingCheckbox->setMaximumSize(QSize(16777215, 26));

        grid_WebServer->addWidget(WebProfileDirectoryListingCheckbox, 1, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        labelRealm = new QLabel(WebServer);
        labelRealm->setObjectName(QString::fromUtf8("labelRealm"));
        labelRealm->setMinimumSize(QSize(0, 26));
        labelRealm->setMaximumSize(QSize(16777215, 26));

        grid_WebServer->addWidget(labelRealm, 2, 0, 1, 1);

        webServerRealmEdit = new QLineEdit(WebServer);
        webServerRealmEdit->setObjectName(QString::fromUtf8("webServerRealmEdit"));
        webServerRealmEdit->setMinimumSize(QSize(0, 26));
        webServerRealmEdit->setMaximumSize(QSize(16777215, 26));

        grid_WebServer->addWidget(webServerRealmEdit, 2, 0, 1, 1);

        labelSessionTimeout = new QLabel(WebServer);
        labelSessionTimeout->setObjectName(QString::fromUtf8("labelSessionTimeout"));
        labelSessionTimeout->setMinimumSize(QSize(0, 26));
        labelSessionTimeout->setMaximumSize(QSize(16777215, 26));

        grid_WebServer->addWidget(labelSessionTimeout, 3, 0, 1, 1);

        webServerSessionTimeout = new QSpinBox(WebServer);
        webServerSessionTimeout->setObjectName(QString::fromUtf8("webServerSessionTimeout"));
        webServerSessionTimeout->setMinimum(1);
        webServerSessionTimeout->setMaximum(1440);
        webServerSessionTimeout->setSingleStep(5);
        webServerSessionTimeout->setValue(60);
        webServerSessionTimeout->setMinimumSize(QSize(0, 26));
        webServerSessionTimeout->setMaximumSize(QSize(16777215, 26));

        grid_WebServer->addWidget(webServerSessionTimeout, 3, 0, 1, 1);

        webServerUserTable = new QTableWidget(WebServer);
        webServerUserTable->setObjectName(QString::fromUtf8("webServerUserTable"));
        webServerUserTable->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        webServerUserTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        grid_WebServer->addWidget(webServerUserTable, 4, 0, 1, 1);

        webServerAddUserButton = new QPushButton(WebServer);
        webServerAddUserButton->setObjectName(QString::fromUtf8("webServerAddUserButton"));
        webServerAddUserButton->setMinimumSize(QSize(0, 26));
        webServerAddUserButton->setMaximumSize(QSize(16777215, 26));

        grid_WebServer->addWidget(webServerAddUserButton, 5, 0, 1, 1);

        webServerEditUserButton = new QPushButton(WebServer);
        webServerEditUserButton->setObjectName(QString::fromUtf8("webServerEditUserButton"));
        webServerEditUserButton->setMinimumSize(QSize(0, 26));
        webServerEditUserButton->setMaximumSize(QSize(16777215, 26));

        grid_WebServer->addWidget(webServerEditUserButton, 5, 0, 1, 1);

        webServerDeleteUserButton = new QPushButton(WebServer);
        webServerDeleteUserButton->setObjectName(QString::fromUtf8("webServerDeleteUserButton"));
        webServerDeleteUserButton->setMinimumSize(QSize(0, 26));
        webServerDeleteUserButton->setMaximumSize(QSize(16777215, 26));

        grid_WebServer->addWidget(webServerDeleteUserButton, 5, 0, 1, 1);


        vlay_webserver_Page->addWidget(WebServer);

        Docroot = new QGroupBox(webserver_Page);
        Docroot->setObjectName(QString::fromUtf8("Docroot"));
        sizePolicy2.setHeightForWidth(Docroot->sizePolicy().hasHeightForWidth());
        Docroot->setSizePolicy(sizePolicy2);
        Docroot->setMinimumSize(QSize(0, 58));
        Docroot->setMaximumSize(QSize(16777215, 58));
        grid_Docroot = new QGridLayout(Docroot);
        grid_Docroot->setObjectName(QString::fromUtf8("grid_Docroot"));
        grid_Docroot->setHorizontalSpacing(10);
        grid_Docroot->setVerticalSpacing(2);
        grid_Docroot->setContentsMargins(6, 2, 6, 2);
        WebProfileDocrootEdit = new QLineEdit(Docroot);
        WebProfileDocrootEdit->setObjectName(QString::fromUtf8("WebProfileDocrootEdit"));
        WebProfileDocrootEdit->setMinimumSize(QSize(0, 26));
        WebProfileDocrootEdit->setMaximumSize(QSize(16777215, 26));

        grid_Docroot->addWidget(WebProfileDocrootEdit, 0, 0, 1, 1);

        browseDocrootButton = new QPushButton(Docroot);
        browseDocrootButton->setObjectName(QString::fromUtf8("browseDocrootButton"));
        browseDocrootButton->setMinimumSize(QSize(0, 26));
        browseDocrootButton->setMaximumSize(QSize(16777215, 26));

        grid_Docroot->addWidget(browseDocrootButton, 0, 0, 1, 1);


        vlay_webserver_Page->addWidget(Docroot);

        Settings->addWidget(webserver_Page);
        security_Page = new QWidget();
        security_Page->setObjectName(QString::fromUtf8("security_Page"));
        security_Page->setMinimumSize(QSize(394, 408));
        security_Page->setMaximumSize(QSize(394, 408));
        security_Page->setAutoFillBackground(true);
        vlay_security_Page = new QVBoxLayout(security_Page);
        vlay_security_Page->setSpacing(0);
        vlay_security_Page->setObjectName(QString::fromUtf8("vlay_security_Page"));
        vlay_security_Page->setContentsMargins(0, 0, 0, 0);
        UserBlocking = new QGroupBox(security_Page);
        UserBlocking->setObjectName(QString::fromUtf8("UserBlocking"));
        sizePolicy2.setHeightForWidth(UserBlocking->sizePolicy().hasHeightForWidth());
        UserBlocking->setSizePolicy(sizePolicy2);
        UserBlocking->setMinimumSize(QSize(0, 86));
        UserBlocking->setMaximumSize(QSize(16777215, 86));
        grid_UserBlocking = new QGridLayout(UserBlocking);
        grid_UserBlocking->setObjectName(QString::fromUtf8("grid_UserBlocking"));
        grid_UserBlocking->setHorizontalSpacing(10);
        grid_UserBlocking->setVerticalSpacing(2);
        grid_UserBlocking->setContentsMargins(6, 2, 6, 2);
        check_BlockNormal = new QRadioButton(UserBlocking);
        check_BlockNormal->setObjectName(QString::fromUtf8("check_BlockNormal"));
        check_BlockNormal->setEnabled(true);
        check_BlockNormal->setAutoExclusive(true);
        check_BlockNormal->setMinimumSize(QSize(0, 26));
        check_BlockNormal->setMaximumSize(QSize(16777215, 26));

        grid_UserBlocking->addWidget(check_BlockNormal, 0, 0, 1, 2, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        check_BlockInvisible = new QRadioButton(UserBlocking);
        check_BlockInvisible->setObjectName(QString::fromUtf8("check_BlockInvisible"));
        check_BlockInvisible->setEnabled(true);
        check_BlockInvisible->setAutoExclusive(true);
        check_BlockInvisible->setMinimumSize(QSize(0, 26));
        check_BlockInvisible->setMaximumSize(QSize(16777215, 26));

        grid_UserBlocking->addWidget(check_BlockInvisible, 1, 0, 1, 2, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);


        vlay_security_Page->addWidget(UserBlocking);

        UserAuth = new QGroupBox(security_Page);
        UserAuth->setObjectName(QString::fromUtf8("UserAuth"));
        sizePolicy2.setHeightForWidth(UserAuth->sizePolicy().hasHeightForWidth());
        UserAuth->setSizePolicy(sizePolicy2);
        UserAuth->setMinimumSize(QSize(0, 58));
        UserAuth->setMaximumSize(QSize(16777215, 58));
        grid_UserAuth = new QGridLayout(UserAuth);
        grid_UserAuth->setObjectName(QString::fromUtf8("grid_UserAuth"));
        grid_UserAuth->setHorizontalSpacing(10);
        grid_UserAuth->setVerticalSpacing(2);
        grid_UserAuth->setContentsMargins(6, 2, 6, 2);
        blockallcheckBox = new QCheckBox(UserAuth);
        blockallcheckBox->setObjectName(QString::fromUtf8("blockallcheckBox"));
        blockallcheckBox->setEnabled(true);
        blockallcheckBox->setMinimumSize(QSize(0, 26));
        blockallcheckBox->setMaximumSize(QSize(16777215, 26));

        grid_UserAuth->addWidget(blockallcheckBox, 0, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);


        vlay_security_Page->addWidget(UserAuth);

        WebPage = new QGroupBox(security_Page);
        WebPage->setObjectName(QString::fromUtf8("WebPage"));
        sizePolicy3.setHeightForWidth(WebPage->sizePolicy().hasHeightForWidth());
        WebPage->setSizePolicy(sizePolicy3);
        WebPage->setMinimumSize(QSize(0, 166));
        grid_WebPage = new QGridLayout(WebPage);
        grid_WebPage->setObjectName(QString::fromUtf8("grid_WebPage"));
        grid_WebPage->setHorizontalSpacing(10);
        grid_WebPage->setVerticalSpacing(2);
        grid_WebPage->setContentsMargins(6, 2, 6, 2);
        WebProfileCheckbox = new QCheckBox(WebPage);
        WebProfileCheckbox->setObjectName(QString::fromUtf8("WebProfileCheckbox"));
        WebProfileCheckbox->setEnabled(true);
        WebProfileCheckbox->setChecked(true);
        WebProfileCheckbox->setMinimumSize(QSize(0, 26));
        WebProfileCheckbox->setMaximumSize(QSize(16777215, 26));

        grid_WebPage->addWidget(WebProfileCheckbox, 0, 0, 1, 2, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        b32address = new QPlainTextEdit(WebPage);
        b32address->setObjectName(QString::fromUtf8("b32address"));
        b32address->setEnabled(true);
        QFont font2;
        font2.setPointSize(8);
        b32address->setFont(font2);
        b32address->setInputMethodHints(Qt::InputMethodHint::ImhNone);
        b32address->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        b32address->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
        b32address->setReadOnly(true);
        b32address->setMinimumSize(QSize(0, 60));
        b32address->setMaximumSize(QSize(16777215, 16777215));
        sizePolicy4.setHeightForWidth(b32address->sizePolicy().hasHeightForWidth());
        b32address->setSizePolicy(sizePolicy4);

        grid_WebPage->addWidget(b32address, 2, 0, 1, 2);

        HideWebCheckbox = new QCheckBox(WebPage);
        HideWebCheckbox->setObjectName(QString::fromUtf8("HideWebCheckbox"));
        HideWebCheckbox->setEnabled(true);
        HideWebCheckbox->setChecked(true);
        HideWebCheckbox->setMinimumSize(QSize(0, 26));
        HideWebCheckbox->setMaximumSize(QSize(16777215, 26));

        grid_WebPage->addWidget(HideWebCheckbox, 1, 0, 1, 2, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);


        vlay_security_Page->addWidget(WebPage);

        Settings->addWidget(security_Page);
        sound_Page = new QWidget();
        sound_Page->setObjectName(QString::fromUtf8("sound_Page"));
        sound_Page->setMinimumSize(QSize(394, 408));
        sound_Page->setMaximumSize(QSize(394, 408));
        sound_Page->setAutoFillBackground(true);
        vlay_sound_Page = new QVBoxLayout(sound_Page);
        vlay_sound_Page->setSpacing(0);
        vlay_sound_Page->setObjectName(QString::fromUtf8("vlay_sound_Page"));
        vlay_sound_Page->setContentsMargins(0, 0, 0, 0);
        SoundConnection = new QGroupBox(sound_Page);
        SoundConnection->setObjectName(QString::fromUtf8("SoundConnection"));
        sizePolicy2.setHeightForWidth(SoundConnection->sizePolicy().hasHeightForWidth());
        SoundConnection->setSizePolicy(sizePolicy2);
        SoundConnection->setMinimumSize(QSize(0, 86));
        SoundConnection->setMaximumSize(QSize(16777215, 86));
        grid_SoundConnection = new QGridLayout(SoundConnection);
        grid_SoundConnection->setObjectName(QString::fromUtf8("grid_SoundConnection"));
        grid_SoundConnection->setHorizontalSpacing(10);
        grid_SoundConnection->setVerticalSpacing(2);
        grid_SoundConnection->setContentsMargins(6, 2, 6, 2);
        checkBoxSound_2 = new QCheckBox(SoundConnection);
        checkBoxSound_2->setObjectName(QString::fromUtf8("checkBoxSound_2"));
        checkBoxSound_2->setEnabled(false);
        checkBoxSound_2->setBaseSize(QSize(90, 0));
        checkBoxSound_2->setMinimumSize(QSize(0, 26));
        checkBoxSound_2->setMaximumSize(QSize(16777215, 26));

        grid_SoundConnection->addWidget(checkBoxSound_2, 1, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        txt_SoundFile2 = new QLineEdit(SoundConnection);
        txt_SoundFile2->setObjectName(QString::fromUtf8("txt_SoundFile2"));
        txt_SoundFile2->setEnabled(false);
        txt_SoundFile2->setMinimumSize(QSize(0, 26));
        txt_SoundFile2->setMaximumSize(QSize(16777215, 26));

        grid_SoundConnection->addWidget(txt_SoundFile2, 1, 1, 1, 1);

        cmd_openFile_2 = new QPushButton(SoundConnection);
        cmd_openFile_2->setObjectName(QString::fromUtf8("cmd_openFile_2"));
        cmd_openFile_2->setMinimumSize(QSize(0, 26));
        cmd_openFile_2->setMaximumSize(QSize(16777215, 26));

        grid_SoundConnection->addWidget(cmd_openFile_2, 1, 2, 1, 1);

        checkBoxSound = new QCheckBox(SoundConnection);
        checkBoxSound->setObjectName(QString::fromUtf8("checkBoxSound"));
        checkBoxSound->setEnabled(false);
        checkBoxSound->setBaseSize(QSize(90, 0));
        checkBoxSound->setMinimumSize(QSize(0, 26));
        checkBoxSound->setMaximumSize(QSize(16777215, 26));

        grid_SoundConnection->addWidget(checkBoxSound, 0, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        txt_SoundFile = new QLineEdit(SoundConnection);
        txt_SoundFile->setObjectName(QString::fromUtf8("txt_SoundFile"));
        txt_SoundFile->setEnabled(false);
        txt_SoundFile->setMinimumSize(QSize(0, 26));
        txt_SoundFile->setMaximumSize(QSize(16777215, 26));

        grid_SoundConnection->addWidget(txt_SoundFile, 0, 1, 1, 1);

        cmd_openFile = new QPushButton(SoundConnection);
        cmd_openFile->setObjectName(QString::fromUtf8("cmd_openFile"));
        cmd_openFile->setMinimumSize(QSize(0, 26));
        cmd_openFile->setMaximumSize(QSize(16777215, 26));

        grid_SoundConnection->addWidget(cmd_openFile, 0, 2, 1, 1);


        vlay_sound_Page->addWidget(SoundConnection);

        SoundFilesend = new QGroupBox(sound_Page);
        SoundFilesend->setObjectName(QString::fromUtf8("SoundFilesend"));
        sizePolicy2.setHeightForWidth(SoundFilesend->sizePolicy().hasHeightForWidth());
        SoundFilesend->setSizePolicy(sizePolicy2);
        SoundFilesend->setMinimumSize(QSize(0, 58));
        SoundFilesend->setMaximumSize(QSize(16777215, 58));
        grid_SoundFilesend = new QGridLayout(SoundFilesend);
        grid_SoundFilesend->setObjectName(QString::fromUtf8("grid_SoundFilesend"));
        grid_SoundFilesend->setHorizontalSpacing(10);
        grid_SoundFilesend->setVerticalSpacing(2);
        grid_SoundFilesend->setContentsMargins(6, 2, 6, 2);
        checkBoxSound_3 = new QCheckBox(SoundFilesend);
        checkBoxSound_3->setObjectName(QString::fromUtf8("checkBoxSound_3"));
        checkBoxSound_3->setEnabled(false);
        checkBoxSound_3->setBaseSize(QSize(90, 0));
        checkBoxSound_3->setMinimumSize(QSize(0, 26));
        checkBoxSound_3->setMaximumSize(QSize(16777215, 26));

        grid_SoundFilesend->addWidget(checkBoxSound_3, 0, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        txt_SoundFile3 = new QLineEdit(SoundFilesend);
        txt_SoundFile3->setObjectName(QString::fromUtf8("txt_SoundFile3"));
        txt_SoundFile3->setEnabled(false);
        txt_SoundFile3->setMinimumSize(QSize(0, 26));
        txt_SoundFile3->setMaximumSize(QSize(16777215, 26));

        grid_SoundFilesend->addWidget(txt_SoundFile3, 0, 1, 1, 1);

        cmd_openFile_3 = new QPushButton(SoundFilesend);
        cmd_openFile_3->setObjectName(QString::fromUtf8("cmd_openFile_3"));
        cmd_openFile_3->setMinimumSize(QSize(0, 26));
        cmd_openFile_3->setMaximumSize(QSize(16777215, 26));

        grid_SoundFilesend->addWidget(cmd_openFile_3, 0, 2, 1, 1);


        vlay_sound_Page->addWidget(SoundFilesend);

        SoundFileReceive = new QGroupBox(sound_Page);
        SoundFileReceive->setObjectName(QString::fromUtf8("SoundFileReceive"));
        sizePolicy2.setHeightForWidth(SoundFileReceive->sizePolicy().hasHeightForWidth());
        SoundFileReceive->setSizePolicy(sizePolicy2);
        SoundFileReceive->setMinimumSize(QSize(0, 86));
        SoundFileReceive->setMaximumSize(QSize(16777215, 86));
        grid_SoundFileReceive = new QGridLayout(SoundFileReceive);
        grid_SoundFileReceive->setObjectName(QString::fromUtf8("grid_SoundFileReceive"));
        grid_SoundFileReceive->setHorizontalSpacing(10);
        grid_SoundFileReceive->setVerticalSpacing(2);
        grid_SoundFileReceive->setContentsMargins(6, 2, 6, 2);
        checkBoxSound_5 = new QCheckBox(SoundFileReceive);
        checkBoxSound_5->setObjectName(QString::fromUtf8("checkBoxSound_5"));
        checkBoxSound_5->setEnabled(false);
        checkBoxSound_5->setBaseSize(QSize(90, 0));
        checkBoxSound_5->setMinimumSize(QSize(0, 26));
        checkBoxSound_5->setMaximumSize(QSize(16777215, 26));

        grid_SoundFileReceive->addWidget(checkBoxSound_5, 1, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        txt_SoundFile5 = new QLineEdit(SoundFileReceive);
        txt_SoundFile5->setObjectName(QString::fromUtf8("txt_SoundFile5"));
        txt_SoundFile5->setEnabled(false);
        txt_SoundFile5->setMinimumSize(QSize(0, 26));
        txt_SoundFile5->setMaximumSize(QSize(16777215, 26));

        grid_SoundFileReceive->addWidget(txt_SoundFile5, 1, 1, 1, 1);

        cmd_openFile_5 = new QPushButton(SoundFileReceive);
        cmd_openFile_5->setObjectName(QString::fromUtf8("cmd_openFile_5"));
        cmd_openFile_5->setMinimumSize(QSize(0, 26));
        cmd_openFile_5->setMaximumSize(QSize(16777215, 26));

        grid_SoundFileReceive->addWidget(cmd_openFile_5, 1, 2, 1, 1);

        checkBoxSound_4 = new QCheckBox(SoundFileReceive);
        checkBoxSound_4->setObjectName(QString::fromUtf8("checkBoxSound_4"));
        checkBoxSound_4->setEnabled(false);
        checkBoxSound_4->setBaseSize(QSize(90, 0));
        checkBoxSound_4->setMinimumSize(QSize(0, 26));
        checkBoxSound_4->setMaximumSize(QSize(16777215, 26));

        grid_SoundFileReceive->addWidget(checkBoxSound_4, 0, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        txt_SoundFile4 = new QLineEdit(SoundFileReceive);
        txt_SoundFile4->setObjectName(QString::fromUtf8("txt_SoundFile4"));
        txt_SoundFile4->setEnabled(false);
        txt_SoundFile4->setMinimumSize(QSize(0, 26));
        txt_SoundFile4->setMaximumSize(QSize(16777215, 26));

        grid_SoundFileReceive->addWidget(txt_SoundFile4, 0, 1, 1, 1);

        cmd_openFile_4 = new QPushButton(SoundFileReceive);
        cmd_openFile_4->setObjectName(QString::fromUtf8("cmd_openFile_4"));
        cmd_openFile_4->setMinimumSize(QSize(0, 26));
        cmd_openFile_4->setMaximumSize(QSize(16777215, 26));

        grid_SoundFileReceive->addWidget(cmd_openFile_4, 0, 2, 1, 1);


        vlay_sound_Page->addWidget(SoundFileReceive);

        SoundChatMessage = new QGroupBox(sound_Page);
        SoundChatMessage->setObjectName(QString::fromUtf8("SoundChatMessage"));
        sizePolicy2.setHeightForWidth(SoundChatMessage->sizePolicy().hasHeightForWidth());
        SoundChatMessage->setSizePolicy(sizePolicy2);
        SoundChatMessage->setMinimumSize(QSize(0, 58));
        SoundChatMessage->setMaximumSize(QSize(16777215, 58));
        grid_SoundChatMessage = new QGridLayout(SoundChatMessage);
        grid_SoundChatMessage->setObjectName(QString::fromUtf8("grid_SoundChatMessage"));
        grid_SoundChatMessage->setHorizontalSpacing(10);
        grid_SoundChatMessage->setVerticalSpacing(2);
        grid_SoundChatMessage->setContentsMargins(6, 2, 6, 2);
        checkBoxSound_6 = new QCheckBox(SoundChatMessage);
        checkBoxSound_6->setObjectName(QString::fromUtf8("checkBoxSound_6"));
        checkBoxSound_6->setEnabled(false);
        checkBoxSound_6->setBaseSize(QSize(90, 0));
        checkBoxSound_6->setMinimumSize(QSize(0, 26));
        checkBoxSound_6->setMaximumSize(QSize(16777215, 26));

        grid_SoundChatMessage->addWidget(checkBoxSound_6, 0, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        txt_SoundFile6 = new QLineEdit(SoundChatMessage);
        txt_SoundFile6->setObjectName(QString::fromUtf8("txt_SoundFile6"));
        txt_SoundFile6->setEnabled(false);
        txt_SoundFile6->setMinimumSize(QSize(0, 26));
        txt_SoundFile6->setMaximumSize(QSize(16777215, 26));

        grid_SoundChatMessage->addWidget(txt_SoundFile6, 0, 1, 1, 1);

        cmd_openFile_6 = new QPushButton(SoundChatMessage);
        cmd_openFile_6->setObjectName(QString::fromUtf8("cmd_openFile_6"));
        cmd_openFile_6->setMinimumSize(QSize(0, 26));
        cmd_openFile_6->setMaximumSize(QSize(16777215, 26));

        grid_SoundChatMessage->addWidget(cmd_openFile_6, 0, 2, 1, 1);


        vlay_sound_Page->addWidget(SoundChatMessage);

        Settings->addWidget(sound_Page);
        chat_Page = new QWidget();
        chat_Page->setObjectName(QString::fromUtf8("chat_Page"));
        chat_Page->setMinimumSize(QSize(394, 408));
        chat_Page->setMaximumSize(QSize(394, 408));
        chat_Page->setAutoFillBackground(true);
        vlay_chat_Page = new QVBoxLayout(chat_Page);
        vlay_chat_Page->setSpacing(0);
        vlay_chat_Page->setObjectName(QString::fromUtf8("vlay_chat_Page"));
        vlay_chat_Page->setContentsMargins(0, 0, 0, 0);
        frame_18 = new QFrame(chat_Page);
        frame_18->setObjectName(QString::fromUtf8("frame_18"));
        frame_18->setFrameShape(QFrame::Shape::NoFrame);
        frame_18->setFrameShadow(QFrame::Shadow::Plain);
        vlay_frame18 = new QVBoxLayout(frame_18);
        vlay_frame18->setObjectName(QString::fromUtf8("vlay_frame18"));
        LocalStyle = new QGroupBox(frame_18);
        LocalStyle->setObjectName(QString::fromUtf8("LocalStyle"));
        LocalStyle->setBaseSize(QSize(0, 120));
        LocalStyle->setFont(font1);
        sizePolicy3.setHeightForWidth(LocalStyle->sizePolicy().hasHeightForWidth());
        LocalStyle->setSizePolicy(sizePolicy3);
        LocalStyle->setMinimumSize(QSize(0, 138));
        grid_LocalStyle = new QGridLayout(LocalStyle);
        grid_LocalStyle->setObjectName(QString::fromUtf8("grid_LocalStyle"));
        grid_LocalStyle->setHorizontalSpacing(10);
        grid_LocalStyle->setVerticalSpacing(2);
        grid_LocalStyle->setContentsMargins(6, 2, 6, 2);
        txtShowCurrentChatStyle = new QTextEdit(LocalStyle);
        txtShowCurrentChatStyle->setObjectName(QString::fromUtf8("txtShowCurrentChatStyle"));
        txtShowCurrentChatStyle->setEnabled(false);
        txtShowCurrentChatStyle->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        txtShowCurrentChatStyle->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        txtShowCurrentChatStyle->setAcceptDrops(false);
        txtShowCurrentChatStyle->setMinimumSize(QSize(0, 60));
        txtShowCurrentChatStyle->setMaximumSize(QSize(16777215, 16777215));
        sizePolicy4.setHeightForWidth(txtShowCurrentChatStyle->sizePolicy().hasHeightForWidth());
        txtShowCurrentChatStyle->setSizePolicy(sizePolicy4);

        grid_LocalStyle->addWidget(txtShowCurrentChatStyle, 1, 0, 1, 6);

        cmdBoldChat = new QToolButton(LocalStyle);
        cmdBoldChat->setObjectName(QString::fromUtf8("cmdBoldChat"));
        cmdBoldChat->setEnabled(true);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/bold.svg"), QSize(), QIcon::Normal, QIcon::Off);
        cmdBoldChat->setIcon(icon1);
        cmdBoldChat->setIconSize(QSize(22, 22));
        cmdBoldChat->setCheckable(true);
        cmdBoldChat->setAutoRaise(true);
        cmdBoldChat->setMinimumSize(QSize(0, 26));
        cmdBoldChat->setMaximumSize(QSize(16777215, 26));

        grid_LocalStyle->addWidget(cmdBoldChat, 0, 1, 1, 1);

        cmdUnderChat = new QToolButton(LocalStyle);
        cmdUnderChat->setObjectName(QString::fromUtf8("cmdUnderChat"));
        cmdUnderChat->setEnabled(true);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/underline.svg"), QSize(), QIcon::Normal, QIcon::Off);
        cmdUnderChat->setIcon(icon2);
        cmdUnderChat->setIconSize(QSize(22, 22));
        cmdUnderChat->setCheckable(true);
        cmdUnderChat->setAutoRaise(true);
        cmdUnderChat->setMinimumSize(QSize(0, 26));
        cmdUnderChat->setMaximumSize(QSize(16777215, 26));

        grid_LocalStyle->addWidget(cmdUnderChat, 0, 2, 1, 1);

        cmdItalicChat = new QToolButton(LocalStyle);
        cmdItalicChat->setObjectName(QString::fromUtf8("cmdItalicChat"));
        cmdItalicChat->setEnabled(true);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/italic.svg"), QSize(), QIcon::Normal, QIcon::Off);
        cmdItalicChat->setIcon(icon3);
        cmdItalicChat->setIconSize(QSize(22, 22));
        cmdItalicChat->setCheckable(true);
        cmdItalicChat->setAutoRaise(true);
        cmdItalicChat->setMinimumSize(QSize(0, 26));
        cmdItalicChat->setMaximumSize(QSize(16777215, 26));

        grid_LocalStyle->addWidget(cmdItalicChat, 0, 3, 1, 1);

        cmdColorChat = new QToolButton(LocalStyle);
        cmdColorChat->setObjectName(QString::fromUtf8("cmdColorChat"));
        cmdColorChat->setEnabled(true);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/palette.svg"), QSize(), QIcon::Normal, QIcon::Off);
        cmdColorChat->setIcon(icon4);
        cmdColorChat->setIconSize(QSize(24, 24));
        cmdColorChat->setAutoRaise(true);
        cmdColorChat->setMinimumSize(QSize(0, 26));
        cmdColorChat->setMaximumSize(QSize(16777215, 26));

        grid_LocalStyle->addWidget(cmdColorChat, 0, 4, 1, 1);

        cmdFontChat = new QToolButton(LocalStyle);
        cmdFontChat->setObjectName(QString::fromUtf8("cmdFontChat"));
        cmdFontChat->setEnabled(true);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icons/font.svg"), QSize(), QIcon::Normal, QIcon::Off);
        cmdFontChat->setIcon(icon5);
        cmdFontChat->setIconSize(QSize(22, 22));
        cmdFontChat->setAutoRaise(true);
        cmdFontChat->setMinimumSize(QSize(0, 26));
        cmdFontChat->setMaximumSize(QSize(16777215, 26));

        grid_LocalStyle->addWidget(cmdFontChat, 0, 5, 1, 1);

        label_4 = new QLabel(LocalStyle);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setMinimumSize(QSize(0, 26));
        label_4->setMaximumSize(QSize(16777215, 26));

        grid_LocalStyle->addWidget(label_4, 0, 0, 1, 1);


        vlay_frame18->addWidget(LocalStyle);

        RemoteStyle = new QGroupBox(frame_18);
        RemoteStyle->setObjectName(QString::fromUtf8("RemoteStyle"));
        RemoteStyle->setBaseSize(QSize(378, 120));
        RemoteStyle->setFont(font1);
        sizePolicy3.setHeightForWidth(RemoteStyle->sizePolicy().hasHeightForWidth());
        RemoteStyle->setSizePolicy(sizePolicy3);
        RemoteStyle->setMinimumSize(QSize(0, 166));
        grid_RemoteStyle = new QGridLayout(RemoteStyle);
        grid_RemoteStyle->setObjectName(QString::fromUtf8("grid_RemoteStyle"));
        grid_RemoteStyle->setHorizontalSpacing(10);
        grid_RemoteStyle->setVerticalSpacing(2);
        grid_RemoteStyle->setContentsMargins(6, 2, 6, 2);
        chatOverrideBox = new QCheckBox(RemoteStyle);
        chatOverrideBox->setObjectName(QString::fromUtf8("chatOverrideBox"));
        chatOverrideBox->setMinimumSize(QSize(0, 26));
        chatOverrideBox->setMaximumSize(QSize(16777215, 26));

        grid_RemoteStyle->addWidget(chatOverrideBox, 0, 0, 1, 6, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        txtOverrideRemote = new QTextEdit(RemoteStyle);
        txtOverrideRemote->setObjectName(QString::fromUtf8("txtOverrideRemote"));
        txtOverrideRemote->setEnabled(false);
        txtOverrideRemote->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        txtOverrideRemote->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        txtOverrideRemote->setAcceptDrops(false);
        txtOverrideRemote->setAutoFormatting(QTextEdit::AutoFormattingFlag::AutoNone);
        txtOverrideRemote->setMinimumSize(QSize(0, 60));
        txtOverrideRemote->setMaximumSize(QSize(16777215, 16777215));
        sizePolicy4.setHeightForWidth(txtOverrideRemote->sizePolicy().hasHeightForWidth());
        txtOverrideRemote->setSizePolicy(sizePolicy4);

        grid_RemoteStyle->addWidget(txtOverrideRemote, 1, 0, 1, 6);

        cmdOverwriteBoldChat = new QToolButton(RemoteStyle);
        cmdOverwriteBoldChat->setObjectName(QString::fromUtf8("cmdOverwriteBoldChat"));
        cmdOverwriteBoldChat->setEnabled(true);
        cmdOverwriteBoldChat->setIcon(icon1);
        cmdOverwriteBoldChat->setIconSize(QSize(22, 22));
        cmdOverwriteBoldChat->setCheckable(true);
        cmdOverwriteBoldChat->setAutoRaise(true);
        cmdOverwriteBoldChat->setMinimumSize(QSize(0, 26));
        cmdOverwriteBoldChat->setMaximumSize(QSize(16777215, 26));

        grid_RemoteStyle->addWidget(cmdOverwriteBoldChat, 2, 0, 1, 1);

        cmdOverwriteUnderChat = new QToolButton(RemoteStyle);
        cmdOverwriteUnderChat->setObjectName(QString::fromUtf8("cmdOverwriteUnderChat"));
        cmdOverwriteUnderChat->setEnabled(true);
        cmdOverwriteUnderChat->setIcon(icon2);
        cmdOverwriteUnderChat->setIconSize(QSize(22, 22));
        cmdOverwriteUnderChat->setCheckable(true);
        cmdOverwriteUnderChat->setAutoRaise(true);
        cmdOverwriteUnderChat->setMinimumSize(QSize(0, 26));
        cmdOverwriteUnderChat->setMaximumSize(QSize(16777215, 26));

        grid_RemoteStyle->addWidget(cmdOverwriteUnderChat, 2, 1, 1, 1);

        cmdOverwriteItalicChat = new QToolButton(RemoteStyle);
        cmdOverwriteItalicChat->setObjectName(QString::fromUtf8("cmdOverwriteItalicChat"));
        cmdOverwriteItalicChat->setEnabled(true);
        cmdOverwriteItalicChat->setIcon(icon3);
        cmdOverwriteItalicChat->setIconSize(QSize(22, 22));
        cmdOverwriteItalicChat->setCheckable(true);
        cmdOverwriteItalicChat->setAutoRaise(true);
        cmdOverwriteItalicChat->setMinimumSize(QSize(0, 26));
        cmdOverwriteItalicChat->setMaximumSize(QSize(16777215, 26));

        grid_RemoteStyle->addWidget(cmdOverwriteItalicChat, 2, 2, 1, 1);

        cmdOverwriteColorChat = new QToolButton(RemoteStyle);
        cmdOverwriteColorChat->setObjectName(QString::fromUtf8("cmdOverwriteColorChat"));
        cmdOverwriteColorChat->setEnabled(true);
        cmdOverwriteColorChat->setIcon(icon4);
        cmdOverwriteColorChat->setIconSize(QSize(24, 24));
        cmdOverwriteColorChat->setAutoRaise(true);
        cmdOverwriteColorChat->setMinimumSize(QSize(0, 26));
        cmdOverwriteColorChat->setMaximumSize(QSize(16777215, 26));

        grid_RemoteStyle->addWidget(cmdOverwriteColorChat, 2, 3, 1, 1);

        cmdOverwriteFontChat = new QToolButton(RemoteStyle);
        cmdOverwriteFontChat->setObjectName(QString::fromUtf8("cmdOverwriteFontChat"));
        cmdOverwriteFontChat->setEnabled(true);
        cmdOverwriteFontChat->setIcon(icon5);
        cmdOverwriteFontChat->setIconSize(QSize(22, 22));
        cmdOverwriteFontChat->setAutoRaise(true);
        cmdOverwriteFontChat->setMinimumSize(QSize(0, 26));
        cmdOverwriteFontChat->setMaximumSize(QSize(16777215, 26));

        grid_RemoteStyle->addWidget(cmdOverwriteFontChat, 2, 4, 1, 1);


        vlay_frame18->addWidget(RemoteStyle);

        Emoticons = new QGroupBox(frame_18);
        Emoticons->setObjectName(QString::fromUtf8("Emoticons"));
        sizePolicy2.setHeightForWidth(Emoticons->sizePolicy().hasHeightForWidth());
        Emoticons->setSizePolicy(sizePolicy2);
        Emoticons->setMinimumSize(QSize(0, 58));
        Emoticons->setMaximumSize(QSize(16777215, 58));
        grid_Emoticons = new QGridLayout(Emoticons);
        grid_Emoticons->setObjectName(QString::fromUtf8("grid_Emoticons"));
        grid_Emoticons->setHorizontalSpacing(10);
        grid_Emoticons->setVerticalSpacing(2);
        grid_Emoticons->setContentsMargins(6, 2, 6, 2);
        checkBoxEmoticons = new QCheckBox(Emoticons);
        checkBoxEmoticons->setObjectName(QString::fromUtf8("checkBoxEmoticons"));
        checkBoxEmoticons->setEnabled(false);
        checkBoxEmoticons->setChecked(true);
        checkBoxEmoticons->setMinimumSize(QSize(0, 26));
        checkBoxEmoticons->setMaximumSize(QSize(16777215, 26));

        grid_Emoticons->addWidget(checkBoxEmoticons, 0, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);


        vlay_frame18->addWidget(Emoticons);

        groupChatStyle = new QGroupBox(frame_18);
        groupChatStyle->setObjectName(QString::fromUtf8("groupChatStyle"));
        sizePolicy2.setHeightForWidth(groupChatStyle->sizePolicy().hasHeightForWidth());
        groupChatStyle->setSizePolicy(sizePolicy2);
        groupChatStyle->setMinimumSize(QSize(0, 58));
        groupChatStyle->setMaximumSize(QSize(16777215, 58));
        grid_groupChatStyle = new QGridLayout(groupChatStyle);
        grid_groupChatStyle->setObjectName(QString::fromUtf8("grid_groupChatStyle"));
        grid_groupChatStyle->setHorizontalSpacing(10);
        grid_groupChatStyle->setVerticalSpacing(2);
        grid_groupChatStyle->setContentsMargins(6, 2, 6, 2);
        labelChatStyle = new QLabel(groupChatStyle);
        labelChatStyle->setObjectName(QString::fromUtf8("labelChatStyle"));
        labelChatStyle->setMinimumSize(QSize(0, 26));
        labelChatStyle->setMaximumSize(QSize(16777215, 26));

        grid_groupChatStyle->addWidget(labelChatStyle, 0, 0, 1, 1);

        comboBoxChatStyle = new QComboBox(groupChatStyle);
        comboBoxChatStyle->setObjectName(QString::fromUtf8("comboBoxChatStyle"));
        comboBoxChatStyle->setMinimumSize(QSize(0, 26));
        comboBoxChatStyle->setMaximumSize(QSize(16777215, 26));

        grid_groupChatStyle->addWidget(comboBoxChatStyle, 0, 1, 1, 1);


        vlay_frame18->addWidget(groupChatStyle);

        checkBox_DisplayImagesInline = new QCheckBox(frame_18);
        checkBox_DisplayImagesInline->setObjectName(QString::fromUtf8("checkBox_DisplayImagesInline"));
        checkBox_DisplayImagesInline->setMinimumSize(QSize(0, 26));
        checkBox_DisplayImagesInline->setMaximumSize(QSize(16777215, 26));

        vlay_frame18->addWidget(checkBox_DisplayImagesInline);

        UserEvents = new QGroupBox(frame_18);
        UserEvents->setObjectName(QString::fromUtf8("UserEvents"));
        sizePolicy2.setHeightForWidth(UserEvents->sizePolicy().hasHeightForWidth());
        UserEvents->setSizePolicy(sizePolicy2);
        UserEvents->setMinimumSize(QSize(0, 58));
        UserEvents->setMaximumSize(QSize(16777215, 58));
        grid_UserEvents = new QGridLayout(UserEvents);
        grid_UserEvents->setObjectName(QString::fromUtf8("grid_UserEvents"));
        grid_UserEvents->setHorizontalSpacing(10);
        grid_UserEvents->setVerticalSpacing(2);
        grid_UserEvents->setContentsMargins(6, 2, 6, 2);
        checkboxUserEvents = new QCheckBox(UserEvents);
        checkboxUserEvents->setObjectName(QString::fromUtf8("checkboxUserEvents"));
        checkboxUserEvents->setFont(font1);
        checkboxUserEvents->setMinimumSize(QSize(0, 26));
        checkboxUserEvents->setMaximumSize(QSize(16777215, 26));

        grid_UserEvents->addWidget(checkboxUserEvents, 0, 0, 1, 1, Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);


        vlay_frame18->addWidget(UserEvents);

        groupBox = new QGroupBox(frame_18);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setEnabled(false);
        sizePolicy2.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy2);
        groupBox->setMinimumSize(QSize(0, 58));
        groupBox->setMaximumSize(QSize(16777215, 58));
        grid_groupBox = new QGridLayout(groupBox);
        grid_groupBox->setObjectName(QString::fromUtf8("grid_groupBox"));
        grid_groupBox->setHorizontalSpacing(10);
        grid_groupBox->setVerticalSpacing(2);
        grid_groupBox->setContentsMargins(6, 2, 6, 2);
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setEnabled(false);
        label_6->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        label_6->setMinimumSize(QSize(0, 26));
        label_6->setMaximumSize(QSize(16777215, 26));

        grid_groupBox->addWidget(label_6, 0, 0, 1, 1);

        spinBox_maxACK = new QSpinBox(groupBox);
        spinBox_maxACK->setObjectName(QString::fromUtf8("spinBox_maxACK"));
        spinBox_maxACK->setEnabled(false);
        spinBox_maxACK->setMinimum(0);
        spinBox_maxACK->setMaximum(180);
        spinBox_maxACK->setValue(60);
        spinBox_maxACK->setMinimumSize(QSize(0, 26));
        spinBox_maxACK->setMaximumSize(QSize(16777215, 26));

        grid_groupBox->addWidget(spinBox_maxACK, 0, 1, 1, 1);


        vlay_frame18->addWidget(groupBox);


        vlay_chat_Page->addWidget(frame_18);

        Settings->addWidget(chat_Page);
        blocklist_page = new QWidget();
        blocklist_page->setObjectName(QString::fromUtf8("blocklist_page"));
        blocklist_page->setMinimumSize(QSize(394, 408));
        blocklist_page->setMaximumSize(QSize(394, 408));
        blocklist_page->setAutoFillBackground(true);
        vlay_blocklist = new QVBoxLayout(blocklist_page);
        vlay_blocklist->setSpacing(4);
        vlay_blocklist->setObjectName(QString::fromUtf8("vlay_blocklist"));
        vlay_blocklist->setContentsMargins(4, 2, 4, 2);
        UserBlockTreeWidget = new QTreeWidget(blocklist_page);
        UserBlockTreeWidget->setObjectName(QString::fromUtf8("UserBlockTreeWidget"));
        UserBlockTreeWidget->setColumnCount(2);

        vlay_blocklist->addWidget(UserBlockTreeWidget);

        hlay_blocklist_buttons = new QHBoxLayout();
        hlay_blocklist_buttons->setSpacing(4);
        hlay_blocklist_buttons->setObjectName(QString::fromUtf8("hlay_blocklist_buttons"));
        blocklist_spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hlay_blocklist_buttons->addItem(blocklist_spacer);

        cmd_unblockUserFromBlockList = new QPushButton(blocklist_page);
        cmd_unblockUserFromBlockList->setObjectName(QString::fromUtf8("cmd_unblockUserFromBlockList"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/emoticons/good.svg"), QSize(), QIcon::Normal, QIcon::Off);
        cmd_unblockUserFromBlockList->setIcon(icon6);
        cmd_unblockUserFromBlockList->setMinimumSize(QSize(80, 26));
        cmd_unblockUserFromBlockList->setMaximumSize(QSize(80, 26));

        hlay_blocklist_buttons->addWidget(cmd_unblockUserFromBlockList);

        cmd_deleteUserFromBlockList = new QPushButton(blocklist_page);
        cmd_deleteUserFromBlockList->setObjectName(QString::fromUtf8("cmd_deleteUserFromBlockList"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/icons/delete.svg"), QSize(), QIcon::Normal, QIcon::Off);
        cmd_deleteUserFromBlockList->setIcon(icon7);
        cmd_deleteUserFromBlockList->setIconSize(QSize(14, 14));
        cmd_deleteUserFromBlockList->setMinimumSize(QSize(80, 26));
        cmd_deleteUserFromBlockList->setMaximumSize(QSize(80, 26));

        hlay_blocklist_buttons->addWidget(cmd_deleteUserFromBlockList);


        vlay_blocklist->addLayout(hlay_blocklist_buttons);

        Settings->addWidget(blocklist_page);
        Sections = new QListWidget(SettingsContainer);
        QFont font3;
        font3.setBold(false);
        QListWidgetItem *__qlistwidgetitem = new QListWidgetItem(Sections);
        __qlistwidgetitem->setFont(font3);
        __qlistwidgetitem->setIcon(icon);
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/icons/network.svg"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem1 = new QListWidgetItem(Sections);
        __qlistwidgetitem1->setIcon(icon8);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/icons/avatar.svg"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem2 = new QListWidgetItem(Sections);
        __qlistwidgetitem2->setIcon(icon9);
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/icons/theme.svg"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem3 = new QListWidgetItem(Sections);
        __qlistwidgetitem3->setIcon(icon10);
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/icons/webserver.svg"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem4 = new QListWidgetItem(Sections);
        __qlistwidgetitem4->setIcon(icon11);
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/icons/security.svg"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem5 = new QListWidgetItem(Sections);
        __qlistwidgetitem5->setIcon(icon12);
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/icons/sound_on.svg"), QSize(), QIcon::Normal, QIcon::Off);
        icon13.addFile(QString::fromUtf8(":/icons/sound_off.svg"), QSize(), QIcon::Disabled, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem6 = new QListWidgetItem(Sections);
        __qlistwidgetitem6->setIcon(icon13);
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/icons/newmail.svg"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem7 = new QListWidgetItem(Sections);
        __qlistwidgetitem7->setIcon(icon14);
        QIcon icon15;
        QString iconThemeName = QString::fromUtf8("blocked.svg");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon15 = QIcon::fromTheme(iconThemeName);
        } else {
            icon15.addFile(QString::fromUtf8(":/icons/blocked.svg"), QSize(), QIcon::Normal, QIcon::Off);
        }
        QListWidgetItem *__qlistwidgetitem8 = new QListWidgetItem(Sections);
        __qlistwidgetitem8->setIcon(icon15);
        Sections->setObjectName(QString::fromUtf8("Sections"));
        Sections->setGeometry(QRect(2, 0, 129, 398));
        sizePolicy1.setHeightForWidth(Sections->sizePolicy().hasHeightForWidth());
        Sections->setSizePolicy(sizePolicy1);
        Sections->setMinimumSize(QSize(0, 398));
        Sections->setMaximumSize(QSize(132, 398));
        QFont font4;
        font4.setFamily(QString::fromUtf8("SansSerif"));
        font4.setPointSize(9);
        font4.setBold(false);
        Sections->setFont(font4);
        Sections->setAcceptDrops(false);
        Sections->setFrameShape(QFrame::Shape::StyledPanel);
        Sections->setFrameShadow(QFrame::Shadow::Plain);
        Sections->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        Sections->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        Sections->setProperty("showDropIndicator", QVariant(false));
        Sections->setIconSize(QSize(22, 22));
        Sections->setFlow(QListView::Flow::TopToBottom);
        Sections->setResizeMode(QListView::ResizeMode::Fixed);
        Sections->setLayoutMode(QListView::LayoutMode::SinglePass);
        Sections->setSpacing(4);
        Sections->setGridSize(QSize(0, 30));
        Sections->setSelectionRectVisible(true);
        Sections->setSortingEnabled(false);
        line_2 = new QFrame(SettingsContainer);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(2, 396, 519, 16));
        line_2->setMinimumSize(QSize(519, 0));
        line_2->setMaximumSize(QSize(519, 16777215));
        line_2->setFrameShadow(QFrame::Shadow::Raised);
        line_2->setFrameShape(QFrame::HLine);
        buttonFrame = new QFrame(SettingsContainer);
        buttonFrame->setObjectName(QString::fromUtf8("buttonFrame"));
        buttonFrame->setGeometry(QRect(352, 400, 179, 43));
        sizePolicy1.setHeightForWidth(buttonFrame->sizePolicy().hasHeightForWidth());
        buttonFrame->setSizePolicy(sizePolicy1);
        buttonFrame->setMinimumSize(QSize(0, 43));
        buttonFrame->setFrameShape(QFrame::Shape::NoFrame);
        buttonFrame->setFrameShadow(QFrame::Shadow::Raised);
        cancel_Button = new QPushButton(buttonFrame);
        cancel_Button->setObjectName(QString::fromUtf8("cancel_Button"));
        cancel_Button->setGeometry(QRect(11, 12, 75, 26));
        cancel_Button->setMinimumSize(QSize(0, 26));
        cancel_Button->setMaximumSize(QSize(92, 26));
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/icons/cancel.svg"), QSize(), QIcon::Normal, QIcon::Off);
        cancel_Button->setIcon(icon16);
        cancel_Button->setIconSize(QSize(14, 14));
        ok_Button = new QPushButton(buttonFrame);
        ok_Button->setObjectName(QString::fromUtf8("ok_Button"));
        ok_Button->setGeometry(QRect(94, 12, 75, 26));
        ok_Button->setMinimumSize(QSize(0, 26));
        ok_Button->setMaximumSize(QSize(92, 26));
        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/icons/tick.svg"), QSize(), QIcon::Normal, QIcon::Off);
        ok_Button->setIcon(icon17);
        ok_Button->setIconSize(QSize(14, 14));
        ok_Button->setAutoRepeat(false);
        Sections->raise();
        buttonFrame->raise();
        Settings->raise();
        line_2->raise();

        retranslateUi(SettingsGui);
        QObject::connect(checkBox_AutoAcceptFiles, SIGNAL(clicked(bool)), cmd_Downloads, SLOT(setEnabled(bool)));
        QObject::connect(Sections, SIGNAL(currentRowChanged(int)), Settings, SLOT(setCurrentIndex(int)));

        ok_Button->setDefault(true);


        QMetaObject::connectSlotsByName(SettingsGui);
    } // setupUi

    void retranslateUi(QDialog *SettingsGui)
    {
        SettingsGui->setWindowTitle(QCoreApplication::translate("SettingsGui", "Settings", nullptr));
        FileXfer->setTitle(QCoreApplication::translate("SettingsGui", "File Transfer", nullptr));
        checkBox_AutoAcceptFiles->setText(QCoreApplication::translate("SettingsGui", "Auto-accept file transfers", nullptr));
        label_autoDL->setText(QCoreApplication::translate("SettingsGui", "Auto-download location:", nullptr));
        cmd_Downloads->setText(QCoreApplication::translate("SettingsGui", "...", nullptr));
        checkBox_Subfolders->setText(QCoreApplication::translate("SettingsGui", "Create sub-folder per contact", nullptr));
        UserCheck->setTitle(QCoreApplication::translate("SettingsGui", "Offline Check", nullptr));
        label_offlinechk->setText(QCoreApplication::translate("SettingsGui", "Offline user check (seconds):", nullptr));
        AutoAway->setTitle(QCoreApplication::translate("SettingsGui", "Auto-Away", nullptr));
#if QT_CONFIG(tooltip)
        AutoAwaySpinBox->setToolTip(QCoreApplication::translate("SettingsGui", "Set to 0 to disable", nullptr));
#endif // QT_CONFIG(tooltip)
        NoActivityLabel->setText(QCoreApplication::translate("SettingsGui", "Auto-away after (minutes):", nullptr));
        SAMLog->setTitle(QCoreApplication::translate("SettingsGui", "SAM Log", nullptr));
        MaxLogLabel->setText(QCoreApplication::translate("SettingsGui", "Max SAM log messages:", nullptr));
        Debugging->setTitle(QCoreApplication::translate("SettingsGui", "Debug", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_DebugLog->setToolTip(QCoreApplication::translate("SettingsGui", "File will be written to your profile folder", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_DebugLog->setText(QCoreApplication::translate("SettingsGui", "Save debug info to DebugLog.txt ", nullptr));
        SAM->setTitle(QCoreApplication::translate("SettingsGui", "SAM Bridge", nullptr));
        label_17->setText(QCoreApplication::translate("SettingsGui", "SAM Host IP:", nullptr));
        label_18->setText(QCoreApplication::translate("SettingsGui", "Port:", nullptr));
        SigType->setTitle(QCoreApplication::translate("SettingsGui", "Signature Type", nullptr));
        requestAuthcheckBox->setText(QCoreApplication::translate("SettingsGui", "Request Authorization", nullptr));
        label_26->setText(QCoreApplication::translate("SettingsGui", "New Destination:", nullptr));
        cmd_DestGenerate->setText(QCoreApplication::translate("SettingsGui", "Generate", nullptr));
        nonpersistdest->setText(QCoreApplication::translate("SettingsGui", "Non-persistent Destination", nullptr));
        Crypto->setTitle(QCoreApplication::translate("SettingsGui", "Encryption", nullptr));
#if QT_CONFIG(tooltip)
        Crypto->setToolTip(QCoreApplication::translate("SettingsGui", "Your existing destination will be deleted", nullptr));
#endif // QT_CONFIG(tooltip)
        Encryption_label->setText(QCoreApplication::translate("SettingsGui", "Encryption Type:", nullptr));
        TunnelConfig->setTitle(QCoreApplication::translate("SettingsGui", "Tunnel Configuration", nullptr));
        label_9->setText(QCoreApplication::translate("SettingsGui", "Tunnel Name:", nullptr));
        label_14->setText(QCoreApplication::translate("SettingsGui", "Outbound", nullptr));
        label_15->setText(QCoreApplication::translate("SettingsGui", "Inbound", nullptr));
        label_10->setText(QCoreApplication::translate("SettingsGui", "Length:", nullptr));
        label_11->setText(QCoreApplication::translate("SettingsGui", "Quantity:", nullptr));
        label_12->setText(QCoreApplication::translate("SettingsGui", "Backup Quantity:", nullptr));
        UserBio->setTitle(QCoreApplication::translate("SettingsGui", "User Profile", nullptr));
        label_19->setText(QCoreApplication::translate("SettingsGui", "Nickname:", nullptr));
        UserSuppInfo->setTitle(QCoreApplication::translate("SettingsGui", "Supplementary Info", nullptr));
        SelectAvatar->setTitle(QCoreApplication::translate("SettingsGui", "Avatar", nullptr));
        ownavatar_label->setText(QString());
        label->setText(QCoreApplication::translate("SettingsGui", "Image will be scaled to 80 x 80\n"
"(stored in your application.ini file)", nullptr));
        cmd_selectAvatarImage->setText(QCoreApplication::translate("SettingsGui", "Select", nullptr));
        cmd_clearAvatarImage->setText(QCoreApplication::translate("SettingsGui", "Clear", nullptr));
        WindowStyle->setTitle(QCoreApplication::translate("SettingsGui", "Window Style", nullptr));
        label_7->setText(QCoreApplication::translate("SettingsGui", "Style:", nullptr));
        CustomStyle->setTitle(QCoreApplication::translate("SettingsGui", "Custom Style Sheet", nullptr));
        label_8->setText(QCoreApplication::translate("SettingsGui", "Style Sheet:", nullptr));
        ContactSort->setTitle(QCoreApplication::translate("SettingsGui", "Contact List Sorting", nullptr));
        checkBox_5->setText(QCoreApplication::translate("SettingsGui", "Enable automatic sort", nullptr));
        radioButton_2->setText(QCoreApplication::translate("SettingsGui", "Sort alphabetically", nullptr));
        radioButton_3->setText(QCoreApplication::translate("SettingsGui", "Sort by date added", nullptr));
        radioButton_4->setText(QCoreApplication::translate("SettingsGui", "Sort by last communication", nullptr));
        radioButton_5->setText(QCoreApplication::translate("SettingsGui", "Sort by last online", nullptr));
        WebServer->setTitle(QCoreApplication::translate("SettingsGui", "Web Server Access", nullptr));
        WebServerAuthCheckbox->setText(QCoreApplication::translate("SettingsGui", "Require login", nullptr));
        WebProfileDirectoryListingCheckbox->setText(QCoreApplication::translate("SettingsGui", "Show file listing when no index.html exists", nullptr));
        labelRealm->setText(QCoreApplication::translate("SettingsGui", "Realm:", nullptr));
        webServerRealmEdit->setText(QCoreApplication::translate("SettingsGui", "I2PChat Webserver", nullptr));
        labelSessionTimeout->setText(QCoreApplication::translate("SettingsGui", "Session timeout (minutes):", nullptr));
        webServerAddUserButton->setText(QCoreApplication::translate("SettingsGui", "Add", nullptr));
        webServerEditUserButton->setText(QCoreApplication::translate("SettingsGui", "Edit", nullptr));
        webServerDeleteUserButton->setText(QCoreApplication::translate("SettingsGui", "Delete", nullptr));
        Docroot->setTitle(QCoreApplication::translate("SettingsGui", "Document Root", nullptr));
        browseDocrootButton->setText(QCoreApplication::translate("SettingsGui", "Browse...", nullptr));
        UserBlocking->setTitle(QCoreApplication::translate("SettingsGui", "User Blocking", nullptr));
#if QT_CONFIG(tooltip)
        check_BlockNormal->setToolTip(QCoreApplication::translate("SettingsGui", "Contact will be informed that they have been blocked", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
        check_BlockNormal->setStatusTip(QString());
#endif // QT_CONFIG(statustip)
        check_BlockNormal->setText(QCoreApplication::translate("SettingsGui", "Normal", nullptr));
#if QT_CONFIG(tooltip)
        check_BlockInvisible->setToolTip(QCoreApplication::translate("SettingsGui", "Contact will view your status as Offline", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
        check_BlockInvisible->setStatusTip(QString());
#endif // QT_CONFIG(statustip)
        check_BlockInvisible->setText(QCoreApplication::translate("SettingsGui", "Invisible", nullptr));
        UserAuth->setTitle(QCoreApplication::translate("SettingsGui", "Authorization", nullptr));
        blockallcheckBox->setText(QCoreApplication::translate("SettingsGui", "Block all unknown Users", nullptr));
        WebPage->setTitle(QCoreApplication::translate("SettingsGui", "Profile Page", nullptr));
        WebProfileCheckbox->setText(QCoreApplication::translate("SettingsGui", "Enable profile web page", nullptr));
#if QT_CONFIG(tooltip)
        b32address->setToolTip(QCoreApplication::translate("SettingsGui", "Share this address with new contacts to facilitate adding you to their contact list", nullptr));
#endif // QT_CONFIG(tooltip)
        b32address->setPlainText(QCoreApplication::translate("SettingsGui", "http://b32.i2p", nullptr));
        HideWebCheckbox->setText(QCoreApplication::translate("SettingsGui", "Hide page when status is invisible for any contact", nullptr));
        SoundConnection->setTitle(QCoreApplication::translate("SettingsGui", "Contacts' Connection Status", nullptr));
        checkBoxSound_2->setText(QCoreApplication::translate("SettingsGui", "Offline:", nullptr));
        cmd_openFile_2->setText(QCoreApplication::translate("SettingsGui", "...", nullptr));
        checkBoxSound->setText(QCoreApplication::translate("SettingsGui", "Online:", nullptr));
        cmd_openFile->setText(QCoreApplication::translate("SettingsGui", "...", nullptr));
        SoundFilesend->setTitle(QCoreApplication::translate("SettingsGui", "File Send", nullptr));
        checkBoxSound_3->setText(QCoreApplication::translate("SettingsGui", "Finished:", nullptr));
        cmd_openFile_3->setText(QCoreApplication::translate("SettingsGui", "...", nullptr));
        SoundFileReceive->setTitle(QCoreApplication::translate("SettingsGui", "File Receive", nullptr));
        checkBoxSound_5->setText(QCoreApplication::translate("SettingsGui", "Finished:", nullptr));
        cmd_openFile_5->setText(QCoreApplication::translate("SettingsGui", "...", nullptr));
        checkBoxSound_4->setText(QCoreApplication::translate("SettingsGui", "Incoming:", nullptr));
        cmd_openFile_4->setText(QCoreApplication::translate("SettingsGui", "...", nullptr));
        SoundChatMessage->setTitle(QCoreApplication::translate("SettingsGui", "Chat Message", nullptr));
        checkBoxSound_6->setText(QCoreApplication::translate("SettingsGui", "New:", nullptr));
        cmd_openFile_6->setText(QCoreApplication::translate("SettingsGui", "...", nullptr));
        LocalStyle->setTitle(QCoreApplication::translate("SettingsGui", "Local Style", nullptr));
#if QT_CONFIG(tooltip)
        cmdBoldChat->setToolTip(QCoreApplication::translate("SettingsGui", "Bold", nullptr));
#endif // QT_CONFIG(tooltip)
        cmdBoldChat->setText(QCoreApplication::translate("SettingsGui", "B", nullptr));
#if QT_CONFIG(tooltip)
        cmdUnderChat->setToolTip(QCoreApplication::translate("SettingsGui", "Underline", nullptr));
#endif // QT_CONFIG(tooltip)
        cmdUnderChat->setText(QCoreApplication::translate("SettingsGui", "U", nullptr));
#if QT_CONFIG(tooltip)
        cmdItalicChat->setToolTip(QCoreApplication::translate("SettingsGui", "Italic", nullptr));
#endif // QT_CONFIG(tooltip)
        cmdItalicChat->setText(QCoreApplication::translate("SettingsGui", "I", nullptr));
#if QT_CONFIG(tooltip)
        cmdColorChat->setToolTip(QCoreApplication::translate("SettingsGui", "Color", nullptr));
#endif // QT_CONFIG(tooltip)
        cmdColorChat->setText(QCoreApplication::translate("SettingsGui", "C", nullptr));
#if QT_CONFIG(tooltip)
        cmdFontChat->setToolTip(QCoreApplication::translate("SettingsGui", "Font", nullptr));
#endif // QT_CONFIG(tooltip)
        cmdFontChat->setText(QString());
        label_4->setText(QCoreApplication::translate("SettingsGui", "Local Style", nullptr));
        RemoteStyle->setTitle(QCoreApplication::translate("SettingsGui", "Override Remote Style", nullptr));
        chatOverrideBox->setText(QCoreApplication::translate("SettingsGui", "Override Remote Style", nullptr));
#if QT_CONFIG(tooltip)
        cmdOverwriteBoldChat->setToolTip(QCoreApplication::translate("SettingsGui", "Bold", nullptr));
#endif // QT_CONFIG(tooltip)
        cmdOverwriteBoldChat->setText(QCoreApplication::translate("SettingsGui", "B", nullptr));
#if QT_CONFIG(tooltip)
        cmdOverwriteUnderChat->setToolTip(QCoreApplication::translate("SettingsGui", "Underline", nullptr));
#endif // QT_CONFIG(tooltip)
        cmdOverwriteUnderChat->setText(QCoreApplication::translate("SettingsGui", "U", nullptr));
#if QT_CONFIG(tooltip)
        cmdOverwriteItalicChat->setToolTip(QCoreApplication::translate("SettingsGui", "Italic", nullptr));
#endif // QT_CONFIG(tooltip)
        cmdOverwriteItalicChat->setText(QCoreApplication::translate("SettingsGui", "I", nullptr));
#if QT_CONFIG(tooltip)
        cmdOverwriteColorChat->setToolTip(QCoreApplication::translate("SettingsGui", "Color", nullptr));
#endif // QT_CONFIG(tooltip)
        cmdOverwriteColorChat->setText(QCoreApplication::translate("SettingsGui", "C", nullptr));
#if QT_CONFIG(tooltip)
        cmdOverwriteFontChat->setToolTip(QCoreApplication::translate("SettingsGui", "Font", nullptr));
#endif // QT_CONFIG(tooltip)
        cmdOverwriteFontChat->setText(QString());
        Emoticons->setTitle(QCoreApplication::translate("SettingsGui", "Emoticons", nullptr));
        checkBoxEmoticons->setText(QCoreApplication::translate("SettingsGui", "Enable local display of emoticons in chat", nullptr));
        groupChatStyle->setTitle(QCoreApplication::translate("SettingsGui", "Chat Style", nullptr));
        labelChatStyle->setText(QCoreApplication::translate("SettingsGui", "Chat Style", nullptr));
        checkBox_DisplayImagesInline->setText(QCoreApplication::translate("SettingsGui", "Display received images inline", nullptr));
        UserEvents->setTitle(QCoreApplication::translate("SettingsGui", "User Events", nullptr));
        checkboxUserEvents->setText(QCoreApplication::translate("SettingsGui", "Display online / offline events in chat", nullptr));
        groupBox->setTitle(QCoreApplication::translate("SettingsGui", "Connection Timeout", nullptr));
        label_6->setText(QCoreApplication::translate("SettingsGui", "Connection timeout (seconds):", nullptr));
        blocklist_page->setStyleSheet(QCoreApplication::translate("SettingsGui", "background-color: palette(window);", nullptr));
        cmd_unblockUserFromBlockList->setText(QCoreApplication::translate("SettingsGui", "Unblock", nullptr));
        cmd_deleteUserFromBlockList->setText(QCoreApplication::translate("SettingsGui", "Delete", nullptr));

        const bool __sortingEnabled = Sections->isSortingEnabled();
        Sections->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = Sections->item(0);
        ___qlistwidgetitem->setText(QCoreApplication::translate("SettingsGui", "General", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = Sections->item(1);
        ___qlistwidgetitem1->setText(QCoreApplication::translate("SettingsGui", "Network", nullptr));
        QListWidgetItem *___qlistwidgetitem2 = Sections->item(2);
        ___qlistwidgetitem2->setText(QCoreApplication::translate("SettingsGui", "User Details", nullptr));
        QListWidgetItem *___qlistwidgetitem3 = Sections->item(3);
        ___qlistwidgetitem3->setText(QCoreApplication::translate("SettingsGui", "Style", nullptr));
        QListWidgetItem *___qlistwidgetitem4 = Sections->item(4);
        ___qlistwidgetitem4->setText(QCoreApplication::translate("SettingsGui", "Web Server", nullptr));
        QListWidgetItem *___qlistwidgetitem5 = Sections->item(5);
        ___qlistwidgetitem5->setText(QCoreApplication::translate("SettingsGui", "Security", nullptr));
        QListWidgetItem *___qlistwidgetitem6 = Sections->item(6);
        ___qlistwidgetitem6->setText(QCoreApplication::translate("SettingsGui", "Sound", nullptr));
        QListWidgetItem *___qlistwidgetitem7 = Sections->item(7);
        ___qlistwidgetitem7->setText(QCoreApplication::translate("SettingsGui", "Chat", nullptr));
        QListWidgetItem *___qlistwidgetitem8 = Sections->item(8);
        ___qlistwidgetitem8->setText(QCoreApplication::translate("SettingsGui", "Blocklist", nullptr));
        Sections->setSortingEnabled(__sortingEnabled);

        cancel_Button->setText(QCoreApplication::translate("SettingsGui", "Cancel", nullptr));
        ok_Button->setText(QCoreApplication::translate("SettingsGui", "OK", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsGui: public Ui_SettingsGui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // SETTINGSGUI_H
