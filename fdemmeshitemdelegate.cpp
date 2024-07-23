#include "fdemmeshitemdelegate.h"

#include <QColorDialog>
#include <QDoubleSpinBox>

enum ColumName
{
    COL_NAME = 0,
    COL_CFACE,
    COL_CEDGE,
    COL_SOURCE
};

FDEMMeshItemDelegate::FDEMMeshItemDelegate(QObject *parent)
    : QStyledItemDelegate (parent)
{

}

QWidget *FDEMMeshItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (COL_CFACE == index.column() || COL_CEDGE == index.column())
    {
        QColorDialog *editor = new QColorDialog(parent);
        return editor;
    }

    return QStyledItemDelegate::createEditor(parent,option,index);
}

void FDEMMeshItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(COL_CFACE == index.column() || COL_CEDGE == index.column())
    {
        QColorDialog *dlg =  qobject_cast<QColorDialog *>(editor);
        if (dlg)
        {
            QVariant color = index.model()->data(index,Qt::EditRole);
            dlg->setCurrentColor(color.value<QColor>());
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor,index);
    }
}

void FDEMMeshItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(COL_CFACE == index.column() || COL_CEDGE == index.column())
    {
        QColorDialog *dlg = qobject_cast<QColorDialog *>(editor);
        if (dlg)
        {
            QColor bgc = dlg->currentColor();
            QColor fgc = (dlg->currentColor().lightnessF() < 0.4) ? QColor(Qt::white) : QColor(Qt::black);
            model->setData(index, bgc, Qt::EditRole);
            model->setData(index, bgc, Qt::BackgroundRole);
            model->setData(index, fgc, Qt::ForegroundRole);
        }
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void FDEMMeshItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
