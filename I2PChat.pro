CONFIG += qt

greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11

QT += network xml dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia core svg

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wall
# QMAKE_POST_LINK=$(STRIP) $(TARGET) // doesn't work with appveyor

DEPENDPATH += . \
	src/gui/ \
	src/backend/

INCLUDEPATH += . \
	src/gui \
	src/backend/

SOURCES += \
	src/backend/Base.cpp \
	src/backend/HttpServer.cpp \
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
	src/backend/UserManager.cpp \
	src/backend/FileTransferManager.cpp \
	src/backend/TextEmotionChanger.cpp \
	src/backend/UnsentChatMessageStorage.cpp \
	src/gui/MainWindow.cpp \
	src/gui/StatusNotifier.cpp \
	src/gui/ChatWidget.cpp \
	src/gui/DebugMessages.cpp \
	src/gui/FileReceive.cpp \
	src/gui/FileSend.cpp \
	src/gui/NewUserWindow.cpp \
	src/gui/RenameWindow.cpp \
	src/gui/SettingsGui.cpp \
	src/gui/About.cpp \
	src/gui/ChatDelegate.cpp


HEADERS += \
	src/backend/Core.h \
	src/backend/HttpServer.h \
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
	src/backend/UserManager.h \
	src/backend/FileTransferManager.h \
	src/backend/TextEmotionChanger.h \
	src/backend/UnsentChatMessageStorage.h \
	src/gui/MainWindow.h \
	src/gui/StatusNotifier.h \
	src/gui/ChatWidget.h \
	src/gui/DebugMessages.h \
	src/gui/FileReceive.h \
	src/gui/FileSend.h \
	src/gui/NewUserWindow.h \
	src/gui/RenameWindow.h \
	src/gui/SettingsGui.h \
	src/gui/gui_icons.h \
	src/gui/About.h \
	src/gui/ChatDelegate.h

FORMS += \
	src/gui/forms/MainWindow.ui \
	src/gui/forms/ChatWidget.ui \
	src/gui/forms/DebugMessages.ui \
	src/gui/forms/FileReceive.ui \
	src/gui/forms/FileSend.ui \
	src/gui/forms/NewUserWindow.ui \
	src/gui/forms/RenameWindow.ui \
	src/gui/forms/SettingsGui.ui \
	src/gui/forms/About.ui

RESOURCES += src/gui/resourcen.qrc

# MinGW static build
mingw {
	QMAKE_LFLAGS   *= -Wl,-Bstatic -static-libgcc -static-libstdc++
	LIBS           *= -lstdc++ -lpthread
}

# Remove AGL framework (deprecated, removed in recent macOS SDKs)
macx {
	LIBS -= -framework AGL
	QMAKE_LFLAGS -= -framework AGL
}

#unix {}

UI_DIR = /tmp/build-i2pchat/uic
OBJECTS_DIR = /tmp/build-i2pchat/obj
RCC_DIR = /tmp/build-i2pchat/qrc
MOC_DIR = /tmp/build-i2pchat/moc
DESTDIR = dist/
RC_FILE = src/gui/icons/i2pmessenger_win.rc

android {
	DEFINES += ANDROID=1
}

include(gitversion.pri)

# Add build timestamp echo commands using QMAKE_POST_LINK
QMAKE_POST_LINK = @echo "Build complete: binary created at $$(DESTDIR)$(TARGET) at: \$$(date '+%Y-%m-%d %H:%M:%S')"
