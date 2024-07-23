#include "fdemmeshwidget.h"
#include "ui_fdemmeshwidget.h"
#include "globalcollection.h"
#include "blockcollection.h"
#include "block.h"
#include "actorcollection.h"
#include "actor.h"
#include "selectcollection.h"
#include "fdemmeshitemdelegate.h"
#include "fdemmeshoptionitemdelegate.h"
#include "fdemmeshimporterfactory.h"
#include "fdemtriangulationfilter.h"
#include "constraineddelaunaytriangulation.h"
#include "constrainedtriangulation.h"
#include "shrinkdialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QSpinBox>
#include <QDoubleSpinBox>

enum ColumnName
{
    COL_BLOCKID = 0,
    COL_FACERGB,
    COL_EDGERGB,
    COL_MESHSRC
};

enum OptionName
{
    OPT_SHAPE = 0,
    OPT_SIZE,
    OPT_SEED,
    OPT_ITER
};


FDEMMeshWidget::FDEMMeshWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMMeshWidget),
    dir(QString(":/images/resources/"))
{
    ui->setupUi(this);    
    this->blockCollection = BlockCollection::GetBlockCollection();
    this->actorCollection = ActorCollection::GetActorCollection();
    this->selectCollection = SelectCollection::GetSelectCollection();

    this->InitBlockMeshWidget();
    this->ConnectSignalSlots();
}

FDEMMeshWidget::~FDEMMeshWidget()
{
    delete ui;

    delete this->meshOptionTableModel;
    delete this->meshOptionDelegate;
    delete this->meshTableModel;
    delete this->meshDelegate;
}

void FDEMMeshWidget::InitBlockMeshWidget()
{
    this->InitBlockMeshTableView();
    this->InitMeshOptionTableView();
    this->InitMeshSeedTableWidget();
    ui->optionGroup->setVisible(false);
}

void FDEMMeshWidget::InitBlockMeshTableView()
{
    this->meshTableModel = new QStandardItemModel(ui->meshTableView);
    ui->meshTableView->setModel(this->meshTableModel);
    this->meshDelegate = new FDEMMeshItemDelegate(ui->meshTableView);
    ui->meshTableView->setItemDelegate(this->meshDelegate);

    // Horizontal Headers
    QFont font(QString("Arial"), -1, QFont::Bold);
    QStringList headers = QStringList() << "Blocks" << "Face Color"  << "Edge Color" << "Source";
    this->meshTableModel->setHorizontalHeaderLabels(headers);
    for(int col = 0; col < headers.size(); col++)
    {
        QStandardItem *item = this->meshTableModel->horizontalHeaderItem(col);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFont(font);
    }

    ui->meshTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->meshTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->meshTableView->setAlternatingRowColors(true);
    ui->meshTableView->setColumnWidth(COL_BLOCKID, 100);
    ui->meshTableView->setColumnWidth(COL_FACERGB, 100);
    ui->meshTableView->setColumnWidth(COL_EDGERGB, 100);
    ui->meshTableView->setColumnWidth(COL_MESHSRC, 100);
}

void FDEMMeshWidget::InitMeshOptionTableView()
{
    this->meshOptionTableModel = new QStandardItemModel(ui->optionTableView);
    ui->optionTableView->setModel(this->meshOptionTableModel);
    this->meshOptionDelegate = new FDEMMeshOptionItemDelegate(ui->optionTableView);
    ui->optionTableView->setItemDelegate(this->meshOptionDelegate);

    // Horizontal Headers
    QFont font(QString("Arial"), -1, QFont::Bold);
    QStringList headers = QStringList() << "Shape" << "Size" << "Seeds" << "Iterations";
    this->meshOptionTableModel->setHorizontalHeaderLabels(headers);
    for(int col = 0; col < headers.size(); col++)
    {
        QStandardItem *item = this->meshOptionTableModel->horizontalHeaderItem(col);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFont(font);
    }

    ui->optionTableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->optionTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->optionTableView->verticalHeader()->setVisible(false);
    ui->optionTableView->setColumnWidth(OPT_SHAPE, 99);
    ui->optionTableView->setColumnWidth(OPT_SIZE, 99);
    ui->optionTableView->setColumnWidth(OPT_SEED, 99);
    ui->optionTableView->setColumnWidth(OPT_ITER, 99);

    //Shape
    QStandardItem* shape = new QStandardItem;
    shape->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    //Size
    QStandardItem* size = new QStandardItem;
    size->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    //Seed
    QStandardItem* seed = new QStandardItem;
    seed->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    //Iter
    QStandardItem* iter = new QStandardItem;
    iter->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QList<QStandardItem *> items;
    items << shape << size << seed << iter;
    this->meshOptionTableModel->appendRow(items);
}

