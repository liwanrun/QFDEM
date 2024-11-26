#include "fdemgeometrywidget.h"
#include "ui_fdemgeometrywidget.h"
#include "block.h"
#include "blockcollection.h"
#include "actorcollection.h"
#include "fdemgeometryitemdelegate.h"
#include "fracturenetworkdialog.h"
#include "fracturenetwork.h"
#include "boundaryclipper.h"
#include "globalcollection.h"
#include "asciifileparser.h"

#include <QDebug>
#include <QIcon>
#include <QLineEdit>
#include <QValidator>
#include <QDateTime>
#include <QMessageBox>
#include <QFileDialog>

#include <exception>

enum ColumName
{
    COL_NAME = 0,
    COL_SHAPE,
    COL_SIZE,
    COL_COLOR
};

FDEMGeometryWidget::FDEMGeometryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMGeometry),
    path(QString(":/images/resources/"))
{
    ui->setupUi(this);

    this->blockCollection = BlockCollection::GetBlockCollection();
    this->actorCollection = ActorCollection::GetActorCollection();

    this->InitGeometryWidget();
    this->ConnectSignalSlots();
}

FDEMGeometryWidget::~FDEMGeometryWidget()
{
    delete ui;
    delete this->treeModel;
    delete this->delegate;
}

void FDEMGeometryWidget::InitGeometryWidget()
{
    this->treeModel = new QStandardItemModel(this);
    ui->treeView->setModel(this->treeModel);
    this->delegate = new FDEMGeometryItemDelegate(this);
    ui->treeView->setItemDelegate(this->delegate);

    this->InitGeometryTreeView();
    ui->applyButton->setEnabled(true);
}

void FDEMGeometryWidget::ConnectSignalSlots()
{
    connect(this->treeModel, &QStandardItemModel::itemChanged,
            this, &FDEMGeometryWidget::slotTreeModelItemChanged);
    connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &FDEMGeometryWidget::slotSelectionChanged);
    connect(ui->tableWidget, &QTableWidget::itemChanged,
            this, &FDEMGeometryWidget::slotTableWidgetItemChanged);

    connect(ui->appendButton, &QPushButton::clicked,
            this, &FDEMGeometryWidget::slotAppendButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked,
            this, &FDEMGeometryWidget::slotRemoveButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked,
            this, &FDEMGeometryWidget::slotClearButtonClicked);
    connect(ui->applyButton, &QPushButton::clicked,
            this, &FDEMGeometryWidget::slotApplyButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked,
            this, &FDEMGeometryWidget::slotResetButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &FDEMGeometryWidget::slotCancelButtonClicked);
    connect(ui->clippingButton, &QPushButton::clicked,
            this, &FDEMGeometryWidget::slotClippingButtonClicked);
    connect(ui->insertDFNButton, &QPushButton::clicked,
            this, &FDEMGeometryWidget::slotInsertDFNButtonClicked);
    connect(ui->importButton, &QPushButton::clicked,
            this, &FDEMGeometryWidget::slotImportButtonClicked);
}

void FDEMGeometryWidget::InitGeometryTreeView()
{
    // Horizontal Headers
    QFont font(QString("Arial"), -1, QFont::Bold);
    QStringList headers = QStringList() << "Primitives" <<  "Shape" << "Size" << "Color";
    this->treeModel->setHorizontalHeaderLabels(headers);
    for(int col = 0; col < headers.size(); col++)
    {
        QStandardItem *item = this->treeModel->horizontalHeaderItem(col);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFont(font);
    }

    // Root Item
    QStandardItem *rootItem = new QStandardItem(QIcon(path + QString("geometry_import.png")),QString("Blocks (0)"));
    rootItem->setEditable(false);
    rootItem->setCheckable(true);
    rootItem->setUserTristate(true);
    rootItem->setCheckState(Qt::Checked);
    this->treeModel->appendRow(rootItem);

    // View Style
    ui->treeView->setFirstColumnSpanned(rootItem->row(),this->treeModel->invisibleRootItem()->index(),true);
    ui->treeView->setExpanded(rootItem->index(),true);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeView->setAlternatingRowColors(true);
    ui->treeView->setIndentation(10);
    ui->treeView->setColumnWidth(COL_NAME,  180);
    ui->treeView->setColumnWidth(COL_SHAPE, 90);
    ui->treeView->setColumnWidth(COL_SIZE,  60);
    ui->treeView->setColumnWidth(COL_COLOR, 60);
}

