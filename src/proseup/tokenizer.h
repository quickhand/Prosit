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

#ifndef PROSEUP_TOKENIZER_H
#define PROSEUP_TOKENIZER_H

#include <QByteArray>
#include <QCoreApplication>
#include <QStringList>
#include <QList>
#include <QString>
class QIODevice;

namespace PROSEUP
{
	enum TokenType
	{
                HEADING,
                P,
                PRE,
                BLOCKQUOTE,
                ATTRIBUTION,
                DIVIDER,
                EMPH,
                STRONG,
                INS,
                DEL,
                HL,
                TEXT
	};
        class Token
        {
        public:
                TokenType ttype;
                QStringList data;
                bool is_start;
                bool is_end;
                int userindex;
                static inline bool isBlock(TokenType testtype) { if(testtype<6) { return true; } else { return false; } }
                inline bool isBlock() { return isBlock(ttype); }
        };

	class Tokenizer
	{
		Q_DECLARE_TR_FUNCTIONS(Tokenizer)

	public:
		Tokenizer();
                QList<Token> tokenize(QIODevice* device);

        private:
                QList<Token> handle_inline(QString& text);
                QList<Token> handle_header(QString& text);
                QList<Token> handle_divider(QString& text);
                QList<Token> handle_p(QString& text);
                QList<Token> handle_pre(QString& text);
                QList<Token> handle_blockquote(QString& text);

	};

}

#endif // TOKENIZER_H