void FDEMMeshWidget::InitMeshSeedTableWidget()
{
    ui->seedTableWidget->setRowCount(0);
    ui->seedTableWidget->setColumnCount(3);

    QStringList seedHeaders;
    seedHeaders << "X" << "Y" << "Z";
    ui->seedTableWidget->setHorizontalHeaderLabels(seedHeaders);
    ui->seedTableWidget->verticalHeader()->setVisible(true);
}

void FDEMMeshWidget::ConnectSignalSlots()
{
    connect(ui->meshTableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &FDEMMeshWidget::slotSelectionChanged);
    connect(this->meshTableModel, &QStandardItemModel::itemChanged,
            this, &FDEMMeshWidget::slotBlockMeshTableModelItemChanged);
    connect(this->meshOptionTableModel, &QStandardItemModel::itemChanged,
            this, &FDEMMeshWidget::slotMeshOptionTableModelItemChanged);
    connect(ui->seedTableWidget, &QTableWidget::itemChanged,
            this, &FDEMMeshWidget::slotMeshSeedTableWidgetItemChanged);

    connect(ui->applyButton, &QPushButton::clicked,
            this, &FDEMMeshWidget::slotApplyButtonClicked);
    connect(ui->restoreButton, &QPushButton::clicked,
            this, &FDEMMeshWidget::slotRestoreButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked,
            this, &FDEMMeshWidget::slotRemoveButtonClicked);
    connect(ui->queryButton, &QPushButton::clicked,
            this, &FDEMMeshWidget::slotQueryButtonClicked);

    connect(ui->voronoiButton, &QPushButton::toggled,
            this, &FDEMMeshWidget::slotVoronoiDiagramToggled);
    connect(ui->constraintVisibilityButton, &QPushButton::toggled,
            this, &FDEMMeshWidget::slotConstraintVisibilityButtonToggled);
    connect(ui->refineLocalQuadButton, &QPushButton::clicked,
            this, &FDEMMeshWidget::slotRefineLocalQuadButtonClicked);
    connect(ui->insertConstraintButton, &QPushButton::clicked,
            this, &FDEMMeshWidget::slotInsertConstraintButtonClicked);
    connect(ui->insertGlobalQuadButton, &QPushButton::clicked,
            this, &FDEMMeshWidget::slotInsertGlobalQuadButtonClicked);
    connect(ui->insertLocalQuadButton, &QPushButton::clicked,
            this, &FDEMMeshWidget::slotInsertLocalQuadButtonClicked);
    connect(ui->splitTriangulationButton, &QPushButton::clicked,
            this, &FDEMMeshWidget::slotSplitTriangulationButtonClicked);
    connect(ui->shrinkButton, &QPushButton::clicked,
            this, &FDEMMeshWidget::slotShrinkBlockMeshButtonClicked);
    connect(ui->renumberButton, &QPushButton::clicked,
            this, &FDEMMeshWidget::slotRenumberBlockMeshButtonClicked);
    connect(ui->importButton, &QPushButton::clicked,
            this, &FDEMMeshWidget::slotImportBlockMeshButtonClicked);
}

