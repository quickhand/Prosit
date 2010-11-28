/***********************************************************************
 *
 * Copyright (C) 2009, 2010 Graeme Gott <graeme@gottcode.org>
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

#include "theme.h"

#include "session.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QImageReader>
#include <QPainter>
#include <QSettings>
#include <QUrl>
#include <QStringList>
#include <QTextBlockFormat>
#include <QHash>
//-----------------------------------------------------------------------------

namespace
{
	bool compareFiles(const QString& filename1, const QString& filename2)
	{
		// Compare sizes
		QFile file1(filename1);
		QFile file2(filename2);
		if (file1.size() != file2.size()) {
			return false;
		}

		// Compare contents
		bool equal = true;
		if (file1.open(QFile::ReadOnly) && file2.open(QFile::ReadOnly)) {
			while (!file1.atEnd()) {
				if (file1.read(1000) != file2.read(1000)) {
					equal = false;
					break;
				}
			}
			file1.close();
			file2.close();
		} else {
			equal = false;
		}
		return equal;
	}

	QString copyImage(const QString& image)
	{
		// Check if already copied
		QDir images(Theme::path() + "/Images/");
		QStringList filenames = images.entryList(QDir::Files);
		foreach (const QString& filename, filenames) {
			if (compareFiles(image, images.filePath(filename))) {
				return filename;
			}
		}

		// Find file name
		QString base = QCryptographicHash::hash(image.toUtf8(), QCryptographicHash::Sha1).toHex();
		QString suffix = QFileInfo(image).suffix().toLower();
		QString filename = QString("%1.%2").arg(base, suffix);

		// Handle file name collisions
		int id = 0;
		while (images.exists(filename)) {
			id++;
			filename = QString("%1-%2.%3").arg(base).arg(id).arg(suffix);
		}

		QFile::copy(image, images.filePath(filename));
		return filename;
	}
}

//-----------------------------------------------------------------------------

QString Theme::m_path;

//-----------------------------------------------------------------------------

Theme::Theme(const QString& name)
	: m_name(name)
{
	if (m_name.isEmpty()) {
		QString untitled;
		int count = 0;
		do {
			count++;
			untitled = tr("Untitled %1").arg(count);
		} while (QFile::exists(filePath(untitled)));
		setValue(m_name, untitled);
	}
	QSettings settings(filePath(m_name), QSettings::IniFormat);

	// Load background settings
	m_background_type = settings.value("Background/Type", 0).toInt();
	m_background_color = settings.value("Background/Color", "#cccccc").toString();
	m_background_path = settings.value("Background/Image").toString();
	m_background_image = settings.value("Background/ImageFile").toString();
	if (!m_background_path.isEmpty() && m_background_image.isEmpty()) {
		setValue(m_background_image, copyImage(m_background_path));
	}

	// Load foreground settings
	m_foreground_color = settings.value("Foreground/Color", "#cccccc").toString();
	m_foreground_opacity = qBound(0, settings.value("Foreground/Opacity", 100).toInt(), 100);
	m_foreground_width = qBound(500, settings.value("Foreground/Width", 700).toInt(), 2000);
	m_foreground_margin = qBound(0, settings.value("Foreground/Margin", 65).toInt(), 250);
	m_foreground_padding = qBound(0, settings.value("Foreground/Padding", 0).toInt(), 250);
	m_foreground_position = qBound(0, settings.value("Foreground/Position", 1).toInt(), 3);

	// Load text settings
	m_text_color = settings.value("Text/Color", "#000000").toString();
	m_text_font.fromString(settings.value("Text/Font", QFont().toString()).toString());
	m_misspelled_color = settings.value("Text/Misspelled", "#ff0000").toString();

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

Theme::~Theme()
{
	if (!isChanged()) {
		return;
	}

	QSettings settings(filePath(m_name), QSettings::IniFormat);

	// Store background settings
	settings.setValue("Background/Type", m_background_type);
	settings.setValue("Background/Color", m_background_color.name());
	if (!m_background_path.isEmpty()) {
		settings.setValue("Background/Image", m_background_path);
	}
	settings.setValue("Background/ImageFile", m_background_image);

	// Store foreground settings
	settings.setValue("Foreground/Color", m_foreground_color.name());
	settings.setValue("Foreground/Width", m_foreground_width);
	settings.setValue("Foreground/Margin", m_foreground_margin);
	settings.setValue("Foreground/Padding", m_foreground_padding);
	settings.setValue("Foreground/Opacity", m_foreground_opacity);
	settings.setValue("Foreground/Position", m_foreground_position);

	// Store text settings
	settings.setValue("Text/Color", m_text_color.name());
	settings.setValue("Text/Font", m_text_font.toString());
	settings.setValue("Text/Misspelled", m_misspelled_color.name());
}

//-----------------------------------------------------------------------------

void Theme::copyBackgrounds()
{
	QDir dir(path() + "/Images");
	QStringList images;

	// Copy images
	QStringList themes = QDir(path(), "*.theme").entryList(QDir::Files);
	foreach (const QString& theme, themes) {
		QSettings settings(path() + "/" + theme, QSettings::IniFormat);
		QString background_path = settings.value("Background/Image").toString();
		QString background_image = settings.value("Background/ImageFile").toString();
		if (background_path.isEmpty() && background_image.isEmpty()) {
			continue;
		}
		if (!background_path.isEmpty() && (background_image.isEmpty() || !dir.exists(background_image))) {
			background_image = copyImage(background_path);
			settings.setValue("Background/ImageFile", background_image);
		}
		images.append(background_image);
	}

	// Delete unused images
	QStringList files = dir.entryList(QDir::Files);
	foreach (const QString& file, files) {
		if (!images.contains(file)) {
			QFile::remove(path() + "/Images/" + file);
		}
	}
}

//-----------------------------------------------------------------------------

QImage Theme::renderBackground(const QString& filename, int type, const QColor& background, const QSize& size)
{
	QImage image(size, QImage::Format_RGB32);
	image.fill(background.rgb());

	QPainter painter(&image);
	if (type > 1) {
		QImageReader source(filename);
		QSize scaled = source.size();
		switch (type) {
		case 3:
			scaled.scale(size, Qt::IgnoreAspectRatio);
			break;
		case 4:
			scaled.scale(size, Qt::KeepAspectRatio);
			break;
		case 5:
			scaled.scale(size, Qt::KeepAspectRatioByExpanding);
			break;
		default:
			break;
		}
		source.setScaledSize(scaled);
		painter.drawImage((size.width() - scaled.width()) / 2, (size.height() - scaled.height()) / 2, source.read());
	} else if (type == 1) {
		painter.fillRect(image.rect(), QImage(filename));
	}
	painter.end();
	return image;
}

//-----------------------------------------------------------------------------

QString Theme::path()
{
	return m_path;
}

//-----------------------------------------------------------------------------

QString Theme::filePath(const QString& theme)
{
	return path() + "/" + QUrl::toPercentEncoding(theme, " ") + ".theme";
}

//-----------------------------------------------------------------------------

QString Theme::iconPath(const QString& theme)
{
	return path() + "/" + QUrl::toPercentEncoding(theme, " ") + ".png";
}

//-----------------------------------------------------------------------------

void Theme::setPath(const QString& path)
{
	m_path = path;
}

//-----------------------------------------------------------------------------

QString Theme::name() const
{
	return m_name;
}

//-----------------------------------------------------------------------------

void Theme::setName(const QString& name)
{
	if (m_name != name) {
		QStringList files = QDir(Session::path(), "*.session").entryList(QDir::Files);
		files.prepend("");
		foreach (const QString& file, files) {
			Session session(file);
			if (session.theme() == m_name) {
				session.setTheme(name);
			}
		}

		QFile::remove(filePath(m_name));
		QFile::remove(iconPath(m_name));
		setValue(m_name, name);
	}
}

//-----------------------------------------------------------------------------

int Theme::backgroundType() const
{
	return m_background_type;
}

//-----------------------------------------------------------------------------

QColor Theme::backgroundColor() const
{
	return m_background_color;
}

//-----------------------------------------------------------------------------

QString Theme::backgroundImage() const
{
	return path() + "/Images/" + m_background_image;
}

//-----------------------------------------------------------------------------

QString Theme::backgroundPath() const
{
	return m_background_path;
}

//-----------------------------------------------------------------------------

void Theme::setBackgroundType(int type)
{
	setValue(m_background_type, type);
}

//-----------------------------------------------------------------------------

void Theme::setBackgroundColor(const QColor& color)
{
	setValue(m_background_color, color);
}

//-----------------------------------------------------------------------------

void Theme::setBackgroundImage(const QString& path)
{
	if (m_background_path != path) {
		setValue(m_background_path, path);
		if (!m_background_path.isEmpty()) {
			m_background_image = copyImage(m_background_path);
		} else {
			m_background_image.clear();
		}
	}
}

//-----------------------------------------------------------------------------

QColor Theme::foregroundColor() const
{
	return m_foreground_color;
}

//-----------------------------------------------------------------------------

int Theme::foregroundWidth() const
{
	return m_foreground_width;
}

//-----------------------------------------------------------------------------

int Theme::foregroundMargin() const
{
	return m_foreground_margin;
}

//-----------------------------------------------------------------------------

int Theme::foregroundPadding() const
{
	return m_foreground_padding;
}

//-----------------------------------------------------------------------------

int Theme::foregroundOpacity() const
{
	return m_foreground_opacity;
}

//-----------------------------------------------------------------------------

int Theme::foregroundPosition() const
{
	return m_foreground_position;
}

//-----------------------------------------------------------------------------

void Theme::setForegroundColor(const QColor& color)
{
	setValue(m_foreground_color, color);
}

//-----------------------------------------------------------------------------

void Theme::setForegroundWidth(int width)
{
	setValue(m_foreground_width, width);
}

//-----------------------------------------------------------------------------

void Theme::setForegroundMargin(int margin)
{
	setValue(m_foreground_margin, margin);
}

//-----------------------------------------------------------------------------

void Theme::setForegroundPadding(int padding)
{
	setValue(m_foreground_padding, padding);
}

//-----------------------------------------------------------------------------

void Theme::setForegroundOpacity(int opacity)
{
	setValue(m_foreground_opacity, opacity);
}

//-----------------------------------------------------------------------------

void Theme::setForegroundPosition(int position)
{
	setValue(m_foreground_position, position);
}

//-----------------------------------------------------------------------------

QColor Theme::textColor() const
{
	return m_text_color;
}

//-----------------------------------------------------------------------------

QFont Theme::textFont() const
{
	return m_text_font;
}

//-----------------------------------------------------------------------------

QColor Theme::misspelledColor() const
{
	return m_misspelled_color;
}

//-----------------------------------------------------------------------------

void Theme::setTextColor(const QColor& color)
{
	setValue(m_text_color, color);
}

//-----------------------------------------------------------------------------

void Theme::setTextFont(const QFont& font)
{
	setValue(m_text_font, font);
}

//-----------------------------------------------------------------------------

void Theme::setMisspelledColor(const QColor& color)
{
	setValue(m_misspelled_color, color);
}

//-----------------------------------------------------------------------------

QTextBlockFormat Theme::defaultFormatForBlock(QString uprop) const
{
    if(!m_block_default_format.keys().contains(uprop))
        return m_block_default_format.find("default").value();

    return m_block_default_format.find(uprop).value();
}

QStringList Theme::definedDefaultFormatsForBlocks() const
{
    QStringList retlist=QStringList(m_block_default_format.keys());
    return retlist;
}
