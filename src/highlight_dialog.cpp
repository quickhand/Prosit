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

#include "highlight_dialog.h"
#include "color_button.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLabel>

//-----------------------------------------------------------------------------

HighlightDialog::HighlightDialog(QWidget *parent=0)
    : QDialog(parent)
{
        setModal(true);
	// Create widgets
//        m_color_button = new ColorButton(this);
//        m_highlight_comment = new QLineEdit(this);
//        connect(m_highlight_comment, SIGNAL(returnPressed()), this, SLOT(closeDialog()));

//	// Lay out dialog
//        QHBoxLayout* layout = new QHBoxLayout(this);
//        layout->addWidget(m_color_button);
//        layout->addWidget(m_highlight_comment);
//        this->setLayout(layout);

        QVBoxLayout *verticalLayout;
        QHBoxLayout *horizontalLayout;
        QSpacerItem *horizontalSpacer;
        QLabel *label;
        QHBoxLayout *horizontalLayout_2;
        QLabel *label_2;
        QDialogButtonBox *buttonBox;

        setWindowTitle("Highlight Settings");
        resize(350, 115);

        verticalLayout = new QVBoxLayout(this);

        horizontalLayout = new QHBoxLayout();

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label = new QLabel("Highlight Color:",this);


        horizontalLayout->addWidget(label);

        m_color_button = new ColorButton();

        horizontalLayout->addWidget(m_color_button);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();

        label_2 = new QLabel("Highlight Label:",this);


        horizontalLayout_2->addWidget(label_2);

        m_highlight_comment = new QLineEdit(this);


        horizontalLayout_2->addWidget(m_highlight_comment);
        connect(m_highlight_comment, SIGNAL(returnPressed()), this, SLOT(closeDialog()));

        verticalLayout->addLayout(horizontalLayout_2);

        buttonBox = new QDialogButtonBox(this);

        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);



        QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(closeDialog()));
        QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


}

//-----------------------------------------------------------------------------

void HighlightDialog::closeDialog()
{
    emit highlight(m_color_button->color(),m_highlight_comment->text());

    emit accept();
}
