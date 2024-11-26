#ifndef FDEMGEOMETRYWIDGET_H
#define FDEMGEOMETRYWIDGET_H

#include <QWidget>
#include <QAction>
#include <QActionGroup>
#include <QStandardItemModel>
#include <QTableWidgetItem>
#include <QItemSelection>

#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>


namespace Ui {class FDEMGeometry;}

class Primitive;
class EntityActor;
class ActorCollection;
class BlockCollection;
class FDEMGeometryItemDelegate;

class FDEMGeometryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMGeometryWidget(QWidget *parent = nullptr);
    ~FDEMGeometryWidget();

private:
    void InitGeometryWidget();

    void ConnectSignalSlots();

    void InitGeometryTreeView();

    QStandardItem *CreateBlockGeometryItem(const QString& blockId);

    QList<QStandardItem *> CreateBlockBoundaryItem(const QString& blockId, const QString& id);

    QList<QStandardItem *> CreateBlockFractureItem(const QString& blockId, const QString& id);

    QList<QStandardItem *> CreateBlockBedplaneItem(const QString& blockId, const QString& id);

    QList<QStandardItem *> CreateBlockClipseedItem(const QString& blockId, const QString& id);

    QSharedPointer<Primitive> PrimitiveFromSelection(const QItemSelection &selection);

    vtkDoubleArray *ReadFromTableWidget();

    void UpdateTableWidget(const QItemSelection &selection);

    void UpdateGeometryInformation(const QItemSelection &selection);

    void AppendGeometryPrimitive(const QModelIndex& parent);

    void RemoveGeometryPrimitive(const QModelIndex& index);

    void ClearGeometryPrimitive(const QModelIndex& parent);

private:
    void UpdateParentCheckState(QStandardItem *item);

public slots:
    void slotTreeModelItemChanged(QStandardItem *item);

    void slotTableWidgetItemChanged(QTableWidgetItem *item);

    void slotSelectionChanged(const QItemSelection &selection, const QItemSelection &deselected);

    void slotAppendButtonClicked();

    void slotRemoveButtonClicked();

    void slotClearButtonClicked();

    void slotApplyButtonClicked();

    void slotResetButtonClicked();

    void slotCancelButtonClicked();

    void slotClippingButtonClicked();

    void slotInsertDFNButtonClicked();

    void slotImportButtonClicked();

    void slotAppendBlockGeometryItem();

    void slotRemoveBlockGeometryItem();

signals:
    void signalOutpuPromptMessage(const QString msg);

    void signalRenderGeometryActor(const QModelIndex& index);         //Notify Render vtkActor

    void signalRemoveGeometryActor(const QModelIndex& index);         //Notify Delete vtkActor

    void signalGeometryActorVisibility(QStandardItem *item);

    void signalAppendBlockMeshItems(const QString& blockId);

    void signalRemoveBlockMeshItems(const QString& blockId);

    void signalDeleteBlockMeshActor(const QString& blockId);

    void signalSetBlockGeometryActorColor(const QString& blockId, const QString& id, const QColor& color);

private:
    Ui::FDEMGeometry *ui;
    QString path;

    QStandardItemModel *treeModel;
    FDEMGeometryItemDelegate* delegate;

    QSharedPointer<BlockCollection> blockCollection;
    QSharedPointer<ActorCollection> actorCollection;
};

#endif // FDEMGEOMETRYWIDGET_H
