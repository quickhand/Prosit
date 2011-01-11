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

#include "theme_dialog.h"

#include "color_button.h"
#include "image_button.h"
#include "theme.h"

#include <QDialogButtonBox>
#include <QFile>
#include <QFontComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QTextBlockFormat>
#include <iostream>

//-----------------------------------------------------------------------------

ThemeDialog::ThemeDialog(Theme& theme, QWidget* parent)
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint),
	m_theme(theme)
{
	setWindowTitle(tr("Modify Theme"));

	// Create name edit
	m_name = new QLineEdit(this);
	m_name->setText(m_theme.name());
	connect(m_name, SIGNAL(textChanged(const QString&)), this, SLOT(checkNameAvailable()));

	QHBoxLayout* name_layout = new QHBoxLayout;
	name_layout->setMargin(0);
	name_layout->addWidget(new QLabel(tr("Name:"), this));
	name_layout->addWidget(m_name);


	QTabWidget* tabs = new QTabWidget(this);

	// Create background group
	QWidget* tab = new QWidget(this);
	tabs->addTab(tab, tr("Background"));

	m_background_type = new QComboBox(tab);
	m_background_type->addItems(QStringList() << tr("No Image") << tr("Tiled") << tr("Centered") << tr("Stretched") << tr("Scaled") << tr("Zoomed"));
	m_background_type->setCurrentIndex(m_theme.backgroundType());
	connect(m_background_type, SIGNAL(activated(int)), this, SLOT(renderPreview()));

	m_background_color = new ColorButton(tab);
	m_background_color->setColor(m_theme.backgroundColor());
	connect(m_background_color, SIGNAL(changed(const QColor&)), this, SLOT(renderPreview()));

	m_background_image = new ImageButton(tab);
	m_background_image->setImage(m_theme.backgroundImage(), m_theme.backgroundPath());
	connect(m_background_image, SIGNAL(changed(const QString&)), this, SLOT(renderPreview()));

	m_clear_image = new QPushButton(tr("Remove"), this);
	connect(m_clear_image, SIGNAL(clicked()), m_background_image, SLOT(unsetImage()));

	QVBoxLayout* image_layout = new QVBoxLayout;
	image_layout->setSpacing(0);
	image_layout->addWidget(m_background_image);
	image_layout->addWidget(m_clear_image);

	QFormLayout* background_layout = new QFormLayout(tab);
	background_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	background_layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
	background_layout->setLabelAlignment(Qt::AlignRight);
	background_layout->addRow(tr("Type:"), m_background_type);
	background_layout->addRow(tr("Color:"), m_background_color);
	background_layout->addRow(tr("Image:"), image_layout);


	// Create foreground group
	tab = new QWidget(this);
	tabs->addTab(tab, tr("Foreground"));

	m_foreground_color = new ColorButton(tab);
	m_foreground_color->setColor(m_theme.foregroundColor());
	connect(m_foreground_color, SIGNAL(changed(const QColor&)), this, SLOT(renderPreview()));

	m_foreground_opacity = new QSpinBox(tab);
	m_foreground_opacity->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_foreground_opacity->setSuffix("%");
	m_foreground_opacity->setRange(0, 100);
	m_foreground_opacity->setValue(m_theme.foregroundOpacity());
	m_foreground_opacity->setToolTip(tr("Opacity"));
	connect(m_foreground_opacity, SIGNAL(valueChanged(int)), this, SLOT(renderPreview()));

	m_foreground_width = new QSpinBox(tab);
	m_foreground_width->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_foreground_width->setSuffix(tr(" pixels"));
	m_foreground_width->setRange(500, 2000);
	m_foreground_width->setValue(m_theme.foregroundWidth());

	m_foreground_position = new QComboBox(tab);
	m_foreground_position->addItems(QStringList() << tr("Left") << tr("Centered") << tr("Right") << tr("Stretched"));
	m_foreground_position->setCurrentIndex(m_theme.foregroundPosition());
	m_foreground_position->setToolTip(tr("Position"));

	m_foreground_rounding = new QSpinBox(tab);
	m_foreground_rounding->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_foreground_rounding->setSuffix(tr(" pixels"));
	m_foreground_rounding->setRange(0, 100);
	m_foreground_rounding->setValue(m_theme.foregroundRounding());

	m_foreground_margin = new QSpinBox(tab);
	m_foreground_margin->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_foreground_margin->setSuffix(tr(" pixels"));
	m_foreground_margin->setRange(10, 250);
	m_foreground_margin->setValue(m_theme.foregroundMargin());

	m_foreground_padding = new QSpinBox(tab);
	m_foreground_padding->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_foreground_padding->setSuffix(tr(" pixels"));
	m_foreground_padding->setRange(0, 250);
	m_foreground_padding->setValue(m_theme.foregroundPadding());

	QHBoxLayout* color_layout = new QHBoxLayout;
	color_layout->setMargin(0);
	color_layout->addWidget(m_foreground_color);
	color_layout->addWidget(m_foreground_opacity);

	QHBoxLayout* size_layout = new QHBoxLayout;
	size_layout->setMargin(0);
	size_layout->addWidget(m_foreground_width);
	size_layout->addWidget(m_foreground_position);

	QFormLayout* foreground_layout = new QFormLayout(tab);
	foreground_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	foreground_layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
	foreground_layout->setLabelAlignment(Qt::AlignRight);
	foreground_layout->addRow(tr("Color:"), color_layout);
	foreground_layout->addRow(tr("Size:"), size_layout);
	foreground_layout->addRow(tr("Rounding:"), m_foreground_rounding);
	foreground_layout->addRow(tr("Margin:"), m_foreground_margin);
	foreground_layout->addRow(tr("Padding:"), m_foreground_padding);


	// Create text group
	tab = new QWidget(this);
	tabs->addTab(tab, tr("Text"));

	m_text_color = new ColorButton(tab);
	m_text_color->setColor(m_theme.textColor());
	connect(m_text_color, SIGNAL(changed(const QColor&)), this, SLOT(renderPreview()));

	m_font_names = new QFontComboBox(tab);
	m_font_names->setEditable(false);
	m_font_names->setCurrentFont(m_theme.textFont());
	connect(m_font_names, SIGNAL(activated(int)), this, SLOT(renderPreview()));

	m_font_sizes = new QComboBox(tab);
	QList<int> font_sizes = QFontDatabase::standardSizes();
	int font_size = QFontInfo(m_theme.textFont()).pointSize();
	int index = 0;
	for (int i = 0; i < font_sizes.count(); ++i) {
		int size = font_sizes.at(i);
		if (size <= font_size) {
			index = i;
		}
		m_font_sizes->addItem(QString::number(size));
	}
	m_font_sizes->setCurrentIndex(index);
	connect(m_font_sizes, SIGNAL(activated(int)), this, SLOT(renderPreview()));

	m_misspelled_color = new ColorButton(tab);
	m_misspelled_color->setColor(m_theme.misspelledColor());
	connect(m_misspelled_color, SIGNAL(changed(const QColor&)), this, SLOT(renderPreview()));

	QHBoxLayout* font_layout = new QHBoxLayout;
	font_layout->addWidget(m_font_names);
	font_layout->addWidget(m_font_sizes);

	QFormLayout* text_layout = new QFormLayout(tab);
	text_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	text_layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
	text_layout->setLabelAlignment(Qt::AlignRight);
	text_layout->addRow(tr("Color:"), m_text_color);
	text_layout->addRow(tr("Font:"), font_layout);
	text_layout->addRow(tr("Misspelled:"), m_misspelled_color);

        // Create styling group
        tab = new QWidget(this);
        tabs->addTab(tab, tr("Text Styling"));
        m_current_block_type = new QComboBox(tab);

        m_current_block_type->addItem(tr("Normal Paragraph"),m_theme.defaultFormatForBlock("default"));
        m_current_block_type->addItem(tr("Heading 1"),m_theme.defaultFormatForBlock("H1"));
        m_current_block_type->addItem(tr("Heading 2"),m_theme.defaultFormatForBlock("H2"));
        m_current_block_type->addItem(tr("Heading 3"),m_theme.defaultFormatForBlock("H3"));
        m_current_block_type->addItem(tr("Heading 4"),m_theme.defaultFormatForBlock("H4"));
        m_current_block_type->addItem(tr("Heading 5"),m_theme.defaultFormatForBlock("H5"));
        m_current_block_type->addItem(tr("Blockquote"),m_theme.defaultFormatForBlock("BLOCKQUOTE"));
        m_current_block_type->addItem(tr("Attribution"),m_theme.defaultFormatForBlock("ATTRIBUTION"));
        m_current_block_type->addItem(tr("Preformatted Text"),m_theme.defaultFormatForBlock("PRE"));
        connect(m_current_block_type,SIGNAL(currentIndexChanged(int)),this,SLOT(updateStylingControls(int)));

        m_styling_adjustment = new QSpinBox(tab);
        m_styling_adjustment->setCorrectionMode(QSpinBox::CorrectToNearestValue);
        m_styling_adjustment->setRange(-5,5);
        connect(m_styling_adjustment,SIGNAL(valueChanged(int)),this,SLOT(updateStylingFormat()));

        m_styling_bold = new QCheckBox(tab);
        connect(m_styling_bold,SIGNAL(toggled(bool)),this,SLOT(updateStylingFormat()));

        m_styling_italic = new QCheckBox(tab);
        connect(m_styling_italic,SIGNAL(toggled(bool)),this,SLOT(updateStylingFormat()));

        m_styling_align = new QComboBox(tab);
        m_styling_align->addItems(QStringList() << tr("Left") << tr("Center") << tr("Right"));
        connect(m_styling_align,SIGNAL(currentIndexChanged(int)),this,SLOT(updateStylingFormat()));

        m_styling_l_margin = new QSpinBox(tab);
        m_styling_l_margin->setCorrectionMode(QSpinBox::CorrectToNearestValue);
        m_styling_l_margin->setSuffix(tr(" pixels"));
        m_styling_l_margin->setRange(0, 250);
        connect(m_styling_l_margin,SIGNAL(valueChanged(int)),this,SLOT(updateStylingFormat()));

        m_styling_r_margin = new QSpinBox(tab);
        m_styling_r_margin->setCorrectionMode(QSpinBox::CorrectToNearestValue);
        m_styling_r_margin->setSuffix(tr(" pixels"));
        m_styling_r_margin->setRange(0, 250);
        connect(m_styling_r_margin,SIGNAL(valueChanged(int)),this,SLOT(updateStylingFormat()));

        m_styling_t_margin = new QSpinBox(tab);
        m_styling_t_margin->setCorrectionMode(QSpinBox::CorrectToNearestValue);
        m_styling_t_margin->setSuffix(tr(" pixels"));
        m_styling_t_margin->setRange(0, 250);
        connect(m_styling_t_margin,SIGNAL(valueChanged(int)),this,SLOT(updateStylingFormat()));

        m_styling_b_margin = new QSpinBox(tab);
        m_styling_b_margin->setCorrectionMode(QSpinBox::CorrectToNearestValue);
        m_styling_b_margin->setSuffix(tr(" pixels"));
        m_styling_b_margin->setRange(0, 250);
        connect(m_styling_b_margin,SIGNAL(valueChanged(int)),this,SLOT(updateStylingFormat()));

        QVBoxLayout* styling_layout = new QVBoxLayout(tab);
        QFormLayout* styling_options_layout = new QFormLayout();
        styling_options_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
        styling_options_layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
        styling_options_layout->setLabelAlignment(Qt::AlignRight);
        styling_options_layout->addRow(tr("Size Adjustment:"), m_styling_adjustment);
        styling_options_layout->addRow(tr("Bold:"), m_styling_bold);
        styling_options_layout->addRow(tr("Italic:"), m_styling_italic);
        styling_options_layout->addRow(tr("Alignment:"), m_styling_align);
        styling_options_layout->addRow(tr("Left Margin:"), m_styling_l_margin);
        styling_options_layout->addRow(tr("Right Margin:"), m_styling_r_margin);
        styling_options_layout->addRow(tr("Top Margin:"), m_styling_t_margin);
        styling_options_layout->addRow(tr("Bottom Margin:"), m_styling_b_margin);

        styling_layout->addWidget(m_current_block_type);
        styling_layout->addLayout(styling_options_layout);


	// Create preview
	m_preview = new QLabel(this);
	m_preview->setAlignment(Qt::AlignCenter);
	renderPreview();


	// Lay out dialog
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	m_ok = buttons->button(QDialogButtonBox::Ok);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QHBoxLayout* contents_layout = new QHBoxLayout;
	contents_layout->setMargin(0);
	contents_layout->addWidget(tabs);
	contents_layout->addWidget(m_preview);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSpacing(12);
	layout->addLayout(name_layout);
	layout->addLayout(contents_layout);
	layout->addWidget(buttons);

        updateStylingControls(m_current_block_type->currentIndex());
}

