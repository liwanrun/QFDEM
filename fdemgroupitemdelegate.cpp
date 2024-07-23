#include "fdemgroupitemdelegate.h"

FDEMGroupItemDelegate::FDEMGroupItemDelegate(QObject *parent)
    : QStyledItemDelegate (parent)
{

}

QWidget *FDEMGroupItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void FDEMGroupItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

void FDEMGroupItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}

void FDEMGroupItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

QSize FDEMGroupItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight( size.height() + 4 );    //height = 25
    return size;
}