QList<QStandardItem *> FDEMMeshWidget::CreateBlockMeshItems(const QString &blockId, QString srcTag)
{
    Block::Triangulation t = this->blockCollection->GetBlockTriangulation(blockId);

    //Name
    QStandardItem* name = new QStandardItem(blockId);
    name->setTextAlignment(Qt::AlignCenter);
    name->setCheckable(true);
    name->setCheckState(Qt::Checked);
    name->setEditable(false);

    //Face Color
    QColor bgcF = t->getFaceColor();
    QColor fgcF = (bgcF.lightnessF() < 0.4) ? QColor(Qt::white) : QColor(Qt::black);
    QFont font;
    font.setCapitalization(QFont::AllUppercase);

    QStandardItem* faceColor = new QStandardItem;
    faceColor->setData(bgcF, Qt::EditRole);
    faceColor->setBackground(bgcF);
    faceColor->setForeground(fgcF);
    faceColor->setTextAlignment(Qt::AlignCenter);
    faceColor->setFont(font);

    //Edge Color
    QColor bgcE = t->getEdgeColor();
    QColor fgcE = (bgcE.lightnessF() < 0.4) ? QColor(Qt::white) : QColor(Qt::black);

    QStandardItem* edgeColor = new QStandardItem;
    edgeColor->setData(bgcE, Qt::EditRole);
    edgeColor->setBackground(bgcE);
    edgeColor->setForeground(fgcE);
    edgeColor->setTextAlignment(Qt::AlignCenter);
    edgeColor->setFont(font);

    //Source
    QStandardItem* source = new QStandardItem(srcTag);
    source->setTextAlignment(Qt::AlignCenter);
    source->setEditable(false);

    QList<QStandardItem *> items;
    items << name << faceColor << edgeColor << source;
    return items;
}

QVector<QPointF> FDEMMeshWidget::GetMeshSeedPoints()
{
    int rows = ui->seedTableWidget->rowCount();
    int cols = ui->seedTableWidget->columnCount();

    QVector<QPointF> seeds;
    for(int i = 0; i < rows; i++)
    {
        double point[3];
        for(int j = 0; j < cols; j++)
        {
            QTableWidgetItem *item = ui->seedTableWidget->item(i,j);
            if(nullptr == item)
            {
                QMessageBox::warning(this,
                                     QString("Invalid Argument"),
                                     QString("Incomplete input data. Please check out!"));

                QStandardItem *item = this->meshOptionTableModel->item(0, OPT_SEED);
                item->setData(seeds.size(), Qt::EditRole);
                return seeds;
            }

            QString value = item->text();
            if (value.isNull() || value.isEmpty())
            {
                QMessageBox::warning(this,
                                     QString("Invalid Argument"),
                                     QString("Incomplete input data. Please check out!"));

                QStandardItem *item = this->meshOptionTableModel->item(0, OPT_SEED);
                item->setData(seeds.size(), Qt::EditRole);
                return seeds;
            }

            point[j] = value.toDouble();
        }

        seeds.push_back(QPointF(point[0], point[1]));
    }

    return seeds;
}

void FDEMMeshWidget::UpdateMeshOptionTableView(ConstrainedDelaunayTriangulation *mesh)
{
    QStandardItem *shape = this->meshOptionTableModel->item(0, OPT_SHAPE);
    shape->setData(mesh->getShape(), Qt::EditRole);

    QStandardItem *size = this->meshOptionTableModel->item(0, OPT_SIZE);
    size->setData(mesh->getSize(), Qt::EditRole);

    QStandardItem *seed = this->meshOptionTableModel->item(0, OPT_SEED);
    seed->setData(mesh->getSeedPoints().size(), Qt::EditRole);

    QStandardItem *iter = this->meshOptionTableModel->item(0, OPT_ITER);
    iter->setData(mesh->getMaxIterationNumber(), Qt::EditRole);
}

void FDEMMeshWidget::UpdateMeshSeedTableWidget(ConstrainedDelaunayTriangulation *mesh)
{
    QVector<CDTP::Point> seeds = mesh->getSeedPoints();
    ui->seedTableWidget->setRowCount(seeds.size());
    int cols = ui->seedTableWidget->columnCount();
    for(int i = 0; i < seeds.size(); i++)
    {
        double coord[3];
        coord[0] = seeds[i].x();
        coord[1] = seeds[i].y();
        coord[2] = 0.0;

        for(int j = 0; j < cols; j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            item->setData(Qt::EditRole, QString::number(coord[j]));
            ui->seedTableWidget->setItem(i, j, item);
        }
    }
}

