#include "fdemmaterialitemdelegate.h"

#include <QComboBox>

FDEMMaterialItemDelegate::FDEMMaterialItemDelegate(QObject *parent)
    : QStyledItemDelegate (parent)
{
    this->solutionNames.push_back(QString("solution-1"));
}

QWidget *FDEMMaterialItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(COL_ETYPE == index.column())
    {
        QComboBox *editor = new QComboBox(parent);
        editor->addItem(QString("Solid Triangle (3)"));
        editor->addItem(QString("Cohesive Quad (4)"));
        editor->addItem(QString("Newtonian Fluid (0)"));

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

void FDEMMaterialItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(COL_ETYPE == index.column())
    {
        QComboBox *elemTypeBox = qobject_cast<QComboBox *>(editor);
        if(elemTypeBox)
        {
            elemTypeBox->setCurrentText(index.model()->data(index,Qt::EditRole).toString());
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

void FDEMMaterialItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(COL_ETYPE == index.column())
    {
        QComboBox *elemTypeBox = qobject_cast<QComboBox *>(editor);
        if(elemTypeBox)
        {
            model->setData(index,elemTypeBox->currentText(),Qt::EditRole);
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

void FDEMMaterialItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}

void FDEMMaterialItemDelegate::SetSolutionNames(QStringList names)
{
    this->solutionNames = names;
}
