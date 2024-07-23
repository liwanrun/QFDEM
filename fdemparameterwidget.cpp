#include "fdemparameterwidget.h"
#include "ui_fdemparameterwidget.h"
#include "fdemvtkrenderwidget.h"
#include "fdeminformationwidget.h"
#include "fdemwindow.h"
#include "fdempreferencewidget.h"
#include "fdemgeometrywidget.h"
#include "fdemmeshwidget.h"
#include "fdemgroupwidget.h"
#include "fdemsolutionwidget.h"
#include "fdemcontactwidget.h"
#include "fdemmaterialwidget.h"
#include "fdemboundarywidget.h"
#include "fdemtemporalwidget.h"
#include "fdemhistorywidget.h"


FDEMParameterWidget::FDEMParameterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMParameterWidget)
{
    ui->setupUi(this);
    ui->paraPanel->setCurrentIndex(0);

    this->fdemPreferenceWidget = new FDEMPreferenceWidget(this);
    ui->preferencePage->layout()->addWidget(fdemPreferenceWidget);

    this->fdemGeometryWidget = new FDEMGeometryWidget(this);
    ui->geometryPage->layout()->addWidget(fdemGeometryWidget);

    this->fdemMeshWidget = new FDEMMeshWidget(this);
    ui->meshPage->layout()->addWidget(fdemMeshWidget);

    this->fdemGroupWidget = new FDEMGroupWidget(this);
    ui->groupPage->layout()->addWidget(fdemGroupWidget);

    this->fdemSolutionWidget = new FDEMSolutionWidget(this);
    ui->solutionPage->layout()->addWidget(fdemSolutionWidget);

    this->fdemContactWidget = new FDEMContactWidget(this);
    ui->contactPage->layout()->addWidget(fdemContactWidget);

    this->fdemMaterialWidget = new FDEMMaterialWidget(this);
    ui->materialPage->layout()->addWidget(fdemMaterialWidget);

    this->fdemBoundaryWidget = new FDEMBoundaryWidget(this);
    ui->boundaryPage->layout()->addWidget(fdemBoundaryWidget);

    this->fdemTemporalWidget = new FDEMTemporalWidget(this);
    ui->temporalPage->layout()->addWidget(fdemTemporalWidget);

    this->fdemHistoryWidget = new FDEMHistoryWidget(this);
    ui->historyPage->layout()->addWidget(fdemHistoryWidget);
}

FDEMParameterWidget::~FDEMParameterWidget()
{
    delete ui;
}

void FDEMParameterWidget::ConnectSignalSlots()
{
    connect(ui->paraPanel, &QToolBox::currentChanged,
            this->fdemInformationWidget, &FDEMInformationWidget::slotOutputModuleName);

    this->ConnectPreferenceSignalSlots();
    this->ConnectSolutionSignalSlots();
    this->ConnectGeometrySignalSlots();
    this->ConnectMeshSignalSlots();
    this->ConnectGroupSignalSlots();
    this->ConnectContactSignalSlots();
    this->ConnectMaterialSignalSlots();
    this->ConnectBoundarySignalSlots();
    this->ConnectTemporalSignalSlots();
    this->ConnectHistorySignalSlots();
}

void FDEMParameterWidget::ConnectPreferenceSignalSlots()
{
    connect(this->fdemPreferenceWidget, &FDEMPreferenceWidget::signalOutputPromptMessage,
            this->fdemInformationWidget, &FDEMInformationWidget::slotOutputPromptMessage);
}

void FDEMParameterWidget::ConnectSolutionSignalSlots()
{
    connect(this->fdemSolutionWidget, &FDEMSolutionWidget::signalOutputPromptMessage,
            this->fdemInformationWidget, &FDEMInformationWidget::slotOutputPromptMessage);

    connect(this->fdemSolutionWidget, &FDEMSolutionWidget::signalSolutionCollectionChanged,
            this->fdemMaterialWidget, &FDEMMaterialWidget::slotSolutionCollectionChanged);

    connect(this->fdemSolutionWidget, &FDEMSolutionWidget::signalSolutionCollectionChanged,
            this->fdemBoundaryWidget, &FDEMBoundaryWidget::slotSolutionCollectionChanged);
}

void FDEMParameterWidget::ConnectContactSignalSlots()
{
    connect(this->fdemContactWidget, &FDEMContactWidget::signalOutputPromptMessage,
            this->fdemInformationWidget, &FDEMInformationWidget::slotOutputPromptMessage);
}

