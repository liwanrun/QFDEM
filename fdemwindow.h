#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPointer>
#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QSharedPointer>

namespace Ui {
class FDEMWindow;
}

class FDEMParameterWidget;
class FDEMVTKRenderWidget;
class FDEMInformationWidget;
class GlobalCollection;

class vtkObject;
class FDEMDatabase;
class FDEMWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FDEMWindow(QWidget *parent = nullptr);
    ~FDEMWindow();

private:
    void InitFDEMWindow();

    void CreateManualActions();

    void CreateToolBarWidget();

    void CreateStatusBarWidget();

    void ConnectSignalSlots();

    void InsertVisualizationItem(const QIcon& icon, const QString& name, const QVariant& userData = 0);

    void RemoveVisualizationItem(QString name);

    void InitColorSeriesSchemes();

    void InitColorFunctionSchemes();

public:
    QPointer<FDEMParameterWidget> getParameterWidget() const;

    QPointer<FDEMVTKRenderWidget> getVTKRenderWidget() const;

    QPointer<FDEMInformationWidget> getInformationWidget() const;

public slots:
    void slotToggleFullScreen(bool checked);

    void slotToggleIndexedLUT(bool checked);

    void slotToggleStatusBar(bool checked);

    void slotAboutTriggered();

    void slotHelpTimestepSelection();

    void slotWorkspaceDirectoryTriggered();

    void slotExportModelTriggered();

    void slotExportGeometryTriggered();

    void slotExportBoundaryTriggered();

    void slotExportFractureTriggered();

    void slotExportTriangulationTriggered();

    void slotExportVoronoiDiagramTriggered();

public slots:
    void slotTranslateActionTriggered();

    void slotRotateActionTriggered();

    void slotScalingActionTriggered();

    void slotGridSettingTriggered();

public slots:
    void slotVisualizationBoxChanged(int index);

    void slotTriangulationModified();

    void slotRenumberVisualizationItem();

    void slotInsertMaterialVisualizationItem(const QString &id);

    void slotClearMaterialVisualizationItem();

    void slotInsertBoundaryVisualizationItem(const QString &id);

    void slotClearBoundaryVisualizationItem();

    void slotInsertTemporalVisualizationItem(const QString &id);

    void slotClearTemporalVisualizationItem();

public slots:
    void slotFractalDimensionTriggered();

public:
    Ui::FDEMWindow *ui;
    QComboBox *visualizationBox;
    QComboBox *componentNameBox;
    QComboBox *representationBox;
    QComboBox *colorSchemeBox;

public:
    QPointer<FDEMParameterWidget> fdemParameterWidget;
    QPointer<FDEMVTKRenderWidget> fdemVTKRenderWidget;
    QPointer<FDEMInformationWidget> fdemInformationWidget;

    QSharedPointer<GlobalCollection> globalCollection;
    QPointer<FDEMDatabase> document;
};

#endif // MAINWINDOW_H
