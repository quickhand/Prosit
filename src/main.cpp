/***********************************************************************
 *
 * Copyright (C) 2008, 2009, 2010 Graeme Gott <graeme@gottcode.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "dictionary.h"
#include "session.h"
#include "sound.h"
#include "theme.h"
#include "window.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QLocale>
#include <QSettings>
#include <QTranslator>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("Prosit");
	app.setApplicationVersion("1.3.1");
	app.setOrganizationDomain("quickhand.org");
	app.setOrganizationName("Quickhand");
	QString appdir = app.applicationDirPath();

	QStringList paths = QIcon::themeSearchPaths();
	paths.prepend(appdir + "/../share/prosit/icons");
	paths.prepend(appdir + "/icons/oxygen");
	QIcon::setThemeSearchPaths(paths);

	QTranslator qt_translator;
	qt_translator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	app.installTranslator(&qt_translator);

	paths.clear();
	paths.append(appdir + "/translations/");
	paths.append(appdir + "/../share/prosit/translations/");
	paths.append(appdir + "/../Resources/translations");
	QString translator_path;
	foreach (const QString& path, paths) {
		if (QFile::exists(path)) {
			translator_path = path;
			break;
		}
	}

	QTranslator translator;
	if (!translator.load("prosit_" + QLocale::system().name(), translator_path)) {
		translator.load("prosit_en", translator_path);
	}
	app.installTranslator(&translator);

	paths.clear();
	paths.append(appdir + "/sounds/");
	paths.append(appdir + "/../share/prosit/sounds/");
	paths.append(appdir + "/../Resources/sounds");
	foreach (const QString& path, paths) {
		if (QFile::exists(path)) {
			Sound::setPath(path);
			break;
		}
	}

	// Find data paths
	QStringList locations;
#if defined(Q_OS_MAC)
	QFileInfo portable(appdir + "/../../../Data");
	QString path = QDir::homePath() + "/Library/Application Support/Quickhand/Prosit/";

	locations.append(QDir::homePath() + "/Library/Spelling");
	locations.append("/Library/Spelling");
	locations.append(appdir + "/../Resources/Dictionaries");
#elif defined(Q_OS_UNIX)
	QFileInfo portable(appdir + "/Data");
	QString path = qgetenv("XDG_DATA_HOME");
	if (path.isEmpty()) {
		path = QDir::homePath() + "/.local/share";
	}
	path += "/prosit/";

	QStringList xdg = QString(qgetenv("XDG_DATA_DIRS")).split(QChar(':'), QString::SkipEmptyParts);
	if (xdg.isEmpty()) {
		xdg.append("/usr/local/share");
		xdg.append("/usr/share");
	}
	QStringList subdirs = QStringList() << "/hunspell" << "/myspell/dicts" << "/myspell";
	foreach (const QString& subdir, subdirs) {
		foreach (const QString& dir, xdg) {
			QString path = dir + subdir;
			if (!locations.contains(path)) {
				locations.append(path);
			}
		}
	}
#elif defined(Q_OS_WIN32)
	QFileInfo portable(appdir + "/Data");
	QString path = QDir::homePath() + "/Application Data/Quickhand/Prosit/";

	locations.append(appdir + "/Dictionaries");
#endif

	// Handle portability
	if (portable.exists() && portable.isWritable()) {
		path = portable.absoluteFilePath();
		QSettings::setDefaultFormat(QSettings::IniFormat);
		QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, path + "/Settings");
	}

	// Create base data path
	QDir dir;
	if (!QFile::exists(path)) {
		dir.mkpath(path);
	}
	dir.setPath(path);

	// Set sessions path
	if (!dir.exists("Sessions")) {
		dir.mkdir("Sessions");
	}
	Session::setPath(dir.absoluteFilePath("Sessions"));

	// Set themes path
	if (!dir.exists("Themes")) {
		if (dir.exists("themes")) {
			dir.rename("themes", "Themes");
		} else {
			dir.mkdir("Themes");
		}
	}
	if (!dir.exists("Themes/Images")) {
		dir.mkdir("Themes/Images");
	}
	Theme::setPath(dir.absoluteFilePath("Themes"));

	// Set dictionary paths
	if (!dir.exists("Dictionaries")) {
		if (dir.exists("dictionaries")) {
			dir.rename("dictionaries", "Dictionaries");
		} else {
			dir.mkdir("Dictionaries");
		}
	}
	Dictionary::setPath(dir.absoluteFilePath("Dictionaries"));
	locations.prepend(Dictionary::path());
	QDir::setSearchPaths("dict", locations);

	// Create theme from old settings
	if (QDir(Theme::path(), "*.theme").entryList(QDir::Files).isEmpty()) {
		QSettings settings;
		Theme theme;

		theme.setBackgroundType(settings.value("Background/Position", 0).toInt());
		theme.setBackgroundColor(settings.value("Background/Color", "#cccccc").toString());
		theme.setBackgroundImage(settings.value("Background/Image").toString());
		settings.remove("Background");

		theme.setForegroundColor(settings.value("Page/Color", "#cccccc").toString());
		theme.setForegroundWidth(settings.value("Page/Width", 700).toInt());
		theme.setForegroundOpacity(settings.value("Page/Opacity", 100).toInt());
		settings.remove("Page");

		theme.setTextColor(settings.value("Text/Color", "#000000").toString());
		theme.setTextFont(settings.value("Text/Font").value<QFont>());
		settings.remove("Text");

		settings.setValue("ThemeManager/Theme", theme.name());
	}

	// Browse to documents
	QDir::setCurrent(QSettings().value("Save/Location", QDir::homePath()).toString());

	// Create main window
	new Window;

	return app.exec();
}