//-----------------------------------------------------------------------------

void ThemeDialog::createPreview(const QString& name)
{
	Theme theme(name);
	ThemeDialog dialog(theme);
	dialog.savePreview();
}

//-----------------------------------------------------------------------------

void ThemeDialog::accept()
{
	m_theme.setName(m_name->text());

	m_theme.setBackgroundType(m_background_type->currentIndex());
	m_theme.setBackgroundColor(m_background_color->color());
	m_theme.setBackgroundImage(m_background_image->toString());

	m_theme.setForegroundColor(m_foreground_color->color());
	m_theme.setForegroundOpacity(m_foreground_opacity->value());
	m_theme.setForegroundWidth(m_foreground_width->value());
	m_theme.setForegroundRounding(m_foreground_rounding->value());
	m_theme.setForegroundMargin(m_foreground_margin->value());
	m_theme.setForegroundPadding(m_foreground_padding->value());
	m_theme.setForegroundPosition(m_foreground_position->currentIndex());

	m_theme.setTextColor(m_text_color->color());
	m_theme.setTextFont(QFont(m_font_names->currentFont().family(), m_font_sizes->currentText().toInt()));
	m_theme.setMisspelledColor(m_misspelled_color->color());

        for(int index=0;index<m_current_block_type->count();index++)
        {
            QTextBlockFormat bf=m_current_block_type->itemData(index).value<QTextFormat>().toBlockFormat();
            QString uprop=bf.stringProperty(QTextFormat::UserProperty);
            if(uprop=="")
                uprop="default";
            m_theme.setDefaultFormatForBlock(uprop,bf);
        }

	savePreview();

	QDialog::accept();
}

