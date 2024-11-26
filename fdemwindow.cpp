#include "fdemwindow.h"
#include "ui_fdemwindow.h"
#include "fdemparameterwidget.h"
#include "fdemvtkrenderwidget.h"
#include "fdeminformationwidget.h"
#include "actorcollection.h"
#include "materialcollection.h"
#include "boundarycollection.h"
#include "temporalcollection.h"
#include "actor.h"
#include "material.h"
#include "boundary.h"
#include "temporal.h"
#include "fdemdocumentexporter.h"
#include "backgroundgriddialog.h"
#include "fractaldimensiondialog.h"
#include "transformdialog.h"
#include "globalcollection.h"

#include <QDebug>
#include <QAction>
#include <QDateTime>
#include <QMessageBox>
#include <QComboBox>
#include <QSizePolicy>
#include <QFileDialog>
#include <QUrl>
#include <QDesktopServices>

#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>

FDEMWindow::FDEMWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FDEMWindow)
{
    ui->setupUi(this);
    this->InitFDEMWindow();
}

FDEMWindow::~FDEMWindow()
{
    delete ui;
    delete fdemParameterWidget;
    delete fdemVTKRenderWidget;
    delete fdemInformationWidget;
}

void FDEMWindow::InitFDEMWindow()
{

    this->globalCollection = GlobalCollection::GetGlobalCollection();
    this->setWindowTitle(QString("QFDEM(Version_3.25.33_LTS)"));
    this->CreateManualActions();
    this->CreateToolBarWidget();
    this->CreateStatusBarWidget();

    fdemParameterWidget = new FDEMParameterWidget;
    ui->parameterWidget->setWidget(this->fdemParameterWidget);
    fdemVTKRenderWidget = new FDEMVTKRenderWidget;
    this->setCentralWidget(this->fdemVTKRenderWidget);
    fdemInformationWidget = new FDEMInformationWidget;
    ui->informationWidget->setWidget(this->fdemInformationWidget);

    fdemParameterWidget->setFdemWindow(this);
    fdemParameterWidget->setFdemVTKRenderWidget(this->fdemVTKRenderWidget);
    fdemParameterWidget->setFdemInformationWidget(this->fdemInformationWidget);
    fdemParameterWidget->ConnectSignalSlots();

    fdemVTKRenderWidget->setFdemWindow(this);
    fdemVTKRenderWidget->setFdemParameterWidget(this->fdemParameterWidget);
    fdemVTKRenderWidget->setFdemInformationWidget(this->fdemInformationWidget);
    fdemVTKRenderWidget->ConnectSignalSlots();

    this->ConnectSignalSlots();
}

void FDEMWindow::CreateManualActions()
{
    QAction *actionPara = ui->parameterWidget->toggleViewAction();
    QAction *actionInfo = ui->informationWidget->toggleViewAction();
    ui->menuWindow->addAction(actionPara);
    ui->menuWindow->addAction(actionInfo);
}

