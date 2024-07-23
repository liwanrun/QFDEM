#ifndef FDEMBOUNDARYITEMDELEGATE_H
#define FDEMBOUNDARYITEMDELEGATE_H

#include <QStyledItemDelegate>

enum ColumnHeader
{
    COL_TITLE = 0,
    COL_FIELD,
    COL_SOLVE
};

class FDEMBoundaryItemDelegate : public QStyledItemDelegate
{
public:
    FDEMBoundaryItemDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const override;

//    void paint(QPainter *painter, const QStyleOptionViewItem &option,
//    const QModelIndex &index) const override ;

public:
    void SetSolutionNames(QStringList names);

private:
    QStringList solutionNames;
};

#endif // FDEMBOUNDARYITEMDELEGATE_H
