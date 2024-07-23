#ifndef FDEMMESHWIDGET_H
#define FDEMMESHWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QTableWidgetItem>
#include <QItemSelection>

namespace Ui {
class FDEMMeshWidget;
}

class BlockCollection;
class ActorCollection;
class SelectCollection;
class FDEMMeshItemDelegate;
class FDEMMeshOptionItemDelegate;
class Triangulation;
class ConstrainedDelaunayTriangulation;

class FDEMMeshWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMMeshWidget(QWidget *parent = nullptr);
    ~FDEMMeshWidget();

    void InitBlockMeshWidget();

    void ConnectSignalSlots();

    QList<QStandardItem *> CreateBlockMeshItems(const QString& blockId, QString srcTag);

private:
    void InitBlockMeshTableView();

    void InitMeshOptionTableView();

    void InitMeshSeedTableWidget();

    QVector<QPointF> GetMeshSeedPoints();

    void UpdateMeshOptionTableView(ConstrainedDelaunayTriangulation *mesh);

    void UpdateMeshSeedTableWidget(ConstrainedDelaunayTriangulation *mesh);

public slots:
    void slotApplyButtonClicked();

    void slotRestoreButtonClicked();

    void slotRemoveButtonClicked();

    void slotQueryButtonClicked();

    void slotBlockMeshTableModelItemChanged(QStandardItem *item);

    void slotMeshOptionTableModelItemChanged(QStandardItem *item);

    void slotMeshSeedTableWidgetItemChanged(QTableWidgetItem *item);

    void slotSelectionChanged(const QItemSelection &selection, const QItemSelection &);

    void slotAppendBlockMeshItems(const QString& blockId);

    void slotRemoveBlockMeshItems(const QString& blockId);

public slots:
    void slotVoronoiDiagramToggled(bool checked);

    void slotImportBlockMeshButtonClicked();

    void slotInsertConstraintButtonClicked();

    void slotInsertGlobalQuadButtonClicked();

    void slotInsertLocalQuadButtonClicked();

    void slotSplitTriangulationButtonClicked();

    void slotShrinkBlockMeshButtonClicked();

    void slotRenumberBlockMeshButtonClicked();

    void slotRefineLocalQuadButtonClicked();

    void slotConstraintVisibilityButtonToggled(bool checked);

signals:
    void signalOutputPromptMessage(const QString msg);

    void signalRenderBlockVoronoiActor(const QString &blockId);

    void signalSetVoronoiActorVisibility(const QString &blockId, bool checked);

    void signalRenderBlockMeshActor(const QString &blockId);

    void signalDeleteBlockMeshActor(const QString &blockId);

    void signalShrinkBlockMeshActor(const QString &blockId);

    void signalSetMeshActorVisibility(const QString &blockId, bool checked);

    void signalRefineLocalQuadButtonClicked();

    void signalSetMeshFaceColor(const QString &blockId, const QColor &color);

    void signalSetMeshEdgeColor(const QString &blockId, const QColor &color);

    void signalRenumberMeshButtonClicked();

    void signalAddVisualizationItem(QIcon icon, QString name);

    void signalTriangulationModified();

    void signalAppendBlockGeometryItem();

    void signalRemoveBlockGeometryItem();

private:
    Ui::FDEMMeshWidget *ui;

    QString dir;
    QStandardItemModel *meshTableModel;
    FDEMMeshItemDelegate* meshDelegate;

    QStandardItemModel *meshOptionTableModel;
    FDEMMeshOptionItemDelegate *meshOptionDelegate;

    QSharedPointer<BlockCollection> blockCollection;
    QSharedPointer<ActorCollection> actorCollection;
    QSharedPointer<SelectCollection> selectCollection;
};

#endif // FDEMMESHWIDGET_H
