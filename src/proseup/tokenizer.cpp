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

#include "tokenizer.h"

#include <QApplication>
#include <QIODevice>
#include <QRegExp>
#include <iostream>
#include <QTextCodec>
//-----------------------------------------------------------------------------

PROSEUP::Tokenizer::Tokenizer()
{

}

//-----------------------------------------------------------------------------

QList<PROSEUP::Token> PROSEUP::Tokenizer::tokenize(QIODevice* device)
{
    QList<PROSEUP::Token> tokenlist;
    QByteArray curlinebytes;
    bool in_pre=false;
    QStringList linearr;
    while(!((curlinebytes=device->readLine()).isEmpty()))
    {
        QByteArray encodedString = "...";
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QString curline=QString(codec->toUnicode(curlinebytes));
        if(curline.length()>0 && curline.endsWith('\n'))
            curline.chop(1);
        if(curline.length()>=4 && curline.endsWith("----"))
        {
            if(in_pre)
            {
                QString joinstr=QString(linearr.join("\n"));
                tokenlist.append(this->handle_pre(joinstr));
                linearr.clear();
                in_pre=false;
            }
            else
                in_pre=true;
        }
        else if(in_pre)
        {
            linearr.append(curline);
        }
        else if(curline.length()>0&&curline.at(0)=='#')
        {
            QString trimstr=QString(curline.trimmed());
            tokenlist.append(this->handle_header(trimstr));
        }
        else if(curline.length()>0&&curline.at(0)=='<')
        {
            QString trimstr=QString(curline.trimmed());
            tokenlist.append(this->handle_blockquote(trimstr));
        }
        else if(curline.length()>0&&curline.at(0)=='@')
        {
            QString trimstr=QString(curline.trimmed());
            tokenlist.append(this->handle_divider(trimstr));
        }
        else if(curline.trimmed().length()>0)
        {
            QString trimstr=QString(curline.trimmed());
            tokenlist.append(this->handle_p(trimstr));
        }
    }
    return tokenlist;

}

//-----------------------------------------------------------------------------

