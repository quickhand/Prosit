/***********************************************************************
 *
 * Copyright (C) 2009, 2010 Graeme Gott <graeme@gottcode.org>
 * Copyright (C) 2010 Marc Lajoie <manorapide@gmail.com>
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

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

class Dictionary;

#include <QSyntaxHighlighter>
#include <QTextCursor>
class QAction;
class QTextEdit;
class Editor;

class Highlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
        Highlighter(Editor* text);

	bool enabled() const;
	QColor misspelledColor() const;
	void setEnabled(bool enabled);
	void setMisspelledColor(const QColor& color);

	virtual bool eventFilter(QObject* watched, QEvent* event);
	virtual void highlightBlock(const QString& text);

private slots:
	void suggestion(QAction* action);

private:
	Dictionary* m_dictionary;
        Editor* m_text;
	QTextCursor m_cursor;
	QTextCursor m_start_cursor;
	bool m_enabled;
	QColor m_misspelled;
	QString m_word;

	QAction* m_add_action;
	QAction* m_check_action;
};

#endif
