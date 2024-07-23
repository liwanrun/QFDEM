#ifndef FDEMGEOMETRYITEMDELEGATE_H
#define FDEMGEOMETRYITEMDELEGATE_H

#include <QStyledItemDelegate>

class FDEMGeometryItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    FDEMGeometryItemDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const override;

//    void paint(QPainter *painter, const QStyleOptionViewItem &option,
//               const QModelIndex &index) const override ;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // FDEMGEOMETRYITEMDELEGATE_H
