#include "fdemgroupwidget.h"
#include "ui_fdemgroupwidget.h"
#include "group.h"
#include "groupcollection.h"
#include "fdemgroupitemdelegate.h"
#include "selectcollection.h"
#include "actor.h"
#include "actorcollection.h"
#include "blockcollection.h"

#include <QMessageBox>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkMultiBlockDataSet.h>

enum ColumnHeader
{
    COL_NAME = 0,
    COL_TEXT,
    COL_SIZE
};


FDEMGroupWidget::FDEMGroupWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMGroupWidget),
    path(QString(":/images/resources/")),
    filter(QString("None"))
{
    ui->setupUi(this);
    this->blockDataSet = nullptr;

    this->treeModel = new QStandardItemModel(this);
    ui->treeView->setModel(this->treeModel);
    this->delegate = new FDEMGroupItemDelegate(this);
    ui->treeView->setItemDelegate(this->delegate);

    this->SetButtonsEnabled(false);
    this->InitGroupTreeView();
    this->ConnectSignalSlots();

    this->nodeSetCollection = NodeSetCollection::GetNodeSetCollection();
    this->elemSetCollection = ElemSetCollection::GetElemSetCollection();
}

FDEMGroupWidget::~FDEMGroupWidget()
{
    delete this->delegate;
    delete this->treeModel;

    if(this->blockDataSet)
    {
        this->blockDataSet->Delete();
    }

    delete ui;
}

void FDEMGroupWidget::ConnectSignalSlots()
{
    connect(this->treeModel, &QStandardItemModel::itemChanged,
            this, &FDEMGroupWidget::slotTreeModelItemChanged);
    connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &FDEMGroupWidget::slotSelectionChanged);

    connect(ui->appendButton, &QPushButton::clicked,
            this, &FDEMGroupWidget::slotAppendButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked,
            this, &FDEMGroupWidget::slotRemoveButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked,
            this, &FDEMGroupWidget::slotClearButtonClicked);

    connect(ui->blockGroupButton, &QPushButton::clicked,
            this, &FDEMGroupWidget::slotBlockGroupButtonClicked);
    connect(ui->grainGroupButton, &QPushButton::clicked,
            this, &FDEMGroupWidget::slotGrainGroupButtonClicked);
    connect(ui->interfaceGroupButton, &QPushButton::clicked,
            this, &FDEMGroupWidget::slotInterfaceGroupButtonClicked);
    connect(ui->convertNodeSetButton, &QPushButton::clicked,
            this, &FDEMGroupWidget::slotConvertNodeSetButtonClicked);
    connect(ui->convertQuadSetButton, &QPushButton::clicked,
            this, &FDEMGroupWidget::slotConvertQuadSetButtonClicked);

    connect(ui->applyButton, &QPushButton::clicked,
            this, &FDEMGroupWidget::slotApplyButtonClicked);
    connect(ui->displayButton, &QPushButton::clicked,
            this, &FDEMGroupWidget::slotDisplayButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &FDEMGroupWidget::slotCancelButtonClicked);
}

