#ifndef CLASS_CHECK_BOX_DELEGATE_H
#define CLASS_CHECK_BOX_DELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QCheckBox>

class class_check_box_delegate : public QItemDelegate
{
    Q_OBJECT
public:
    class_check_box_delegate(QObject *parent = 0);
    QWidget *createEditor ( QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const;
    void setEditorData ( QWidget * editor, const QModelIndex & index ) const;

signals:

public slots:

};

#endif // CLASS_CHECK_BOX_DELEGATE_H