void FDEMParameterWidget::ConnectGeometrySignalSlots()
{
    connect(this->fdemGeometryWidget, &FDEMGeometryWidget::signalOutpuPromptMessage,
            this->fdemInformationWidget, &FDEMInformationWidget::slotOutputPromptMessage);

    connect(this->fdemGeometryWidget, &FDEMGeometryWidget::signalRenderGeometryActor,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotRenderGeometryActor);
    connect(this->fdemGeometryWidget, &FDEMGeometryWidget::signalRemoveGeometryActor,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotRemoveGeometryActor);
    connect(this->fdemGeometryWidget, &FDEMGeometryWidget::signalGeometryActorVisibility,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotGeometryActorVisibility);
    connect(this->fdemGeometryWidget, &FDEMGeometryWidget::signalSetBlockGeometryActorColor,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotSetBlockGeometryActorColor);

    connect(this->fdemGeometryWidget, &FDEMGeometryWidget::signalAppendBlockMeshItems,
            this->fdemMeshWidget, &FDEMMeshWidget::slotAppendBlockMeshItems);
    connect(this->fdemGeometryWidget, &FDEMGeometryWidget::signalRemoveBlockMeshItems,
            this->fdemMeshWidget, &FDEMMeshWidget::slotRemoveBlockMeshItems);
    connect(this->fdemGeometryWidget, &FDEMGeometryWidget::signalDeleteBlockMeshActor,
            this->fdemMeshWidget, &FDEMMeshWidget::signalDeleteBlockMeshActor);
}

void FDEMParameterWidget::ConnectMeshSignalSlots()
{
    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalOutputPromptMessage,
            this->fdemInformationWidget, &FDEMInformationWidget::slotOutputPromptMessage);

    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalRenderBlockVoronoiActor,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotRenderBlockVoronoiActor);
    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalSetVoronoiActorVisibility,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotSetVoronoiActorVisibility);

    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalRenderBlockMeshActor,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotRenderBlockMeshActor);
    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalDeleteBlockMeshActor,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotDeleteBlockMeshActor);
    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalShrinkBlockMeshActor,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotShrinkBlockMeshActor);
    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalSetMeshActorVisibility,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotSetMeshActorVisibility);
    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalSetMeshFaceColor,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotSetMeshFaceColor);
    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalSetMeshEdgeColor,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotSetMeshEdgeColor);
    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalRenumberMeshButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotRenumberMeshButtonClicked);
    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalRefineLocalQuadButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotRefineLocalCohesiveQuads);

    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalRenumberMeshButtonClicked,
            this->fdemWindow, &FDEMWindow::slotRenumberVisualizationItem);
    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalTriangulationModified,
            this->fdemWindow, &FDEMWindow::slotTriangulationModified);
    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalAppendBlockGeometryItem,
            this->fdemGeometryWidget, &FDEMGeometryWidget::slotAppendBlockGeometryItem);
    connect(this->fdemMeshWidget, &FDEMMeshWidget::signalRemoveBlockGeometryItem,
            this->fdemGeometryWidget, &FDEMGeometryWidget::slotRemoveBlockGeometryItem);
}

void FDEMParameterWidget::ConnectGroupSignalSlots()
{
    connect(this->fdemGroupWidget, &FDEMGroupWidget::signalOutputPromptMessage,
            this->fdemInformationWidget, &FDEMInformationWidget::slotOutputPromptMessage);

    connect(this->fdemGroupWidget, &FDEMGroupWidget::signalApplyGroupButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotApplyGroupButtonClicked);
    connect(this->fdemGroupWidget, &FDEMGroupWidget::signalHighlightGroupButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotHighlightGroupButtonClicked);
    connect(this->fdemGroupWidget, &FDEMGroupWidget::signalCancelGroupButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotCancelGroupButtonClicked);
}