void FDEMGroupWidget::InitGroupTreeView()
{
    // Horizontal Headers
    QFont font(QString("Arial"), -1, QFont::Bold);
    QStringList headers = QStringList() << "Name" << "Label" << "Size";
    this->treeModel->setHorizontalHeaderLabels(headers);
    for(int col = 0; col < headers.size(); col++)
    {
        QStandardItem *item = this->treeModel->horizontalHeaderItem(col);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFont(font);
    }

    // Root Item
    QStandardItem *nodeSetRootItem = new QStandardItem(QIcon(path + QString("pqPointData.png")), QString("NodeSets (0)"));
    nodeSetRootItem->setEditable(false);
    this->treeModel->appendRow(nodeSetRootItem);
    ui->treeView->setFirstColumnSpanned(nodeSetRootItem->row(), this->treeModel->invisibleRootItem()->index(), true);
    ui->treeView->setExpanded(nodeSetRootItem->index(), true);

    QStandardItem *elemSetRootItem = new QStandardItem(QIcon(path + QString("pqCellData.png")), QString("ElemSets (0)"));
    elemSetRootItem->setEditable(false);
    this->treeModel->appendRow(elemSetRootItem);
    ui->treeView->setFirstColumnSpanned(elemSetRootItem->row(), this->treeModel->invisibleRootItem()->index(), true);
    ui->treeView->setExpanded(elemSetRootItem->index(), true);

    // View Style
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeView->setAlternatingRowColors(true);
    ui->treeView->setIndentation(20);
    ui->treeView->setColumnWidth(COL_NAME, 150);
    ui->treeView->setColumnWidth(COL_TEXT, 120);
    ui->treeView->setColumnWidth(COL_SIZE, 100);

    // Selection Mode
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void FDEMGroupWidget::SetButtonsEnabled(bool arg)
{
    ui->removeButton->setEnabled(arg);
    ui->clearButton->setEnabled(arg);
}

QList<QStandardItem *> FDEMGroupWidget::CreateGroupItem(QSharedPointer<Group> group)
{
    QString path = QString(":/images/resources/");
    //Name
    QStandardItem* name = new QStandardItem;
    name->setEditable(false);
    name->setData(group->getName(), Qt::EditRole);
    name->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    //Label
    QStandardItem* text = new QStandardItem;
    text->setData(group->getText(), Qt::EditRole);
    text->setIcon(QIcon(path + QString("record.png")));
    text->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    //Size
    QStandardItem* size = new QStandardItem;
    size->setData(static_cast<int>(group->getSize()), Qt::EditRole);
    size->setEditable(false);
    size->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QList<QStandardItem *> items;
    items << name << text << size;
    return items;
}

QSharedPointer<Group> FDEMGroupWidget::GroupFromSelection(const QString name)
{
    if(name.contains(QChar('-')))
    {
        int index = name.split(QChar('-')).last().toInt();
        QString groupId = name.split(QChar('-')).first();
        QSharedPointer<GroupCollection> groupCollection = this->GetCollection(groupId);
        return groupCollection->GetGroup(index);
    }

    return nullptr;
}

void FDEMGroupWidget::UpdateGroupTableWidget(const QVector<size_t> &globalIds, const QVector<size_t> &pedigreeIds)
{
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(2);
    QStringList headers = { "Global ID", "Pedigree ID" };
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->setRowCount(static_cast<int>(globalIds.size()));

    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QTableWidgetItem *item;

        item = new QTableWidgetItem;
        item->setText(QString().number(globalIds[i]));
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget->setItem(i, 0, item);

        item = new QTableWidgetItem;
        item->setText(QString().number(pedigreeIds[i]));
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget->setItem(i, 1, item);
    }
}

void FDEMGroupWidget::HightlightGroupEntity(QSharedPointer<Group> group)
{
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    this->blockDataSet = actorCollection->GetBlockActorDataSet();

    vtkNew<vtkIdTypeArray> globalIds;
    globalIds->SetNumberOfValues(group->getSize());
    for(int i = 0; i < group->getSize(); i++)
    {
        globalIds->InsertNextValue(group->getGlobalIds()[i]);
    }

    int selectStyle = (Group::POINT == group->getType()) ? Selection::POINT : Selection::CELL;
    QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
    selectCollection->SetSelectStyle(Selection::UNION);
    selectCollection->SetSelectField(selectStyle);
    selectCollection->SetInputDataSet(blockDataSet);
    selectCollection->GlobalIDsSelect(globalIds);
    selectCollection->HighlightSelection();
}

QSharedPointer<GroupCollection> FDEMGroupWidget::GetCollection(QString id)
{
    if(QString("NodeSets") == id || QString("nodeSet") == id)
    {
        return this->nodeSetCollection;
    }
    else if(QString("ElemSets") == id || QString("elemSet") == id)
    {
        return this->elemSetCollection;
    }

    return nullptr;
}

void FDEMGroupWidget::GetGlobalIdsOfElemSet(QSharedPointer<Group> elemSet, QVector<size_t> &globalIds)
{
    QVector<size_t> elemSetIds = elemSet->getGlobalIds();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();
    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        QSharedPointer<TriangulationActor> mesh = iter.value()->GetTriangulationActor();
        vtkPolyData *pd = mesh->GetVTKActorMapper()->GetInput();
        vtkDataArray *cellGlobalIds = pd->GetCellData()->GetGlobalIds();
        vtkDataArray *pointGlobalIds = pd->GetPointData()->GetGlobalIds();

        for (int idx = 0; idx < elemSetIds.size(); idx++)
        {
            vtkIdType cellId = cellGlobalIds->LookupValue(elemSetIds[idx]);
            vtkIdType npts, *pts;
            pd->GetCellPoints(cellId, npts, pts);
            for (vtkIdType i = 0; i < npts; i++)
            {
                globalIds.push_back(pointGlobalIds->GetVariantValue(pts[i]).ToLongLong());
            }
        }
    }
}

