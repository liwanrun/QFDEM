#ifndef FDEMMESHOPTIONITEMDELEGATE_H
#define FDEMMESHOPTIONITEMDELEGATE_H

#include <QStyledItemDelegate>

class FDEMMeshOptionItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    FDEMMeshOptionItemDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const override;

//    void paint(QPainter *painter, const QStyleOptionViewItem &option,
//               const QModelIndex &index) const override ;
};

#endif // FDEMMESHOPTIONITEMDELEGATE_H
