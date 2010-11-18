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

#include "writer.h"

#include <QFile>
#include <QTextBlock>
#include <QTextCodec>
#include <QTextEdit>
#include <QStringList>
#include <iostream>
//-----------------------------------------------------------------------------

PROSEUP::Writer::Writer()
{

}

//-----------------------------------------------------------------------------

bool PROSEUP::Writer::write(const QString& filename, QTextEdit* text)
{
	QFile file(filename);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		return false;
	}

        bool emph_on=false;
        bool strong_on=false;
        bool ins_on=false;
        bool del_on=false;
        bool hl_on=false;
        bool last_was_pre=false;
        QBrush hlbrush;
        QString hlcomment;
	for (QTextBlock block = text->document()->begin(); block.isValid(); block = block.next()) {
		QTextBlockFormat block_format = block.blockFormat();

                QString startblock,endblock;
                QString uprop=block_format.stringProperty(QTextFormat::UserProperty);
                if(block.length()==0&&uprop!="PRE"&&!uprop.startsWith("DIVIDER"))
                    continue;
                if(block!=text->document()->begin() && uprop!="ATTRIBUTION")
                {
                    startblock="\n";
                    if(uprop!="PRE")
                        startblock+="\n";
                }
                if(uprop!="PRE" && last_was_pre)
                {
                    startblock+="----\n";
                    last_was_pre=false;
                }
                if(uprop.startsWith("H") && uprop.at(1).isDigit())
                {
                    //std::cout<<"HEADING THING! uprop="<<uprop.toStdString()<<std::endl;
                    int headerlevel=uprop.at(1).digitValue();
                    for(int i=0;i<headerlevel;i++)
                        startblock+="#";
                }
                else if(uprop=="BLOCKQUOTE")
                    startblock+="<";
                else if(uprop=="PRE")
                {
                    if(!last_was_pre)
                    {
                        startblock+="----\n";
                        last_was_pre=true;
                    }
                }
                else if(uprop=="ATTRIBUTION")
                {
                    startblock+="-";
                }
                else if(uprop.startsWith("DIVIDER"))
                {
                    int dividerlevel=uprop.at(7).digitValue();
                    for(int i=0;i<dividerlevel;i++)
                        startblock+="@";
                    file.write(startblock.toUtf8());
                    continue;
                }
                file.write(startblock.toUtf8());
		if (block.begin() != block.end()) {

			for (QTextBlock::iterator iter = block.begin(); iter != block.end(); ++iter) {
                                QTextFragment fragment = iter.fragment();

                                //QTextCursor mycursor=QTextCursor(text->document());
                                //mycursor.setPosition(block.position()+fragment.position());
                                QTextCharFormat char_format =text->document()->allFormats().at(iter.fragment().charFormatIndex()).toCharFormat();//fragment.charFormat();

                                QString innertext="";
                                if(hl_on) {
                                    //if(char_format.background()==Qt::NoBrush || char_format.background()==block.blockFormat().background() || char_format.background()!=hlbrush || char_format.stringProperty(QTextFormat::UserProperty)!=hlcomment)
                                    if(char_format.background()==Qt::NoBrush || char_format.background()==block.blockFormat().background() || char_format.background()!=hlbrush || char_format.stringProperty(QTextFormat::TextToolTip)!=hlcomment)
                                    {
                                        innertext += "%}";
                                        hl_on=false;
                                    }

                                }
                                if (!(char_format.fontStrikeOut()) && del_on) {
                                        innertext += "-}";
                                        del_on=false;
                                }
                                if (!(char_format.fontUnderline()) && ins_on) {
                                        innertext += "+}";
                                        ins_on=false;
                                }
                                if (char_format.fontWeight() == QFont::Normal && strong_on) {
                                        innertext+="~";
                                        strong_on=false;
                                }
                                if (!(char_format.fontItalic()) && emph_on) {
                                        innertext+="*";
                                        emph_on=false;
                                }

                                if (char_format.fontItalic() && !emph_on) {
                                        innertext+="*";
                                        emph_on=true;
                                }
                                if (char_format.fontWeight() == QFont::Bold && !strong_on) {
                                        innertext+="~";
                                        strong_on=true;
				}
                                if (char_format.fontUnderline() && !ins_on) {
                                        innertext += "{+";
                                        ins_on=true;
				}
                                if (char_format.fontStrikeOut() && !del_on) {
                                        innertext += "{-";
                                        del_on=true;
                                }

                                if(char_format.background()!=Qt::NoBrush && char_format.background()!=block.blockFormat().background() && !hl_on) {
                                    hlcomment=char_format.stringProperty(QTextFormat::TextToolTip);
                                    hlbrush=char_format.background();
                                    innertext += "{";
                                    if(hlbrush.color().name()!="lightyellow")
                                        innertext+=QString("%")+hlbrush.color().name()+QString("%");
                                    if(hlcomment.length()>0)
                                        innertext+=QString("%")+hlcomment+QString("%");

                                    innertext += "%";
                                    hlcomment=char_format.stringProperty(QTextFormat::TextToolTip);
                                    hlbrush=char_format.background();
                                    hl_on=true;
                                }
                                innertext+=fragment.text();



                                file.write(innertext.toUtf8());





			}
		}
                if(hl_on)
                {
                    if(!(block.next().isValid()
                        && block.next().begin().fragment().charFormat().background()==hlbrush
                        && block.next().begin().fragment().charFormat().stringProperty(QTextFormat::TextToolTip)==hlcomment))
                    {
                        endblock += "%}";
                        hl_on=false;
                    }
                }
                if(del_on)
                {
                    if(!(block.next().isValid()&&block.next().begin().fragment().charFormat().fontStrikeOut()))
                    {
                        endblock+="-}";
                        del_on=false;
                    }
                }
                if(ins_on)
                {
                    if(!(block.next().isValid()&&block.next().begin().fragment().charFormat().fontUnderline()))
                    {
                        endblock+="+}";
                        ins_on=false;
                    }
                }
                if(strong_on)
                {
                    endblock+="~";
                    strong_on=false;
                }
                if(emph_on)
                {
                    endblock+="*";
                    emph_on=false;
                }
                if(uprop.startsWith("H") && uprop.at(1).isDigit())
                {
                    int headerlevel=uprop.at(1).digitValue();
                    for(int i=0;i<headerlevel;i++)
                        endblock+="#";
                }
                else if(uprop=="BLOCKQUOTE")
                    endblock+=">";
                file.write(endblock.toUtf8());
	}


	bool saved = (file.error() == QFile::NoError);
	file.close();
	return saved;
}

//-----------------------------------------------------------------------------