void FDEMGeometryWidget::UpdateTableWidget(const QItemSelection &selection)
{
    QSharedPointer<Primitive> primitive = this->PrimitiveFromSelection(selection);
    if(primitive)
    {
        int size = primitive->getSize();
        int cols = primitive->getCols();
        ui->tableWidget->setRowCount(size);
        ui->tableWidget->setColumnCount(cols);

        QString shape = primitive->getShape();
        if(QString("Polygon") == shape)
        {
            QStringList headers;
            headers << "X" << "Y" << "Z";
            ui->tableWidget->setHorizontalHeaderLabels(headers);
        }
        else if(QString("Ellipse") == shape)
        {
            QStringList headers;
            headers << "X" << "Y" << "Z";
            ui->tableWidget->setHorizontalHeaderLabels(headers);
        }
        else if(QString("Segment") == shape)
        {
            QStringList headers;
            headers << "X0" << "Y0" << "X1" << "Y1";
            ui->tableWidget->setHorizontalHeaderLabels(headers);
        }
        else if(QString("Point") == shape)
        {
            QStringList headers;
            headers << "X" << "Y" << "Z";
            ui->tableWidget->setHorizontalHeaderLabels(headers);
        }

        vtkDoubleArray *array = primitive->getTableData();
        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j < cols; j++)
            {
                QTableWidgetItem *item = new QTableWidgetItem;
                item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                item->setText(QString::number(array->GetTuple(i)[j]));
                ui->tableWidget->setItem(i, j, item);
            }
        }
    }
    else
    {
        ui->tableWidget->setRowCount(0);
        ui->tableWidget->setColumnCount(0);
        return;
    }
}

QList<QStandardItem *> FDEMGeometryWidget::CreateBlockBoundaryItem(const QString& blockId, const QString& id)
{
    QSharedPointer<Primitive> primitive = this->blockCollection->GetBlockBoundary(blockId, id);
    this->actorCollection->GetBlockBoundaryActor(blockId, id);

    //Name
    QStandardItem* name = new QStandardItem(id);
    name->setData(blockId,Qt::UserRole + 1);
    name->setData(id,Qt::UserRole + 2);
    name->setEditable(false);
    name->setCheckable(true);
    name->setCheckState(Qt::Checked);

    //Shape
    QStandardItem* shape = new QStandardItem;
    shape->setData(primitive->getShape(),Qt::EditRole);
    shape->setIcon(QIcon(path + QString("polygon_Geo.png")));
    shape->setData(blockId,Qt::UserRole + 1);
    shape->setData(id,Qt::UserRole + 2);

    //Size
    QStandardItem* size = new QStandardItem;
    size->setData(primitive->getSize(),Qt::EditRole);
    size->setData(blockId,Qt::UserRole + 1);
    size->setData(id,Qt::UserRole + 2);
    size->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    //Color
    QColor bgc = primitive->getColor();
    QColor fgc = (bgc.lightnessF() < 0.4) ? QColor(Qt::white) : QColor(Qt::black);
    QFont font;
    font.setCapitalization(QFont::AllUppercase);

    QStandardItem* color = new QStandardItem;
    color->setData(primitive->getColor(),Qt::EditRole);
    color->setBackground(primitive->getColor());
    color->setForeground(fgc);
    color->setTextAlignment(Qt::AlignCenter);
    color->setFont(font);
    color->setData(blockId,Qt::UserRole + 1);
    color->setData(id,Qt::UserRole + 2);

    QList<QStandardItem *> items;
    items << name << shape << size << color ;
    return items;
}

QList<QStandardItem *> FDEMGeometryWidget::CreateBlockFractureItem(const QString &blockId, const QString &id)
{
    QSharedPointer<Primitive> primitive = this->blockCollection->GetBlockFracture(blockId, id);
    this->actorCollection->GetBlockFractureActor(blockId, id);

    //Name
    QStandardItem* name = new QStandardItem(id);
    name->setData(blockId,Qt::UserRole + 1);
    name->setData(id,Qt::UserRole + 2);
    name->setEditable(false);
    name->setCheckable(true);
    name->setCheckState(Qt::Checked);

    //Shape
    QStandardItem* shape = new QStandardItem;
    shape->setData(primitive->getShape(),Qt::EditRole);
    shape->setIcon(QIcon(path + QString("segment_Geo.png")));
    shape->setData(blockId,Qt::UserRole + 1);
    shape->setData(id,Qt::UserRole + 2);

    //Size
    QStandardItem* size = new QStandardItem;
    size->setData(primitive->getSize(),Qt::EditRole);
    size->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    size->setData(blockId,Qt::UserRole + 1);
    size->setData(id,Qt::UserRole + 2);

    //Color
    QColor bgc = primitive->getColor();
    QColor fgc = (bgc.lightnessF() < 0.4) ? QColor(Qt::white) : QColor(Qt::black);
    QFont font;
    font.setCapitalization(QFont::AllUppercase);

    QStandardItem* color = new QStandardItem;
    color->setData(primitive->getColor(),Qt::EditRole);
    color->setBackground(primitive->getColor());
    color->setForeground(fgc);
    color->setTextAlignment(Qt::AlignCenter);
    color->setFont(font);
    color->setData(blockId,Qt::UserRole + 1);
    color->setData(id,Qt::UserRole + 2);

    QList<QStandardItem *> items;
    items << name << shape << size << color;
    return items;
}

