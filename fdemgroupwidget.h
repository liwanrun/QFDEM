#ifndef FDEMGROUPWIDGET_H
#define FDEMGROUPWIDGET_H

#include <QWidget>
#include <QSharedPointer>
#include <QStandardItemModel>
#include <QItemSelection>
#include <QTableWidgetItem>

namespace Ui {
class FDEMGroupWidget;
}

class Group;
class GroupCollection;
class FDEMGroupItemDelegate;
class vtkMultiBlockDataSet;

class FDEMGroupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMGroupWidget(QWidget *parent = nullptr);
    ~FDEMGroupWidget();

private:
    void InitGroupTreeView();

    void ConnectSignalSlots();

    void SetButtonsEnabled(bool arg);

    QList<QStandardItem *> CreateGroupItem(QSharedPointer<Group> group);

    QSharedPointer<Group> GroupFromSelection(const QString name);

    void UpdateGroupTableWidget(const QVector<size_t>& globalIds, const QVector<size_t> &pedigreeIds);

private:
    void HightlightGroupEntity(QSharedPointer<Group> group);

    QSharedPointer<GroupCollection> GetCollection(QString id);

    void GetGlobalIdsOfElemSet(QSharedPointer<Group> elemSet, QVector<size_t>& globalIds);

    void GetPedigreeIdsOfElemSet(QSharedPointer<Group> elemSet, QVector<size_t>& pedigreeIds);

public slots:
    void slotTreeModelItemChanged(QStandardItem *item);

    void slotSelectionChanged(const QItemSelection &selection, const QItemSelection &deselected);

    void slotAppendButtonClicked();

    void slotRemoveButtonClicked();

    void slotClearButtonClicked();

    void slotBlockGroupButtonClicked();

    void slotGrainGroupButtonClicked();

    void slotInterfaceGroupButtonClicked();

    void slotConvertNodeSetButtonClicked();

    void slotConvertQuadSetButtonClicked();

    void slotApplyButtonClicked();

    void slotDisplayButtonClicked();

    void slotCancelButtonClicked();

signals:
    void signalOutputPromptMessage(const QString msg);

    void signalApplyGroupButtonClicked();

    void signalHighlightGroupButtonClicked();

    void signalCancelGroupButtonClicked();

private:
    void AppendSubGroup(const QModelIndex& root);

    void RemoveSubGroup(const QModelIndex& root);

    void ClearSubGroup(const QModelIndex& root);

private:
    Ui::FDEMGroupWidget *ui;
    QString path;
    QString filter;

    QStandardItemModel* treeModel;
    FDEMGroupItemDelegate* delegate;

//    QSharedPointer<GroupCollection> groupCollection;

    QSharedPointer<GroupCollection> nodeSetCollection;
    QSharedPointer<GroupCollection> elemSetCollection;

    vtkMultiBlockDataSet *blockDataSet;
};

#endif // FDEMGROUPWIDGET_H