void FDEMWindow::CreateToolBarWidget()
{
    QString path = QString(":/images/resources/");

    this->visualizationBox = new QComboBox(this);
    this->visualizationBox->setIconSize(QSize(18, 18));
    this->visualizationBox->setMinimumSize(200, 24);
    this->visualizationBox->setMaximumSize(200, 24);
    this->visualizationBox->addItem(QIcon(path + QString("pqCellData.png")), QString("Solid Color"), 1);
    this->visualizationBox->setCurrentText(QString("Solid Color"));

    this->componentNameBox = new QComboBox(this);
    this->componentNameBox->setMinimumSize(80, 24);
    this->componentNameBox->setMaximumSize(80, 24);
    this->componentNameBox->setEnabled(false);

    this->representationBox = new QComboBox(this);
    this->representationBox->setMinimumSize(180, 24);
    this->representationBox->setMaximumSize(180, 24);
    this->representationBox->addItem(QString("Points"));
    this->representationBox->addItem(QString("Wireframe"));
    this->representationBox->addItem(QString("Surface"));
    this->representationBox->addItem(QString("Surface With Edges"));
    this->representationBox->addItem(QString("FeatureEdges"));
    this->representationBox->setCurrentText(QString("Surface With Edges"));

    this->colorSchemeBox = new QComboBox(this);
    this->colorSchemeBox->setMinimumSize(180, 24);
    this->colorSchemeBox->setMaximumSize(200, 24);
    this->colorSchemeBox->addItem(QString("Classic RGB Jet"));
    this->colorSchemeBox->addItem(QString("Classic RGB Rainbow Uniform"));
    this->colorSchemeBox->addItem(QString("Classic HSV Rainbow Tecplot"));
    this->colorSchemeBox->addItem(QString("Classic Diverging Cool to Warm"));
    this->colorSchemeBox->addItem(QString("Classic LAB Cool to Warm (Extended)"));
    this->colorSchemeBox->setCurrentText(QString("Classic Diverging Cool to Warm"));

    ui->dataToolBar->layout()->setSpacing(6);
    ui->dataToolBar->addWidget(this->visualizationBox);
    ui->dataToolBar->addWidget(this->componentNameBox);

    ui->viewToolBar->layout()->setSpacing(6);
    ui->viewToolBar->addWidget(this->colorSchemeBox);
    ui->viewToolBar->addWidget(this->representationBox);
}

void FDEMWindow::CreateStatusBarWidget()
{

}

void FDEMWindow::ConnectSignalSlots()
{
    connect(ui->actionWorkspace, &QAction::triggered,
            this, &FDEMWindow::slotWorkspaceDirectoryTriggered);
    connect(ui->actionExportModel, &QAction::triggered,
            this, &FDEMWindow::slotExportModelTriggered);
    connect(ui->actionExportGeometry, &QAction::triggered,
            this, &FDEMWindow::slotExportGeometryTriggered);

    connect(ui->actionExportBoundary, &QAction::triggered,
            this, &FDEMWindow::slotExportBoundaryTriggered);
    connect(ui->actionExportFracture, &QAction::triggered,
            this, &FDEMWindow::slotExportFractureTriggered);

    connect(ui->actionExportTriangulation, &QAction::triggered,
            this, &FDEMWindow::slotExportTriangulationTriggered);
    connect(ui->actionExportVoronoi, &QAction::triggered,
            this, &FDEMWindow::slotExportVoronoiDiagramTriggered);

    connect(ui->actionTranslate, &QAction::triggered,
            this, &FDEMWindow::slotTranslateActionTriggered);
    connect(ui->actionRotate, &QAction::triggered,
            this, &FDEMWindow::slotRotateActionTriggered);
    connect(ui->actionScale, &QAction::triggered,
            this, &FDEMWindow::slotScalingActionTriggered);

    connect(ui->actionWindowStatusBar, &QAction::toggled,
            this, &FDEMWindow::slotToggleStatusBar);
    connect(ui->actionViewFullScreen, &QAction::toggled,
            this, &FDEMWindow::slotToggleFullScreen);
    connect(ui->actionIndexedLUT, &QAction::triggered,
            this, &FDEMWindow::slotToggleIndexedLUT);

    connect(ui->actionAbout, &QAction::triggered,
            this, &FDEMWindow::slotAboutTriggered);
    connect(ui->actionHelpTimestep, &QAction::triggered,
            this, &FDEMWindow::slotHelpTimestepSelection);

    connect(ui->actionSettingsGrid, &QAction::triggered,
            this, &FDEMWindow::slotGridSettingTriggered);
    connect(ui->actionFractalDim, &QAction::triggered,
            this, &FDEMWindow::slotFractalDimensionTriggered);

    connect(ui->actionResetRange, &QAction::triggered,
            fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotRescaleTableRange);
    connect(ui->actionViewAxes, &QAction::triggered,
            fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotViewAxesToggled);
    connect(ui->actionViewGrid, &QAction::triggered,
            fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotViewBackgroundGridToggled);
    connect(ui->actionViewFrontView, &QAction::triggered,
            fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotFrontViewTriggered);
    connect(ui->actionViewResetCamera, &QAction::triggered,
            fdemVTKRenderWidget,&FDEMVTKRenderWidget::slotResetCameraTriggered);
    connect(ui->actionViewNodeIds, &QAction::triggered,
            fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotMeshNodeIdsToggled);
    connect(ui->actionViewCellIds, &QAction::triggered,
            fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotMeshCellIdsToggled);

    connect(this->visualizationBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMWindow::slotVisualizationBoxChanged);
    connect(this->visualizationBox, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
            fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotVisualizationBoxChanged);
    connect(this->componentNameBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotComponentNameBoxChanged);
    connect(this->colorSchemeBox, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
            fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotColorSchemeBoxChanged);
    connect(this->representationBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotRepresentationBoxChanged);
}

