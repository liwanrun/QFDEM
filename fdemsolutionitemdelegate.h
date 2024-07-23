#ifndef FDEMSOLUTIONITEMDELEGATE_H
#define FDEMSOLUTIONITEMDELEGATE_H

#include <QStyledItemDelegate>

enum ColumnHeader
{
    COL_STEP = 0,
    COL_ROLE
};

class FDEMSolutionItemDelegate : public QStyledItemDelegate
{
public:
    FDEMSolutionItemDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const override;

//    void paint(QPainter *painter, const QStyleOptionViewItem &option,
//               const QModelIndex &index) const override ;
};

#endif // FDEMSOLUTIONITEMDELEGATE_H