//-----------------------------------------------------------------------------

void ThemeDialog::checkNameAvailable()
{
	QString name = m_name->text();
	bool empty = name.isEmpty();
	bool changed = (name != m_theme.name());
	bool exists = QFile::exists(Theme::filePath(name));
	m_ok->setEnabled(!changed || (!empty && !exists));
}

//-----------------------------------------------------------------------------

void ThemeDialog::renderPreview()
{
	QPixmap preview(":/shadow.png");
	{
		QPainter painter(&preview);
		painter.translate(9, 6);
		painter.setClipRect(0, 0, 200, 150);

		int type = m_background_type->currentIndex();
		m_background_image->setEnabled(type > 0);
		m_clear_image->setEnabled(m_background_image->isEnabled() && !m_background_image->image().isEmpty());

		// Draw background
		QImage background = Theme::renderBackground(m_background_image->image(), type, m_background_color->color(), QSize(200, 150));
		painter.drawImage(QPoint(99, 74) - background.rect().center(), background);

		// Draw foreground
		QColor color = m_foreground_color->color();
		color.setAlpha(m_foreground_opacity->value() * 2.55f);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setBrush(color);
		painter.setPen(Qt::NoPen);
		painter.drawRoundedRect(QRect(20, 20, 160, 110), m_foreground_rounding->value(), m_foreground_rounding->value());

		// Draw text
		painter.setPen(m_text_color->color());
		painter.setFont(QFont(m_font_names->currentFont().family(), m_font_sizes->currentText().toInt()));
		painter.drawText(QRect(23, 23, 154, 104), Qt::TextWordWrap, tr("The quick brown fox jumps over the lazy dog"));
	}
	m_preview->setPixmap(preview);
}