void FDEMWindow::InsertVisualizationItem(const QIcon &icon, const QString &name, const QVariant &userData)
{
    if(-1 == this->visualizationBox->findText(name))
    {
        this->visualizationBox->addItem(icon, name, userData);
        this->visualizationBox->model()->sort(0, Qt::AscendingOrder);
    }
}

void FDEMWindow::RemoveVisualizationItem(QString name)
{
    this->visualizationBox->removeItem(this->visualizationBox->findText(name));
    this->visualizationBox->setCurrentText(QString("Solid Color"));
}

void FDEMWindow::InitColorSeriesSchemes()
{
    disconnect(this->colorSchemeBox, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
               fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotColorSchemeBoxChanged);

    this->colorSchemeBox->clear();
    this->colorSchemeBox->addItem(QString("Spectrum"));
    this->colorSchemeBox->addItem(QString("Brewer Diverging Purple-Orange (11)"));
    this->colorSchemeBox->addItem(QString("Brewer Diverging Spectral (11)"));
    this->colorSchemeBox->addItem(QString("Brewer Diverging Brown-Blue-Green (11)"));
    this->colorSchemeBox->addItem(QString("Brewer Qualitative Accent"));
    this->colorSchemeBox->addItem(QString("Brewer Qualitative Dark2"));
    this->colorSchemeBox->addItem(QString("Brewer Qualitative Set2"));
    this->colorSchemeBox->addItem(QString("Brewer Qualitative Pastel2"));
    this->colorSchemeBox->addItem(QString("Brewer Qualitative Pastel1"));
    this->colorSchemeBox->addItem(QString("Brewer Qualitative Set1"));
    this->colorSchemeBox->addItem(QString("Brewer Qualitative Paired"));
    this->colorSchemeBox->addItem(QString("Brewer Qualitative Set3"));
    this->colorSchemeBox->addItem(QString("Custom KAAMStep"));

    connect(this->colorSchemeBox, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
            fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotColorSchemeBoxChanged);

    this->colorSchemeBox->setCurrentText(QString("Brewer Qualitative Set3"));
}

void FDEMWindow::InitColorFunctionSchemes()
{
    disconnect(this->colorSchemeBox, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
               fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotColorSchemeBoxChanged);

    this->colorSchemeBox->clear();
    this->colorSchemeBox->addItem(QString("Classic RGB Jet"));
    this->colorSchemeBox->addItem(QString("Classic RGB Rainbow Uniform"));
    this->colorSchemeBox->addItem(QString("Classic HSV Rainbow Tecplot"));
    this->colorSchemeBox->addItem(QString("Classic Diverging Cool to Warm"));
    this->colorSchemeBox->addItem(QString("Classic LAB Cool to Warm (Extended)"));

    connect(this->colorSchemeBox, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
            fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotColorSchemeBoxChanged);

    this->colorSchemeBox->setCurrentText(QString("Classic Diverging Cool to Warm"));
}

