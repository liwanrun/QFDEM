#ifndef FRACTURENETWORKDELEGATE_H
#define FRACTURENETWORKDELEGATE_H

#include <QStyledItemDelegate>

enum ColumnHeader
{
    COL_NAME,
    COL_TYPE,
    COL_SIZE
};

class FractureNetworkDelegate : public QStyledItemDelegate
{
public:
    FractureNetworkDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const override;

//    void paint(QPainter *painter, const QStyleOptionViewItem &option,
//               const QModelIndex &index) const override ;
};

#endif // FRACTURENETWORKDELEGATE_H