void FDEMMeshWidget::slotApplyButtonClicked()
{
    QItemSelection selection = ui->meshTableView->selectionModel()->selection();
    if(!selection.isEmpty())
    {
        QString blockId = selection.indexes().at(COL_BLOCKID).data(Qt::DisplayRole).toString();
        Block::Triangulation cdt = this->blockCollection->GetBlockTriangulation(blockId);
        ConstrainedDelaunayTriangulation *mesh = dynamic_cast<ConstrainedDelaunayTriangulation*>(cdt.get());
        mesh->setSeedPoints(this->GetMeshSeedPoints());
        mesh->ClearRefinedDelaunayMesh();

        Block::PrimitiveMap boundary_map = this->blockCollection->GetBlockBoundaryMap(blockId);
        BlockActor::PrimitiveActorMap boundaryActor_map = this->actorCollection->GetBlockBoundaryMap(blockId);
        for(auto iter = boundaryActor_map.begin(); iter != boundaryActor_map.end(); ++iter)
        {
            vtkPolyData *poly = (*iter)->GetInputDataSet();
            mesh->InsertConstraints(poly, Triangulation::BOUNDARY);
            boundary_map[iter.key()]->setEmbedded(true);
        }

        Block::PrimitiveMap fracture_map = this->blockCollection->GetBlockFractureMap(blockId);
        BlockActor::PrimitiveActorMap fractureActor_map = this->actorCollection->GetBlockFractureMap(blockId);
        for(auto iter = fractureActor_map.begin(); iter != fractureActor_map.end(); ++iter)
        {
            vtkPolyData *poly = (*iter)->GetInputDataSet();
            mesh->InsertConstraints(poly, Triangulation::FRACTURE);
            fracture_map[iter.key()]->setEmbedded(true);
        }

        Block::PrimitiveMap bedplane_map = this->blockCollection->GetBlockBedplaneMap(blockId);
        BlockActor::PrimitiveActorMap bedplaneActor_map = this->actorCollection->GetBlockBedplaneMap(blockId);
        for(auto iter = bedplaneActor_map.begin(); iter != bedplaneActor_map.end(); ++iter)
        {
            vtkPolyData *poly = (*iter)->GetInputDataSet();
            mesh->InsertConstraints(poly, Triangulation::BEDPLANE);
            bedplane_map[iter.key()]->setEmbedded(true);
        }

        mesh->RefineDelaunayMesh(blockId);
        mesh->InitTriangulationNodeIds();
        mesh->InitTriangulationFaceIds();
        mesh->setMeshSourceFlag(Triangulation::GENERATED);
        mesh->setMeshFilterFlag(Triangulation::PRIMITIVE);

        emit this->signalRenderBlockMeshActor(blockId);

        ui->applyButton->setEnabled(false);
        ui->renumberButton->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this,
                             QString("Invalid Operation"),
                             QString("Select the triangulation to be modified. Thanks!"));
    }
}

void FDEMMeshWidget::slotRestoreButtonClicked()
{
    BlockCollection::BlockMap block_map = this->blockCollection->GetBlockMap();
    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        QSharedPointer<Triangulation> mesh = iter.value()->GetTriangulation();
        mesh->setMeshFilterFlag(Triangulation::PRIMITIVE);

        //1.Update triangulation info
        mesh->RestoreTriangulationStatus();
        mesh->setShrinkFactor(1.0);
        mesh->InitTriangulationNodeIds();
        mesh->InitTriangulationFaceIds();

        //2.Update triangulation actor
        emit this->signalRenderBlockMeshActor(iter.key());
        ui->renumberButton->setEnabled(true);
    }

    emit this->signalTriangulationModified();
}

