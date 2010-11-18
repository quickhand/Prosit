#include "editor.h"
#include <QTextEdit>
#include <QMimeData>
#include <QTextDocumentFragment>
#include <QObjectUserData>
Editor::Editor(QWidget *parent) :
    QTextEdit(parent)
{

}

void Editor::insertFromMimeData ( const QMimeData * source )
{
        QTextEdit::insertPlainText (source->text());
}
