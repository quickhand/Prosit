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

#ifndef HIGHLIGHT_DIALOG_H
#define HIGHLIGHT_DIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class ColorButton;


class HighlightDialog : public QDialog
{
	Q_OBJECT
public:
        HighlightDialog(QWidget *parent);
signals:
        void highlight(QColor color,QString comment);
private slots:
        void closeDialog();
private:
        ColorButton* m_color_button;
        QLineEdit* m_highlight_comment;
};

#endif