QList<QStandardItem *> FDEMGeometryWidget::CreateBlockBedplaneItem(const QString &blockId, const QString &id)
{
    QSharedPointer<Primitive> bedplane = this->blockCollection->GetBlockBedplane(blockId, id);
    this->actorCollection->GetBlockBedplaneActor(blockId, id);

    //Name
    QStandardItem* name = new QStandardItem(id);
    name->setData(blockId,Qt::UserRole + 1);
    name->setData(id,Qt::UserRole + 2);
    name->setEditable(false);
    name->setCheckable(true);
    name->setCheckState(Qt::Checked);

    //Shape
    QStandardItem* shape = new QStandardItem;
    shape->setData(bedplane->getShape(),Qt::EditRole);
    shape->setData(QIcon(path + QString("segment_Geo.png")),Qt::DecorationRole);
    shape->setData(blockId,Qt::UserRole + 1);
    shape->setData(id,Qt::UserRole + 2);

    //Size
    QStandardItem* size = new QStandardItem;
    size->setData(bedplane->getSize(),Qt::EditRole);
    size->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    size->setData(blockId,Qt::UserRole + 1);
    size->setData(id,Qt::UserRole + 2);

    //Color
    QColor bgc = bedplane->getColor();
    QColor fgc = (bgc.lightnessF() < 0.4) ? QColor(Qt::white) : QColor(Qt::black);
    QFont font;
    font.setCapitalization(QFont::AllUppercase);

    QStandardItem* color = new QStandardItem;
    color->setData(bedplane->getColor(),Qt::EditRole);
    color->setBackground(bedplane->getColor());
    color->setForeground(fgc);
    color->setTextAlignment(Qt::AlignCenter);
    color->setFont(font);
    color->setData(blockId,Qt::UserRole + 1);
    color->setData(id,Qt::UserRole + 2);

    QList<QStandardItem *> items;
    items << name << shape << size << color;
    return items;
}

QList<QStandardItem *> FDEMGeometryWidget::CreateBlockClipseedItem(const QString &blockId, const QString &id)
{
    QSharedPointer<Primitive> clipseed = this->blockCollection->GetBlockClipseed(blockId, id);
    this->actorCollection->GetBlockClipseedActor(blockId, id);

    //Name
    QStandardItem* name = new QStandardItem(id);
    name->setData(blockId,Qt::UserRole + 1);
    name->setData(id,Qt::UserRole + 2);
    name->setEditable(false);
    name->setCheckable(true);
    name->setCheckState(Qt::Checked);

    //Shape
    QStandardItem* shape = new QStandardItem;
    shape->setData(clipseed->getShape(),Qt::EditRole);
    shape->setData(QIcon(path + QString("point_Geo.png")),Qt::DecorationRole);
    shape->setData(blockId,Qt::UserRole + 1);
    shape->setData(id,Qt::UserRole + 2);

    //Size
    QStandardItem* size = new QStandardItem;
    size->setData(clipseed->getSize(),Qt::EditRole);
    size->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    size->setData(blockId,Qt::UserRole + 1);
    size->setData(id,Qt::UserRole + 2);

    //Color
    QColor bgc = clipseed->getColor();
    QColor fgc = (bgc.lightnessF() < 0.4) ? QColor(Qt::white) : QColor(Qt::black);
    QFont font;
    font.setCapitalization(QFont::AllUppercase);

    QStandardItem* color = new QStandardItem;
    color->setData(clipseed->getColor(),Qt::EditRole);
    color->setBackground(clipseed->getColor());
    color->setForeground(fgc);
    color->setTextAlignment(Qt::AlignCenter);
    color->setFont(font);
    color->setData(blockId,Qt::UserRole + 1);
    color->setData(id,Qt::UserRole + 2);

    QList<QStandardItem *> items;
    items << name << shape << size << color;
    return items;
}

QStandardItem* FDEMGeometryWidget::CreateBlockGeometryItem(const QString &blockId)
{
    QSharedPointer<Block> block = this->blockCollection->GetBlock(blockId);

    QStandardItem* blockItem = new QStandardItem(blockId);
    blockItem->setEditable(false);
    blockItem->setCheckable(true);
    blockItem->setUserTristate(true);
    blockItem->setCheckState(Qt::Checked);

    QString label;
    label = QString("Boundaries (%1)").arg(block->GetBoundaryMap().size());
    QStandardItem* boundaryGroup = new QStandardItem(QIcon(path + QString("boundary_Geo.png")), label);
    boundaryGroup->setEditable(false);
    boundaryGroup->setCheckable(true);
    boundaryGroup->setUserTristate(true);
    boundaryGroup->setCheckState(Qt::Checked);
    blockItem->appendRow(boundaryGroup);

    label = QString("Fractures (%1)").arg(block->GetFractureMap().size());
    QStandardItem* fractureGroup = new QStandardItem(QIcon(path + QString("fracture_Geo.png")), label);
    fractureGroup->setEditable(false);
    fractureGroup->setCheckable(true);
    fractureGroup->setUserTristate(true);
    fractureGroup->setCheckState(Qt::Checked);
    blockItem->appendRow(fractureGroup);

    label = QString("Bedplanes (%1)").arg(block->GetBedplaneMap().size());
    QStandardItem* bedplaneGroup = new QStandardItem(QIcon(path + QString("bedPlane_Geo.png")), label);
    bedplaneGroup->setEditable(false);
    bedplaneGroup->setCheckable(true);
    bedplaneGroup->setUserTristate(true);
    bedplaneGroup->setCheckState(Qt::Checked);
    blockItem->appendRow(bedplaneGroup);

    label = QString("Clipseeds (%1)").arg(block->GetClipseedMap().size());
    QStandardItem* clipseedGroup = new QStandardItem(QIcon(path + QString("clipSeed_Geo.png")), label);
    clipseedGroup->setEditable(false);
    clipseedGroup->setCheckable(true);
    clipseedGroup->setUserTristate(true);
    clipseedGroup->setCheckState(Qt::Checked);
    blockItem->appendRow(clipseedGroup);

    return blockItem;
}

