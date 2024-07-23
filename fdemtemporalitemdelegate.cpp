#include "fdemtemporalitemdelegate.h"

#include <QComboBox>

FDEMTemporalItemDelegate::FDEMTemporalItemDelegate(QObject *parent)
    : QStyledItemDelegate (parent)
{

}

QWidget *FDEMTemporalItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString path = QString(":/images/resources/");

    if(COL_FIELD == index.column())
    {
        QComboBox *editor = new QComboBox(parent);
        editor->addItem(QIcon(path + QString("pqPointData.png")), QString("Mechanical"));
        editor->addItem(QIcon(path + QString("pqPointData.png")), QString("Hydraulic"));
        editor->addItem(QIcon(path + QString("pqPointData.png")), QString("Thermal"));
        editor->addItem(QIcon(path + QString("pqCellData.png")), QString("Stress(Strain)"));

        return editor;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void FDEMTemporalItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(COL_FIELD == index.column())
    {
        QComboBox *fieldBox = qobject_cast<QComboBox *>(editor);
        if(fieldBox)
        {
            fieldBox->setCurrentText(index.model()->data(index,Qt::EditRole).toString());
        }
    }
}

void FDEMTemporalItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
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
}

void FDEMTemporalItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
