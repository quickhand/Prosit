TEMPLATE = app
CONFIG += warn_on release
macx {
	# Uncomment the following line to compile on PowerPC Macs
	# QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
	CONFIG += x86 ppc
}

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

unix: !macx {
	TARGET = prosit
} else {
	TARGET = Prosit
}

macx {
	INCLUDEPATH += src/qsound /Library/Frameworks/hunspell.framework/Headers /Library/Frameworks/libzip.framework/Headers
	LIBS += -framework hunspell -framework libzip
	HEADERS += src/qsound/sound.h
	SOURCES += src/qsound/sound.cpp
} else:win32 {
	INCLUDEPATH += src/ao hunspell libao libzip
	LIBS += ./hunspell/hunspell1.dll ./libao/libao-4.dll ./libzip/libzip0.dll
	HEADERS += src/ao/sound.h
	SOURCES += src/ao/sound.cpp
} else {
	INCLUDEPATH += src/ao
	QMAKE_CXXFLAGS += $$system(pkg-config --cflags ao hunspell libzip)
	LIBS += $$system(pkg-config --libs ao hunspell libzip)
	HEADERS += src/ao/sound.h
	SOURCES += src/ao/sound.cpp
}

HEADERS += src/alert.h \
	src/alert_layer.h \
	src/block_stats.h \
	src/color_button.h \
	src/deltas.h \
	src/dictionary.h \
	src/document.h \
	src/find_dialog.h \
	src/highlighter.h \
	src/image_button.h \
	src/load_screen.h \
	src/preferences.h \
	src/preferences_dialog.h \
	src/session.h \
	src/session_manager.h \
	src/settings_file.h \
	src/smart_quotes.h \
	src/spell_checker.h \
	src/stack.h \
	src/stats.h \
	src/theme.h \
	src/theme_dialog.h \
	src/theme_manager.h \
	src/timer.h \
	src/timer_display.h \
	src/timer_manager.h \
	src/window.h \
    src/proseup/writer.h \
    src/proseup/tokenizer.h \
    src/proseup/reader.h

SOURCES += src/alert.cpp \
	src/alert_layer.cpp \
	src/block_stats.cpp \
	src/color_button.cpp \
	src/deltas.cpp \
	src/dictionary.cpp \
	src/document.cpp \
	src/find_dialog.cpp \
	src/highlighter.cpp \
	src/image_button.cpp \
	src/load_screen.cpp \
	src/main.cpp \
	src/preferences.cpp \
	src/preferences_dialog.cpp \
	src/session.cpp \
	src/session_manager.cpp \
	src/smart_quotes.cpp \
	src/spell_checker.cpp \
	src/stack.cpp \
	src/stats.cpp \
	src/theme.cpp \
	src/theme_dialog.cpp \
	src/theme_manager.cpp \
	src/timer.cpp \
	src/timer_display.cpp \
	src/timer_manager.cpp \
	src/window.cpp \
    src/proseup/writer.cpp \
    src/proseup/tokenizer.cpp \
    src/proseup/reader.cpp

TRANSLATIONS = translations/prosit_en.ts \
	translations/prosit_es.ts \
	translations/prosit_fr.ts \
	translations/prosit_pl.ts \
	translations/prosit_pt.ts \
	translations/prosit_pt_BR.ts

RESOURCES = icons/icons.qrc
macx {
	ICON = icons/prosit.icns
}
win32 {
	RC_FILE = icons/icon.rc
}

unix: !macx {
	isEmpty(PREFIX) {
		PREFIX = /usr/local
	}

	target.path = $$PREFIX/bin/

	icon.files = icons/prosit.png
	icon.path = $$PREFIX/share/icons/hicolor/48x48/apps

	icons.files = icons/oxygen/hicolor/*
	icons.path = $$PREFIX/share/prosit/icons/hicolor

	desktop.files = icons/prosit.desktop
	desktop.path = $$PREFIX/share/applications/

	qm.files = translations/*.qm
	qm.path = $$PREFIX/share/prosit/translations

	sounds.files = sounds/*
	sounds.path = $$PREFIX/share/prosit/sounds

	INSTALLS += target icon desktop icons qm sounds
}