void FDEMMeshWidget::slotRemoveButtonClicked()
{
    ui->meshTableView->selectRow(meshTableModel->rowCount() - 1);
    QItemSelection selection = ui->meshTableView->selectionModel()->selection();
    if(!selection.isEmpty())
    {
        QString blockId = selection.indexes().at(COL_BLOCKID).data(Qt::DisplayRole).toString();
        this->slotRemoveBlockMeshItems(blockId);
        emit this->signalDeleteBlockMeshActor(blockId);
        emit this->signalRemoveBlockGeometryItem();
    }

    if(!meshTableModel->rowCount())
    {
        ui->voronoiButton->setChecked(false);
        ui->constraintVisibilityButton->setChecked(false);
    }
}

void FDEMMeshWidget::slotQueryButtonClicked()
{
    QSharedPointer<BlockCollection> blockCollection = BlockCollection::GetBlockCollection();
    BlockCollection::BlockMap block_map = blockCollection->GetBlockMap();

    if(block_map.isEmpty())
    {
        QMessageBox::warning(this, QString("Solution warning"),
                             QString("There is no meshes at all! Check it out"));
        return;
    }

    double minimumSize = 1.0e+15;
    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        double min_size = iter.value()->GetMeshMinimumSize();
        minimumSize = fmin(minimumSize, min_size);

        QString msg = QString("[>>>] Mesh size of %1:    [%2]").arg(iter.key()).arg(min_size);
        emit this->signalOutputPromptMessage(msg);
    }

    blockCollection->minimumMeshSize = minimumSize;
    emit this->signalOutputPromptMessage(QString("[>>>] Minimum mesh size:    [%1]").arg(minimumSize));

    double minimumArea = 1.0e+15;
    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        double min_area = iter.value()->GetMeshMinimumArea();
        minimumArea = fmin(minimumArea, min_area);

        QString msg = QString("[>>>] Mesh area of %1:    [%2]").arg(iter.key()).arg(min_area);
        emit this->signalOutputPromptMessage(msg);
    }

    blockCollection->minimumMeshArea = minimumArea;
    emit this->signalOutputPromptMessage(QString("[>>>] Minimum mesh area:    [%1]").arg(minimumArea));
}

void FDEMMeshWidget::slotImportBlockMeshButtonClicked()
{
    QString directory = GlobalCollection::GetGlobalCollection()->getWorkspace();
    QString filter = QString("Gmsh File (*.msh *.msh4);;Distmesh File (*.ply);;"
                             "Abaqus File (*.inp);;");
    QFileDialog dlg(this);
    dlg.setDirectory(directory);
    dlg.setNameFilter(filter);
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setFileMode(QFileDialog::ExistingFiles);

    if(QDialog::Accepted == dlg.exec())
    {
        QString filter = dlg.selectedNameFilter();
        QStringList fnames = dlg.selectedFiles();

        for(QString fname : fnames)
        {
            //1.Create constrained triangulation
            QSharedPointer<FDEMMeshImporterFactory> factory =
                    QSharedPointer<FDEMMeshImporterFactory>(new FDEMMeshImporterFactory);
            QSharedPointer<FDEMMeshImporter> importer = factory->Create(fname);
            importer->SetFileName(fname);
            importer->Import();

            //2.Render triangulation actor
            QMap<QString, QSharedPointer<Block>> block_map = this->blockCollection->GetBlockMap();
            QMap<QString, QSharedPointer<BlockActor>> actor_map = this->actorCollection->GetBlockActorMap();
            for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
            {
                if(actor_map.contains(iter.key()))
                    continue;

                this->meshTableModel->appendRow(CreateBlockMeshItems(iter.key(), QString("Imported")));
                emit this->signalAppendBlockGeometryItem();
                emit this->signalRenderBlockMeshActor(iter.key());
            }
        }
        ui->renumberButton->setEnabled(true);
        GlobalCollection::GetGlobalCollection()->setWorkspace(dlg.directory().path());
    }

    emit this->signalTriangulationModified();
}

