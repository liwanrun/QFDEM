#include "fdemvtkrenderwidget.h"
#include "ui_fdemvtkrenderwidget.h"
#include "fdemparameterwidget.h"
#include "fdeminformationwidget.h"
#include "fdemwindow.h"
#include "blockcollection.h"
#include "block.h"
#include "actorcollection.h"
#include "actor.h"
#include "temporalcollection.h"
#include "temporal.h"
#include "colorcollection.h"
#include "colorscheme.h"
#include "selectcollection.h"
#include "fdemshapesource.h"
#include "fdemmeshsource.h"
#include "fdemshapefilter.h"
#include "interactorstyleinteractivequery.h"
#include "interactorstylerubberbandselection.h"
#include "interactorstyleinteractiveselection.h"
#include "interactorstyledrawpolygonselection.h"
#include "backgroundgrid.h"
#include "filterselectdialog.h"

#include <QDebug>
#include <QStatusBar>
#include <QModelIndex>
#include <QMessageBox>
#include <QFileDialog>

#include <vtkNamedColors.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkCamera.h>
#include <vtkProperty.h>

#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyData.h>
#include <vtkAppendPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkAssemblyPath.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkAreaPicker.h>
#include <vtkInformation.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>

#include <vtkColorSeries.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <vtkScalarBarRepresentation.h>
#include <vtkDataSetMapper.h>
#include <vtkStructuredGrid.h>

#include <vtkBMPWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkPNGWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkWindowToImageFilter.h>

#include <array>


FDEMVTKRenderWidget::FDEMVTKRenderWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FDEMVTKRenderWidget)
{
    ui->setupUi(this);
    this->blockCollection = BlockCollection::GetBlockCollection();
    this->actorCollection = ActorCollection::GetActorCollection();
    this->colorCollection = ColorCollection::GetColorCollection();
    this->selectCollection = SelectCollection::GetSelectCollection();

    this->CreateManualActions();
    this->InitVTKRenderWidget();

//    this->AddSphereActor();
}

FDEMVTKRenderWidget::~FDEMVTKRenderWidget()
{
    delete ui;
}

void FDEMVTKRenderWidget::InitVTKRenderWidget()
{
    /* Render Context */
    renderWin = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    ui->vtkRenderWidget->SetRenderWindow(renderWin);
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWin->AddRenderer(renderer);
    rwi = renderWin->GetInteractor();
    rwiStyle = rwi->GetInteractorStyle();

    /* Background Color */
    bkgColor = vtkSmartPointer<vtkNamedColors>::New();
    std::array<unsigned char,4> bkg{82, 87, 110, 255};
    bkgColor->SetColor("BkgColor", bkg.data());
    renderer->SetBackground(bkgColor->GetColor3d("BkgColor").GetData());

    this->InitOrientationMarkerWidget();
    this->InitBackgroundGrid();
    this->InitScalarBarWidget();
}

void FDEMVTKRenderWidget::InitOrientationMarkerWidget()
{
    vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
    axesActor->SetPosition(0.0,0.0,0.0);
    axesActor->AxisLabelsOn();
    axesActor->PickableOff();
    triadMarker = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    triadMarker->SetOrientationMarker(axesActor);
    triadMarker->SetInteractor(renderWin->GetInteractor());
    triadMarker->SetViewport(0.0,0.0,0.2,0.2);
    triadMarker->SetOutlineColor(0.9300,0.5700,0.1300);
    triadMarker->SetEnabled(true);
    triadMarker->InteractiveOff();
}

void FDEMVTKRenderWidget::InitBackgroundGrid()
{
    this->backgroundGrid = BackgroundGrid::GetBackgroundGrid();
    vtkSmartPointer<vtkStructuredGrid> sgrid = backgroundGrid->GenerateGrid();
    vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputData(sgrid);

    this->bkgGridActor = vtkSmartPointer<vtkActor>::New();
    this->bkgGridActor->PickableOff();
    this->bkgGridActor->DragableOff();
    this->bkgGridActor->VisibilityOff();
    this->bkgGridActor->GetProperty()->SetRepresentationToWireframe();
    this->bkgGridActor->GetProperty()->SetAmbient(1.0);
    this->bkgGridActor->GetProperty()->SetDiffuse(0.0);
    this->bkgGridActor->GetProperty()->SetLighting(false);
    this->bkgGridActor->SetMapper(mapper);

    renderer->AddActor(this->bkgGridActor);
    renderer->ResetCamera();
    renderWin->Render();
}

void FDEMVTKRenderWidget::InitScalarBarWidget()
{
    vtkSmartPointer<vtkLookupTable> lut;
    QString name = QString("Classic Diverging Cool to Warm");
    lut.TakeReference(this->colorCollection->CreateLookupTable(name));

    vtkNew<vtkScalarBarActor> scalarBar;
    scalarBar->SetLookupTable(lut);
    scalarBar->SetNumberOfLabels(8);
    scalarBar->DrawAnnotationsOff();
    scalarBar->DrawNanAnnotationOff();
    scalarBar->SetNanAnnotation("nan");
    scalarBar->UnconstrainedFontSizeOn();
    scalarBar->SetTextPositionToSucceedScalarBar();
    scalarBar->SetFixedAnnotationLeaderLineColor(true);

    this->scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
    this->scalarBarWidget->SetScalarBarActor(scalarBar);
    this->scalarBarWidget->SetInteractor(renderWin->GetInteractor());
    this->scalarBarWidget->GetScalarBarRepresentation()->SetPosition(0.9, 0.1);
    this->scalarBarWidget->GetScalarBarRepresentation()->SetPosition2(0.05, 0.4);
    this->scalarBarWidget->Off();
}

