/***********************************************************************
 *
 * Copyright (C) 2010 Graeme Gott <graeme@gottcode.org>
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

#ifndef SESSION_H
#define SESSION_H

#include <QCoreApplication>
#include <QStringList>
class QSettings;

class Session
{
	Q_DECLARE_TR_FUNCTIONS(Session)

public:
	Session(const QString& name);
	~Session();

	int active() const;
	QSettings* data() const;
	QStringList files() const;
	QString name() const;
	QStringList positions() const;
	QString theme() const;

	void setName(const QString& name);
	void setTheme(const QString& theme);

	static QString path();
	static QString pathFromName(const QString& name);
	static QString pathToName(const QString& path);
	static void setPath(const QString& path);

private:
	QSettings* m_data;
	QString m_name;
	bool m_default;
	static QString m_path;
};

#endif
