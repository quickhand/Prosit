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

#include "timer_manager.h"

#include "stack.h"
#include "timer.h"
#include "timer_display.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLocale>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------

TimerManager::TimerManager(Stack* documents, QWidget* parent)
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint),
	m_documents(documents)
{
	setWindowTitle(tr("Timers"));

	// Set up interaction with timer display
	m_display = new TimerDisplay(m_timers, this);
	m_display->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_display, SIGNAL(clicked()), this, SLOT(toggleVisibility()));
	connect(m_display, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(recentTimerMenuRequested(const QPoint&)));

	// Create clock
	m_clock_label = new QLabel(this);
	m_clock_label->setAlignment(Qt::AlignCenter);

	m_clock_timer = new QTimer(this);
	m_clock_timer->setInterval(1000);
	connect(m_clock_timer, SIGNAL(timeout()), this, SLOT(updateClock()));
	startClock();

	// Create timers layout
	QWidget* timers_widget = new QWidget(this);

	m_timers_layout = new QVBoxLayout(timers_widget);
	m_timers_layout->addStretch();
	m_timers_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);

	m_timers_area = new QScrollArea(this);
	m_timers_area->setWidget(timers_widget);
	m_timers_area->setWidgetResizable(true);

	// Create action buttons
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
	connect(buttons, SIGNAL(rejected()), this, SLOT(close()));

	m_new_button = buttons->addButton(tr("New"), QDialogButtonBox::ActionRole);
	m_new_button->setDefault(true);
	connect(m_new_button, SIGNAL(clicked()), this, SLOT(newTimer()));

	m_recent_timers = new QMenu(this);
	m_recent_button = buttons->addButton(tr("Recent"), QDialogButtonBox::ActionRole);
	m_recent_button->setMenu(m_recent_timers);
	setupRecentMenu();
	connect(m_recent_timers, SIGNAL(triggered(QAction*)), this, SLOT(recentTimer(QAction*)));

	// Lay out window
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_clock_label);
	layout->addWidget(m_timers_area, 1);
	layout->addWidget(buttons);
	setMinimumHeight(sizeHint().width());

	QSettings settings;
	settings.beginGroup("Timers");
	resize(settings.value("DialogSize").toSize());

	// Load currently running timers
	QStringList ids = settings.childKeys();
	foreach (const QString& id, ids) {
		int i = id.mid(5).toInt();
		if (!id.startsWith("Timer") || i == 0) {
			continue;
		}
		Timer* timer = new Timer(id, m_documents, this);
		addTimer(timer);
		timerChanged(timer);
	}

	m_new_button->setFocus();
}

//-----------------------------------------------------------------------------