void FDEMVTKRenderWidget::CreateManualActions()
{
    // Select criteria
    this->selectFilterBox = new QComboBox;
    this->selectFilterBox->setMaximumSize(150, 22);
    this->selectFilterBox->setMinimumSize(150, 22);
    QStringList names = {"All", "Triangles", "Quads", "Constrained Quads", "Unconstrained Quads"};
    this->selectFilterBox->addItems(names);
    this->selectFilterBox->setCurrentText(QString("All"));

    QString path = QString(":/images/resources/");
    ui->toolBar->setIconSize(QSize(18, 18));

    this->actionCaptureScreen = new QAction(QIcon(path + QString("pqCaptureScreenshot.png")), QString("Capture Screen"));
    this->actionCaptureScreen->setCheckable(false);

    this->actionSelectInvert = new QAction(QIcon(path + QString("pqInvert.png")), QString("Invert selection"));
    this->actionSelectInvert->setCheckable(false);

    this->actionSelectPlus = new QAction(QIcon(path + QString("pqSelectPlus.png")),QString("Select Plus"));
    this->actionSelectPlus->setCheckable(true);

    this->actionSelectMinus = new QAction(QIcon(path + QString("pqSelectMinus.png")),QString("Select Minus"));
    this->actionSelectMinus->setCheckable(true);

    this->actionSelectBlock = new QAction(QIcon(path + QString("pqSelectBlock.png")),QString("Select Block"));
    this->actionSelectBlock->setCheckable(true);

    this->actionRubberBandSelectPoint = new QAction(QIcon(path + QString("pqSurfaceSelectionPoint.png")),QString("Select Points by Rectangle"));
    this->actionRubberBandSelectPoint->setCheckable(true);

    this->actionRubberBandSelectCell = new QAction(QIcon(path + QString("pqSurfaceSelectionCell.png")),QString("Select Cells by Rectangle"));
    this->actionRubberBandSelectCell->setCheckable(true);

    this->actionPolygonSelectPoint = new QAction(QIcon(path + QString("pqPolygonSelectSurfacePoint.png")),QString("Select Points by Polygon"));
    this->actionPolygonSelectPoint->setCheckable(true);

    this->actionPolygonSelectCell = new QAction(QIcon(path + QString("pqPolygonSelectSurfaceCell.png")),QString("Select Cells by Polygon"));
    this->actionPolygonSelectCell->setCheckable(true);

    this->actionInteractiveSelectPoint = new QAction(QIcon(path + QString("pqSurfaceSelectionPointInteractive.png")),QString("Select Points One by One"));
    this->actionInteractiveSelectPoint->setCheckable(true);

    this->actionInteractiveSelectCell = new QAction(QIcon(path + QString("pqSurfaceSelectionCellInteractive.png")),QString("Select Cells One by One"));
    this->actionInteractiveSelectCell->setCheckable(true);

    this->actionQueryPoint = new QAction(QIcon(path + QString("pqSurfaceHoveringPoint.png")),QString("Query Point Information"));
    this->actionQueryPoint->setCheckable(true);

    this->actionQueryCell = new QAction(QIcon(path + QString("pqSurfaceHoveringCell.png")),QString("Query Cell Information"));
    this->actionQueryCell->setCheckable(true);

    this->actionCancelSelect = new QAction(QIcon(path + QString("pqCancel24.png")),QString("Cancel Selection"));
    this->actionCancelSelect->setChecked(false);

    this->actionFilterSelect = new QAction(QIcon(path + QString("pqFindData.png")),QString("Select Entity by Filter"));
    this->actionFilterSelect->setChecked(false);

    //ToolBar Actions
    ui->toolBar->addWidget(this->selectFilterBox);
    ui->toolBar->addAction(this->actionCaptureScreen);
    ui->toolBar->addAction(this->actionSelectInvert);
    ui->toolBar->addSeparator();

    this->selectOptionGroup = new QActionGroup(this);
    this->selectOptionGroup->setExclusive(false);
    this->selectOptionGroup->addAction(this->actionSelectPlus);
    this->selectOptionGroup->addAction(this->actionSelectMinus);
    ui->toolBar->addActions(this->selectOptionGroup->actions());
    ui->toolBar->addSeparator();

    this->selectMannerGroup = new QActionGroup(this);
    this->selectMannerGroup->setExclusive(false);
    this->selectMannerGroup->addAction(this->actionSelectBlock);
    this->selectMannerGroup->addAction(this->actionRubberBandSelectPoint);
    this->selectMannerGroup->addAction(this->actionRubberBandSelectCell);
    this->selectMannerGroup->addAction(this->actionPolygonSelectPoint);
    this->selectMannerGroup->addAction(this->actionPolygonSelectCell);
    this->selectMannerGroup->addAction(this->actionInteractiveSelectPoint);
    this->selectMannerGroup->addAction(this->actionInteractiveSelectCell);
    this->selectMannerGroup->addAction(this->actionQueryPoint);
    this->selectMannerGroup->addAction(this->actionQueryCell);
    this->selectMannerGroup->addAction(this->actionFilterSelect);
    this->selectMannerGroup->addAction(this->actionCancelSelect);
    ui->toolBar->addActions(this->selectMannerGroup->actions());
}

void FDEMVTKRenderWidget::ConnectSignalSlots()
{
    connect(this->actionCaptureScreen, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotCaptureScreenshot);

    connect(this->selectFilterBox, &QComboBox::currentTextChanged,
            this, &FDEMVTKRenderWidget::slotSelectFilterChanged);
    connect(this->actionSelectPlus, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotPlusSelection);
    connect(this->actionSelectMinus, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotMinusSelection);

    connect(this->actionSelectBlock, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotRubberBandSelectBlock);
    connect(this->actionRubberBandSelectPoint, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotRubberBandSelectPoint);
    connect(this->actionRubberBandSelectCell, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotRubberBandSelectCell);
    connect(this->actionPolygonSelectPoint, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotPolygonSelectPoint);
    connect(this->actionPolygonSelectCell, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotPolygonSelectCell);
    connect(this->actionInteractiveSelectPoint, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotInteractiveSelectPoint);
    connect(this->actionInteractiveSelectCell, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotInteractiveSelectCell);
    connect(this->actionQueryPoint, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotQueryPointInfo);
    connect(this->actionQueryCell, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotQueryCellInfo);

    connect(this->actionFilterSelect, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotFilterSelection);
    connect(this->actionCancelSelect, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotCancelSelection);
    connect(this->actionSelectInvert, &QAction::triggered,
            this, &FDEMVTKRenderWidget::slotInvertSelection);

    this->MouseMoveEvent->Connect(rwi, vtkCommand::MouseMoveEvent, this, SLOT(slotMouseMoveEvent()));
    this->EndPickEvent->Connect(rwi, vtkCommand::EndPickEvent, this, SLOT(slotEndPickEvent()));
    this->UserEvent->Connect(rwi, vtkCommand::UserEvent, this, SLOT(slotUserEvent()));
}

