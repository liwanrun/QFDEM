#ifndef FDEMMATERIALITEMDELEGATE_H
#define FDEMMATERIALITEMDELEGATE_H

#include <QStyledItemDelegate>

enum ColumnHeader
{
    COL_TITLE = 0,
    COL_ETYPE,
    COL_SOLVE
};

class FDEMMaterialItemDelegate : public QStyledItemDelegate
{
public:
    FDEMMaterialItemDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const override;

//    void paint(QPainter *painter, const QStyleOptionViewItem &option,
//               const QModelIndex &index) const override ;

public:
    void SetSolutionNames(QStringList names);

private:
    QStringList solutionNames;
};

#endif // FDEMMATERIALITEMDELEGATE_H
