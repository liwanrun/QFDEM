#include "fdemgeometryitemdelegate.h"

#include <QDebug>
#include <QComboBox>
#include <QColorDialog>
#include <QPainter>

enum ColumName
{
    COL_NAME = 0,
    COL_SHAPE,
    COL_SIZE,
    COL_COLOR
};


FDEMGeometryItemDelegate::FDEMGeometryItemDelegate(QObject *parent)
    : QStyledItemDelegate (parent)
{

}

QWidget *FDEMGeometryItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString path = QString(":/images/resources/");
    QString group = index.parent().data().toString();
    QString name = group.split(QChar(' ')).first();

    if(COL_SHAPE == index.column())
    {
        QComboBox *editor = new QComboBox(parent);
        if(QString("Boundaries") == name)
        {
            editor->addItem(QIcon(path + QString("polygon_Geo.png")),QString("Polygon"));
            editor->addItem(QIcon(path + QString("ellipse_Geo.png")),QString("Ellipse"));
        }
        else if(QString("Clipseeds") == name)
        {
            editor->addItem(QIcon(path + QString("point_Geo.png")),QString("Point"));
        }
        else
        {
            editor->addItem(QIcon(path + QString("polygon_Geo.png")),QString("Polygon"));
            editor->addItem(QIcon(path + QString("ellipse_Geo.png")),QString("Ellipse"));
            editor->addItem(QIcon(path + QString("segment_Geo.png")),QString("Segment"));
        }

        return editor;
    }
    else if (COL_COLOR == index.column())
    {
        QColorDialog *editor = new QColorDialog(parent);
        return editor;
    }

    return QStyledItemDelegate::createEditor(parent,option,index);
}

void FDEMGeometryItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(COL_SHAPE == index.column())
    {
        QComboBox *shapeBox = qobject_cast<QComboBox *>(editor);
        if(shapeBox)
        {
          shapeBox->setCurrentText(index.model()->data(index,Qt::EditRole).toString());
//            shapeBox->setCurrentIndex(index.model()->data(index,Qt::UserRole).toInt());
        }
    }
    else if(COL_COLOR == index.column())
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

void FDEMGeometryItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(COL_SHAPE == index.column())
    {
        QComboBox *shapeBox = qobject_cast<QComboBox *>(editor);
        if(shapeBox)
        {
            int id = shapeBox->currentIndex();
            model->setData(index,shapeBox->currentText(),Qt::EditRole);
            model->setData(index,shapeBox->itemIcon(id),Qt::DecorationRole);
//            model->setData(index,shapeBox->currentIndex(),Qt::UserRole);
        }
    }
    else if(COL_COLOR == index.column())
    {
        QColorDialog *dlg = qobject_cast<QColorDialog *>(editor);
        if (dlg)
        {
            QColor bgc = dlg->currentColor();
            QColor fgc = (dlg->currentColor().lightnessF() < 0.4) ? QColor(Qt::white) : QColor(Qt::black);
            model->setData(index,bgc,Qt::EditRole);
            model->setData(index,bgc,Qt::BackgroundRole);
            model->setData(index,fgc,Qt::ForegroundRole);
        }
    }
    else
    {
        QStyledItemDelegate::setModelData(editor,model,index);
    }
}

void FDEMGeometryItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}

QSize FDEMGeometryItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight( size.height() + 4 );    //height = 25
    return size;
}