void FDEMVTKRenderWidget::AddSphereActor()
{
    vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetCenter(0.0, 0.0, 0.0);
    sphereSource->SetRadius(100.0);
    sphereSource->SetPhiResolution(20);
    sphereSource->SetThetaResolution(30);
    sphereSource->SetLatLongTessellation(true);

    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(0.0,1.0,0.0);

    renderer->AddActor(actor);
    renderer->ResetCamera();
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotViewAxesToggled(bool checked)
{
    triadMarker->GetOrientationMarker()->SetVisibility(checked);
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotViewBackgroundGridToggled(bool checked)
{
    this->bkgGridActor->SetVisibility(checked);
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotFrontViewTriggered()
{
    double d = renderer->GetActiveCamera()->GetDistance();
    double *bounds = renderer->ComputeVisiblePropBounds();
    double x = (bounds[0] + bounds[1]) / 2.0;
    double y = (bounds[2] + bounds[3]) / 2.0;
    double z = (bounds[4] + bounds[5]) / 2.0;

    renderer->GetActiveCamera()->SetViewUp(0.0, 1.0, 0.0);
    renderer->GetActiveCamera()->SetFocalPoint(x, y, z);
    renderer->GetActiveCamera()->SetPosition(x, y, z + d);
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotResetCameraTriggered()
{
    renderer->ResetCamera();
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotMeshNodeIdsToggled(bool checked)
{
    this->actorCollection->SetNodeIdsVisibility(checked);
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotMeshCellIdsToggled(bool checked)
{
    this->actorCollection->SetCellIdsVisibility(checked);
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotColorSchemeBoxChanged(const QString &text)
{
    if(text.isEmpty() || text.isNull())
        return;

    vtkSmartPointer<vtkLookupTable> newLUT;
    newLUT.TakeReference(this->colorCollection->CreateLookupTable(text));
    vtkScalarBarActor *scalarBar = this->scalarBarWidget->GetScalarBarActor();
    newLUT->SetRange(scalarBar->GetLookupTable()->GetRange());
    scalarBar->SetDrawAnnotations(newLUT->GetIndexedLookup());
    scalarBar->SetDrawNanAnnotation(newLUT->GetIndexedLookup());

    this->scalarBarWidget->GetScalarBarActor()->SetLookupTable(newLUT);
    this->actorCollection->SetTriangulationLookupTable(newLUT);
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotRepresentationBoxChanged(int index)
{
    this->actorCollection->SetTriangulationRepresentation(index);
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotVisualizationBoxChanged(const QString &text)
{
    scalarBarWidget->SetEnabled((text == QString("Solid Color")) ? false : true);
    scalarBarWidget->GetScalarBarActor()->SetTitle(text.toStdString().c_str());
    vtkScalarsToColors *lut = scalarBarWidget->GetScalarBarActor()->GetLookupTable();
    lut->SetVectorMode(vtkScalarsToColors::MAGNITUDE);

    this->actorCollection->SetTriangulationLookupTable(lut);
    this->actorCollection->SelectTriangulationColorArray(text);
    double *range = this->actorCollection->GetTriangulationScalarRange(text, -1);
    lut->SetRange(range[0], range[1]);

    renderWin->Render();
}

void FDEMVTKRenderWidget::slotComponentNameBoxChanged(int index)
{
    vtkScalarBarActor *scalarBar = this->scalarBarWidget->GetScalarBarActor();
    QString arrayName = QString(scalarBar->GetTitle());
    double *range = this->actorCollection->GetTriangulationScalarRange(arrayName, index - 1);

    vtkScalarsToColors *lut = scalarBar->GetLookupTable();
    lut->SetRange(range[0], range[1]);
    lut->SetVectorMode(index ? vtkScalarsToColors::COMPONENT : vtkScalarsToColors::MAGNITUDE);
    lut->SetVectorComponent(index - 1);

    renderWin->Render();
}

void FDEMVTKRenderWidget::slotRescaleTableRange()
{
    vtkScalarBarActor *scalarBar = this->scalarBarWidget->GetScalarBarActor();
    QString arrayName = QString(scalarBar->GetTitle());

    if(!arrayName.isEmpty())
    {
        vtkScalarsToColors* lut = scalarBar->GetLookupTable();
        int comp = lut->GetVectorMode() ? lut->GetVectorComponent() : -1;
        double *range = this->actorCollection->GetTriangulationScalarRange(arrayName, comp);
        lut->SetRange(range[0], range[1]);
    }

    renderWin->Render();
}

void FDEMVTKRenderWidget::slotMouseMoveEvent()
{
    int pixel[2];
    double coord[4];

    rwi->GetEventPosition(pixel);
    renderer->SetDisplayPoint(pixel[0], pixel[1], 0.0);
    renderer->DisplayToWorld();
    renderer->GetWorldPoint(coord);

    QString x = QString::number(coord[0],'g',6);
    QString y = QString::number(coord[1],'g',6);
    QString z = QString::number(coord[2],'g',6);
    QString tuple = "World Location: [" + x + ", " + y + ", " + z + "]";
    FDEMWindow *fdemWin = static_cast<FDEMWindow *>(parent());
    if(fdemWin) fdemWin->statusBar()->showMessage(tuple);
}

void FDEMVTKRenderWidget::slotUserEvent()
{
    FDEMWindow *fdemWin = static_cast<FDEMWindow *>(parent());
    QString msg = QString("User event is triggered.");
    if(fdemWin) fdemWin->getInformationWidget()->slotOutputPromptMessage(QString("[>>>] ") + msg);
}

void FDEMVTKRenderWidget::slotCaptureScreenshot()
{
    QString caption = QString("Capture screen shot...");
    QString filter = QString("Image Format (*.bmp *.png *.jpg)");
    QString fname = QFileDialog::getSaveFileName(this, caption, QDir::currentPath() + QString("/screen.bmp"), filter);
    if(!fname.isEmpty())
    {
        QFileInfo fileInfo(fname);
        vtkNew<vtkWindowToImageFilter> filter;
        filter->ReadFrontBufferOff();
        filter->SetScale(2.0);
        filter->SetInput(this->renderWin);

        if(QString("bmp") == fileInfo.suffix())
        {
            filter->SetInputBufferTypeToRGBA();
            vtkNew<vtkBMPWriter> writer;
            writer->SetInputConnection(filter->GetOutputPort());
            writer->SetFileName(fname.toStdString().c_str());
            writer->Write();
        }
        else if(QString("png") == fileInfo.suffix())
        {
            filter->SetInputBufferTypeToRGBA();
            vtkNew<vtkPNGWriter> writer;
            writer->SetInputConnection(filter->GetOutputPort());
            writer->SetFileName(fname.toStdString().c_str());
            writer->Write();
        }
        else if(QString("jpg") == fileInfo.suffix())
        {
            filter->SetInputBufferTypeToRGB();
            vtkNew<vtkJPEGWriter> writer;
            writer->SetInputConnection(filter->GetOutputPort());
            writer->SetFileName(fname.toStdString().c_str());
            writer->Write();
        }

        this->renderer->ResetCamera();
        this->renderWin->Render();
        this->fdemInformationWidget->slotOutputPromptMessage(QString("[>>>] Path: \"%1\"").arg(fname));
    }
}

void FDEMVTKRenderWidget::slotSelectFilterChanged(const QString &text)
{
    this->selectCollection->SetSelectFilter(text);
}

void FDEMVTKRenderWidget::slotEndPickEvent()
{
//    if(FDEMWindow *fdemWin = static_cast<FDEMWindow *>(parent()))
//    {
//        QString msg = QString("Picking is done.");
//        fdemWin->getInformationWidget()->slotOutputMessage(QString("[>>>] ") + msg);
//    }

    this->renderer->AddActor(this->selectCollection->GetSelectActor());
    this->renderWin->Render();
    this->actionSelectInvert->setEnabled(true);

    if(this->rwi->GetInteractorStyle()->IsA("InteractorStyleInteractiveSelection"))
    {
        return;
    }

    if(this->rwi->GetInteractorStyle()->IsA("InteractorStyleDrawPolygonSelection"))
    {
        return;
    }

    for(QAction* action : this->selectMannerGroup->actions())
    {
        action->setChecked(false);
    }
}

void FDEMVTKRenderWidget::slotPlusSelection(bool checked)
{
    if(checked)
    {
        for(QAction* action : this->selectOptionGroup->actions())
        {
            if(action != this->actionSelectPlus)
                action->setChecked(false);
        }
        this->selectCollection->SetSelectStyle(Selection::UNION);
    }
    else
    {
        this->selectCollection->SetSelectStyle(Selection::NORMAL);
    }

}

void FDEMVTKRenderWidget::slotMinusSelection(bool checked)
{
    if(checked)
    {
        for(QAction* action : this->selectOptionGroup->actions())
        {
            if(action != this->actionSelectMinus)
                action->setChecked(false);
        }
        this->selectCollection->SetSelectStyle(Selection::MINUS);
    }
    else
    {
        this->selectCollection->SetSelectStyle(Selection::NORMAL);
    }
}

void FDEMVTKRenderWidget::slotRubberBandSelectBlock(bool checked)
{
    if(checked)
    {
        for(QAction *action : this->selectMannerGroup->actions())
        {
            if(action != this->actionSelectBlock)
                action->setChecked(false);
        }

        this->selectCollection->SetSelectContent(Selection::BLOCKS);
        if(Selection::CELL != this->selectCollection->GetSelectField())
        {
            for(QAction* action : this->selectOptionGroup->actions())
                action->setChecked(false);

            this->selectCollection->SetInvertSelect(false);
            this->selectCollection->SetSelectStyle(Selection::NORMAL);
            this->selectCollection->Initialize();

            this->renderer->RemoveActor(this->selectCollection->GetSelectActor());
            this->renderWin->Render();
        }
        this->selectCollection->SetSelectField(Selection::CELL);

        if(InteractorStyleRubberBandSelection *style =
                InteractorStyleRubberBandSelection::SafeDownCast(this->rwi->GetInteractorStyle()))
        {
            style->InitializeSelectMode();
        }
        else
        {
            style = InteractorStyleRubberBandSelection::New();
            style->SetDefaultRenderer(this->renderer);
            this->rwi->SetInteractorStyle(style);
            style->Delete();
        }
    }
    else
    {
        this->rwi->SetInteractorStyle(this->rwiStyle);
    }
}

void FDEMVTKRenderWidget::slotRubberBandSelectPoint(bool checked)
{
    if(checked)
    {
        for(QAction* action : this->selectMannerGroup->actions())
        {
            if(action != this->actionRubberBandSelectPoint)
                action->setChecked(false);
        }

        this->selectCollection->SetSelectContent(Selection::FRUSTUM);
        if(Selection::POINT != this->selectCollection->GetSelectField())
        {
            for(QAction* action : this->selectOptionGroup->actions())
                action->setChecked(false);

            this->selectCollection->SetInvertSelect(false);
            this->selectCollection->SetSelectStyle(Selection::NORMAL);
            this->selectCollection->Initialize();

            this->renderer->RemoveActor(this->selectCollection->GetSelectActor());
            this->renderWin->Render();
        }
        this->selectCollection->SetSelectField(Selection::POINT);

        if(InteractorStyleRubberBandSelection *style =
            InteractorStyleRubberBandSelection::SafeDownCast(this->rwi->GetInteractorStyle()))
        {
            style->InitializeSelectMode();
        }
        else
        {
            style = InteractorStyleRubberBandSelection::New();
            style->SetDefaultRenderer(this->renderer);
            this->rwi->SetInteractorStyle(style);
            style->Delete();
        }
    }
    else
    {
        this->rwi->SetInteractorStyle(this->rwiStyle);
    }
}

void FDEMVTKRenderWidget::slotRubberBandSelectCell(bool checked)
{
    if(checked)
    {
        for(QAction* action : this->selectMannerGroup->actions())
        {
            if(action != this->actionRubberBandSelectCell)
                action->setChecked(false);
        }

        this->selectCollection->SetSelectContent(Selection::FRUSTUM);
        if(Selection::CELL != this->selectCollection->GetSelectField())
        {
            for(QAction* action : this->selectOptionGroup->actions())
                action->setChecked(false);

            this->selectCollection->SetInvertSelect(false);
            this->selectCollection->SetSelectStyle(Selection::NORMAL);
            this->selectCollection->Initialize();

            this->renderer->RemoveActor(this->selectCollection->GetSelectActor());
            this->renderWin->Render();
        }
        this->selectCollection->SetSelectField(Selection::CELL);

        if(InteractorStyleRubberBandSelection *style =
            InteractorStyleRubberBandSelection::SafeDownCast(this->rwi->GetInteractorStyle()))
        {
           style->InitializeSelectMode();
        }
        else
        {
            style = InteractorStyleRubberBandSelection::New();
            style->SetDefaultRenderer(this->renderer);
            this->rwi->SetInteractorStyle(style);
            style->Delete();
        }
    }
    else
    {
        this->rwi->SetInteractorStyle(this->rwiStyle);
    }
}

void FDEMVTKRenderWidget::slotPolygonSelectPoint(bool checked)
{
    if(checked)
    {
        for(QAction* action : this->selectMannerGroup->actions())
        {
            if(action != this->actionPolygonSelectPoint)
                action->setChecked(false);
        }

        if(Selection::POINT != this->selectCollection->GetSelectField())
        {
            for(QAction* action : this->selectOptionGroup->actions())
                action->setChecked(false);

            this->selectCollection->SetInvertSelect(false);
            this->selectCollection->SetSelectStyle(Selection::NORMAL);
            this->selectCollection->Initialize();

            this->renderer->RemoveActor(this->selectCollection->GetSelectActor());
            this->renderWin->Render();
        }
        this->selectCollection->SetSelectField(Selection::POINT);

        if(!this->rwi->GetInteractorStyle()->IsA("InteractorStyleDrawPolygonSelection"))
        {
            vtkNew<InteractorStyleDrawPolygonSelection> style;
            style->SetDefaultRenderer(this->renderer);
            this->rwi->SetInteractorStyle(style);
        }
    }
    else
    {
        this->rwi->SetInteractorStyle(this->rwiStyle);
    }
}

void FDEMVTKRenderWidget::slotPolygonSelectCell(bool checked)
{
    if(checked)
    {
        for(QAction* action : this->selectMannerGroup->actions())
        {
            if(action != this->actionPolygonSelectCell)
                action->setChecked(false);
        }

        if(Selection::CELL != this->selectCollection->GetSelectField())
        {
            for(QAction* action : this->selectOptionGroup->actions())
                action->setChecked(false);

            this->selectCollection->SetInvertSelect(false);
            this->selectCollection->SetSelectStyle(Selection::NORMAL);
            this->selectCollection->Initialize();

            this->renderer->RemoveActor(this->selectCollection->GetSelectActor());
            this->renderWin->Render();
        }
        this->selectCollection->SetSelectField(Selection::CELL);

        if(!this->rwi->GetInteractorStyle()->IsA("InteractorStyleDrawPolygonSelection"))
        {
            vtkNew<InteractorStyleDrawPolygonSelection> style;
            style->SetDefaultRenderer(this->renderer);
            this->rwi->SetInteractorStyle(style);
        }
    }
    else
    {
        this->rwi->SetInteractorStyle(this->rwiStyle);
    }
}

void FDEMVTKRenderWidget::slotInteractiveSelectPoint(bool checked)
{
    if(checked)
    {
        for(QAction* action : this->selectMannerGroup->actions())
        {
            if(action != this->actionInteractiveSelectPoint)
                action->setChecked(false);
        }

        if(Selection::POINT != this->selectCollection->GetSelectField())
        {
            this->selectCollection->SetInvertSelect(false);
            this->selectCollection->Initialize();

            this->renderer->RemoveActor(this->selectCollection->GetSelectActor());
            this->renderWin->Render();
        }
        this->selectCollection->SetSelectField(Selection::POINT);

        if(!this->rwi->GetInteractorStyle()->IsA("InteractorStyleInteractiveSelection"))
        {
            vtkNew<InteractorStyleInteractiveSelection> style;
            style->SetDefaultRenderer(this->renderer);
            this->rwi->SetInteractorStyle(style);
        }
    }
    else
    {
        this->rwi->SetInteractorStyle(this->rwiStyle);
    }
}

void FDEMVTKRenderWidget::slotInteractiveSelectCell(bool checked)
{
    if(checked)
    {
        for(QAction* action : this->selectMannerGroup->actions())
        {
            if(action != this->actionInteractiveSelectCell)
                action->setChecked(false);
        }

        if(Selection::CELL != this->selectCollection->GetSelectField())
        {
            this->selectCollection->SetInvertSelect(false);
            this->selectCollection->Initialize();

            this->renderer->RemoveActor(this->selectCollection->GetSelectActor());
            this->renderWin->Render();
        }
        this->selectCollection->SetSelectField(Selection::CELL);

        if(!this->rwi->GetInteractorStyle()->IsA("InteractorStyleInteractiveSelection"))
        {
            vtkNew<InteractorStyleInteractiveSelection> style;
            style->SetDefaultRenderer(this->renderer);
            this->rwi->SetInteractorStyle(style);
        }
    }
    else
    {
        this->rwi->SetInteractorStyle(this->rwiStyle);
    }
}

void FDEMVTKRenderWidget::slotQueryPointInfo(bool checked)
{
    if(checked)
    {
        for(QAction* action : this->selectMannerGroup->actions())
        {
            if(action != this->actionQueryPoint)
                action->setChecked(false);
        }

        this->selectCollection->SetInvertSelect(false);
        this->selectCollection->SetSelectField(Selection::POINT);
        this->selectCollection->Initialize();

        this->renderer->RemoveActor(this->selectCollection->GetSelectActor());
        this->renderWin->Render();

        if(!this->rwi->GetInteractorStyle()->IsA("InteractorStyleInteractiveQuery"))
        {
            vtkNew<InteractorStyleInteractiveQuery> style;
            style->SetDefaultRenderer(this->renderer);
            this->rwi->SetInteractorStyle(style);
        }
    }
    else
    {
        this->rwi->SetInteractorStyle(this->rwiStyle);
    }
}

void FDEMVTKRenderWidget::slotQueryCellInfo(bool checked)
{
    if(checked)
    {
        for(QAction* action : this->selectMannerGroup->actions())
        {
            if(action != this->actionQueryCell)
                action->setChecked(false);
        }

        this->selectCollection->SetInvertSelect(false);
        this->selectCollection->SetSelectField(Selection::CELL);
        this->selectCollection->Initialize();

        this->renderer->RemoveActor(this->selectCollection->GetSelectActor());
        this->renderWin->Render();

        if(!this->rwi->GetInteractorStyle()->IsA("InteractorStyleInteractiveQuery"))
        {
            vtkNew<InteractorStyleInteractiveQuery> style;
            style->SetDefaultRenderer(this->renderer);
            this->rwi->SetInteractorStyle(style);
        }
    }
    else
    {
        this->rwi->SetInteractorStyle(this->rwiStyle);
    }
}

void FDEMVTKRenderWidget::slotInvertSelection()
{
    for(QAction* action : this->selectOptionGroup->actions())
        action->setChecked(false);

    for(QAction* action : this->selectMannerGroup->actions())
        action->setChecked(false);

    int invert = !(this->selectCollection->IsInvertSelect());
    this->selectCollection->InvertSelection(invert);
    this->selectCollection->SetInvertSelect(invert);
    this->selectCollection->SetSelectStyle(Selection::NORMAL);

    this->renderer->AddActor(this->selectCollection->GetSelectActor());
    this->renderWin->Render();

    this->rwi->SetInteractorStyle(this->rwiStyle);
}

void FDEMVTKRenderWidget::slotFilterSelection()
{
    FilterSelectDialog *findDlg = new FilterSelectDialog(this);
    findDlg->SetDefaultRenderer(this->renderer);
    findDlg->setAttribute(Qt::WA_DeleteOnClose);
    findDlg->show();
}

void FDEMVTKRenderWidget::UpdateBackgroundGrid()
{
    vtkSmartPointer<vtkStructuredGrid> sgrid = backgroundGrid->GenerateGrid();
    vtkSmartPointer<vtkDataSetMapper> mapper =
            vtkDataSetMapper::SafeDownCast(this->bkgGridActor->GetMapper());
    mapper->SetInputData(sgrid);
    this->renderWin->Render();
}

void FDEMVTKRenderWidget::TranslateTriangulation(double *comp)
{
    this->actorCollection->TranslateTriangulationGeometry(comp);
    this->renderWin->Render();
}

void FDEMVTKRenderWidget::RotateTriangulation(double *comp)
{
    this->actorCollection->RotateTriangulationGeometry(comp);
    this->renderWin->Render();
}

void FDEMVTKRenderWidget::ScaleTriangulation(double *comp)
{
    this->actorCollection->ScaleTriangulationGeometry(comp);
    this->renderWin->Render();
}

void FDEMVTKRenderWidget::slotCancelSelection()
{
    for(QAction* action : this->selectOptionGroup->actions())
        action->setChecked(false);

    for(QAction* action : this->selectMannerGroup->actions())
        action->setChecked(false);

    this->selectCollection->SetInvertSelect(false);
    this->selectCollection->SetSelectStyle(Selection::NORMAL);
    this->selectCollection->Initialize();

    this->renderer->RemoveActor(this->selectCollection->GetSelectActor());
    this->renderWin->Render();

    this->actionSelectInvert->setEnabled(false);
    this->rwi->SetInteractorStyle(this->rwiStyle);
}

void FDEMVTKRenderWidget::slotRenderGeometryActor(const QModelIndex &index)
{
    QString id = index.data().toString();
    QString entry = id.split(QChar('-')).first();
    if(QString("boundary") == entry)
    {
        QString blockId = index.data(Qt::UserRole + 1).toString();
        QSharedPointer<Primitive> primitive = blockCollection->GetBlockBoundary(blockId, id);
        QSharedPointer<PrimitiveActor> actor = actorCollection->GetBlockBoundaryActor(blockId, id);
        actor->Update(primitive);

        renderer->AddActor(actor->GetVTKRenderActor());
        renderer->ResetCamera();
        renderWin->Render();
    }
    else if(QString("fracture") == entry)
    {
        QString blockId = index.data(Qt::UserRole + 1).toString();
        QSharedPointer<Primitive> primitive = blockCollection->GetBlockFracture(blockId, id);
        QSharedPointer<PrimitiveActor> actor = actorCollection->GetBlockFractureActor(blockId, id);
        actor->Update(primitive);

        renderer->AddActor(actor->GetVTKRenderActor());
        renderWin->Render();
    }
    else if(QString("bedplane") == entry)
    {
        QString blockId = index.data(Qt::UserRole + 1).toString();
        QSharedPointer<Primitive> primitive = blockCollection->GetBlockBedplane(blockId, id);
        QSharedPointer<PrimitiveActor> actor = actorCollection->GetBlockBedplaneActor(blockId, id);
        actor->Update(primitive);

        renderer->AddActor(actor->GetVTKRenderActor());
        renderWin->Render();
    }
    else if(QString("clipseed") == entry)
    {
        QString blockId = index.data(Qt::UserRole + 1).toString();
        QSharedPointer<Primitive> primitive = blockCollection->GetBlockClipseed(blockId, id);
        QSharedPointer<PrimitiveActor> actor = actorCollection->GetBlockClipseedActor(blockId, id);
        actor->Update(primitive);

        renderer->AddActor(actor->GetVTKRenderActor());
        renderWin->Render();
    }
}

void FDEMVTKRenderWidget::slotRemoveGeometryActor(const QModelIndex& index)
{
    QString id = index.data().toString();
    QString type = id.split(QChar('-')).first();
    if(QString("block") == type)
    {
        QString blockId = index.data().toString();

        /* Remove Triangulation */
        QSharedPointer<TriangulationActor> meshActor = this->actorCollection->GetBlockTriangulationActor(blockId);
        renderer->RemoveActor(meshActor->GetVTKRenderActor());

        /* Remove Primitives */
        BlockActor::PrimitiveActorMap entityMap;
        entityMap = actorCollection->GetBlockBoundaryMap(blockId);
        for(auto iter = entityMap.begin(); iter != entityMap.end(); ++iter)
        {
            renderer->RemoveActor((*iter)->GetVTKRenderActor());
        }

        entityMap = actorCollection->GetBlockFractureMap(blockId);
        for(auto iter = entityMap.begin(); iter != entityMap.end(); ++iter)
        {
            renderer->RemoveActor((*iter)->GetVTKRenderActor());
        }

        entityMap = actorCollection->GetBlockBedplaneMap(blockId);
        for(auto iter = entityMap.begin(); iter != entityMap.end(); ++iter)
        {
            renderer->RemoveActor((*iter)->GetVTKRenderActor());
        }

        entityMap = actorCollection->GetBlockClipseedMap(blockId);
        for(auto iter = entityMap.begin(); iter != entityMap.end(); ++iter)
        {
            renderer->RemoveActor((*iter)->GetVTKRenderActor());
        }

        renderer->ResetCamera();
        renderWin->Render();
    }
    else if(QString("boundary") == type)
    {
        QString blockId = index.data(Qt::UserRole + 1).toString();
        QSharedPointer<PrimitiveActor> actor = actorCollection->GetBlockBoundaryActor(blockId, id);
        renderer->RemoveActor(actor->GetVTKRenderActor());
        renderer->ResetCamera();
        renderWin->Render();
    }
    else if(QString("fracture") == type)
    {
        QString blockId = index.data(Qt::UserRole + 1).toString();
        QSharedPointer<PrimitiveActor> actor = actorCollection->GetBlockFractureActor(blockId, id);
        renderer->RemoveActor(actor->GetVTKRenderActor());
        renderer->ResetCamera();
        renderWin->Render();
    }
    else if(QString("bedplane") == type)
    {
        QString blockId = index.data(Qt::UserRole + 1).toString();
        QSharedPointer<PrimitiveActor> actor = actorCollection->GetBlockBedplaneActor(blockId, id);
        renderer->RemoveActor(actor->GetVTKRenderActor());
        renderer->ResetCamera();
        renderWin->Render();
    }
}

void FDEMVTKRenderWidget::slotGeometryActorVisibility(QStandardItem *item)
{
    QString entity = item->data(Qt::DisplayRole).toString();
    QString prefix = entity.split(QChar('-')).first();

    if(QString("boundary") == prefix)
    {
        QString blockId = item->data(Qt::UserRole + 1).toString();
        QSharedPointer<PrimitiveActor> primitiveActor =
                this->actorCollection->GetBlockBoundaryActor(blockId, entity);
        primitiveActor->GetVTKRenderActor()->SetVisibility(item->checkState());
    }
    else if(QString("fracture") == prefix)
    {
        QString blockId = item->data(Qt::UserRole + 1).toString();
        QSharedPointer<PrimitiveActor> primitiveActor =
                this->actorCollection->GetBlockFractureActor(blockId, entity);
        primitiveActor->GetVTKRenderActor()->SetVisibility(item->checkState());
    }
    else if(QString("bedplane") == prefix)
    {
        QString blockId = item->data(Qt::UserRole + 1).toString();
        QSharedPointer<PrimitiveActor> primitiveActor =
                this->actorCollection->GetBlockBedplaneActor(blockId, entity);
        primitiveActor->GetVTKRenderActor()->SetVisibility(item->checkState());
    }
    else if(QString("clipseed") == prefix)
    {
        QString blockId = item->data(Qt::UserRole + 1).toString();
        QSharedPointer<PrimitiveActor> primitiveActor =
                this->actorCollection->GetBlockClipseedActor(blockId, entity);
        primitiveActor->GetVTKRenderActor()->SetVisibility(item->checkState());
    }

    renderWin->Render();
}

void FDEMVTKRenderWidget::slotSetBlockGeometryActorColor(const QString &blockId, const QString &id, const QColor &color)
{
    QSharedPointer<PrimitiveActor> actor;

    QString entry = id.split(QChar('-')).first();
    if(QString("boundary") == entry)
    {
        actor = this->actorCollection->GetBlockBoundaryActor(blockId, id);
        vtkProperty *prop = actor->GetVTKRenderActor()->GetProperty();
        prop->SetColor(color.redF(), color.greenF(), color.blueF());
        renderWin->Render();
    }
    else if(QString("fracture") == entry)
    {
        actor = this->actorCollection->GetBlockFractureActor(blockId, id);
        vtkProperty *prop = actor->GetVTKRenderActor()->GetProperty();
        prop->SetColor(color.redF(), color.greenF(), color.blueF());
        renderWin->Render();
    }
    else if(QString("bedplane") == entry)
    {
        actor = this->actorCollection->GetBlockBedplaneActor(blockId, id);
        vtkProperty *prop = actor->GetVTKRenderActor()->GetProperty();
        prop->SetColor(color.redF(), color.greenF(), color.blueF());
        renderWin->Render();
    }
    else if(QString("clipseed") == entry)
    {
        actor = this->actorCollection->GetBlockClipseedActor(blockId, id);
        vtkProperty *prop = actor->GetVTKRenderActor()->GetProperty();
        prop->SetColor(color.redF(), color.greenF(), color.blueF());
        renderWin->Render();
    }
}

void FDEMVTKRenderWidget::slotRenderBlockVoronoiActor(const QString &blockId)
{
    QSharedPointer<VoronoiDiagram> voronoi = this->blockCollection->GetBlockVoronoiDiagram(blockId);
    QSharedPointer<VoronoiDiagramActor> actor = this->actorCollection->GetBlockVoronoiDiagramActor(blockId);

    actor->Update(voronoi);
    actor->GetVTKRenderActor()->SetVisibility(true);

    renderer->AddActor(actor->GetVTKRenderActor());
    renderer->ResetCamera();
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotSetVoronoiActorVisibility(const QString &blockId, bool checked)
{
    QSharedPointer<VoronoiDiagramActor> actor = this->actorCollection->GetBlockVoronoiDiagramActor(blockId);
    actor->SetVisibility(checked);
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotRenderBlockMeshActor(const QString& blockId)
{
    QSharedPointer<Triangulation> mesh = this->blockCollection->GetBlockTriangulation(blockId);
    QSharedPointer<TriangulationActor> actor = this->actorCollection->GetBlockTriangulationActor(blockId);

    actor->Update(mesh);
    actor->UpdateInformation(blockId);
    actor->UpdateNodeIdLabelActor();
    actor->UpdateCellIdLabelActor();

    renderer->AddActor(actor->GetVTKRenderActor());
    renderer->AddActor(actor->GetFeatureEdgeActor());
    renderer->AddActor2D(actor->GetNodeIdLabelActor());
    renderer->AddActor2D(actor->GetCellIdLabelActor());

    renderer->ResetCamera();
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotDeleteBlockMeshActor(const QString &blockId)
{
    QSharedPointer<VoronoiDiagramActor> voronoiActor = this->actorCollection->GetBlockVoronoiDiagramActor(blockId);
    renderer->RemoveActor(voronoiActor->GetVTKRenderActor());

    QSharedPointer<TriangulationActor> meshActor = this->actorCollection->GetBlockTriangulationActor(blockId);
    renderer->RemoveActor(meshActor->GetVTKRenderActor());
    renderer->RemoveActor(meshActor->GetCellIdLabelActor());
    renderer->RemoveActor(meshActor->GetNodeIdLabelActor());

    renderer->ResetCamera();
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotShrinkBlockMeshActor(const QString &blockId)
{
    QSharedPointer<Triangulation> mesh = this->blockCollection->GetBlockTriangulation(blockId);
    QSharedPointer<TriangulationActor> actor = this->actorCollection->GetBlockTriangulationActor(blockId);

    actor->Shrink(mesh);
    actor->UpdateInformation(blockId);
    actor->UpdateNodeIdLabelActor();
    actor->UpdateCellIdLabelActor();

    renderer->ResetCamera();
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotSetMeshActorVisibility(const QString &blockId, bool checked)
{
    QSharedPointer<TriangulationActor> actor = this->actorCollection->GetBlockTriangulationActor(blockId);
    actor->SetVisibility(checked);
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotSetMeshFaceColor(const QString &blockId, const QColor &color)
{
    QSharedPointer<TriangulationActor> meshActor = this->actorCollection->GetBlockTriangulationActor(blockId);
    meshActor->SetFaceColor(color);
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotSetMeshEdgeColor(const QString &blockId, const QColor &color)
{
    QSharedPointer<TriangulationActor> meshActor = this->actorCollection->GetBlockTriangulationActor(blockId);
    meshActor->SetEdgeColor(color);
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotClearImportedMeshActors()
{
    this->actorCollection->Clear();
    renderer->RemoveAllViewProps();
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotRenumberMeshButtonClicked()
{
    this->actorCollection->ApplyTriangulationGlobalIds();
    this->actorCollection->ApplyTriangulationPedigreeId();

    vtkIdType numNodes = this->actorCollection->GetNumberOfNodes();
    vtkIdType numCells = this->actorCollection->GetNumberOfCells();
    vtkIdType numTriangles = this->actorCollection->GetNumberOfTriangles();
    vtkIdType numQuads = this->actorCollection->GetNumberOfQuads();

    this->fdemInformationWidget->slotOutputPromptMessage(QString("[>>>] Number of nodes:      [%1]").arg(numNodes));
    this->fdemInformationWidget->slotOutputPromptMessage(QString("[>>>] Number of cells:      [%1]").arg(numCells));
    this->fdemInformationWidget->slotOutputPromptMessage(QString("[>>>] Number of triangles:  [%1]").arg(numTriangles));
    this->fdemInformationWidget->slotOutputPromptMessage(QString("[>>>] Number of quads:      [%1]").arg(numQuads));
}

void FDEMVTKRenderWidget::slotRefineLocalCohesiveQuads()
{
    this->blockCollection->UpdateTriangulationConstraint();
    this->slotCancelSelection();
}

void FDEMVTKRenderWidget::slotApplyGroupButtonClicked()
{
    this->slotCancelSelection();
}

void FDEMVTKRenderWidget::slotHighlightGroupButtonClicked()
{
    renderer->AddActor(this->selectCollection->GetSelectActor());
    renderWin->Render();
}

void FDEMVTKRenderWidget::slotCancelGroupButtonClicked()
{
    this->slotCancelSelection();
}

void FDEMVTKRenderWidget::slotApplyMaterialButtonClicked(const QString& id)
{
    this->actorCollection->ApplyTriangulationMaterialIndexArray(id);
    this->actorCollection->ApplyTriangulationMaterialFieldArray(id);
    this->slotCancelSelection();
}

void FDEMVTKRenderWidget::slotCancelMaterialButtonClicked(const QString &id)
{
    this->actorCollection->CancelTriangulationMaterialId(id);
    this->slotRescaleTableRange();
}

void FDEMVTKRenderWidget::slotClearMaterialButtonClicked()
{
    this->actorCollection->ClearTriangulationMaterialId();
    this->slotRescaleTableRange();
}

void FDEMVTKRenderWidget::slotApplyBoundaryButtonClicked(const QString &id)
{
    this->actorCollection->ApplyTriangulationBoundaryIndexArray(id);
    this->actorCollection->ApplyTriangulationBoundaryFieldArray(id);
    this->slotCancelSelection();
}

void FDEMVTKRenderWidget::slotCancelBoundaryButtonClicked(const QString &id)
{
    this->actorCollection->CancelTriangulationBoundaryFieldArray(id);
    this->slotRescaleTableRange();
}

void FDEMVTKRenderWidget::slotClearBoundaryButtonClicked()
{
    this->actorCollection->ClearTriangulationBoundaryFieldArray();
    this->slotRescaleTableRange();
}

void FDEMVTKRenderWidget::slotApplyTemporalButtonClicked(const QString &id)
{
    QSharedPointer<TemporalCollection> temporalCollection = TemporalCollection::GetTemporalCollection();
    QSharedPointer<Temporal> temporal = temporalCollection->GetTemporal(id);

    if(Temporal::INISTRESS != temporal->getTemporalType())
    {
        this->actorCollection->ApplyTriangulationTemporalIndexArray(id);
        this->actorCollection->ApplyTriangulationTemporalFieldArray(id);
    }
    this->slotCancelSelection();
}

void FDEMVTKRenderWidget::slotCancelTemporalButtonClicked(const QString &id)
{
    this->actorCollection->CancelTriangulationTemporalFieldArray(id);
    this->slotRescaleTableRange();
}

void FDEMVTKRenderWidget::slotClearTemporalButtonClicked()
{
    this->actorCollection->ClearTriangulationTemporalFieldArray();
    this->slotRescaleTableRange();
}

void FDEMVTKRenderWidget::slotApplyHistoryButtonClicked()
{
    this->slotCancelSelection();
}

void FDEMVTKRenderWidget::slotCancelHistoryButtonClicked()
{
    this->slotCancelSelection();
}

QPointer<FDEMWindow> FDEMVTKRenderWidget::getFdemWindow() const
{
    return fdemWindow;
}

void FDEMVTKRenderWidget::setFdemWindow(const QPointer<FDEMWindow> &value)
{
    fdemWindow = value;
}

QPointer<FDEMInformationWidget> FDEMVTKRenderWidget::getFdemInformationWidget() const
{
    return fdemInformationWidget;
}

void FDEMVTKRenderWidget::setFdemInformationWidget(const QPointer<FDEMInformationWidget> &value)
{
    fdemInformationWidget = value;
}

QPointer<FDEMParameterWidget> FDEMVTKRenderWidget::getFdemParameterWidget() const
{
    return fdemParameterWidget;
}

void FDEMVTKRenderWidget::setFdemParameterWidget(const QPointer<FDEMParameterWidget> &value)
{
    fdemParameterWidget = value;
}