QSharedPointer<Primitive> FDEMGeometryWidget::PrimitiveFromSelection(const QItemSelection &selection)
{
    QString blockId = selection.indexes()[COL_NAME].data(Qt::UserRole + 1).toString();
    QString id = selection.indexes()[COL_NAME].data(Qt::UserRole + 2).toString();
    QString entry = id.split(QChar('-')).first();

    QSharedPointer<Primitive> primitive;
    if(QString("boundary") == entry)
    {
        primitive = this->blockCollection->GetBlockBoundary(blockId, id);
        ui->clippingButton->setEnabled(false);
    }
    else if(QString("fracture") == entry)
    {
        primitive = this->blockCollection->GetBlockFracture(blockId, id);
        ui->clippingButton->setEnabled(true);
    }
    else if(QString("bedplane") == entry)
    {
        primitive = this->blockCollection->GetBlockBedplane(blockId, id);
        ui->clippingButton->setEnabled(true);
    }
    else if(QString("clipseed") == entry)
    {
        primitive = this->blockCollection->GetBlockClipseed(blockId, id);
        ui->clippingButton->setEnabled(false);
    }
    else
    {
        primitive = nullptr;
        ui->clippingButton->setEnabled(false);
    }

    return primitive;
}

vtkDoubleArray *FDEMGeometryWidget::ReadFromTableWidget()
{
    uint rows = ui->tableWidget->rowCount();
    uint cols = ui->tableWidget->columnCount();

    vtkDoubleArray* matrix = vtkDoubleArray::New();
    matrix->SetNumberOfComponents(cols);
    matrix->SetNumberOfTuples(rows);

    double *tuple = new double[cols];
    for(uint i = 0; i < rows; i++)
    {
        for(uint j = 0; j < cols; j++)
        {
            QTableWidgetItem *item = ui->tableWidget->item(i,j);
            if(nullptr == item)
            {
                delete [] tuple;
                throw std::invalid_argument("Incomplete input data. Please check out!");
            }

            QString value = item->text();
            if (value.isNull() || value.isEmpty())
            {
                delete [] tuple;
                throw std::invalid_argument("Incomplete input data. Please check out!");
            }

            tuple[j] = value.toDouble();
        }

        matrix->SetTuple(i,tuple);
    }
    delete [] tuple;

    return matrix;
}

void FDEMGeometryWidget::UpdateGeometryInformation(const QItemSelection &selection)
{
    QSharedPointer<Primitive> primitive = this->PrimitiveFromSelection(selection);
    if(primitive)
    {
        try
        {
            primitive->setTableData(this->ReadFromTableWidget());
        }
        catch (std::exception& e)
        {
            QMessageBox::warning(this,QString("Invalid Argument"),QString(e.what()));
            return;
        }
    }
}