void FDEMWindow::slotToggleFullScreen(bool checked)
{
    ui->parameterWidget->setVisible(!checked);
    ui->informationWidget->setVisible(!checked);
    this->menuBar()->setVisible(!checked);
    this->statusBar()->setVisible(!checked);

    if(checked)
    {
        this->showFullScreen();
    }
    else
    {
        this->showNormal();
    }
}

void FDEMWindow::slotToggleIndexedLUT(bool checked)
{
    if(checked)
    {
        this->InitColorSeriesSchemes();
    }
    else
    {
        this->InitColorFunctionSchemes();
    }
}

void FDEMWindow::slotToggleStatusBar(bool checked)
{
    this->statusBar()->setVisible(checked);
}

void FDEMWindow::slotAboutTriggered()
{
    QMessageBox::about(this, QString("About ") + this->windowTitle(),
                       QString("<div><center><h1>") + QString("QFDEM(Version_3.25.33_LTS)") + "</h1>" +
                       "<div>Copyright &#169; 2025 Wanrun Li</div>"
                       "<div>This program is the preprocessor of FDEM input files.</div>"
                       "<div>If you have any problems, feel free to send emails to "
                       "<a href=\"mailto:liwanrun@qq.com\">liwanrun@qq.com</a></div>");
}

void FDEMWindow::slotHelpTimestepSelection()
{
    QDesktopServices::openUrl(QUrl("file:./manuals/selection of stable timestep.pdf", QUrl::TolerantMode));
}

