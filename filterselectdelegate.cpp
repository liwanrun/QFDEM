#include "filterselectdelegate.h"

#include <QComboBox>

FilterSelectDelegate::FilterSelectDelegate(QObject *parent)
    : QStyledItemDelegate (parent)
{

}

QWidget *FilterSelectDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString path = QString(":/images/resources/");

    if(COL_SELECT == index.column())
    {
        QComboBox *editor = new QComboBox(parent);
        editor->addItem(QString("Nearest to (~)"));
        editor->addItem(QString("Intersect with (=)"));
        editor->addItem(QString("Within (<)"));

        return editor;
    }
    else if(COL_FILTER == index.column())
    {
        QComboBox *editor = new QComboBox(parent);
        editor->addItem(QString("Point(s)"));
        editor->addItem(QString("Segment(s)"));
        editor->addItem(QString("Rectangle(s)"));
        editor->addItem(QString("Circle(s)"));

        return editor;
    }
    else if(COL_LOGICS == index.column())
    {
        QComboBox *editor = new QComboBox(parent);
        editor->addItem(QIcon(path + QString("pqSelectNormal")), QString("Normal"));
        editor->addItem(QIcon(path + QString("pqSelectPlus")), QString("Union"));
        editor->addItem(QIcon(path + QString("pqSelectMinus")), QString("Minus"));

        return editor;
    }

    return QStyledItemDelegate::createEditor(parent,option,index);
}

void FilterSelectDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(COL_SELECT == index.column())
    {
        QComboBox *selectBox = qobject_cast<QComboBox *>(editor);
        if(selectBox)
        {
            selectBox->setCurrentText(index.model()->data(index,Qt::EditRole).toString());
        }
    }
    else if(COL_FILTER == index.column())
    {
        QComboBox *filterBox = qobject_cast<QComboBox *>(editor);
        if(filterBox)
        {
            filterBox->setCurrentText(index.model()->data(index,Qt::EditRole).toString());
        }
    }
    else if(COL_LOGICS == index.column())
    {
        QComboBox *logicsBox = qobject_cast<QComboBox *>(editor);
        if(logicsBox)
        {
            logicsBox->setCurrentText(index.model()->data(index,Qt::EditRole).toString());
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor,index);
    }
}

void FilterSelectDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(COL_SELECT == index.column())
    {
        QComboBox *selectBox = qobject_cast<QComboBox *>(editor);
        if(selectBox)
        {
            model->setData(index,selectBox->currentText(),Qt::EditRole);
        }
    }
    else if(COL_FILTER == index.column())
    {
        QComboBox *filterBox = qobject_cast<QComboBox *>(editor);
        if(filterBox)
        {
            model->setData(index,filterBox->currentText(),Qt::EditRole);
        }
    }
    else if(COL_LOGICS == index.column())
    {
        QComboBox *logicsBox = qobject_cast<QComboBox *>(editor);
        if(logicsBox)
        {
            int id = logicsBox->currentIndex();
            model->setData(index,logicsBox->currentText(),Qt::EditRole);
            model->setData(index,logicsBox->itemIcon(id),Qt::DecorationRole);
        }
    }
    else
    {
        QStyledItemDelegate::setModelData(editor,model,index);
    }
}

void FilterSelectDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}

QSize FilterSelectDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight( size.height() + 4 );    //height = 25
    return size;
}
