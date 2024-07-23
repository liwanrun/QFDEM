#ifndef FDEMVTKRENDERWIDGET_H
#define FDEMVTKRENDERWIDGET_H

#include <QMainWindow>
#include <QPointer>
#include <QStandardItem>
#include <QItemSelection>
#include <QActionGroup>
#include <QComboBox>

#include <vtkSmartPointer.h>
#include <vtkInteractorObserver.h>

namespace Ui {
class FDEMVTKRenderWidget;
}

class FDEMWindow;
class FDEMParameterWidget;
class FDEMInformationWidget;
class BlockCollection;
class ActorCollection;
class ColorCollection;
class SelectCollection;
class BackgroundGrid;

class vtkRenderer;
class vtkGenericOpenGLRenderWindow;
class QVTKInteractor;
class vtkInteractorStyleTrackballCamera;
class vtkNamedColors;
class vtkOrientationMarkerWidget;
class vtkEventQtSlotConnect;
class vtkWorldPointPicker;
class vtkScalarBarWidget;
class vtkScalarsToColors;
class vtkLookupTable;
class vtkActor;

class EntityActor;

class FDEMVTKRenderWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit FDEMVTKRenderWidget(QWidget *parent = nullptr);

    ~FDEMVTKRenderWidget();

    void ConnectSignalSlots();

public:
    QPointer<FDEMParameterWidget> getFdemParameterWidget() const;
    void setFdemParameterWidget(const QPointer<FDEMParameterWidget> &value);

    QPointer<FDEMInformationWidget> getFdemInformationWidget() const;
    void setFdemInformationWidget(const QPointer<FDEMInformationWidget> &value);

    QPointer<FDEMWindow> getFdemWindow() const;
    void setFdemWindow(const QPointer<FDEMWindow> &value);

private:
    void InitVTKRenderWidget();

    void InitOrientationMarkerWidget();

    void InitBackgroundGrid();

    void InitScalarBarWidget();

    void CreateManualActions();

    void AddSphereActor();

public slots:
    void slotViewAxesToggled(bool checked);

    void slotViewBackgroundGridToggled(bool checked);

    void slotFrontViewTriggered();

    void slotResetCameraTriggered();

    void slotMeshNodeIdsToggled(bool checked);

    void slotMeshCellIdsToggled(bool checked);

    void slotColorSchemeBoxChanged(const QString& text);

    void slotRepresentationBoxChanged(int index);

    void slotVisualizationBoxChanged(const QString& text);

    void slotComponentNameBoxChanged(int index);

    void slotRescaleTableRange();

public slots:
    void slotMouseMoveEvent();

    void slotEndPickEvent();

    void slotUserEvent();

public slots:
    void slotCaptureScreenshot();

    void slotSelectFilterChanged(const QString& text);

    void slotPlusSelection(bool checked);

    void slotMinusSelection(bool checked);

    void slotRubberBandSelectBlock(bool checked);

    void slotRubberBandSelectPoint(bool checked);

    void slotRubberBandSelectCell(bool checked);

    void slotPolygonSelectPoint(bool checked);

    void slotPolygonSelectCell(bool checked);

    void slotInteractiveSelectPoint(bool checked);

    void slotInteractiveSelectCell(bool checked);

    void slotQueryPointInfo(bool checked);

    void slotQueryCellInfo(bool checked);

    void slotCancelSelection();

    void slotInvertSelection();

    void slotFilterSelection();

public slots:
    void UpdateBackgroundGrid();

    void TranslateTriangulation(double *comp);

    void RotateTriangulation(double *comp);

    void ScaleTriangulation(double *comp);

public slots:
    void slotRenderGeometryActor(const QModelIndex& index);

    void slotRemoveGeometryActor(const QModelIndex& index);

    void slotGeometryActorVisibility(QStandardItem *item);

    void slotSetBlockGeometryActorColor(const QString& blockId, const QString& id, const QColor& color);

public slots:
    void slotRenderBlockVoronoiActor(const QString &blockId);

    void slotSetVoronoiActorVisibility(const QString &blockId, bool checked);

    void slotRenderBlockMeshActor(const QString &blockId);

    void slotDeleteBlockMeshActor(const QString &blockId);

    void slotShrinkBlockMeshActor(const QString &blockId);

    void slotSetMeshActorVisibility(const QString &blockId, bool checked);

    void slotSetMeshFaceColor(const QString& blockId, const QColor& color);

    void slotSetMeshEdgeColor(const QString& blockId, const QColor& color);

    void slotClearImportedMeshActors();

    void slotRenumberMeshButtonClicked();

    void slotRefineLocalCohesiveQuads();

public slots:
    void slotApplyGroupButtonClicked();

    void slotHighlightGroupButtonClicked();

    void slotCancelGroupButtonClicked();

public slots:
    void slotApplyMaterialButtonClicked(const QString& id);

    void slotCancelMaterialButtonClicked(const QString& id);

    void slotClearMaterialButtonClicked();

public slots:
    void slotApplyBoundaryButtonClicked(const QString& id);

    void slotCancelBoundaryButtonClicked(const QString& id);

    void slotClearBoundaryButtonClicked();

public slots:
    void slotApplyTemporalButtonClicked(const QString& id);

    void slotCancelTemporalButtonClicked(const QString& id);

    void slotClearTemporalButtonClicked();

public slots:
    void slotApplyHistoryButtonClicked();

    void slotCancelHistoryButtonClicked();

private:
    Ui::FDEMVTKRenderWidget *ui;
    QAction *actionCaptureScreen;
    QAction *actionToggleSelect;
    QAction *actionSelectInvert;
    QAction *actionSelectPlus;
    QAction *actionSelectMinus;
    QAction *actionSelectBlock;
    QAction *actionRubberBandSelectPoint;
    QAction *actionRubberBandSelectCell;
    QAction *actionPolygonSelectPoint;
    QAction *actionPolygonSelectCell;
    QAction *actionInteractiveSelectPoint;
    QAction *actionInteractiveSelectCell;
    QAction *actionQueryPoint;
    QAction *actionQueryCell;
    QAction *actionFilterSelect;
    QAction *actionCancelSelect;
    QActionGroup *selectOptionGroup;
    QActionGroup *selectMannerGroup;
    QComboBox *selectFilterBox;

    QPointer<FDEMWindow> fdemWindow;
    QPointer<FDEMParameterWidget> fdemParameterWidget;
    QPointer<FDEMInformationWidget> fdemInformationWidget;

    QSharedPointer<BlockCollection> blockCollection;
    QSharedPointer<ActorCollection> actorCollection;
    QSharedPointer<ColorCollection> colorCollection;
    QSharedPointer<SelectCollection> selectCollection;
    QSharedPointer<BackgroundGrid> backgroundGrid;

public:
    vtkSmartPointer<vtkNamedColors> bkgColor;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindowInteractor> rwi;
    vtkSmartPointer<vtkInteractorObserver> rwiStyle;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWin;

    vtkSmartPointer<vtkScalarBarWidget> scalarBarWidget;
    vtkSmartPointer<vtkOrientationMarkerWidget> triadMarker;
    vtkSmartPointer<vtkActor> bkgGridActor;

public:
    vtkNew<vtkEventQtSlotConnect> MouseMoveEvent;
    vtkNew<vtkEventQtSlotConnect> EndPickEvent;
    vtkNew<vtkEventQtSlotConnect> UserEvent;
};

#endif // FDEMVTKRENDERWIDGET_H