void FDEMGroupWidget::GetPedigreeIdsOfElemSet(QSharedPointer<Group> elemSet, QVector<size_t> &pedigreeIds)
{
    QVector<size_t> elemSetIds = elemSet->getGlobalIds();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();
    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        QSharedPointer<TriangulationActor> mesh = iter.value()->GetTriangulationActor();
        vtkPolyData *pd = mesh->GetVTKActorMapper()->GetInput();
        vtkDataArray *cellGlobalIds = pd->GetCellData()->GetGlobalIds();
        vtkAbstractArray *pointPedigreeIds = pd->GetPointData()->GetPedigreeIds();

        for (int idx = 0; idx < elemSetIds.size(); idx++)
        {
            vtkIdType cellId = cellGlobalIds->LookupValue(elemSetIds[idx]);
            vtkIdType npts, *pts;
            pd->GetCellPoints(cellId, npts, pts);
            for (vtkIdType i = 0; i < npts; i++)
            {
                pedigreeIds.push_back(pointPedigreeIds->GetVariantValue(pts[i]).ToLongLong());
            }
        }
    }
}

void FDEMGroupWidget::slotTreeModelItemChanged(QStandardItem *item)
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QString name = selection.indexes().at(COL_NAME).data().toString();
    QSharedPointer<Group> group = this->GroupFromSelection(name);
    if(nullptr != group)
    {
        switch (item->column())
        {
        case COL_SIZE:
        {
            uint size = item->data(Qt::EditRole).toUInt();
            group->setSize(size);
            ui->tableWidget->setRowCount(size);
            ui->applyButton->setEnabled(true);
            break;
        }
        case COL_TEXT:
        {
            QString label = item->data(Qt::EditRole).toString();
            group->setText(label);
            break;
        }
        }
    }
}

void FDEMGroupWidget::slotSelectionChanged(const QItemSelection &selection, const QItemSelection &)
{
    if(selection.isEmpty()) return;

    QString name = selection.indexes().at(COL_NAME).data().toString();
    ui->convertNodeSetButton->setEnabled(name.startsWith(QString("elemSet")));
    ui->convertQuadSetButton->setEnabled(name.startsWith(QString("elemSet")));

    QSharedPointer<Group> group = this->GroupFromSelection(name);
    if(nullptr != group)
    {
        this->UpdateGroupTableWidget(group->getGlobalIds(), group->getPedigreeIds());
    }
    else
    {
        ui->tableWidget->setRowCount(0);
        ui->tableWidget->setColumnCount(0);
    }

    QModelIndex index = selection.indexes()[(COL_NAME)];
    this->SetButtonsEnabled(this->treeModel->rowCount(index));
}

void FDEMGroupWidget::slotAppendButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if (selection.isEmpty())
    {
        QMessageBox::information(this, QString("Invalid Operation"),
                                 QString("Please select root item first.\nThank you!"));
        return;
    }

    QModelIndex index = selection.indexes()[COL_NAME];
    this->AppendSubGroup(index);
}

void FDEMGroupWidget::slotRemoveButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QModelIndex index = selection.indexes()[COL_NAME];
    this->RemoveSubGroup(index);
}

void FDEMGroupWidget::slotClearButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if (selection.isEmpty())
    {
        QMessageBox::information(this, QString("Invalid Operation"),
                                 QString("Please select root item first.\nThank you!"));
        return;
    }

    QModelIndex index = selection.indexes()[COL_NAME];
    this->ClearSubGroup(index);
}