void FDEMGeometryWidget::AppendGeometryPrimitive(const QModelIndex &parent)
{
    int index = this->treeModel->rowCount(parent) + 1;
    QString group = parent.data().toString().split(QChar(' ')).first();

    if(QString("Blocks") == group)
    {
        QString blockId = QString("block-") + QString().number(index);
        this->blockCollection->AppendBlock(blockId);
        this->actorCollection->AppendBlockActor(blockId);

        QStandardItem* item = this->CreateBlockGeometryItem(blockId);
        this->treeModel->itemFromIndex(parent)->appendRow(item);
        int numBlocks = this->treeModel->rowCount(parent);
        this->treeModel->itemFromIndex(parent)->setText(group + QString(" (%1)").arg(numBlocks));
        ui->treeView->setExpanded(parent, true);

        //Create Block Meshes
        emit this->signalAppendBlockMeshItems(blockId);
    }
    else if(QString("Boundaries") == group)
    {
        QString blockId = parent.parent().data().toString();
        QString id = QString("boundary-") + QString().number(index);
        QList<QStandardItem *> items = this->CreateBlockBoundaryItem(blockId, id);
        this->treeModel->itemFromIndex(parent)->appendRow(items);

        int numRows = this->treeModel->rowCount(parent);
        this->treeModel->itemFromIndex(parent)->setText(group + QString(" (%1)").arg(numRows));
        ui->treeView->setExpanded(parent, true);
    }
    else if(QString("Fractures") == group)
    {
        QString blockId = parent.parent().data().toString();
        QString id = QString("fracture-") + QString().number(index);
        QList<QStandardItem *> items = this->CreateBlockFractureItem(blockId, id);
        this->treeModel->itemFromIndex(parent)->appendRow(items);

        int numRows = this->treeModel->rowCount(parent);
        this->treeModel->itemFromIndex(parent)->setText(group + QString(" (%1)").arg(numRows));
        ui->treeView->setExpanded(parent, true);
    }
    else if(QString("Bedplanes") == group)
    {
        QString blockId = parent.parent().data().toString();
        QString id = QString("bedplane-") + QString().number(index);
        QList<QStandardItem *> items = this->CreateBlockBedplaneItem(blockId, id);
        this->treeModel->itemFromIndex(parent)->appendRow(items);

        int numRows = this->treeModel->rowCount(parent);
        this->treeModel->itemFromIndex(parent)->setText(group + QString(" (%1)").arg(numRows));
        ui->treeView->setExpanded(parent, true);
    }
    else if(QString("Clipseeds") == group)
    {
        QString blockId = parent.parent().data().toString();
        QString id = QString("clipseed-") + QString().number(index);
        QList<QStandardItem *> items = this->CreateBlockClipseedItem(blockId, id);
        this->treeModel->itemFromIndex(parent)->appendRow(items);

        int numRows = this->treeModel->rowCount(parent);
        this->treeModel->itemFromIndex(parent)->setText(group + QString(" (%1)").arg(numRows));
        ui->treeView->setExpanded(parent, true);
    }
}

void FDEMGeometryWidget::RemoveGeometryPrimitive(const QModelIndex &index)
{
    QString group = index.parent().data().toString().split(QChar(' ')).first();
    QString entry = index.data().toString().split(QChar('-')).first();

    if(QString("block") == entry)
    {
        //Delete block meshes
        QString blockId = index.data(Qt::DisplayRole).toString();
        emit this->signalRemoveBlockMeshItems(blockId);
        this->treeModel->removeRow(index.row(), index.parent());
        int numBlocks = this->treeModel->rowCount(index.parent());
        this->treeModel->itemFromIndex(index.parent())->setText(group + QString(" (%1)").arg(numBlocks));

        //Delete block collections
        this->actorCollection->RemoveBlockActor(blockId);
        this->blockCollection->RemoveBlock(blockId);
    }
    else if(QString("boundary") == entry)
    {
        QString blockId = index.data(Qt::UserRole + 1).toString();
        QString id = index.data(Qt::UserRole + 2).toString();
        this->actorCollection->RemoveBlockBoundaryActor(blockId, id);
        this->blockCollection->RemoveBlockBoundary(blockId, id);

        this->treeModel->removeRow(index.row(),index.parent());
        int numRows = this->treeModel->rowCount(index.parent());
        this->treeModel->itemFromIndex(index.parent())->setText(group + QString(" (%1)").arg(numRows));
    }
    else if(QString("fracture") == entry)
    {
        QString blockId = index.data(Qt::UserRole + 1).toString();
        QString id = index.data(Qt::UserRole + 2).toString();
        this->actorCollection->RemoveBlockFractureActor(blockId, id);
        this->blockCollection->RemoveBlockFracture(blockId, id);

        this->treeModel->removeRow(index.row(),index.parent());
        int numRows = this->treeModel->rowCount(index.parent());
        this->treeModel->itemFromIndex(index.parent())->setText(group + QString(" (%1)").arg(numRows));
    }
    else if(QString("bedplane") == entry)
    {
        QString blockId = index.data(Qt::UserRole + 1).toString();
        QString id = index.data(Qt::UserRole + 2).toString();
        this->actorCollection->RemoveBlockBedplaneActor(blockId, id);
        this->blockCollection->RemoveBlockBedplane(blockId, id);

        this->treeModel->removeRow(index.row(),index.parent());
        int numRows = this->treeModel->rowCount(index.parent());
        this->treeModel->itemFromIndex(index.parent())->setText(group + QString(" (%1)").arg(numRows));
    }
    else if(QString("clipseed") == entry)
    {
        QString blockId = index.data(Qt::UserRole + 1).toString();
        QString id = index.data(Qt::UserRole + 2).toString();
        this->actorCollection->RemoveBlockClipseedActor(blockId, id);
        this->blockCollection->RemoveBlockClipseed(blockId, id);

        this->treeModel->removeRow(index.row(),index.parent());
        int numRows = this->treeModel->rowCount(index.parent());
        this->treeModel->itemFromIndex(index.parent())->setText(group + QString(" (%1)").arg(numRows));
    }
}