QList<PROSEUP::Token> PROSEUP::Tokenizer::handle_inline(QString& text)
{
//Here's a ~*paragr*aph~ with some    odd. Punctuation; (whatdyya think?).
    QList<PROSEUP::Token> tokenlist;
    //QRegExp splitter=QRegExp("\\\\?(\\*|~|\\{\\+|\\+\\}|\\{\\-|\\-\\}|\\{(?:%[^%\\*~\\{\\}<>]+%)?(?:%[^%\\*~\\{\\}<>]+%)*%|%\\})");
    QRegExp splitter=QRegExp("\\\\?(\\{[^\\+\\-%][^%]*%|\\*|~|\\{\\+|\\+\\}|\\{\\-|\\-\\}|\\{%|%\\})");
    QStringList splitlist;
    int stringind=0;
    int tagind;
    while((tagind=text.indexOf(splitter,stringind))>=0)
    {
        int taglen=splitter.cap(0).length();
        if(tagind!=stringind)
        {
             splitlist.append(text.mid(stringind,tagind-stringind));
        }
        splitlist.append(text.mid(tagind,taglen));


        stringind=tagind+taglen;
    }
    if(stringind<(text.length()-1))
    {
        splitlist.append(text.right(text.length()-stringind));

    }

    bool emph_on=false;
    bool strong_on=false;

    bool lastemph=false;
    for(int i=0;i<splitlist.length();i++)
    {
        //std::cout<<splitlist[i].toStdString()<<std::endl;
        if(splitlist[i]=="*")
        {
            if(!emph_on)
                lastemph=true;
            PROSEUP::Token token;
            token.ttype=PROSEUP::EMPH;
            token.is_start=!emph_on;
            token.is_end=emph_on;
            emph_on=!emph_on;
            tokenlist.append(token);
        }
        else if(splitlist[i]=="~")
        {
            if(!strong_on)
                lastemph=false;
            PROSEUP::Token token;
            token.ttype=PROSEUP::STRONG;
            token.is_start=!strong_on;
            token.is_end=strong_on;
            strong_on=!strong_on;
            tokenlist.append(token);
        }
        else if(splitlist[i]=="{-")
        {
            PROSEUP::Token token;
            token.ttype=PROSEUP::DEL;
            token.is_start=true;
            token.is_end=false;
            tokenlist.append(token);
        }
        else if(splitlist[i]=="-}")
        {
            PROSEUP::Token token;
            token.ttype=PROSEUP::DEL;
            token.is_start=false;
            token.is_end=true;
            tokenlist.append(token);
        }
        else if(splitlist[i]=="{+")
        {
            PROSEUP::Token token;
            token.ttype=PROSEUP::INS;
            token.is_start=true;
            token.is_end=false;
            tokenlist.append(token);

        }
        else if(splitlist[i]=="+}")
        {
            PROSEUP::Token token;
            token.ttype=PROSEUP::INS;
            token.is_start=false;
            token.is_end=true;
            tokenlist.append(token);
        }
        else if(splitlist[i].startsWith("{") && splitlist[i].endsWith("%"))
        {
            PROSEUP::Token token;
            token.ttype=PROSEUP::HL;
            token.is_start=true;
            token.is_end=false;
            //QRegExp splitter2=QRegExp("\\{%|%%");
            //QStringList splitlist2=splitlist[i].split(splitter2,QString::SkipEmptyParts);
            QStringList splitlist2=splitlist[i].mid(1,splitlist[i].length()-2).split(QChar(';'),QString::KeepEmptyParts);
            if(splitlist2.length()>1||(splitlist2.length()==1&&splitlist2[0].length()>0))
                token.data.append(splitlist2);
            tokenlist.append(token);
        }
        else if(splitlist[i]=="%}")
        {
            PROSEUP::Token token;
            token.ttype=PROSEUP::HL;
            token.is_start=false;
            token.is_end=true;
            tokenlist.append(token);
        }
        else
        {
            PROSEUP::Token token;
            token.ttype=PROSEUP::TEXT;
            token.is_start=false;
            token.is_end=false;
            token.data.append(unescape_string(splitlist[i]));
            tokenlist.append(token);
        }
    }
    if(emph_on && strong_on)
    {
        PROSEUP::Token newtag1,newtag2;
        newtag1.ttype=PROSEUP::EMPH;
        newtag1.is_start=false;
        newtag1.is_end=true;
        newtag2.ttype=PROSEUP::EMPH;
        newtag2.is_start=false;
        newtag2.is_end=true;
        if(lastemph)
        {
            tokenlist.append(newtag2);
            tokenlist.append(newtag1);
        }
        else
        {
            tokenlist.append(newtag1);
            tokenlist.append(newtag2);
        }
    }
    else if(emph_on)
    {
        PROSEUP::Token newtag;
        newtag.ttype=PROSEUP::EMPH;
        newtag.is_start=false;
        newtag.is_end=true;
        tokenlist.append(newtag);
    }
    else if(strong_on)
    {
        PROSEUP::Token newtag;
        newtag.ttype=PROSEUP::EMPH;
        newtag.is_start=false;
        newtag.is_end=true;
        tokenlist.append(newtag);
    }

    return tokenlist;
}

//-----------------------------------------------------------------------------

QList<PROSEUP::Token> PROSEUP::Tokenizer::handle_header(QString& text)
{
    QString textcopy=QString(text);
    int headerlevel;
    for(headerlevel=0;text.at(headerlevel)==QChar('#');headerlevel++);
    int truncatechars;
    for(truncatechars=0;text.at(text.length()-1-truncatechars)==QChar('#');truncatechars++);
    textcopy.chop(truncatechars);
    textcopy=textcopy.remove(0,headerlevel);
    textcopy=textcopy.trimmed();

    QList<PROSEUP::Token> tokenlist;
    PROSEUP::Token starttoken,endtoken;
    starttoken.ttype=PROSEUP::HEADING;
    starttoken.data.append(QString::number(headerlevel));
    starttoken.is_start=true;
    starttoken.is_end=false;
    endtoken.ttype=PROSEUP::HEADING;
    endtoken.data.append(QString::number(headerlevel));
    endtoken.is_start=false;
    endtoken.is_end=true;


    tokenlist.append(starttoken);
    tokenlist.append(this->handle_inline(textcopy));
    tokenlist.append(endtoken);
    return tokenlist;
}

//-----------------------------------------------------------------------------