void FDEMGroupWidget::slotBlockGroupButtonClicked()
{
    QSharedPointer<BlockCollection> blockCollection = BlockCollection::GetBlockCollection();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();

    vtkIdType groupOff = this->elemSetCollection->GetGroupSize();
    ActorCollection::BlockActorMap actor_map = actorCollection->GetBlockActorMap();
    for(auto iter = actor_map.begin(); iter != actor_map.end(); ++iter)
    {
        QString blk = iter.key().split(QChar('-'), QString::SkipEmptyParts).last();
        vtkPolyData *pd = iter.value()->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        vtkAbstractArray *pedigreeIds = pd->GetCellData()->GetPedigreeIds();
        vtkDataArray *globalIds = pd->GetCellData()->GetGlobalIds();

        for(vtkIdType i = 0; i < pd->GetNumberOfCells(); i++)
        {
            QString elt;
            vtkIdType idx = blk.toInt();
            if (VTK_TRIANGLE == pd->GetCellType(i)) {
                elt = QString("Tri");
            }
            else if (VTK_QUAD == pd->GetCellType(i)) {
                elt = QString("Coh");
                idx = blockCollection->GetBlockMap().size() + idx;
            }

            QString tag = QString("block-%1(%2)").arg(blk).arg(elt);
            QSharedPointer<Group> group = this->elemSetCollection->GetGroup(idx + groupOff);
            group->getGlobalIds().push_back(globalIds->GetVariantValue(i).ToLongLong());
            group->getPedigreeIds().push_back(pedigreeIds->GetVariantValue(i).ToLongLong());
            group->setSize(group->getPedigreeIds().size());
            group->setType(Group::CELL);
            group->setText(tag);
        }
    }

    QStandardItem *root = this->treeModel->item(1, 0);
    GroupCollection::GroupMap group_map = this->elemSetCollection->GetGroupMap();
    for(auto iter = group_map.begin(); iter != group_map.end(); ++iter)
    {
        if(iter.key() <= groupOff) continue;
        root->appendRow(this->CreateGroupItem(iter.value()));
        root->setText(QString("ElemSets (%1)").arg(this->treeModel->rowCount(root->index())));
        this->SetButtonsEnabled(true);
    }
}

void FDEMGroupWidget::slotGrainGroupButtonClicked()
{
    QSharedPointer<BlockCollection> blockCollection = BlockCollection::GetBlockCollection();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();

    vtkIdType numBlocks = blockCollection->getBlockGrainSize();
    vtkIdType numGroups = this->elemSetCollection->GetGroupSize();
    ActorCollection::BlockActorMap actor_map = actorCollection->GetBlockActorMap();
    for(auto iter = actor_map.begin(); iter != actor_map.end(); ++iter)
    {
        QString blk = iter.key().split(QChar('-'), QString::SkipEmptyParts).last();
        vtkPolyData *pd = iter.value()->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        vtkAbstractArray *pedigreeIds = pd->GetCellData()->GetPedigreeIds();
        vtkDataArray *globalIds = pd->GetCellData()->GetGlobalIds();
        vtkAbstractArray *grainIds = pd->GetCellData()->GetAbstractArray("Cell Grain Ids");
        if(nullptr == grainIds) continue;

        for(vtkIdType i = 0; i < pd->GetNumberOfCells(); i++)
        {
            vtkIdType gid = grainIds->GetVariantValue(i).ToInt();
            if(-1 == gid) continue;

            QString elt;
            vtkIdType idx = gid;
            if (VTK_TRIANGLE == pd->GetCellType(i)) {
                elt = QString("Tri");
            }
            else if (VTK_QUAD == pd->GetCellType(i)) {
                elt = QString("Coh");
                idx = gid + numBlocks;
            }

            QString tag = QString("grain-%1-%2(%3)").arg(gid).arg(blk).arg(elt);
            QSharedPointer<Group> grain = this->elemSetCollection->GetGroup(idx + numGroups);
            grain->getGlobalIds().push_back(globalIds->GetVariantValue(i).ToLongLong());
            grain->getPedigreeIds().push_back(pedigreeIds->GetVariantValue(i).ToLongLong());
            grain->setSize(grain->getPedigreeIds().size());
            grain->setType(Group::GRAIN);
            grain->setText(tag);
        }
    }

    QStandardItem *root = this->treeModel->item(1, 0);
    GroupCollection::GroupMap group_map = this->elemSetCollection->GetGroupMap();
    for(auto iter = group_map.begin(); iter != group_map.end(); ++iter)
    {
        if(iter.key() <= numGroups) continue;
        root->appendRow(this->CreateGroupItem(iter.value()));
        root->setText(QString("ElemSets (%1)").arg(this->treeModel->rowCount(root->index())));
        this->SetButtonsEnabled(true);
    }
}