void FDEMGeometryWidget::ClearGeometryPrimitive(const QModelIndex &parent)
{
    QString prefix = parent.data().toString().split(QChar(' ')).first();
    if(QString("Blocks") == prefix)
    {
        int numRows = this->treeModel->rowCount(parent);
        for(int i = 0; i < numRows; i++)
        {
            QModelIndex index = this->treeModel->index(i, COL_NAME, parent);
            QString blockId = index.data(Qt::DisplayRole).toString();

            emit this->signalDeleteBlockMeshActor(blockId);
            emit this->signalRemoveBlockMeshItems(blockId);
            emit this->signalRemoveGeometryActor(index);
        }

        this->treeModel->removeRows(0, numRows, parent);
        this->treeModel->itemFromIndex(parent)->setData(prefix + QString(" (0)"), Qt::DisplayRole);
        this->blockCollection->Clear();
        this->actorCollection->Clear();
    }
    else if(QString("Boundaries") == prefix)
    {
        QString blockId = parent.parent().data().toString();
        emit this->signalDeleteBlockMeshActor(blockId);

        int numRows = this->treeModel->rowCount(parent);
        for(int i = 0; i < numRows; i++)
        {
            QModelIndex index = this->treeModel->index(i, COL_NAME, parent);
            emit this->signalRemoveGeometryActor(index);
        }

        this->treeModel->removeRows(0, numRows, parent);
        this->treeModel->itemFromIndex(parent)->setData(prefix + QString(" (0)"), Qt::DisplayRole);
        this->blockCollection->GetBlockBoundaryMap(blockId).clear();
    }
    else if(QString("Fractures") == prefix)
    {
        QString blockId = parent.parent().data().toString();
        emit this->signalDeleteBlockMeshActor(blockId);

        int numRows = this->treeModel->rowCount(parent);
        for(int i = 0; i < numRows; i++)
        {
            QModelIndex index = this->treeModel->index(i, COL_NAME, parent);
            emit this->signalRemoveGeometryActor(index);
        }

        this->treeModel->removeRows(0, numRows, parent);
        this->treeModel->itemFromIndex(parent)->setData(prefix + QString(" (0)"), Qt::DisplayRole);
        this->blockCollection->GetBlockFractureMap(blockId).clear();
    }
    else if(QString("Bedplanes") == prefix)
    {
        QString blockId = parent.parent().data().toString();
        emit this->signalDeleteBlockMeshActor(blockId);

        int numRows = this->treeModel->rowCount(parent);
        for(int i = 0; i < numRows; i++)
        {
            QModelIndex index = this->treeModel->index(i, COL_NAME, parent);
            emit this->signalRemoveGeometryActor(index);
        }

        this->treeModel->removeRows(0, numRows, parent);
        this->treeModel->itemFromIndex(parent)->setData(prefix + QString(" (0)"), Qt::DisplayRole);
        this->blockCollection->GetBlockBedplaneMap(blockId).clear();
    }
    else if(QString("Clipseeds") == prefix)
    {
        QString blockId = parent.parent().data().toString();
        emit this->signalDeleteBlockMeshActor(blockId);

        int numRows = this->treeModel->rowCount(parent);
        for(int i = 0; i < numRows; i++)
        {
            QModelIndex index = this->treeModel->index(i, COL_NAME, parent);
            emit this->signalRemoveGeometryActor(index);
        }

        this->treeModel->removeRows(0, numRows, parent);
        this->treeModel->itemFromIndex(parent)->setData(prefix + QString(" (0)"), Qt::DisplayRole);
        this->blockCollection->GetBlockClipseedMap(blockId).clear();
    }
}

void FDEMGeometryWidget::UpdateParentCheckState(QStandardItem *item)
{
    if(nullptr == item->parent())
        return;

    QStandardItem *parentItem = item->parent();
    int numChildren = parentItem->rowCount();
    int numCheckedChildren = 0;
    bool hasPartialChecked = false;

    for(int i = 0; i < numChildren; i++)
    {
        QStandardItem* childItem = parentItem->child(i);
        if(Qt::Checked == childItem->checkState())
        {
            numCheckedChildren++;
        }
        else if(Qt::PartiallyChecked == childItem->checkState())
        {
            hasPartialChecked = true;
            break;
        }
    }

    if(hasPartialChecked)
    {
        parentItem->setCheckState(Qt::PartiallyChecked);
    }
    else if(0 == numCheckedChildren)
    {
        parentItem->setCheckState(Qt::Unchecked);
    }
    else if(numChildren == numCheckedChildren)
    {
        parentItem->setCheckState(Qt::Checked);
    }
    else
    {
        parentItem->setCheckState(Qt::PartiallyChecked);
    }
}

