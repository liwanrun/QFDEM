#include "fracturenetworkdelegate.h"

#include <QComboBox>
#include <QSpinBox>

FractureNetworkDelegate::FractureNetworkDelegate(QObject *parent)
    : QStyledItemDelegate (parent)
{

}

QWidget *FractureNetworkDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(COL_TYPE == index.column())
    {
        QComboBox *editor = new QComboBox(parent);
        editor->addItem(QString("Point-Trace (PMD)"));
        editor->addItem(QString("Point-Point (PTP)"));
        return editor;
    }
    else if(COL_SIZE == index.column())
    {
        QSpinBox *editor = new QSpinBox(parent);
        editor->setMinimum(0);
        editor->setMaximum(100000);
        return editor;
    }

    return QStyledItemDelegate::createEditor(parent,option,index);
}

void FractureNetworkDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(COL_TYPE == index.column())
    {
        QComboBox *typeBox = qobject_cast<QComboBox *>(editor);
        if(typeBox)
        {
            typeBox->setCurrentText(index.model()->data(index,Qt::EditRole).toString());
        }
    }
    else if(COL_SIZE == index.column())
    {
        QSpinBox *spinBox =  qobject_cast<QSpinBox *>(editor);
        if (spinBox)
        {
            QVariant value = index.model()->data(index, Qt::EditRole);
            spinBox->setValue(value.toInt());
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void FractureNetworkDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(COL_TYPE == index.column())
    {
        QComboBox *typeBox = qobject_cast<QComboBox *>(editor);
        if(typeBox)
        {
            model->setData(index, typeBox->currentText(), Qt::EditRole);
        }
    }
    else if(COL_SIZE == index.column())
    {
        QSpinBox *spinBox =  qobject_cast<QSpinBox *>(editor);
        if (spinBox)
        {
            int value = spinBox->value();
            model->setData(index, value, Qt::EditRole);
        }
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void FractureNetworkDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