void FDEMGroupWidget::slotInterfaceGroupButtonClicked()
{
    QSharedPointer<BlockCollection> blockCollection = BlockCollection::GetBlockCollection();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();

    vtkIdType numGroups = this->elemSetCollection->GetGroupSize();
    ActorCollection::BlockActorMap actor_map = actorCollection->GetBlockActorMap();
    for(auto iter = actor_map.begin(); iter != actor_map.end(); ++iter)
    {
        QString blk = iter.key().split(QChar('-'), QString::SkipEmptyParts).last();
        vtkPolyData *pd = iter.value()->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        vtkAbstractArray *pedigreeIds = pd->GetCellData()->GetPedigreeIds();
        vtkDataArray *globalIds = pd->GetCellData()->GetGlobalIds();
        vtkAbstractArray *constraintTags = pd->GetCellData()->GetAbstractArray("Cell Constraint Tags");
        if(nullptr == constraintTags) continue;

        for(vtkIdType i = 0; i < pd->GetNumberOfCells(); i++)
        {
            if(VTK_TRIANGLE == pd->GetCellType(i)) continue;

            QString elt = QString("Coh");
            vtkIdType idx = blk.toInt();
            vtkIdType interfaceTag = constraintTags->GetVariantValue(i).ToInt();
            if(interfaceTag > 0)
            {
                QString tag = QString("interface-%1(%2)").arg(blk).arg(elt);
                QSharedPointer<Group> group = this->elemSetCollection->GetGroup(idx + numGroups);
                group->getGlobalIds().push_back(globalIds->GetVariantValue(i).ToLongLong());
                group->getPedigreeIds().push_back(pedigreeIds->GetVariantValue(i).ToLongLong());
                group->setSize(group->getPedigreeIds().size());
                group->setType(Group::CELL);
                group->setText(tag);
            }
        }
    }

    QStandardItem *root = this->treeModel->item(1, 0);
    GroupCollection::GroupMap group_map = this->elemSetCollection->GetGroupMap();
    for(auto iter = group_map.begin(); iter != group_map.end(); ++iter)
    {
        if(iter.key() <= numGroups) continue;
        root->appendRow(this->CreateGroupItem(iter.value()));
        root->setText(QString("ElemSets (%1)").arg(this->treeModel->rowCount(root->index())));
        this->SetButtonsEnabled(true);
    }
}

void FDEMGroupWidget::slotConvertNodeSetButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QString name = selection.indexes().at(COL_NAME).data().toString();
    if(!name.startsWith(QString("elemSet"))) return;

    int numCols = this->treeModel->columnCount();
    QModelIndexList indexes = selection.indexes();
    for(int i = 0; i < indexes.size(); i+=numCols)
    {
        name = selection.indexes().at(i).data().toString();
        QSharedPointer<Group> elemSet = this->GroupFromSelection(name);

        QVector<size_t> nodeSetGlobalIds;
        QVector<size_t> nodeSetPedigreeIds;
        this->GetGlobalIdsOfElemSet(elemSet, nodeSetGlobalIds);
        this->GetPedigreeIdsOfElemSet(elemSet, nodeSetPedigreeIds);

        int index = this->nodeSetCollection->GetGroupMap().size() + 1;
        QSharedPointer<Group> nodeGroup = this->nodeSetCollection->GetGroup(index);
        nodeGroup->setType(Group::POINT);
        nodeGroup->setText(elemSet->getText());
        nodeGroup->setGlobalIds(nodeSetGlobalIds);
        nodeGroup->setPedigreeIds(nodeSetPedigreeIds);
        nodeGroup->setSize(nodeSetGlobalIds.size());

        QStandardItem *root = this->treeModel->item(0, 0);
        GroupCollection::GroupMap group_map = this->nodeSetCollection->GetGroupMap();
        root->appendRow(this->CreateGroupItem(nodeGroup));
        root->setText(QString("NodeSets (%1)").arg(this->treeModel->rowCount(root->index())));
        this->SetButtonsEnabled(true);
    }

    this->slotCancelButtonClicked();
}