bool TimerManager::cancelEditing()
{
	bool check = false;
	foreach (Timer* timer, m_timers) {
		check |= timer->isEditing();
	}
	if (check) {
		if (QMessageBox::question(this, tr("Question"), tr("Cancel editing timers?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No) {
			return false;
		} else {
			foreach (Timer* timer, m_timers) {
				timer->cancelEditing();
			}
		}
	}
	return true;
}

//-----------------------------------------------------------------------------

TimerDisplay* TimerManager::display() const
{
	return m_display;
}

//-----------------------------------------------------------------------------

void TimerManager::saveTimers()
{
	foreach (Timer* timer, m_timers) {
		timer->save();
	}
}

//-----------------------------------------------------------------------------

void TimerManager::closeEvent(QCloseEvent* event)
{
	if (cancelEditing()) {
		QSettings().setValue("Timers/DialogSize", size());
		m_new_button->setFocus();
		event->accept();
	} else {
		event->ignore();
	}
}

//-----------------------------------------------------------------------------

void TimerManager::hideEvent(QHideEvent* event)
{
	m_clock_timer->stop();
	QDialog::hideEvent(event);
}

//-----------------------------------------------------------------------------

void TimerManager::showEvent(QShowEvent* event)
{
	startClock();
	QDialog::showEvent(event);
}

//-----------------------------------------------------------------------------

void TimerManager::newTimer()
{
	Timer* timer = new Timer(m_documents, this);
	addTimer(timer);
}

//-----------------------------------------------------------------------------

void TimerManager::recentTimer(QAction* action)
{
	QStringList values = action->data().toStringList();
	Timer* timer = new Timer(values.takeAt(0).toInt(), values, m_documents, this);
	addTimer(timer);
	timerChanged(timer);
}

//-----------------------------------------------------------------------------

void TimerManager::recentTimerMenuRequested(const QPoint& pos)
{
	if (!m_recent_timers->isEmpty()) {
		m_recent_timers->popup(m_display->mapToGlobal(pos));
	}
}

//-----------------------------------------------------------------------------

void TimerManager::timerChanged(Timer* timer)
{
	setupRecentMenu();

	if (m_timers.count() == 1) {
		updateDisplay();
		return;
	}

	// Find new timer position based on when it ends
	int index = m_timers.indexOf(timer);
	int new_index = 0;
	for (new_index = 0; new_index < m_timers.count(); ++new_index) {
		Timer* test = m_timers[new_index];
		if ((*timer <= *test) && (timer != test)) {
			break;
		}
	}

	// Move timer to new position
	if (new_index != index) {
		if (new_index > index) {
			new_index--;
		}
		m_timers.move(index, new_index);

		// Re-add all timers to widget so that tab order will be correct
		for (int i = 0; i < m_timers.count(); ++i) {
			m_timers[i]->setParent(0);
		}
		for (int i = 0; i < m_timers.count(); ++i) {
			m_timers_layout->insertWidget(i, m_timers[i]);
		}

		// Scroll to current timer
		QApplication::processEvents();
		m_timers_area->ensureWidgetVisible(timer, 0, 0);
	}

	updateDisplay();
}

//-----------------------------------------------------------------------------

void TimerManager::timerDeleted(QObject* object)
{
	for (int i = 0; i < m_timers.count(); ++i) {
		if (m_timers.at(i) == object) {
			m_timers.removeAt(i);
			break;
		}
	}
	updateDisplay();
}

//-----------------------------------------------------------------------------

void TimerManager::timerEdited(Timer* timer)
{
	m_timers_area->ensureWidgetVisible(timer, 0, 0);
}

//-----------------------------------------------------------------------------

void TimerManager::toggleVisibility()
{
	setVisible(!isVisible());
}

//-----------------------------------------------------------------------------

void TimerManager::updateClock()
{
	m_clock_label->setText(QTime::currentTime().toString(Qt::DefaultLocaleLongDate).simplified());
}

//-----------------------------------------------------------------------------

void TimerManager::addTimer(Timer* timer)
{
	m_timers_layout->insertWidget(0, timer);
	m_timers.append(timer);
	connect(timer, SIGNAL(changed(Timer*)), this, SLOT(timerChanged(Timer*)));
	connect(timer, SIGNAL(destroyed(QObject*)), this, SLOT(timerDeleted(QObject*)));
	connect(timer, SIGNAL(edited(Timer*)), this, SLOT(timerEdited(Timer*)));
	connect(m_documents, SIGNAL(documentAdded(Document*)), timer, SLOT(documentAdded(Document*)));
	connect(m_documents, SIGNAL(documentRemoved(Document*)), timer, SLOT(documentRemoved(Document*)));
	m_timers_area->ensureWidgetVisible(timer, 0, 0);
}

//-----------------------------------------------------------------------------

void TimerManager::setupRecentMenu()
{
	QList<QAction*> end_timers;
	QList<QAction*> delay_timers;

	for (int i = 0; i < 2; ++i) {
		QString type = QString::number(i);
		QStringList recent = QSettings().value(QString("Timers/Recent%1").arg(i)).toStringList();
		foreach (const QString& timer, recent) {
			QString time = timer.section(' ', 0, 0);
			QString memo = timer.section(' ', 1).simplified();
			memo.truncate(140);

			QAction* action = new QAction(this);
			action->setData(QStringList() << type << time << memo);
			if (i == 0) {
				time = QTime::fromString(time, Qt::ISODate).toString(tr("+HH:mm:ss")).simplified();
				delay_timers.append(action);
			} else {
				time = QTime::fromString(time, Qt::ISODate).toString(Qt::DefaultLocaleLongDate).simplified();
				end_timers.append(action);
			}
			memo = fontMetrics().elidedText(memo, Qt::ElideRight, 300);
			action->setText(!memo.isEmpty() ? tr("%1 - %2").arg(time).arg(memo) : time);
		}
	}

	m_recent_timers->clear();
	foreach (QAction* action, delay_timers) {
		m_recent_timers->addAction(action);
	}
	m_recent_timers->addSeparator();
	foreach (QAction* action, end_timers) {
		m_recent_timers->addAction(action);
	}

	m_recent_button->setEnabled(!m_recent_timers->isEmpty());
}

//-----------------------------------------------------------------------------

void TimerManager::startClock()
{
	updateClock();
	int delay = 1000 - QTime::currentTime().msec();
	QTimer::singleShot(delay, m_clock_timer, SLOT(start()));
	QTimer::singleShot(delay, this, SLOT(updateClock()));
}

//-----------------------------------------------------------------------------

void TimerManager::updateDisplay()
{
	foreach (Timer* timer, m_timers) {
		if (timer->isRunning()) {
			m_display->setTimer(timer);
			return;
		}
	}
	m_display->setTimer(0);
}

//-----------------------------------------------------------------------------
