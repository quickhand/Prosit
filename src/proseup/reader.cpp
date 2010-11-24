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

#include "reader.h"
#include "../editor.h"
#include <QFile>
#include <QList>
#include <QStringList>
#include <QTextBlock>
#include <QTextCodec>
#include <QTextEdit>
#include <iostream>
//-----------------------------------------------------------------------------

PROSEUP::Reader::Reader()
{

}

//-----------------------------------------------------------------------------

QString PROSEUP::Reader::errorString() const
{
	return m_error;
}

//-----------------------------------------------------------------------------

bool PROSEUP::Reader::hasError() const
{
	return !m_error.isEmpty();
}

//-----------------------------------------------------------------------------

void PROSEUP::Reader::read(const QString& filename, Editor* text)
{
    QStringList tokennames;
    tokennames.append("HEADING");
    tokennames.append("P");
    tokennames.append("PRE");
    tokennames.append("BLOCKQUOTE");
    tokennames.append("ATTRIBUTION");
    tokennames.append("DIVIDER");
    tokennames.append("EMPH");
    tokennames.append("STRONG");
    tokennames.append("INS");
    tokennames.append("DEL");
    tokennames.append("HL");
    tokennames.append("TEXT");


	try {
		// Open file
		m_text = 0;
		QFile file(filename);
		if (!file.open(QFile::ReadOnly)) {
			return;
		}
		m_text = text;
		m_text->setUndoRedoEnabled(false);


                QList<Token> tokenlist=m_token.tokenize(&file);
                QTextCharFormat curformat;
                for(int i=0;i<tokenlist.length();i++)
                {
                    Token cur=tokenlist[i];
                    if(cur.ttype==PROSEUP::TEXT)
                        text->textCursor().insertText(cur.data[0],curformat);
                    else if(cur.ttype==PROSEUP::EMPH)
                        curformat.setFontItalic(cur.is_start&&!cur.is_end);
                    else if(cur.ttype==PROSEUP::STRONG)
                        curformat.setFontWeight((cur.is_start&&!cur.is_end) ? QFont::Bold : QFont::Normal);
                    else if(cur.ttype==PROSEUP::INS)
                        curformat.setFontUnderline(cur.is_start&&!cur.is_end);
                    else if(cur.ttype==PROSEUP::DEL)
                        curformat.setFontStrikeOut(cur.is_start&&!cur.is_end);
                    else if(cur.ttype==PROSEUP::HL)
                    {
                        if(cur.is_start&&!cur.is_end)
                        {
                            QBrush backb=QBrush(curformat.background());
                            backb.setStyle(Qt::SolidPattern);
                            QColor mycolor;
                            QString hlcomment;
                            if(cur.data.length()>=2)
                            {
                                if(cur.data[0].length()>0)
                                    mycolor.setNamedColor(cur.data[0]);
                                else
                                    mycolor.setNamedColor("lightyellow");
                                hlcomment=cur.data[1];
                            }
                            else if(cur.data.length()==1 && cur.data[0].length()>0)
                                    mycolor.setNamedColor(cur.data[0]);
                            else
                                mycolor.setNamedColor("lightyellow");
                            backb.setColor(mycolor);
                            curformat.setBackground(backb);
                            //curformat.setProperty(QTextFormat::UserProperty,hlcomment);
                            curformat.setProperty(QTextFormat::TextToolTip,hlcomment);
                        }
                        else
                        {
                            curformat.clearBackground();
                            curformat.clearProperty(QTextFormat::TextToolTip);
                        }

                    }
                    else if(PROSEUP::Token::isBlock(cur.ttype))
                    {
                        if(cur.is_start)
                        {
                            QTextBlockFormat curblockfmt;
                            if(cur.ttype==PROSEUP::HEADING)
                                curblockfmt.setProperty(QTextFormat::UserProperty,QString("H")+cur.data[0]);
                            else if(cur.ttype==PROSEUP::BLOCKQUOTE)
                                curblockfmt.setProperty(QTextFormat::UserProperty,QString("BLOCKQUOTE"));
                            else if(cur.ttype==PROSEUP::ATTRIBUTION)
                                curblockfmt.setProperty(QTextFormat::UserProperty,QString("ATTRIBUTION"));
                            else if(cur.ttype==PROSEUP::PRE)
                                curblockfmt.setProperty(QTextFormat::UserProperty,QString("PRE"));
                            else if(cur.ttype==PROSEUP::DIVIDER)
                                curblockfmt.setProperty(QTextFormat::UserProperty,QString("DIVIDER")+cur.data[0]);
                            if(text->textCursor().atStart())
                            {
                                //QTextBlockFormat temp=QTextBlockFormat(text->textCursor().blockFormat());
                                //temp.merge(curblockfmt);
                                text->textCursor().setBlockFormat(curblockfmt);
                                text->textCursor().setCharFormat(curformat);
                            }
                            else
                            {
                                //QTextBlockFormat temp=QTextBlockFormat(text->textCursor().blockFormat());
                                //temp.merge(curblockfmt);
                                //std::cout<<"Inserting: "<<temp.stringProperty(QTextFormat::UserProperty).toStdString()<<std::endl;
                                //text->textCursor().setBlockFormat(temp);
                                text->textCursor().insertBlock(curblockfmt,curformat);
                            }
                        }
                        if(cur.is_end)
                        {
                            curformat.setFontItalic(false);
                            curformat.setFontWeight(QFont::Normal);

                        }
                    }


                    //std::cout<<"["<<tokennames.at(tokenlist[i].ttype).toStdString()<<"["<<tokenlist[i].data.join(",").toStdString()<<"]],";
                }
		file.close();
	} catch (const QString& error) {
		m_error = error;
	}
	m_text->setUndoRedoEnabled(true);
}