void FDEMWindow::slotWorkspaceDirectoryTriggered()
{
    QString dir = QFileDialog::getExistingDirectory(
                this,
                tr("Select Workspace Directory"),
                this->globalCollection->getWorkspace(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    this->globalCollection->setWorkspace(dir);
}

void FDEMWindow::slotExportModelTriggered()
{
    QString header = QLatin1String("Export Model.y to Solver");
    QString filter = QLatin1String("Text files (*.y)");
    QString fname = QFileDialog::getSaveFileName(this, header, this->globalCollection->getWorkspace(), filter);

    if(fname.isEmpty() || fname.isNull()) return;
    
    QSharedPointer<FDEMDocumentExporter> exporter =
            QSharedPointer<FDEMDocumentExporter>(new FDEMDocumentExporter(fname));
    exporter->Write();

    this->fdemInformationWidget->slotOutputPromptMessage(QString("[>>>] Path: \"%1\"").arg(fname));
}

void FDEMWindow::slotExportGeometryTriggered()
{
    QString header = QLatin1String("Export Geometry to Gmsh");
    QString filter = QLatin1String("Gmsh GEO (*.geo)");
    QString fname = QFileDialog::getSaveFileName(this, header, this->globalCollection->getWorkspace(), filter);
    if(!fname.isEmpty())
    {
        QFile file(fname);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(nullptr,QString("Export Error"),QString("Failed to open the file!"));
            return;
        }

        QTextStream out(&file);
        QLocale locale = QLocale::English;
        QString datetime = locale.toString(QDateTime::currentDateTime(),QString("yyyy/MM/dd(dddd) HH:mm:ss"));
        out << QString("// Gmsh project created on ") << datetime << reset << endl;
        out << QString("SetFactory(\"OpenCASCADE\");") << endl;
        out << QString("//+") << endl;

        vtkIdType nodeOff = 0;
        vtkIdType edgeOff = 0;

        QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
        ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

        for(auto block_iter = block_map.begin(); block_iter != block_map.end(); ++block_iter)
        {
            BlockActor::PrimitiveActorMap actor_map = block_iter.value()->GetBoundaryMap();
            for(auto iter = actor_map.begin(); iter != actor_map.end(); ++iter)
            {
                vtkPolyData* pd = block_iter.value()->GetBoundaryActor(iter.key())->GetVTKActorMapper()->GetInput();
                vtkPoints *points = pd->GetPoints();
                for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
                {
                    double x[3];
                    points->GetPoint(i, x);
                    out << forcepoint << QString("Point(%1) = {%2, %3, %4, %5};")
                           .arg(i+1 + nodeOff).arg(x[0]).arg(x[1]).arg(x[2]).arg(1.0) << endl;
                    out << QString("//+") << endl;
                }

                vtkCellArray *lines = pd->GetLines();
                for(vtkIdType i = 0; i < lines->GetNumberOfCells(); i++)
                {
                    vtkIdType npts;
                    vtkIdType *ids;

                    lines->GetCell(i, npts, ids);
                    for(int j = 0; j < npts-1; j++)
                    {
                        out << QString("Line(%1) = {%2, %3};")
                               .arg(j+1 + edgeOff).arg(ids[j]+1 + nodeOff).arg(ids[j+1]+1 + nodeOff) << endl;
                        out << QString("//+") << endl;
                    }
                    edgeOff += (npts - 1);
                }

                nodeOff += points->GetNumberOfPoints();
            }
        }

        for(auto block_iter = block_map.begin(); block_iter != block_map.end(); ++block_iter)
        {
            BlockActor::PrimitiveActorMap actor_map = block_iter.value()->GetFractureMap();
            for(auto iter = actor_map.begin(); iter != actor_map.end(); ++iter)
            {
                vtkPolyData* pd = block_iter.value()->GetFractureActor(iter.key())->GetVTKActorMapper()->GetInput();
                vtkPoints *points = pd->GetPoints();
                for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
                {
                    double x[3];
                    points->GetPoint(i, x);
                    out << forcepoint << QString("Point(%1) = {%2, %3, %4, %5};")
                           .arg(i+1 + nodeOff).arg(x[0]).arg(x[1]).arg(x[2]).arg(1.0) << endl;
                    out << QString("//+") << endl;
                }

                vtkIdType elemOff = 0;
                vtkCellArray *lines = pd->GetLines();
                for(vtkIdType i = 0; i < lines->GetNumberOfCells(); i++)
                {
                    vtkIdType npts;
                    vtkIdType *ids;

                    lines->GetCell(elemOff, npts, ids);
                    out << QString("Line(%1) = {%2, %3};")
                           .arg(edgeOff+1).arg(ids[0]+1 + nodeOff).arg(ids[1]+1 + nodeOff) << endl;
                    out << QString("//+") << endl;

                    elemOff += (npts + 1);
                    edgeOff += 1;
                }

                nodeOff += points->GetNumberOfPoints();
            }
        }

        this->fdemInformationWidget->slotOutputPromptMessage(QString("[>>>] Path: \"%1\"").arg(fname));
    }
}

void FDEMWindow::slotExportBoundaryTriggered()
{
    QString header = QLatin1String("Export Boundaries to VTK");
    QString filter = QLatin1String("VTK files (*.vtk)");
    QString fname = QFileDialog::getSaveFileName(this, header, this->globalCollection->getWorkspace(), filter);
    if(!fname.isEmpty())
    {
        QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
        ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

        int index = 0;
        for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
        {
            QString prefix = fname.remove(QString(".vtk")) + QString("_%1").arg(index++);
            BlockActor::PrimitiveActorMap actor_map = iter.value()->GetBoundaryMap();

            int jndex = 0;
            for(auto jter = actor_map.begin(); jter != actor_map.end(); ++jter)
            {
                QString vtkfile = prefix + QString("_%1.vtk").arg(jndex++);
                vtkPolyData* pd = iter.value()->GetBoundaryActor(jter.key())->GetVTKActorMapper()->GetInput();
                vtkNew<vtkPolyDataWriter> writer;
                writer->SetFileName(vtkfile.toStdString().c_str());
                writer->SetInputData(pd);
                writer->WriteArrayMetaDataOff();
                writer->Write();
            }
        }
        this->fdemInformationWidget->slotOutputPromptMessage(QString("[>>>] Path: \"%1\"").arg(fname));
    }
}

void FDEMWindow::slotExportFractureTriggered()
{
    QString header = QLatin1String("Export Fractures to VTK");
    QString filter = QLatin1String("VTK files (*.vtk)");
    QString fname = QFileDialog::getSaveFileName(this, header, this->globalCollection->getWorkspace(), filter);
    if(!fname.isEmpty())
    {
        QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
        ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

        int index = 0;
        for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
        {
            QString prefix = fname.remove(QString(".vtk")) + QString("_%1").arg(index++);
            BlockActor::PrimitiveActorMap actor_map = iter.value()->GetFractureMap();

            int jndex = 0;
            for(auto jter = actor_map.begin(); jter != actor_map.end(); ++jter)
            {
                QString vtkfile = prefix + QString("_%1.vtk").arg(jndex++);
                vtkPolyData* pd = iter.value()->GetFractureActor(jter.key())->GetVTKActorMapper()->GetInput();
                vtkNew<vtkPolyDataWriter> writer;
                writer->SetFileName(vtkfile.toStdString().c_str());
                writer->SetInputData(pd);
                writer->WriteArrayMetaDataOff();
                writer->Write();
            }
        }
        this->fdemInformationWidget->slotOutputPromptMessage(QString("[>>>] Path: \"%1\"").arg(fname));
    }
}

void FDEMWindow::slotExportTriangulationTriggered()
{
    QString header = QLatin1String("Export Triangulation to VTK");
    QString filter = QLatin1String("VTK files (*.vtk)");
    QString fname = QFileDialog::getSaveFileName(this, header, this->globalCollection->getWorkspace(), filter);
    if(!fname.isEmpty())
    {
        QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
        ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

        int index = 0;
        for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
        {
            QString vtkfile = fname.remove(QString(".vtk")) + QString("_%1.vtk").arg(index++);
            vtkPolyData* pd = iter.value()->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
            vtkNew<vtkPolyDataWriter> writer;
            writer->SetFileName(vtkfile.toStdString().c_str());
            writer->SetInputData(pd);
            writer->WriteArrayMetaDataOff();
            writer->Write();
        }
        this->fdemInformationWidget->slotOutputPromptMessage(QString("[>>>] Path: \"%1\"").arg(fname));
    }
}

void FDEMWindow::slotExportVoronoiDiagramTriggered()
{
    QString header = QLatin1String("Export Voronoi Diagram to VTK");
    QString filter = QLatin1String("VTK files (*.vtk)");
    QString fname = QFileDialog::getSaveFileName(this, header, this->globalCollection->getWorkspace(), filter);
    if(!fname.isEmpty())
    {
        QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
        ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

        int index = 0;
        for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
        {
            QString vtkfile = fname.remove(QString(".vtk")) + QString("_%1.vtk").arg(index++);
            vtkPolyData* pd = iter.value()->GetVoronoiDiagramActor()->GetVTKActorMapper()->GetInput();
            vtkNew<vtkPolyDataWriter> writer;
            writer->SetFileName(vtkfile.toStdString().c_str());
            writer->SetInputData(pd);
            writer->WriteArrayMetaDataOff();
            writer->Write();
        }
        this->fdemInformationWidget->slotOutputPromptMessage(QString("[>>>] Path: \"%1\"").arg(fname));
    }
}

void FDEMWindow::slotTranslateActionTriggered()
{
    TransformDialog *dlg = new TransformDialog(0, this->fdemVTKRenderWidget);
    dlg->setWindowTitle(QString("Translate Dialog"));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void FDEMWindow::slotRotateActionTriggered()
{
    TransformDialog *dlg = new TransformDialog(1, this->fdemVTKRenderWidget);
    dlg->setWindowTitle(QString("Rotate Dialog"));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void FDEMWindow::slotScalingActionTriggered()
{
    TransformDialog *dlg = new TransformDialog(2, this->fdemVTKRenderWidget);
    dlg->setWindowTitle(QString("Scaling Dialog"));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void FDEMWindow::slotGridSettingTriggered()
{
    BackgroundGridDialog *gridDlg = new BackgroundGridDialog(this->fdemVTKRenderWidget);
    gridDlg->setAttribute(Qt::WA_DeleteOnClose);
    gridDlg->show();
}

void FDEMWindow::slotVisualizationBoxChanged(int index)
{
    QString path = QString(":/images/resources/");
    int numComps = this->visualizationBox->itemData(index).toInt();
    this->componentNameBox->setEnabled((numComps - 1) ? true : false);

    if(1 == numComps)
    {
        this->componentNameBox->clear();
    }
    else if(2 == numComps)
    {
        QStringList texts = QStringList() << "Magnitude" << "X" << "Y";

        this->componentNameBox->clear();
        this->componentNameBox->insertItems(0, texts);
    }
    else if(3 == numComps)
    {
        QStringList texts = QStringList() << "Magnitude" << "XX" << "YY" << "XY";

        this->componentNameBox->clear();
        this->componentNameBox->insertItems(0, texts);
    }
}

void FDEMWindow::slotTriangulationModified()
{
    this->visualizationBox->setCurrentText(QString("Solid Color"));
    disconnect(this->visualizationBox, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
               fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotVisualizationBoxChanged);

    QString path = QString(":/images/resources/");
    this->visualizationBox ->clear();
    this->visualizationBox->addItem(QIcon(path + QString("pqCellData.png")),QString("Solid Color"), 1);
    this->visualizationBox->setCurrentText(QString("Solid Color"));

    connect(this->visualizationBox, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
            fdemVTKRenderWidget, &FDEMVTKRenderWidget::slotVisualizationBoxChanged);
}

void FDEMWindow::slotRenumberVisualizationItem()
{
    QString path = QString(":/images/resources/");
    QIcon pointIcon = QIcon(path + QString("pqPointData.png"));
    QIcon cellIcon = QIcon(path + QString("pqCellData.png"));

    QString pointGlobalName = QString("Point Global Ids");
    this->InsertVisualizationItem(pointIcon, pointGlobalName, 1);

    QString pointPedigreeName = QString("Point Pedigree Ids");
    this->InsertVisualizationItem(pointIcon, pointPedigreeName, 1);

    QString pointBoundName = QString("Point Boundary Tags");
    this->InsertVisualizationItem(pointIcon, pointBoundName, 1);

    QString cellGlobalName = QString("Cell Global Ids");
    this->InsertVisualizationItem(cellIcon, cellGlobalName, 1);

    QString cellPedigreeName = QString("Cell Pedigree Ids");
    this->InsertVisualizationItem(cellIcon, cellPedigreeName, 1);

    QString cellTypeName = QString("Cell Types");
    this->InsertVisualizationItem(cellIcon, cellTypeName, 1);

    QString cellStatusName = QString("Cell Constraint Tags");
    this->InsertVisualizationItem(cellIcon, cellStatusName, 1);

    QString cellBlockName = QString("Cell Block Ids");
    this->InsertVisualizationItem(cellIcon, cellBlockName, 1);

    QString cellGroupName = QString("Cell Grain Ids");
    this->InsertVisualizationItem(cellIcon, cellGroupName, 1);
}

void FDEMWindow::slotInsertMaterialVisualizationItem(const QString& id)
{
    //Add Data Attributes
    QString path = QString(":/images/resources/");
    QIcon cellIcon = QIcon(path + QString("pqCellData.png"));

    QSharedPointer<MaterialCollection> materialCollection = MaterialCollection::GetMaterialCollection();
    QSharedPointer<Material> material = materialCollection->GetMaterial(id);

    this->InsertVisualizationItem(cellIcon, material->GetIndexArrayName(), 1);
    this->InsertVisualizationItem(cellIcon, material->GetYoungModulusArrayName(), 1);
}

void FDEMWindow::slotClearMaterialVisualizationItem()
{
    this->RemoveVisualizationItem(QString("Cell Material Ids"));
    this->RemoveVisualizationItem(QString("Cell Material Young Modulus"));
}

void FDEMWindow::slotInsertBoundaryVisualizationItem(const QString& id)
{
    //Add Data Attributes
    QString path = QString(":/images/resources/");
    QIcon pointIcon = QIcon(path + QString("pqPointData.png"));

    QSharedPointer<BoundaryCollection> boundaryCollection = BoundaryCollection::GetBoundaryCollection();
    QSharedPointer<Boundary> boundary = boundaryCollection->GetBoundary(id);

    this->InsertVisualizationItem(pointIcon, boundary->GetIndexArrayName(), 1);
    this->InsertVisualizationItem(pointIcon, boundary->GetFieldArrayName(), boundary->GetFieldArrayComponents());
}

void FDEMWindow::slotClearBoundaryVisualizationItem()
{
    this->RemoveVisualizationItem(QString("Point Boundary Ids (M)"));
    this->RemoveVisualizationItem(QString("Point Boundary Traction (M)"));
    this->RemoveVisualizationItem(QString("Point Boundary Force (M)"));
    this->RemoveVisualizationItem(QString("Point Boundary Acceleration (M)"));
    this->RemoveVisualizationItem(QString("Point Boundary Velocity (M)"));

    this->RemoveVisualizationItem(QString("Point Boundary Ids (H)"));
    this->RemoveVisualizationItem(QString("Point Boundary Pressure (H)"));
    this->RemoveVisualizationItem(QString("Point Boundary FlowRate (H)"));

    this->RemoveVisualizationItem(QString("Point Boundary Ids (T)"));
    this->RemoveVisualizationItem(QString("Point Boundary Temperature (T)"));
    this->RemoveVisualizationItem(QString("Point Boundary HeatFlux (T)"));
}

void FDEMWindow::slotInsertTemporalVisualizationItem(const QString &id)
{
    //Add Data Attributes
    QString path = QString(":/images/resources/");
    QIcon pointIcon = QIcon(path + QString("pqPointData.png"));

    QSharedPointer<TemporalCollection> temporalCollection = TemporalCollection::GetTemporalCollection();
    QSharedPointer<Temporal> temporal = temporalCollection->GetTemporal(id);
    this->InsertVisualizationItem(pointIcon, temporal->GetIndexArrayName(), 1);
    this->InsertVisualizationItem(pointIcon, temporal->GetFieldArrayName(), temporal->GetFieldArrayComponents());
}

void FDEMWindow::slotClearTemporalVisualizationItem()
{
    this->RemoveVisualizationItem(QString("Point Initial Ids (M)"));
    this->RemoveVisualizationItem(QString("Point Initial Velocity (M)"));
    this->RemoveVisualizationItem(QString("Point Initial Ids (H)"));
    this->RemoveVisualizationItem(QString("Point Initial Pressure (H)"));
    this->RemoveVisualizationItem(QString("Point Initial Ids (T)"));
    this->RemoveVisualizationItem(QString("Point Initial Temperature (T)"));

    this->visualizationBox->setCurrentText(QString("Solid Color"));
}

void FDEMWindow::slotFractalDimensionTriggered()
{
    FractalDimensionDialog *fractalDlg = new FractalDimensionDialog(this);
    fractalDlg->setWindowTitle(QString("Fractal Dialog"));
    fractalDlg->setAttribute(Qt::WA_DeleteOnClose);
    fractalDlg->show();
}

QPointer<FDEMInformationWidget> FDEMWindow::getInformationWidget() const
{
    return fdemInformationWidget;
}

QPointer<FDEMVTKRenderWidget> FDEMWindow::getVTKRenderWidget() const
{
    return fdemVTKRenderWidget;
}

QPointer<FDEMParameterWidget> FDEMWindow::getParameterWidget() const
{
    return fdemParameterWidget;
}