void FDEMMeshWidget::slotVoronoiDiagramToggled(bool checked)
{
    if(!this->meshTableModel->rowCount())
        return;

    QItemSelection selection = ui->meshTableView->selectionModel()->selection();
    if(!selection.isEmpty())
    {
        QModelIndex index = selection.indexes().at(COL_BLOCKID);
        QString blockId = index.data(Qt::DisplayRole).toString();

        QSharedPointer<VoronoiDiagramActor> voronoiActor = this->actorCollection->GetBlockVoronoiDiagramActor(blockId);
        if(nullptr != voronoiActor->GetVTKActorMapper()->GetInput())
        {
            emit this->signalSetVoronoiActorVisibility(blockId, checked);
        }
        else
        {
            QSharedPointer<Triangulation> triangulation = this->blockCollection->GetBlockTriangulation(blockId);
            QSharedPointer<VoronoiDiagram> voronoi = this->blockCollection->GetBlockVoronoiDiagram(blockId);
            voronoi->Construct(triangulation.data());

            double bounds[6];
            QSharedPointer<TriangulationActor> triangulationActor = this->actorCollection->GetBlockTriangulationActor(blockId);
            triangulationActor->GetVTKActorMapper()->GetBounds(bounds);
            voronoi->CropBoundary(bounds);

            emit this->signalRenderBlockVoronoiActor(blockId);
        }
    }
    else
    {
        QMessageBox::warning(this,
                             QString("Invalid Operation"),
                             QString("Select the triangulation to be modified. Thanks!"));
        return;
    }
}

void FDEMMeshWidget::slotInsertConstraintButtonClicked()
{
    QItemSelection selection = ui->meshTableView->selectionModel()->selection();
    if(!selection.isEmpty())
    {
        QModelIndex index = selection.indexes().at(COL_BLOCKID);
        QString blockId = index.data(Qt::DisplayRole).toString();

        //1.Update triangulation info
        QSharedPointer<InsertConstraintFilter> mesh_filter =
                QSharedPointer<InsertConstraintFilter>(new InsertConstraintFilter);
        QSharedPointer<Triangulation> mesh = this->blockCollection->GetBlockTriangulation(blockId);
        Block::PrimitiveMap fractures = this->blockCollection->GetBlockFractureMap(blockId);

        mesh->setMeshFilterFlag(Triangulation::PRIMITIVE);
        mesh_filter->SetConstraintInfo(fractures);
        mesh_filter->SetTriangulation(mesh);
        mesh_filter->filter();

        emit this->signalTriangulationModified();
        ui->renumberButton->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this,
                             QString("Invalid Operation"),
                             QString("Select the triangulation to be modified. Thanks!"));
        return;
    }
}

void FDEMMeshWidget::slotInsertGlobalQuadButtonClicked()
{
    QItemSelection selection = ui->meshTableView->selectionModel()->selection();
    if(!selection.isEmpty())
    {
        QModelIndex index = selection.indexes().at(COL_BLOCKID);
        QString blockId = index.data(Qt::DisplayRole).toString();

        //1.Update triangulation info
        QSharedPointer<InsertGlobalQuadFilter> mesh_filter =
                QSharedPointer<InsertGlobalQuadFilter>(new InsertGlobalQuadFilter);
        QSharedPointer<Triangulation> mesh = this->blockCollection->GetBlockTriangulation(blockId);
        mesh->setMeshFilterFlag(Triangulation::GQHANDLED);
        mesh_filter->SetTriangulation(mesh);
        mesh_filter->filter();

        //2.Update triangulation actor
        emit this->signalRenderBlockMeshActor(blockId);
        emit this->signalTriangulationModified();
        ui->renumberButton->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this,
                             QString("Invalid Operation"),
                             QString("Select the triangulation to be modified. Thanks!"));
        return;
    }
}