void FDEMGeometryWidget::slotTreeModelItemChanged(QStandardItem *item)
{
    if(item->isCheckable())
    {
        Qt::CheckState parentState = item->checkState();
        int rowCount = item->rowCount();

        if(Qt::Unchecked == parentState || Qt::Checked == parentState)
        {
            for(int i = 0; i < rowCount; i++)
            {
                item->child(i, 0)->setCheckState(parentState);
            }
        }

        this->UpdateParentCheckState(item);
        emit this->signalGeometryActorVisibility(item);
    }
    else
    {
        QItemSelection selection = ui->treeView->selectionModel()->selection();
        QSharedPointer<Primitive> primitive = this->PrimitiveFromSelection(selection);
        if(primitive)
        {
            switch (item->column())
            {
            case COL_SHAPE:
            {
                QString shape = item->data(Qt::EditRole).toString();
                primitive->setShape(shape);

                QModelIndex index = item->index().siblingAtColumn(COL_SIZE);
                this->treeModel->setData(index,primitive->getSize(),Qt::EditRole);
                this->UpdateTableWidget(selection);
                ui->applyButton->setEnabled(true);
                break;
            }
            case COL_SIZE:
            {
                uint size = item->data(Qt::EditRole).toUInt();
                primitive->setSize(size);
                ui->tableWidget->setRowCount(size);
                ui->applyButton->setEnabled(true);
                break;
            }
            case COL_COLOR:
            {
                QString blockId = selection.indexes()[COL_NAME].data(Qt::UserRole + 1).toString();
                QString id = selection.indexes()[COL_NAME].data(Qt::UserRole + 2).toString();
                QColor color = item->data(Qt::EditRole).value<QColor>();
                emit this->signalSetBlockGeometryActorColor(blockId, id, color);
                primitive->setColor(color);
                break;
            }
            }
        }
    }
}

void FDEMGeometryWidget::slotTableWidgetItemChanged(QTableWidgetItem *)
{
    ui->applyButton->setEnabled(true);
}

void FDEMGeometryWidget::slotSelectionChanged(const QItemSelection &selection, const QItemSelection &)
{
    if(selection.isEmpty()) return;
    this->UpdateTableWidget(selection);
}

void FDEMGeometryWidget::slotAppendButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if (selection.isEmpty())
    {
        QMessageBox::information(this,
                                 QString("Invalid Operation"),
                                 QString("Please select root item first.\nThank you!"));
        return;
    }

    QModelIndex index = selection.indexes()[COL_NAME];
    this->AppendGeometryPrimitive(index);
}

void FDEMGeometryWidget::slotAppendBlockGeometryItem()
{
    // Triggered by slotImport in FDEMMeshWidget
    QStandardItem *root = this->treeModel->invisibleRootItem();
    QStandardItem *parent = root->child(0, COL_NAME);
    int id = this->treeModel->rowCount(parent->index()) + 1;
    QString blockId = QString("block-") + QString().number(id);
    parent->appendRow(this->CreateBlockGeometryItem(blockId));
    int numBlocks = this->treeModel->rowCount(parent->index());
    parent->setText(QString("Blocks") + QString(" (%1)").arg(numBlocks));
}

void FDEMGeometryWidget::slotRemoveBlockGeometryItem()
{
    QStandardItem *root = this->treeModel->invisibleRootItem();
    QStandardItem *parent = root->child(0, COL_NAME);
    int id = this->treeModel->rowCount(parent->index()) - 1;
    QModelIndex index = this->treeModel->index(id, COL_NAME, parent->index());
    QString blockId = index.data(Qt::DisplayRole).toString();

    emit this->signalRemoveGeometryActor(index);
    this->treeModel->removeRow(index.row(), index.parent());
    int numBlocks = this->treeModel->rowCount(index.parent());
    parent->setText(QString("Blocks") + QString(" (%1)").arg(numBlocks));

    //Delete block collections
    this->blockCollection->RemoveBlock(blockId);
    this->actorCollection->RemoveBlockActor(blockId);
}

void FDEMGeometryWidget::slotRemoveButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QModelIndex index = selection.indexes()[COL_NAME];
    emit this->signalRemoveGeometryActor(index);
    this->RemoveGeometryPrimitive(index);
}

void FDEMGeometryWidget::slotClearButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QModelIndex index = selection.indexes()[COL_NAME];
    this->ClearGeometryPrimitive(index);
}

void FDEMGeometryWidget::slotApplyButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    this->UpdateGeometryInformation(selection);
    QModelIndex index = selection.indexes()[COL_NAME];
    emit this->signalRenderGeometryActor(index);
    ui->applyButton->setEnabled(false);
}

void FDEMGeometryWidget::slotResetButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    this->UpdateTableWidget(selection);
}

void FDEMGeometryWidget::slotCancelButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    this->UpdateTableWidget(selection);
}

