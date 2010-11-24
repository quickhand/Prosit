#ifndef EDITOR_H
#define EDITOR_H

#include <QTextEdit>

class Editor : public QTextEdit
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = 0);

protected:
    void insertFromMimeData ( const QMimeData * source );
    QMimeData* createMimeDataFromSelection() const;
signals:

public slots:

};

#endif // EDITOR_H