void FDEMMeshWidget::slotInsertLocalQuadButtonClicked()
{
    QItemSelection selection = ui->meshTableView->selectionModel()->selection();
    if(!selection.isEmpty())
    {
        QModelIndex index = selection.indexes().at(COL_BLOCKID);
        QString blockId = index.data(Qt::DisplayRole).toString();
        QSharedPointer<Triangulation> mesh = this->blockCollection->GetBlockTriangulation(blockId);
        try
        {
            //1.Update triangulation info
            QSharedPointer<InsertLocalQuadFilter> mesh_filter =
                    QSharedPointer<InsertLocalQuadFilter>(new InsertLocalQuadFilter);
            mesh->setMeshFilterFlag(Triangulation::LQHANDLED);
            mesh_filter->SetTriangulation(mesh);
            mesh_filter->filter();
        }
        catch (std::exception& e)
        {
            QMessageBox::warning(this,
                                 QString("Invalid Geometry Condition"),
                                 QString(e.what()));
            return;
        }

        //2.Update triangulation actor
        emit this->signalRenderBlockMeshActor(blockId);
        emit this->signalTriangulationModified();
        ui->renumberButton->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this,
                             QString("Invalid Operation"),
                             QString("Select the item to be modified. Thanks!"));
        return;
    }
}

void FDEMMeshWidget::slotSplitTriangulationButtonClicked()
{
    QItemSelection selection = ui->meshTableView->selectionModel()->selection();
    if(!selection.isEmpty())
    {
        QModelIndex index = selection.indexes().at(COL_BLOCKID);
        QString blockId = index.data(Qt::DisplayRole).toString();
        QSharedPointer<Triangulation> mesh = this->blockCollection->GetBlockTriangulation(blockId);

        //1.Update triangulation info
        QSharedPointer<SplitTriangulationFilter> mesh_filter =
                QSharedPointer<SplitTriangulationFilter>(new SplitTriangulationFilter);
        mesh->setMeshFilterFlag(Triangulation::STHANDLED);
        mesh_filter->SetTriangulation(mesh);
        mesh_filter->filter();

        //2.Update triangulation actor
        emit this->signalRenderBlockMeshActor(blockId);
        emit this->signalTriangulationModified();
        ui->renumberButton->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this,
                             QString("Invalid Operation"),
                             QString("Select the item to be modified. Thanks!"));
        return;
    }
}

void FDEMMeshWidget::slotShrinkBlockMeshButtonClicked()
{
    QItemSelection selection = ui->meshTableView->selectionModel()->selection();
    if(!selection.isEmpty())
    {
        QModelIndex index = selection.indexes().at(COL_BLOCKID);
        QString blockId = index.data(Qt::DisplayRole).toString();
        QSharedPointer<Triangulation> mesh = this->blockCollection->GetBlockTriangulation(blockId);

        int percentage = static_cast<int>(mesh->getShrinkFactor() * 100.0);
        ShrinkDialog dlg(this, percentage);
        if(QDialog::Accepted == dlg.exec())
        {
            mesh->setShrinkFactor(dlg.GetShrinkFactor() / 100.0);
            emit this->signalShrinkBlockMeshActor(blockId);
        }
    }
    else
    {
        QMessageBox::warning(this,
                             QString("Invalid Operation"),
                             QString("Select the item to be modified. Thanks!"));
        return;
    }
}

void FDEMMeshWidget::slotRenumberBlockMeshButtonClicked()
{
    emit this->signalRenumberMeshButtonClicked();
    ui->renumberButton->setEnabled(false);
}

void FDEMMeshWidget::slotRefineLocalQuadButtonClicked()
{
    emit this->signalRefineLocalQuadButtonClicked();
    ui->renumberButton->setEnabled(true);
}

void FDEMMeshWidget::slotConstraintVisibilityButtonToggled(bool checked)
{
    Triangulation::setConstraintVisible(checked);
}