void FDEMGeometryWidget::slotClippingButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QString blockId = selection.indexes()[COL_NAME].data(Qt::UserRole + 1).toString();
    QString groupId = selection.indexes()[COL_NAME].data(Qt::DisplayRole).toString();
    QString prefix = groupId.split(QChar('-'), QString::SkipEmptyParts).front();

    // Specify Clipping Region
    QSharedPointer<BoundaryClipper> clipper = QSharedPointer<BoundaryClipper>(new BoundaryClipper());
    Block::PrimitiveMap boundary_map = this->blockCollection->GetBlock(blockId)->GetBoundaryMap();
    clipper->SetClippingBoundary(boundary_map);
    Block::PrimitiveMap clipseed_map = this->blockCollection->GetBlock(blockId)->GetClipseedMap();
    clipper->SetClippingSentinel(clipseed_map);
    // Update Clipping Primitive
    QSharedPointer<Primitive> primitive = this->blockCollection->GetBlockFracture(blockId, groupId);
    vtkDoubleArray *dataArray = clipper->Clip(primitive.data());

    // Update User Interface
    QIcon icon = QIcon(path + QString("segment_Geo.png"));
    this->treeModel->setData(selection.indexes()[COL_SHAPE], QString("Segment"), Qt::EditRole);
    this->treeModel->setData(selection.indexes()[COL_SHAPE], icon, Qt::DecorationRole);
    this->treeModel->setData(selection.indexes()[COL_SIZE], dataArray->GetNumberOfTuples());
    primitive->getTableData()->Delete();
    primitive->setTableData(dataArray);
    this->UpdateTableWidget(selection);
    this->slotApplyButtonClicked();
}

void FDEMGeometryWidget::slotInsertDFNButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if (selection.isEmpty())
    {
        QMessageBox::information(this, QString("Invalid Operation"),
                                 QString("Please select fracture group first.\nThank you!"));
        return;
    }
    QModelIndex index = selection.indexes()[COL_NAME];
    QString blockId = index.parent().data().toString();
    QString groupId = index.data().toString().split(QChar(' ')).first();

    //DFN Dialog
    QSharedPointer<FractureNetworkDialog> dialog =
            QSharedPointer<FractureNetworkDialog>(new FractureNetworkDialog(this));
    if(dialog->exec())
    {
        QRectF doi = dialog->GetDomainOfInterest();
        QSharedPointer<FractureNetwork> dfn = dialog->GetFractureNetwork();
        dfn->Generate(doi);

        FractureNetwork::FractureMap fracture_map = dfn->GetFractureMap();
        for(auto iter = fracture_map.begin(); iter != fracture_map.end(); ++iter)
        {
            vtkIdType numTuples = iter.value()->GetNumberOfFractures();
            std::vector<QLineF> lines = iter.value()->GetFractureVector();

            int id = blockCollection->GetBlockFractureMap(blockId).size();
            QString fractureId = QString("fracture-%1").arg(id + 1);
            QSharedPointer<Primitive> primitive = blockCollection->GetBlockFracture(blockId, fractureId);
            primitive->setSize(numTuples);

            vtkDoubleArray *array = vtkDoubleArray::New();
            array->SetNumberOfComponents(4);
            array->SetNumberOfTuples(numTuples);
            for(vtkIdType i = 0; i < numTuples; i++)
            {
                 array->SetTuple4(i, lines[i].x1(), lines[i].y1(), lines[i].x2(), lines[i].y2());
            }
            primitive->setTableData(array);

            QList<QStandardItem *> items = this->CreateBlockFractureItem(blockId, fractureId);
            this->treeModel->itemFromIndex(index)->appendRow(items);

            int size = this->treeModel->rowCount(index);
            this->treeModel->itemFromIndex(index)->setText(groupId + QString(" (%1)").arg(size));
            ui->treeView->setExpanded(index, true);
        }
    }
}

void FDEMGeometryWidget::slotImportButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if (selection.isEmpty())
    {
        QMessageBox::information(this, QString("Invalid Operation"),
                                 QString("Please select fracture group."));
        return;
    }

    QModelIndex index = selection.indexes()[COL_NAME];
    QString blockId = index.parent().data().toString();
    QString groupId = index.data().toString().split(QChar(' ')).first();
    if(QString("Fractures") != groupId)
    {
        QMessageBox::information(this, QString("Invalid Operation"),
                                 QString("Please select fracture group."));
        return;
    }

    QSharedPointer<GlobalCollection> context = GlobalCollection::GetGlobalCollection();
    QString folder = context->getWorkspace();
    QString filter = QString("VTK File (*.vtk);;");
    QString fname = QFileDialog::getOpenFileName(this, tr("Open geometry file"), folder, filter);

    // parse vtk file
    ASCIIFileParser parser;
    parser.ParseVtkFile(fname);
    vtkDoubleArray *endPoints = parser.GetPointArray();
    int id = blockCollection->GetBlockFractureMap(blockId).size();
    QString fractureId = QString("fracture-%1").arg(id + 1);
    QSharedPointer<Primitive> primitive = blockCollection->GetBlockFracture(blockId, fractureId);
    primitive->setSize(endPoints->GetNumberOfTuples());
    primitive->setTableData(endPoints);
    qDebug() << endPoints->GetNumberOfTuples();

    QList<QStandardItem *> items = this->CreateBlockFractureItem(blockId, fractureId);
    this->treeModel->itemFromIndex(index)->appendRow(items);

    int size = this->treeModel->rowCount(index);
    this->treeModel->itemFromIndex(index)->setText(groupId + QString(" (%1)").arg(size));
    ui->treeView->setExpanded(index, true);
}
