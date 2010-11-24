#include "editor.h"
#include "document.h"
#include <QTextEdit>
#include <QMimeData>
#include <QTextDocumentFragment>
#include <QObjectUserData>
#include <iostream>
#include <QMetaClassInfo>
Editor::Editor(QWidget *parent) :
    QTextEdit(parent)
{
    setAcceptDrops(false);
}

void Editor::insertFromMimeData ( const QMimeData * source )
{

    bool from_prosit=source->property("is_prosit").toBool();
    if(from_prosit)
        QTextEdit::insertFromMimeData(source);
    else
        QTextEdit::insertPlainText (source->text());
    Document* castparent = dynamic_cast<Document*>(this->parent());
    if(castparent != 0) {
       castparent->cleanUpDocument(true);
    }
}

QMimeData* Editor::createMimeDataFromSelection() const
{
    QMimeData* mdata=QTextEdit::createMimeDataFromSelection();
    mdata->setProperty("is_prosit",true);
    return mdata;
}