void FDEMParameterWidget::ConnectMaterialSignalSlots()
{
    connect(this->fdemMaterialWidget, &FDEMMaterialWidget::signalOutputPromptMessage,
            this->fdemInformationWidget, &FDEMInformationWidget::slotOutputPromptMessage);

    connect(this->fdemMaterialWidget, &FDEMMaterialWidget::signalApplyMaterialButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotApplyMaterialButtonClicked);
    connect(this->fdemMaterialWidget, &FDEMMaterialWidget::signalCancelMaterialButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotCancelMaterialButtonClicked);
    connect(this->fdemMaterialWidget, &FDEMMaterialWidget::signalClearMaterialButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotClearMaterialButtonClicked);

    connect(this->fdemMaterialWidget, &FDEMMaterialWidget::signalApplyMaterialButtonClicked,
            this->fdemWindow, &FDEMWindow::slotInsertMaterialVisualizationItem);
    connect(this->fdemMaterialWidget, &FDEMMaterialWidget::signalClearMaterialButtonClicked,
            this->fdemWindow, &FDEMWindow::slotClearMaterialVisualizationItem);
}

void FDEMParameterWidget::ConnectBoundarySignalSlots()
{
    connect(this->fdemBoundaryWidget, &FDEMBoundaryWidget::signalOutputPromptMessage,
            this->fdemInformationWidget, &FDEMInformationWidget::slotOutputPromptMessage);

    connect(this->fdemBoundaryWidget, &FDEMBoundaryWidget::signalApplyBoundaryButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotApplyBoundaryButtonClicked);
    connect(this->fdemBoundaryWidget, &FDEMBoundaryWidget::signalCancelBoundaryButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotCancelBoundaryButtonClicked);
    connect(this->fdemBoundaryWidget, &FDEMBoundaryWidget::signalClearBoundaryButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotClearBoundaryButtonClicked);

    connect(this->fdemBoundaryWidget, &FDEMBoundaryWidget::signalApplyBoundaryButtonClicked,
            this->fdemWindow, &FDEMWindow::slotInsertBoundaryVisualizationItem);
    connect(this->fdemBoundaryWidget, &FDEMBoundaryWidget::signalClearBoundaryButtonClicked,
            this->fdemWindow, &FDEMWindow::slotClearBoundaryVisualizationItem);
}

void FDEMParameterWidget::ConnectTemporalSignalSlots()
{
    connect(this->fdemTemporalWidget, &FDEMTemporalWidget::signalOutputPromptMessage,
            this->fdemInformationWidget, &FDEMInformationWidget::slotOutputPromptMessage);

    connect(this->fdemTemporalWidget, &FDEMTemporalWidget::signalApplyTemporalButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotApplyTemporalButtonClicked);
    connect(this->fdemTemporalWidget, &FDEMTemporalWidget::signalCancelTemporalButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotCancelTemporalButtonClicked);
    connect(this->fdemTemporalWidget, &FDEMTemporalWidget::signalClearTemporalButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotClearTemporalButtonClicked);

    connect(this->fdemTemporalWidget, &FDEMTemporalWidget::signalApplyTemporalButtonClicked,
            this->fdemWindow, &FDEMWindow::slotInsertTemporalVisualizationItem);
    connect(this->fdemTemporalWidget, &FDEMTemporalWidget::signalClearTemporalButtonClicked,
            this->fdemWindow, &FDEMWindow::slotClearTemporalVisualizationItem);
}

void FDEMParameterWidget::ConnectHistorySignalSlots()
{
    connect(this->fdemHistoryWidget, &FDEMHistoryWidget::signalOutputPromptMessage,
            this->fdemInformationWidget, &FDEMInformationWidget::slotOutputPromptMessage);

    connect(this->fdemHistoryWidget, &FDEMHistoryWidget::signalApplyHistoryButtonClicked,
            this->fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotApplyHistoryButtonClicked);
}

QPointer<FDEMWindow> FDEMParameterWidget::getFdemWindow() const
{
    return fdemWindow;
}

void FDEMParameterWidget::setFdemWindow(const QPointer<FDEMWindow> &value)
{
    fdemWindow = value;
}

QPointer<FDEMInformationWidget> FDEMParameterWidget::getFdemInformationWidget() const
{
    return fdemInformationWidget;
}

void FDEMParameterWidget::setFdemInformationWidget(const QPointer<FDEMInformationWidget> &value)
{
    fdemInformationWidget = value;
}

QPointer<FDEMVTKRenderWidget> FDEMParameterWidget::getFdemVTKRenderWidget() const
{
    return fdemVTKRenderWidget;
}

void FDEMParameterWidget::setFdemVTKRenderWidget(const QPointer<FDEMVTKRenderWidget> &value)
{
    fdemVTKRenderWidget = value;
}

Ui::FDEMParameterWidget *FDEMParameterWidget::getUi() const
{
    return ui;
}
