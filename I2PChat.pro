CONFIG += qt

greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11

QT += network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia core svg

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS   += -std=c++11
QMAKE_CXXFLAGS += -Wall

DEPENDPATH += . \
	src/gui/ \
	src/backend/

INCLUDEPATH += . \
	src/gui \
	src/backend/

SOURCES += \
	src/backend/Main.cpp \
	src/backend/Core.cpp \
	src/backend/User.cpp \
	src/backend/I2PSamMessageAnalyser.cpp \
	src/backend/DebugMessageManager.cpp \
	src/backend/Protocol.cpp \
	src/backend/PacketManager.cpp \
	src/backend/FileTransferSend.cpp \
	src/backend/FileTransferReceive.cpp \
	src/backend/SoundManager.cpp \
	src/backend/ConnectionManager.cpp \
	src/backend/SessionController.cpp \
	src/backend/I2PStream.cpp \
	src/backend/ChatmessageChanger.cpp \
	src/backend/UserBlockManager.cpp \
	src/backend/HttpOverStreamObject.cpp \
	src/backend/UserManager.cpp \
	src/backend/FileTransferManager.cpp \
	src/backend/TextEmotionChanger.cpp \
	src/backend/UnsentChatMessageStorage.cpp \
	src/gui/form_Main.cpp \
	src/gui/form_chatwidget.cpp \
	src/gui/form_DebugMessages.cpp \
	src/gui/form_fileReceive.cpp \
	src/gui/form_fileSend.cpp \
	src/gui/form_newUser.cpp \
	src/gui/form_rename.cpp \
	src/gui/form_settingsgui.cpp \
	src/gui/form_UserSearch.cpp \
	src/gui/form_about.cpp \
	src/gui/form_TopicSubscribe.cpp


HEADERS += \
	src/backend/Core.h \
	src/backend/User.h \
	src/backend/I2PSamMessageAnalyser.h \
	src/backend/DebugMessageManager.h \
	src/backend/Protocol.h \
	src/backend/PacketManager.h \
	src/backend/FileTransferSend.h \
	src/backend/FileTransferReceive.h \
	src/backend/SoundManager.h \
	src/backend/ConnectionManager.h \
	src/backend/SessionController.h \
	src/backend/I2PStream.h \
	src/backend/ChatmessageChanger.h \
	src/backend/UserBlockManager.h \
	src/backend/HttpOverStreamObject.h \
	src/backend/UserManager.h \
	src/backend/FileTransferManager.h \
	src/backend/TextEmotionChanger.h \
	src/backend/UnsentChatMessageStorage.h \
	src/gui/form_Main.h \
	src/gui/form_chatwidget.h \
	src/gui/form_DebugMessages.h \
	src/gui/form_fileReceive.h \
	src/gui/form_fileSend.h \
	src/gui/form_newUser.h \
	src/gui/form_rename.h \
	src/gui/form_settingsgui.h \
	src/gui/form_UserSearch.h \
	src/gui/form_about.h \
	src/gui/gui_icons.h \
	src/gui/form_TopicSubscribe.h

FORMS += \
	src/gui/form_Main.ui \
	src/gui/form_chatwidget.ui \
	src/gui/form_DebugMessages.ui \
	src/gui/form_fileReceive.ui \
	src/gui/form_fileSend.ui \
	src/gui/form_newUser.ui \
	src/gui/form_rename.ui \
	src/gui/form_settingsgui.ui \
	src/gui/form_userSearch.ui \
	src/gui/form_about.ui \  
	src/gui/form_topicSubscribe.ui

RESOURCES += src/gui/resourcen.qrc

# Windows static build
win32 {
	QMAKE_CXXFLAGS *= -Wno-deprecated-copy -Wno-class-memaccess
	QMAKE_LFLAGS   *= -Wl,-Bstatic -static-libgcc -static-libstdc++
	LIBS           *= -lstdc++ -lpthread
}

#unix {}

UI_DIR = src/gui
OBJECTS_DIR = temp/obj
RCC_DIR = temp/qrc
MOC_DIR = temp/moc
RC_FILE = src/gui/icons/i2pmessenger_win.rc

android {
	DEFINES += ANDROID=1
}

include(gitversion.pri)