//-----------------------------------------------------------------------------

void ThemeDialog::savePreview()
{
	if (m_preview->pixmap()) {
		m_preview->pixmap()->save(Theme::iconPath(m_theme.name()));
	} else {
		qWarning("Theme preview was not created.");
	}
}

//-----------------------------------------------------------------------------

void ThemeDialog::updateStylingControls(int index)
{

    QTextBlockFormat bf=m_current_block_type->itemData(index).value<QTextFormat>().toBlockFormat();

    m_styling_adjustment->setValue(bf.intProperty(QTextFormat::FontSizeAdjustment));
    m_styling_italic->setChecked(bf.boolProperty(QTextFormat::FontItalic));

    if(bf.intProperty(QTextFormat::FontWeight)==QFont::Bold)
        m_styling_bold->setChecked(true);
    else
        m_styling_bold->setChecked(false);

    if(bf.alignment()==Qt::AlignRight)
        m_styling_align->setCurrentIndex(2);
    else if(bf.alignment()==Qt::AlignCenter)
        m_styling_align->setCurrentIndex(1);
    else
        m_styling_align->setCurrentIndex(0);

    m_styling_l_margin->setValue((int)bf.doubleProperty(QTextFormat::BlockLeftMargin));
    m_styling_r_margin->setValue((int)bf.doubleProperty(QTextFormat::BlockRightMargin));
    m_styling_t_margin->setValue((int)bf.doubleProperty(QTextFormat::BlockTopMargin));
    m_styling_b_margin->setValue((int)bf.doubleProperty(QTextFormat::BlockBottomMargin));
}

