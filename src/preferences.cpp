/***********************************************************************
 *
 * Copyright (C) 2008, 2009, 2010 Graeme Gott <graeme@gottcode.org>
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

#include "preferences.h"

#include "dictionary.h"

#include <QLocale>
#include <QSettings>

//-----------------------------------------------------------------------------

Preferences::Preferences()
{
	QSettings settings;

	m_goal_type = settings.value("Goal/Type", 1).toInt();
	m_goal_minutes = settings.value("Goal/Minutes", 30).toInt();
	m_goal_words = settings.value("Goal/Words", 1000).toInt();

	m_show_characters = settings.value("Stats/ShowCharacters", true).toBool();
	m_show_pages = settings.value("Stats/ShowPages", true).toBool();
	m_show_paragraphs = settings.value("Stats/ShowParagraphs", true).toBool();
	m_show_words = settings.value("Stats/ShowWords", true).toBool();

	m_page_type = settings.value("Stats/PageSizeType", 0).toInt();
	m_page_characters = settings.value("Stats/CharactersPerPage", 1500).toInt();
	m_page_paragraphs = settings.value("Stats/ParagraphsPerPage", 5).toInt();
	m_page_words = settings.value("Stats/WordsPerPage", 250).toInt();

	m_accurate_wordcount = settings.value("Stats/AccurateWordcount", true).toBool();

	m_always_center = settings.value("Edit/AlwaysCenter", false).toBool();
	m_block_cursor = settings.value("Edit/BlockCursor", false).toBool();
	m_rich_text = settings.value("Edit/RichText", false).toBool();
	m_smooth_fonts = settings.value("Edit/SmoothFonts", true).toBool();
	m_smart_quotes = settings.value("Edit/SmartQuotes", true).toBool();
	m_double_quotes = settings.value("Edit/SmartDoubleQuotes", -1).toInt();
	m_single_quotes = settings.value("Edit/SmartSingleQuotes", -1).toInt();
	m_typewriter_sounds = settings.value("Edit/TypewriterSounds", true).toBool();

	m_auto_save = settings.value("Save/Auto", true).toBool();
	m_save_positions = settings.value("Save/RememberPositions", true).toBool();

	m_toolbar_style = settings.value("Toolbar/Style", Qt::ToolButtonTextUnderIcon).toInt();
	m_toolbar_actions = QStringList() << "New" << "Open" << "Save" << "|" << "Undo" << "Redo" << "|" << "Cut" << "Copy" << "Paste" << "|" << "Find" << "Replace";
	m_toolbar_actions = settings.value("Toolbar/Actions", m_toolbar_actions).toStringList();

	m_highlight_misspelled = settings.value("Spelling/HighlightMisspelled", true).toBool();
	m_ignore_numbers = settings.value("Spelling/IgnoreNumbers", true).toBool();
	m_ignore_uppercase = settings.value("Spelling/IgnoreUppercase", true).toBool();
	m_language = settings.value("Spelling/Language", QLocale::system().name()).toString();

	m_dictionary = new Dictionary;
	QStringList languages = m_dictionary->availableLanguages();
	if (!languages.isEmpty() && !languages.contains(m_language)) {
		int close = languages.indexOf(QRegExp(m_language.left(2) + ".*"));
		m_language = (close != -1) ? languages.at(close) : (languages.contains("en_US") ? "en_US" : languages.first());
	}
	m_dictionary->setLanguage(m_language);
	m_dictionary->setIgnoreNumbers(m_ignore_numbers);
	m_dictionary->setIgnoreUppercase(m_ignore_uppercase);

        //setup paragraph styles
        QTextBlockFormat baseformat;
        baseformat.setBottomMargin(10.0);
        baseformat.setAlignment(Qt::AlignLeft);
        m_block_default_format.insert("default",baseformat);
        QTextBlockFormat h1format=QTextBlockFormat(baseformat),h2format=QTextBlockFormat(baseformat),h3format=QTextBlockFormat(baseformat),h4format=QTextBlockFormat(baseformat),h5format=QTextBlockFormat(baseformat);
        h1format.setProperty(QTextFormat::UserProperty,"H1");
        h2format.setProperty(QTextFormat::UserProperty,"H2");
        h3format.setProperty(QTextFormat::UserProperty,"H3");
        h4format.setProperty(QTextFormat::UserProperty,"H4");
        h5format.setProperty(QTextFormat::UserProperty,"H5");
        m_block_default_format.insert("H1",h1format);
        m_block_default_format.insert("H2",h2format);
        m_block_default_format.insert("H3",h3format);
        m_block_default_format.insert("H4",h4format);
        m_block_default_format.insert("H5",h5format);
        QTextBlockFormat bqformat=QTextBlockFormat(baseformat);
        bqformat.setProperty(QTextFormat::UserProperty,"BLOCKQUOTE");
        bqformat.setLeftMargin(50.0);
        bqformat.setRightMargin(50.0);
        m_block_default_format.insert("BLOCKQUOTE",bqformat);
        QTextBlockFormat attformat=QTextBlockFormat(baseformat);
        attformat.setAlignment(Qt::AlignRight);
        attformat.setProperty(QTextFormat::UserProperty,"ATTRIBUTION");
        attformat.setBottomMargin(15.0);
        attformat.setLeftMargin(50.0);
        attformat.setRightMargin(50.0);
        attformat.setNonBreakableLines(true);
        m_block_default_format.insert("ATTRIBUTION",attformat);
        QTextBlockFormat preformat=QTextBlockFormat();
        preformat.setProperty(QTextFormat::UserProperty,"PRE");
        preformat.setNonBreakableLines(true);
        m_block_default_format.insert("PRE",preformat);
        QTextBlockFormat d1format=QTextBlockFormat();
        d1format.setProperty(QTextFormat::UserProperty,"DIVIDER1");
        m_block_default_format.insert("DIVIDER1",d1format);
        QTextBlockFormat d2format=QTextBlockFormat();
        d2format.setProperty(QTextFormat::UserProperty,"DIVIDER2");
        d2format.setTopMargin(5);
        d2format.setBottomMargin(5);
        m_block_default_format.insert("DIVIDER2",d2format);
        QTextBlockFormat d3format=QTextBlockFormat();
        d3format.setProperty(QTextFormat::UserProperty,"DIVIDER3");
        d3format.setTopMargin(5);
        d3format.setBottomMargin(10);
        d3format.setProperty(QTextFormat::BlockTrailingHorizontalRulerWidth,QTextLength(QTextLength::PercentageLength,20));
        m_block_default_format.insert("DIVIDER3",d3format);
        QTextBlockFormat d4format=QTextBlockFormat();
        d4format.setProperty(QTextFormat::UserProperty,"DIVIDER4");
        d4format.setTopMargin(10);
        d4format.setBottomMargin(15);
        d4format.setProperty(QTextFormat::BlockTrailingHorizontalRulerWidth,QTextLength(QTextLength::PercentageLength,50));
        m_block_default_format.insert("DIVIDER4",d4format);
        QTextBlockFormat d5format=QTextBlockFormat();
        d5format.setProperty(QTextFormat::UserProperty,"DIVIDER5");
        d5format.setTopMargin(15);
        d5format.setBottomMargin(25);
        d5format.setProperty(QTextFormat::BlockTrailingHorizontalRulerWidth,QTextLength(QTextLength::PercentageLength,100));
        m_block_default_format.insert("DIVIDER5",d5format);
}

//-----------------------------------------------------------------------------

Preferences::~Preferences()
{
	delete m_dictionary;
	if (!isChanged()) {
		return;
	}

	QSettings settings;

	settings.setValue("Goal/Type", m_goal_type);
	settings.setValue("Goal/Minutes", m_goal_minutes);
	settings.setValue("Goal/Words", m_goal_words);

	settings.setValue("Stats/ShowCharacters", m_show_characters);
	settings.setValue("Stats/ShowPages", m_show_pages);
	settings.setValue("Stats/ShowParagraphs", m_show_paragraphs);
	settings.setValue("Stats/ShowWords", m_show_words);

	settings.setValue("Stats/PageSizeType", m_page_type);
	settings.setValue("Stats/CharactersPerPage", m_page_characters);
	settings.setValue("Stats/ParagraphsPerPage", m_page_paragraphs);
	settings.setValue("Stats/WordsPerPage", m_page_words);

	settings.setValue("Stats/AccurateWordcount", m_accurate_wordcount);

	settings.setValue("Edit/AlwaysCenter", m_always_center);
	settings.setValue("Edit/BlockCursor", m_block_cursor);
	settings.setValue("Edit/RichText", m_rich_text);
	settings.setValue("Edit/SmoothFonts", m_smooth_fonts);
	settings.setValue("Edit/SmartQuotes", m_smart_quotes);
	settings.setValue("Edit/SmartDoubleQuotes", m_double_quotes);
	settings.setValue("Edit/SmartSingleQuotes", m_single_quotes);
	settings.setValue("Edit/TypewriterSounds", m_typewriter_sounds);

	settings.setValue("Save/Auto", m_auto_save);
	settings.setValue("Save/RememberPositions", m_save_positions);

	settings.setValue("Toolbar/Style", m_toolbar_style);
	settings.setValue("Toolbar/Actions", m_toolbar_actions);

	settings.setValue("Spelling/HighlightMisspelled", m_highlight_misspelled);
	settings.setValue("Spelling/IgnoreNumbers", m_ignore_numbers);
	settings.setValue("Spelling/IgnoreUppercase", m_ignore_uppercase);
	settings.setValue("Spelling/Language", m_language);
}

//-----------------------------------------------------------------------------

int Preferences::goalType() const
{
	return m_goal_type;
}

//-----------------------------------------------------------------------------

int Preferences::goalMinutes() const
{
	return m_goal_minutes;
}

//-----------------------------------------------------------------------------

int Preferences::goalWords() const
{
	return m_goal_words;
}

//-----------------------------------------------------------------------------

void Preferences::setGoalType(int goal)
{
	setValue(m_goal_type, goal);
}

//-----------------------------------------------------------------------------

void Preferences::setGoalMinutes(int goal)
{
	setValue(m_goal_minutes, goal);
}

//-----------------------------------------------------------------------------

void Preferences::setGoalWords(int goal)
{
	setValue(m_goal_words, goal);
}

//-----------------------------------------------------------------------------

bool Preferences::showCharacters() const
{
	return m_show_characters;
}

//-----------------------------------------------------------------------------

bool Preferences::showPages() const
{
	return m_show_pages;
}

//-----------------------------------------------------------------------------

bool Preferences::showParagraphs() const
{
	return m_show_paragraphs;
}

//-----------------------------------------------------------------------------

bool Preferences::showWords() const
{
	return m_show_words;
}

//-----------------------------------------------------------------------------

void Preferences::setShowCharacters(bool show)
{
	setValue(m_show_characters, show);
}

//-----------------------------------------------------------------------------

void Preferences::setShowPages(bool show)
{
	setValue(m_show_pages, show);
}

//-----------------------------------------------------------------------------

void Preferences::setShowParagraphs(bool show)
{
	setValue(m_show_paragraphs, show);
}

//-----------------------------------------------------------------------------

void Preferences::setShowWords(bool show)
{
	setValue(m_show_words, show);
}

//-----------------------------------------------------------------------------

int Preferences::pageType() const
{
	return m_page_type;
}

//-----------------------------------------------------------------------------

int Preferences::pageCharacters() const
{
	return m_page_characters;
}

//-----------------------------------------------------------------------------

int Preferences::pageParagraphs() const
{
	return m_page_paragraphs;
}

//-----------------------------------------------------------------------------

int Preferences::pageWords() const
{
	return m_page_words;
}

//-----------------------------------------------------------------------------

void Preferences::setPageType(int type)
{
	setValue(m_page_type, type);
}

//-----------------------------------------------------------------------------

void Preferences::setPageCharacters(int characters)
{
	setValue(m_page_characters, characters);
}

//-----------------------------------------------------------------------------

void Preferences::setPageParagraphs(int paragraphs)
{
	setValue(m_page_paragraphs, paragraphs);
}

//-----------------------------------------------------------------------------

void Preferences::setPageWords(int words)
{
	setValue(m_page_words, words);
}

//-----------------------------------------------------------------------------

bool Preferences::accurateWordcount() const
{
	return m_accurate_wordcount;
}

//-----------------------------------------------------------------------------

void Preferences::setAccurateWordcount(bool accurate)
{
	setValue(m_accurate_wordcount, accurate);
}

//-----------------------------------------------------------------------------

bool Preferences::alwaysCenter() const
{
	return m_always_center;
}

//-----------------------------------------------------------------------------

bool Preferences::blockCursor() const
{
	return m_block_cursor;
}

//-----------------------------------------------------------------------------

bool Preferences::richText() const
{
	return m_rich_text;
}

//-----------------------------------------------------------------------------

bool Preferences::smoothFonts() const
{
	return m_smooth_fonts;
}

//-----------------------------------------------------------------------------

bool Preferences::smartQuotes() const
{
	return m_smart_quotes;
}

//-----------------------------------------------------------------------------

int Preferences::doubleQuotes() const
{
	return m_double_quotes;
}

//-----------------------------------------------------------------------------

int Preferences::singleQuotes() const
{
	return m_single_quotes;
}

//-----------------------------------------------------------------------------

bool Preferences::typewriterSounds() const
{
	return m_typewriter_sounds;
}

//-----------------------------------------------------------------------------

void Preferences::setAlwaysCenter(bool center)
{
	setValue(m_always_center, center);
}

//-----------------------------------------------------------------------------

void Preferences::setBlockCursor(bool block)
{
	setValue(m_block_cursor, block);
}

//-----------------------------------------------------------------------------

void Preferences::setRichText(bool rich)
{
	setValue(m_rich_text, rich);
}

//-----------------------------------------------------------------------------

void Preferences::setSmoothFonts(bool smooth)
{
	setValue(m_smooth_fonts, smooth);
}

//-----------------------------------------------------------------------------

void Preferences::setSmartQuotes(bool quotes)
{
	setValue(m_smart_quotes, quotes);
}

//-----------------------------------------------------------------------------

void Preferences::setDoubleQuotes(int quotes)
{
	setValue(m_double_quotes, quotes);
}

//-----------------------------------------------------------------------------

void Preferences::setSingleQuotes(int quotes)
{
	setValue(m_single_quotes, quotes);
}

//-----------------------------------------------------------------------------

void Preferences::setTypewriterSounds(bool sounds)
{
	setValue(m_typewriter_sounds, sounds);
}

//-----------------------------------------------------------------------------

bool Preferences::autoSave() const
{
	return m_auto_save;
}

//-----------------------------------------------------------------------------

bool Preferences::savePositions() const
{
	return m_save_positions;
}

//-----------------------------------------------------------------------------

void Preferences::setAutoSave(bool save)
{
	setValue(m_auto_save, save);
}

//-----------------------------------------------------------------------------

void Preferences::setSavePositions(bool save)
{
	setValue(m_save_positions, save);
}

//-----------------------------------------------------------------------------

int Preferences::toolbarStyle() const
{
	return m_toolbar_style;
}

//-----------------------------------------------------------------------------

QStringList Preferences::toolbarActions() const
{
	return m_toolbar_actions;
}

//-----------------------------------------------------------------------------

void Preferences::setToolbarStyle(int style)
{
	setValue(m_toolbar_style, style);
}

//-----------------------------------------------------------------------------

void Preferences::setToolbarActions(const QStringList& actions)
{
	setValue(m_toolbar_actions, actions);
}

//-----------------------------------------------------------------------------

bool Preferences::highlightMisspelled() const
{
	return m_highlight_misspelled;
}

//-----------------------------------------------------------------------------

bool Preferences::ignoredWordsWithNumbers() const
{
	return m_ignore_numbers;
}

//-----------------------------------------------------------------------------

bool Preferences::ignoredUppercaseWords() const
{
	return m_ignore_uppercase;
}

//-----------------------------------------------------------------------------

QString Preferences::language() const
{
	return m_language;
}

//-----------------------------------------------------------------------------

void Preferences::setHighlightMisspelled(bool highlight)
{
	setValue(m_highlight_misspelled, highlight);
}

//-----------------------------------------------------------------------------

void Preferences::setIgnoreWordsWithNumbers(bool ignore)
{
	setValue(m_ignore_numbers, ignore);
}

//-----------------------------------------------------------------------------

void Preferences::setIgnoreUppercaseWords(bool ignore)
{
	setValue(m_ignore_uppercase, ignore);
}

//-----------------------------------------------------------------------------

void Preferences::setLanguage(const QString& language)
{
	setValue(m_language, language);
}

//-----------------------------------------------------------------------------

QTextBlockFormat Preferences::defaultFormatForBlock(QString uprop) const
{
    if(!m_block_default_format.keys().contains(uprop))
        return m_block_default_format.find("default").value();

    return m_block_default_format.find(uprop).value();
}
QStringList Preferences::definedDefaultFormatsForBlocks() const
{
    QStringList retlist=QStringList(m_block_default_format.keys());
    return retlist;
}