void FDEMGroupWidget::slotConvertQuadSetButtonClicked()
{
    return;
}

void FDEMGroupWidget::slotApplyButtonClicked()
{
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QString name = selection.indexes().at(COL_NAME).data().toString();
    QSharedPointer<Group> group = this->GroupFromSelection(name);
    if(group)
    {
        QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
        if(group->getType() != selectCollection->GetSelectField())
        {
            QMessageBox::warning(this, QString("Invalid Operation"),
                                 QString("Select the correct entities. Thanks!"));
            emit this->signalApplyGroupButtonClicked();
            return;
        }

        QVector<size_t> globalIds = selectCollection->GetExtractGlobalIds();
        QVector<size_t> pedigreeIds = selectCollection->GetExtractPedigreeIds();
        if(globalIds.empty()) return;

        group->setGlobalIds(globalIds);
        group->setPedigreeIds(pedigreeIds);
        group->setSize(globalIds.size());

        this->treeModel->setData(selection.indexes()[COL_SIZE], pedigreeIds.size());
        this->UpdateGroupTableWidget(group->getGlobalIds(), group->getPedigreeIds());
    }

    emit this->signalApplyGroupButtonClicked();
}

void FDEMGroupWidget::slotDisplayButtonClicked()
{
    emit this->signalCancelGroupButtonClicked();
    QItemSelection selection = ui->treeView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QString name = selection.indexes().at(COL_NAME).data().toString();
    if(!name.contains(QChar('-'))) return;

    int numCols = this->treeModel->columnCount();
    QModelIndexList indexes = selection.indexes();
    for(int i = 0; i < indexes.size(); i+=numCols)
    {
        name = selection.indexes().at(i).data().toString();
        int index = name.split(QChar('-')).last().toInt();
        QString prefix = name.split(QChar('-')).first();
        QSharedPointer<GroupCollection> collection = this->GetCollection(prefix);
        QSharedPointer<Group> group = collection->GetGroup(index);
        this->HightlightGroupEntity(group);
    }
    emit this->signalHighlightGroupButtonClicked();
}

void FDEMGroupWidget::slotCancelButtonClicked()
{
    emit this->signalCancelGroupButtonClicked();
    ui->treeView->selectionModel()->clearSelection();
}

void FDEMGroupWidget::AppendSubGroup(const QModelIndex &root)
{
    QString nodeName = root.data().toString();
    if(!nodeName.contains(QChar(' '))) return;

    QString groupId = nodeName.split(QChar(' ')).first();
    QSharedPointer<GroupCollection> groupCollection = this->GetCollection(groupId);
    int index = this->treeModel->rowCount(root) + 1;
    QSharedPointer<Group> group = groupCollection->GetGroup(index);

    QStandardItem* item = this->treeModel->itemFromIndex(root);
    item->appendRow(this->CreateGroupItem(group));
    item->setText(groupId + QString(" (%1)").arg(this->treeModel->rowCount(root)));
    this->SetButtonsEnabled(this->treeModel->rowCount(root));
    ui->treeView->setExpanded(root, true);
}

void FDEMGroupWidget::RemoveSubGroup(const QModelIndex &root)
{
    if(this->treeModel->rowCount(root))
    {
        QString groupId = root.data().toString().split(QChar(' ')).first();
        QSharedPointer<GroupCollection> collection = this->GetCollection(groupId);

        int index = this->treeModel->rowCount(root);
        collection->RemoveGroup(index);
        this->treeModel->removeRow(index - 1, root);

        int size = this->treeModel->rowCount(root);
        this->treeModel->itemFromIndex(root)->setText(groupId + QString(" (%1)").arg(size));
        this->SetButtonsEnabled(size);
    }
}

void FDEMGroupWidget::ClearSubGroup(const QModelIndex &root)
{
    if(this->treeModel->rowCount(root))
    {
        QString groupId = root.data().toString().split(QChar(' ')).first();
        QSharedPointer<GroupCollection> collection = this->GetCollection(groupId);

        this->treeModel->removeRows(0, this->treeModel->rowCount(root), root);
        collection->ClearGroupMap();

        int size = this->treeModel->rowCount(root);
        this->treeModel->itemFromIndex(root)->setText(groupId + QString(" (%1)").arg(size));
        this->SetButtonsEnabled(false);
    }
}