//----------

void ThemeDialog::updateStylingFormat()
{
    int index=m_current_block_type->currentIndex();

    QTextBlockFormat bf=m_current_block_type->itemData(index).value<QTextFormat>().toBlockFormat();
    if(m_styling_bold->isChecked())
        bf.setProperty(QTextFormat::FontWeight,QFont::Bold);
    else
        bf.clearProperty(QTextFormat::FontWeight);

    if(m_styling_italic->isChecked())
        bf.setProperty(QTextFormat::FontItalic,true);
    else
        bf.clearProperty(QTextFormat::FontItalic);

    if(m_styling_adjustment->value()!=0)
        bf.setProperty(QTextFormat::FontSizeAdjustment,m_styling_adjustment->value());
    else
        bf.clearProperty(QTextFormat::FontSizeAdjustment);

    if(m_styling_align->currentIndex()==2)
        bf.setProperty(QTextFormat::BlockAlignment,Qt::AlignRight);
    else if(m_styling_align->currentIndex()==1)
        bf.setProperty(QTextFormat::BlockAlignment,Qt::AlignCenter);
    else
        bf.setProperty(QTextFormat::BlockAlignment,Qt::AlignLeft);

    if(m_styling_l_margin->value()!=0)
        bf.setProperty(QTextFormat::BlockLeftMargin,(double)m_styling_l_margin->value());
    else
        bf.clearProperty(QTextFormat::BlockLeftMargin);

    if(m_styling_r_margin->value()!=0)
        bf.setProperty(QTextFormat::BlockRightMargin,(double)m_styling_r_margin->value());
    else
        bf.clearProperty(QTextFormat::BlockRightMargin);

    if(m_styling_t_margin->value()!=0)
        bf.setProperty(QTextFormat::BlockTopMargin,(double)m_styling_t_margin->value());
    else
        bf.clearProperty(QTextFormat::BlockTopMargin);

    if(m_styling_b_margin->value()!=0)
        bf.setProperty(QTextFormat::BlockBottomMargin,(double)m_styling_b_margin->value());
    else
        bf.clearProperty(QTextFormat::BlockBottomMargin);

    m_current_block_type->setItemData(index,bf);
}
