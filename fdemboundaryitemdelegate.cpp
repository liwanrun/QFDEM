#include "fdemboundaryitemdelegate.h"

#include <QComboBox>

FDEMBoundaryItemDelegate::FDEMBoundaryItemDelegate(QObject *parent)
    : QStyledItemDelegate (parent)
{
    this->solutionNames.push_back(QString("solution-1"));
}

QWidget *FDEMBoundaryItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString path = QString(":/images/resources/");

    if(COL_FIELD == index.column())
    {
        QComboBox *editor = new QComboBox(parent);
        editor->addItem(QIcon(path + QString("pqPointData.png")), QString("Mechanical"));
        editor->addItem(QIcon(path + QString("pqPointData.png")), QString("Hydraulic"));
        editor->addItem(QIcon(path + QString("pqPointData.png")), QString("Thermal"));

        return editor;
    }
    else if(COL_SOLVE == index.column())
    {
        QComboBox *editor = new QComboBox(parent);
        editor->addItems(this->solutionNames);

        return editor;
    }

    return QStyledItemDelegate::createEditor(parent,option,index);
}

void FDEMBoundaryItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(COL_FIELD == index.column())
    {
        QComboBox *fieldBox = qobject_cast<QComboBox *>(editor);
        if(fieldBox)
        {
            fieldBox->setCurrentText(index.model()->data(index,Qt::EditRole).toString());
        }
    }
    else if(COL_SOLVE == index.column())
    {
        QComboBox *solutionBox = qobject_cast<QComboBox *>(editor);
        if(solutionBox)
        {
            solutionBox->setCurrentText(index.model()->data(index,Qt::EditRole).toString());
        }
    }
}

void FDEMBoundaryItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(COL_FIELD == index.column())
    {
        QComboBox *fieldBox = qobject_cast<QComboBox *>(editor);
        if(fieldBox)
        {
            int id = fieldBox->currentIndex();
            model->setData(index, fieldBox->currentText(),Qt::EditRole);
            model->setData(index, fieldBox->itemIcon(id),Qt::DecorationRole);
        }
    }
    else if(COL_SOLVE == index.column())
    {
        QComboBox *solutionBox = qobject_cast<QComboBox *>(editor);
        if(solutionBox)
        {
            model->setData(index,solutionBox->currentText(),Qt::EditRole);
        }
    }
}

void FDEMBoundaryItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}

void FDEMBoundaryItemDelegate::SetSolutionNames(QStringList names)
{
    this->solutionNames = names;
}
