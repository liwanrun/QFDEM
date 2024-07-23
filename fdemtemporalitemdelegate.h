#ifndef FDEMTEMPORALITEMDELEGATE_H
#define FDEMTEMPORALITEMDELEGATE_H

#include <QStyledItemDelegate>

enum ColumnHeader
{
    COL_TITLE = 0,
    COL_FIELD,
    COL_STATE
};

class FDEMTemporalItemDelegate : public QStyledItemDelegate
{
public:
    FDEMTemporalItemDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const override;

//    void paint(QPainter *painter, const QStyleOptionViewItem &option,
//               const QModelIndex &index) const override ;
};

#endif // FDEMTEMPORALITEMDELEGATE_H
