#include "fdemmeshoptionitemdelegate.h"
#include <QDoubleSpinBox>

enum ColumName
{
    COL_SHAPE,
    COL_SIZE,
    COL_SEED,
    COL_ITER
};


FDEMMeshOptionItemDelegate::FDEMMeshOptionItemDelegate(QObject *parent)
    : QStyledItemDelegate (parent)
{

}

QWidget *FDEMMeshOptionItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(COL_SHAPE == index.column())
    {
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setDecimals(3);
        editor->setSingleStep(0.001);
        editor->setMinimum(0.100);
        editor->setMaximum(0.250);
        return editor;
    }
    else if(COL_SIZE == index.column())
    {
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setDecimals(3);
        editor->setSingleStep(0.001);
        editor->setMinimum(0.000);
        return editor;
    }

    return QStyledItemDelegate::createEditor(parent,option,index);
}

void FDEMMeshOptionItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(COL_SHAPE == index.column())
    {
        QDoubleSpinBox *sb =  qobject_cast<QDoubleSpinBox *>(editor);
        if (sb)
        {
            QVariant value = index.model()->data(index,Qt::EditRole);
            sb->setValue(value.toDouble());
        }
    }
    else if(COL_SIZE == index.column())
    {
        QDoubleSpinBox *sb =  qobject_cast<QDoubleSpinBox *>(editor);
        if (sb)
        {
            QVariant value = index.model()->data(index,Qt::EditRole);
            sb->setValue(value.toDouble());
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor,index);
    }
}

void FDEMMeshOptionItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(COL_SHAPE == index.column())
    {
        QDoubleSpinBox *sb =  qobject_cast<QDoubleSpinBox *>(editor);
        if (sb)
        {
            double value = sb->value();
            model->setData(index, value, Qt::EditRole);
        }
    }
    else if(COL_SIZE == index.column())
    {
        QDoubleSpinBox *sb =  qobject_cast<QDoubleSpinBox *>(editor);
        if (sb)
        {
            double value = sb->value();
            model->setData(index, value, Qt::EditRole);
        }
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void FDEMMeshOptionItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
