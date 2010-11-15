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

#ifndef ALERT_LAYER_H
#define ALERT_LAYER_H

class Alert;

#include <QList>
#include <QWidget>
class QVBoxLayout;

class AlertLayer : public QWidget
{
	Q_OBJECT

public:
	AlertLayer(QWidget* parent);

	void addAlert(const QString& text, const QStringList& details);

private slots:
	void alertDestroyed(QObject* alert);
	void dismissAlert();

private:
	QList<Alert*> m_alerts;
	QVBoxLayout* m_alerts_layout;
};

#endif
