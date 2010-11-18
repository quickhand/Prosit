/***********************************************************************
 *
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

#ifndef PROSEUP_READER_H
#define PROSEUP_READER_H

#include "tokenizer.h"

#include <QCoreApplication>
#include <QStack>
#include <QTextBlockFormat>
#include <QTextCharFormat>
class QString;
class QTextEdit;

namespace PROSEUP
{
	class Reader
	{
		Q_DECLARE_TR_FUNCTIONS(Reader)

	public:
		Reader();
		QString errorString() const;
		bool hasError() const;
		void read(const QString& filename, QTextEdit* text);

	private:
		Tokenizer m_token;
		QString m_error;
		QTextEdit* m_text;
	};
}

#endif