void FDEMMeshWidget::slotBlockMeshTableModelItemChanged(QStandardItem *item)
{
    ui->meshTableView->selectRow(item->row());
    QItemSelection selection = ui->meshTableView->selectionModel()->selection();
    QModelIndex index = selection.indexes().at(COL_BLOCKID);
    QString blockId = index.data(Qt::DisplayRole).toString();

    Block::Triangulation t = this->blockCollection->GetBlockTriangulation(blockId);
    switch(item->column())
    {
    case COL_BLOCKID:
    {
        bool checked = item->checkState();
        emit this->signalSetMeshActorVisibility(blockId, checked);
        break;
    }
    case COL_FACERGB:
    {
        QColor color = item->data(Qt::EditRole).value<QColor>();
        emit this->signalSetMeshFaceColor(blockId, color);
        t->setFaceColor(color);
        break;
    }
    case COL_EDGERGB:
    {
        QColor color = item->data(Qt::EditRole).value<QColor>();
        emit this->signalSetMeshEdgeColor(blockId, color);
        t->setEdgeColor(color);
        break;
    }
    }
}

void FDEMMeshWidget::slotMeshOptionTableModelItemChanged(QStandardItem *item)
{
    QItemSelection selection = ui->meshTableView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QModelIndex index = selection.indexes().at(COL_BLOCKID);
    QString blockId = index.data(Qt::DisplayRole).toString();

    Block::Triangulation cdt = this->blockCollection->GetBlockTriangulation(blockId);
    if(ConstrainedDelaunayTriangulation *mesh = dynamic_cast<ConstrainedDelaunayTriangulation*>(cdt.get()))
    {
        switch(item->column())
        {
        case OPT_SHAPE:
        {
            double shape = item->data(Qt::EditRole).toDouble();
            mesh->setShape(shape);
            ui->applyButton->setEnabled(true);
            break;
        }
        case OPT_SIZE:
        {
            double size = item->data(Qt::EditRole).toDouble();
            mesh->setSize(size);
            ui->applyButton->setEnabled(true);
            break;
        }
        case OPT_SEED:
        {
            int rows = item->data(Qt::EditRole).toInt();
            ui->seedTableWidget->setRowCount(rows);
            ui->seedTableWidget->setEnabled(rows);
            ui->applyButton->setEnabled(true);
            break;
        }
        case OPT_ITER:
        {
            int iters = item->data(Qt::EditRole).toInt();
            mesh->setMaxIterationNumber(iters);
            ui->applyButton->setEnabled(true);
            break;
        }
        }
    }
}

void FDEMMeshWidget::slotMeshSeedTableWidgetItemChanged(QTableWidgetItem *item)
{
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->applyButton->setEnabled(true);
}

void FDEMMeshWidget::slotSelectionChanged(const QItemSelection &selection, const QItemSelection &)
{
    if(selection.isEmpty()) return;

    QString blockId = selection.indexes().at(COL_BLOCKID).data(Qt::DisplayRole).toString();
    QString meshSrc = selection.indexes().at(COL_MESHSRC).data(Qt::DisplayRole).toString();
    Block::Triangulation cdt = this->blockCollection->GetBlockTriangulation(blockId);

    if(QString("Generated") == meshSrc)
    {
        ConstrainedDelaunayTriangulation *mesh = static_cast<ConstrainedDelaunayTriangulation*>(cdt.get());
        this->UpdateMeshOptionTableView(mesh);
        this->UpdateMeshSeedTableWidget(mesh);
        ui->optionGroup->setVisible(true);
    }
    else if(QString("Imported") == meshSrc)
    {
        ui->optionGroup->setVisible(false);
    }

    /* Update Voronoi Button */
    QSharedPointer<VoronoiDiagramActor> voronoiActor = this->actorCollection->GetBlockVoronoiDiagramActor(blockId);
    ui->voronoiButton->setChecked(voronoiActor->GetVTKRenderActor()->GetVisibility());
}

void FDEMMeshWidget::slotAppendBlockMeshItems(const QString &blockId)
{
    this->meshTableModel->appendRow(this->CreateBlockMeshItems(blockId, QString("Generated")));
}

void FDEMMeshWidget::slotRemoveBlockMeshItems(const QString &blockId)
{
    QList<QStandardItem*> items = this->meshTableModel->findItems(blockId);
    this->meshTableModel->removeRow(items.first()->row());

    ui->optionGroup->setVisible(this->meshTableModel->rowCount());
    ui->applyButton->setEnabled(true);
    emit this->signalTriangulationModified();
}
