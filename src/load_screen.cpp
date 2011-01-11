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

#include "load_screen.h"

#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------

LoadScreen::LoadScreen(QWidget* parent)
	: QLabel(parent)
{
	setPixmap(QString(":/load.png"));
	setAlignment(Qt::AlignCenter);
	setStyleSheet("LoadScreen { background-color: #666666; }");

	m_text = new QLabel(this);
	m_text->hide();
	m_text->setAlignment(Qt::AlignCenter);
	m_text->setStyleSheet("QLabel {color: black; background-color: #aaaaaa; border-top-left-radius: 0.25em; border-top-right-radius: 0.25em; padding: 0.25em; }");

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addStretch();
	layout->addWidget(m_text, 0, Qt::AlignCenter);

	m_hide_effect = new QGraphicsOpacityEffect(this);
	m_hide_effect->setOpacity(1.0);
	setGraphicsEffect(m_hide_effect);

	m_hide_timer = new QTimer(this);
	m_hide_timer->setInterval(30);
	connect(m_hide_timer, SIGNAL(timeout()), this, SLOT(fade()));
}

//-----------------------------------------------------------------------------

void LoadScreen::setText(const QString& step)
{
	m_text->setText(step);
	m_text->setVisible(!step.isEmpty());

	if (m_hide_timer->isActive()) {
		m_hide_timer->stop();
	}
	m_hide_effect->setOpacity(1.0);

	show();
	raise();
	QApplication::processEvents();
}

//-----------------------------------------------------------------------------

void LoadScreen::finish()
{
	m_hide_effect->setOpacity(1.0);
	m_hide_timer->start();
}

//-----------------------------------------------------------------------------

void LoadScreen::hideEvent(QHideEvent* event)
{
	QApplication::restoreOverrideCursor();
	QLabel::hideEvent(event);
}

//-----------------------------------------------------------------------------

void LoadScreen::showEvent(QShowEvent* event)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	QLabel::showEvent(event);
}

//-----------------------------------------------------------------------------

void LoadScreen::fade()
{
	m_hide_effect->setOpacity(m_hide_effect->opacity() - 0.2);
	if (m_hide_effect->opacity() <= 0.01) {
		m_hide_timer->stop();
		hide();
	}
}

//-----------------------------------------------------------------------------
