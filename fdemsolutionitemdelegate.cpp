#include "fdemsolutionitemdelegate.h"

#include <QLineEdit>

FDEMSolutionItemDelegate::FDEMSolutionItemDelegate(QObject *parent)
    : QStyledItemDelegate (parent)
{

}

QWidget *FDEMSolutionItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString path = QString(":/images/resources/");

    if(COL_ROLE == index.column())
    {
        QLineEdit *editor = new QLineEdit(parent);
        return editor;
    }

    return QStyledItemDelegate::createEditor(parent,option,index);
}

void FDEMSolutionItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(COL_ROLE == index.column())
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
        if(lineEdit)
        {
            lineEdit->setText(index.model()->data(index, Qt::EditRole).toString());
        }
    }
}

void FDEMSolutionItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QString path = QString(":/images/resources/");

    if(COL_ROLE == index.column())
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
        if(lineEdit)
        {
            model->setData(index, lineEdit->text(), Qt::EditRole);
            model->setData(index, QIcon(path + QString("record.png")), Qt::DecorationRole);
        }
    }
}

void FDEMSolutionItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