QList<PROSEUP::Token> PROSEUP::Tokenizer::handle_divider(QString& text)
{

    QList<PROSEUP::Token> tokenlist;

    int dividerlevel;
    for(dividerlevel=0;text.at(dividerlevel)==QChar('@');dividerlevel++);

    PROSEUP::Token dividertoken;
    dividertoken.ttype=PROSEUP::DIVIDER;
    dividertoken.is_start=true;
    dividertoken.is_end=true;
    dividertoken.data.append(QString::number(dividerlevel));
    tokenlist.append(dividertoken);
    return tokenlist;

}

//-----------------------------------------------------------------------------

QList<PROSEUP::Token> PROSEUP::Tokenizer::handle_p(QString& text)
{
    QList<PROSEUP::Token> tokenlist;
    PROSEUP::Token starttoken,endtoken;
    starttoken.ttype=PROSEUP::P;
    starttoken.is_start=true;
    starttoken.is_end=false;
    endtoken.ttype=PROSEUP::P;
    endtoken.is_start=false;
    endtoken.is_end=true;
    tokenlist.append(starttoken);
    tokenlist.append(this->handle_inline(text));
    tokenlist.append(endtoken);
    return tokenlist;
}

//-----------------------------------------------------------------------------

QList<PROSEUP::Token> PROSEUP::Tokenizer::handle_pre(QString& text)
{
    QList<PROSEUP::Token> tokenlist;
    PROSEUP::Token starttoken,endtoken;
    starttoken.ttype=PROSEUP::PRE;
    starttoken.is_start=true;
    starttoken.is_end=false;
    endtoken.ttype=PROSEUP::PRE;
    endtoken.is_start=false;
    endtoken.is_end=true;
    tokenlist.append(starttoken);
    tokenlist.append(this->handle_inline(text));
    tokenlist.append(endtoken);
    return tokenlist;
}

//-----------------------------------------------------------------------------

QList<PROSEUP::Token> PROSEUP::Tokenizer::handle_blockquote(QString& text)
{
    QList<PROSEUP::Token> tokenlist;

    PROSEUP::Token starttoken,endtoken;
    starttoken.ttype=PROSEUP::BLOCKQUOTE;
    starttoken.is_start=true;
    starttoken.is_end=false;

    endtoken.ttype=PROSEUP::BLOCKQUOTE;
    endtoken.is_start=false;
    endtoken.is_end=true;

    QString copytext=QString(text);
    copytext=copytext.remove(0,1);
    if(copytext.endsWith('>'))
    {
        copytext.chop(1);
        copytext=copytext.trimmed();
        tokenlist.append(starttoken);
        tokenlist.append(this->handle_inline(copytext));
        tokenlist.append(endtoken);
    }
    else
    {
        int attind=copytext.lastIndexOf(">-");
        if(attind<0)
        {
            copytext=copytext.trimmed();
            tokenlist.append(starttoken);
            tokenlist.append(this->handle_inline(copytext));
            tokenlist.append(endtoken);
        }
        else
        {
            QString left=copytext.left(attind);
            QString right=copytext.right(copytext.length()-attind-2);
            left=left.trimmed();
            right=right.trimmed();
            PROSEUP::Token attstarttoken,attendtoken;
            attstarttoken.ttype=PROSEUP::ATTRIBUTION;
            attstarttoken.is_start=true;
            attstarttoken.is_end=false;
            attendtoken.ttype=PROSEUP::ATTRIBUTION;
            attendtoken.is_start=false;
            attendtoken.is_end=true;
            tokenlist.append(starttoken);
            tokenlist.append(this->handle_inline(left));
            tokenlist.append(endtoken);
            tokenlist.append(attstarttoken);
            tokenlist.append(this->handle_inline(right));
            tokenlist.append(attendtoken);

        }

    }
    return tokenlist;
}
QString PROSEUP::escape_string(QString text,bool is_at_start) {
        text.replace("\\","\\\\");
        if(is_at_start &&(text.startsWith('-')||text.startsWith('@')||text.startsWith('<')||text.startsWith('#')))
                text.insert(0,QChar('\\'));
        text.replace("*","\\*");
        text.replace("~","\\~");
        text.replace("{","\\{");
        text.replace("}","\\}");
        return text;
}

QString PROSEUP::unescape_string(QString text) {
        text.replace("\\\\","&#92;");
        text.replace("\\","");
        text.replace("&#92;","\\");
        return text;
}

//-----------------------------------------------------------------------------
