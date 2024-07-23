#include "fdemhistoryitemdelegate.h"

#include <QComboBox>

FDEMHistoryItemDelegate::FDEMHistoryItemDelegate(QObject *parent)
    : QStyledItemDelegate (parent)
{

}

QWidget *FDEMHistoryItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString path = QString(":/images/resources/");

    if(COL_SCOPE == index.column())
    {
        QComboBox *editor = new QComboBox(parent);
        editor->addItem(QIcon(path + QString("pqGlobalData.png")), QString("Global"));
        editor->addItem(QIcon(path + QString("pqPointData.png")), QString("Point(s)"));
        editor->addItem(QIcon(path + QString("pqCellData.png")), QString("Cell(s)"));

        return editor;
    }

    return QStyledItemDelegate::createEditor(parent,option,index);
}

void FDEMHistoryItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(COL_SCOPE == index.column())
    {
        QComboBox *scopeBox = qobject_cast<QComboBox *>(editor);
        if(scopeBox)
        {
            scopeBox->setCurrentText(index.model()->data(index,Qt::EditRole).toString());
        }
    }
}

void FDEMHistoryItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(COL_SCOPE == index.column())
    {
        QComboBox *scopeBox = qobject_cast<QComboBox *>(editor);
        if(scopeBox)
        {
            int id = scopeBox->currentIndex();
            model->setData(index, scopeBox->currentText(), Qt::EditRole);
            model->setData(index, scopeBox->itemIcon(id), Qt::DecorationRole);
        }
    }
}

void FDEMHistoryItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
