#ifndef FILTERSELECTDELEGATE_H
#define FILTERSELECTDELEGATE_H

#include <QStyledItemDelegate>

enum ColumnHeader
{
    COL_HEADER = 0,
    COL_FILTER,
    COL_SELECT,
    COL_NUMBER,
    COL_LOGICS
};

class FilterSelectDelegate : public QStyledItemDelegate
{
public:
    FilterSelectDelegate(QObject *parent);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const override;

//    void paint(QPainter *painter, const QStyleOptionViewItem &option,
//               const QModelIndex &index) const override ;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // FILTERSELECTDELEGATE_H
