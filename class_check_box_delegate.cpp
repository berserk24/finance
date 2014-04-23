#include <QtGui>
#include "class_check_box_delegate.h"

class_check_box_delegate::class_check_box_delegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *class_check_box_delegate::createEditor (QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QCheckBox *box = new QCheckBox(parent);
    box->show();
    return box;
}

void class_check_box_delegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const
 {
     bool value = index.model()->data(index, Qt::EditRole).toInt();

     QCheckBox *checkBox = new QCheckBox(editor);
     checkBox->show();
     checkBox->setChecked(value);
 }
