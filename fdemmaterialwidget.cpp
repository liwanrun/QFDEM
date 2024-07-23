#include "fdemmaterialwidget.h"
#include "ui_fdemmaterialwidget.h"
#include "fdemmaterialitemdelegate.h"
#include "qttreepropertybrowser.h"
#include "fdemtrianglematerialwidget.h"
#include "fdemcohesivematerialwidget.h"
#include "fdemntsfluidmaterialwidget.h"
#include "mechanicalconstitutivemodel.h"
#include "blockcollection.h"
#include "actorcollection.h"
#include "selectcollection.h"
#include "groupcollection.h"
#include "group.h"
#include "materialcollection.h"
#include "material.h"

#include <QMessageBox>
#include <QStandardItemModel>

FDEMMaterialWidget::FDEMMaterialWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMMaterialWidget)
{
    ui->setupUi(this);

    this->tableModel = new QStandardItemModel(this);
    ui->tableView->setModel(this->tableModel);
    this->delegate = new FDEMMaterialItemDelegate(this);
    ui->tableView->setItemDelegate(this->delegate);

    this->SetButtonsEnabled(false);
    this->InitMaterialTableView();
    this->InitMaterialProperty();
    this->ConnectSignalSlots();
    
    this->materialCollection = MaterialCollection::GetMaterialCollection();
}

FDEMMaterialWidget::~FDEMMaterialWidget()
{
    delete this->initialWidget;
    delete this->triangleWidget;
    delete this->quadWidget;
    delete this->fluidWidget;

    delete this->tableModel;
    delete this->delegate;

    delete ui;
}

void FDEMMaterialWidget::InitMaterialTableView()
{
    //Headers
    QFont font(QString("Arial"), -1, QFont::Bold);
    QStringList headers = QStringList() << "Name" << "Element Type" << "Solution";
    this->tableModel->setHorizontalHeaderLabels(headers);
    for(int col = 0; col < headers.size(); col++)
    {
        QStandardItem *item = this->tableModel->horizontalHeaderItem(col);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFont(font);
    }

    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setColumnWidth(COL_TITLE, 120);
    ui->tableView->setColumnWidth(COL_ETYPE, 120);
    ui->tableView->setColumnWidth(COL_SOLVE, 120);
}

void FDEMMaterialWidget::InitMaterialProperty()
{
    this->initialWidget = new QtTreePropertyBrowser;
    ui->propertyWidget->layout()->addWidget(this->initialWidget);
    this->initialWidget->show();

    this->triangleWidget = new FDEMTriangleMaterialWidget;
    ui->propertyWidget->layout()->addWidget(this->triangleWidget);
    this->triangleWidget->hide();

    this->quadWidget = new FDEMCohesiveMaterialWidget;
    ui->propertyWidget->layout()->addWidget(this->quadWidget);
    this->quadWidget->hide();

    this->fluidWidget = new FDEMNTSFluidMaterialWidget;
    ui->propertyWidget->layout()->addWidget(this->fluidWidget);
    this->fluidWidget->hide();
}

void FDEMMaterialWidget::ConnectSignalSlots()
{
    connect(ui->appendButton, &QPushButton::clicked,
            this, &FDEMMaterialWidget::slotAppendButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked,
            this, &FDEMMaterialWidget::slotRemoveButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked,
            this, &FDEMMaterialWidget::slotClearButtonClicked);
    connect(ui->rescaleButton, &QPushButton::clicked,
            this, &FDEMMaterialWidget::slotRescaleButtonClicked);

    connect(this->tableModel, &QStandardItemModel::itemChanged,
            this, &FDEMMaterialWidget::slotTableModelItemChanged);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &FDEMMaterialWidget::slotSelectionChanged);

    connect(ui->applyButton, &QPushButton::clicked,
            this, &FDEMMaterialWidget::slotApplyButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &FDEMMaterialWidget::slotCancelButtonClicked);
    connect(ui->defaultButton, &QPushButton::clicked,
            this, &FDEMMaterialWidget::slotDefaultButtonClicked);
}

void FDEMMaterialWidget::SetButtonsEnabled(bool arg)
{
    ui->removeButton->setEnabled(arg);
    ui->clearButton->setEnabled(arg);
    ui->applyButton->setEnabled(arg);
    ui->cancelButton->setEnabled(arg);
    ui->defaultButton->setEnabled(arg);
}

QList<QStandardItem *> FDEMMaterialWidget::CreateMaterialItems(const QString &materialId)
{
    //Material title
    QStandardItem* title = new QStandardItem(materialId);
    title->setTextAlignment(Qt::AlignCenter);
    title->setEditable(false);

    //Material element
    QStandardItem* etype = new QStandardItem;
    etype->setData(QString("Solid Triangle (3)"), Qt::EditRole);
    etype->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    //Material state
    QStandardItem* state = new QStandardItem;
    state->setData(QString("solution-1"), Qt::EditRole);
    state->setTextAlignment(Qt::AlignCenter);

    QList<QStandardItem *> items;
    items << title << etype << state;
    return items;
}

void FDEMMaterialWidget::slotAppendButtonClicked()
{
    int index = this->tableModel->rowCount() + 1;
    QString matId = QString("material-%1").arg(index);

    this->materialCollection->AppendMaterial(matId);
    this->tableModel->appendRow(this->CreateMaterialItems(matId));

    this->SetButtonsEnabled(true);
}

void FDEMMaterialWidget::slotRemoveButtonClicked()
{
    int index = this->tableModel->rowCount();
    QString matId = QString("material-%1").arg(index);
    emit this->signalCancelMaterialButtonClicked(matId);

    this->tableModel->removeRow(this->tableModel->rowCount() - 1);
    this->materialCollection->RemoveMaterial(matId);
    this->SetButtonsEnabled(this->tableModel->rowCount());

    ui->tableView->selectRow(this->tableModel->rowCount() - 1);
    if(0 == this->tableModel->rowCount())
    {
        this->initialWidget->show();
        this->triangleWidget->hide();
        this->quadWidget->hide();
        this->fluidWidget->hide();
        emit this->signalClearMaterialButtonClicked();
    }
}

void FDEMMaterialWidget::slotClearButtonClicked()
{
    this->tableModel->removeRows(0, this->tableModel->rowCount());
    this->materialCollection->Clear();
    this->SetButtonsEnabled(false);

    if(!this->tableModel->rowCount())
    {
        this->initialWidget->show();
        this->triangleWidget->hide();
        this->quadWidget->hide();
        this->fluidWidget->hide();
        emit this->signalClearMaterialButtonClicked();
    }
}

void FDEMMaterialWidget::slotRescaleButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(selection.empty()) return;

    QSharedPointer<BlockCollection> blockCollection = BlockCollection::GetBlockCollection();
    QSharedPointer<MaterialCollection> materialCollection = MaterialCollection::GetMaterialCollection();
    MaterialCollection::MaterialMap material_map = materialCollection->GetMaterialMap();

    QString materialId = selection.indexes().at(COL_TITLE).data().toString();
    if(Material::TRIANGLE == material_map[materialId]->getMaterialType())
    {
        TriangleMaterial *material = static_cast<TriangleMaterial*>(material_map[materialId].data());
        if(1 == material->mechanicalModelType)
        {
            TriangleMechanicalModel *model = static_cast<TriangleMechanicalModel*>(material->mechanicalModel.data());
            model->viscousDamping = 2.0*blockCollection->minimumMeshSize*sqrt(model->youngModulus*material->density);

            QString msg = QString("[>>>] Damping coefficient:      [%1]").arg(model->viscousDamping);
            emit this->signalOutputPromptMessage(msg);
        }
    }
    else
    {
        QMessageBox::warning(this, QString("Material warning"),
                             QString("Damping is unnecessary. Check it out!"));
        return;
    }
}

void FDEMMaterialWidget::slotTableModelItemChanged(QStandardItem *item)
{
    QString matId = this->tableModel->item(item->row(), COL_TITLE)->data(Qt::DisplayRole).toString();
    QString solId = this->tableModel->item(item->row(), COL_SOLVE)->data(Qt::EditRole).toString();

    if(COL_ETYPE == item->column())
    {
        QString elemType = item->data(Qt::EditRole).toString();
        if(QString("Solid Triangle (3)") == elemType)
        {
            QSharedPointer<TriangleMaterial> newMaterial =
                    QSharedPointer<TriangleMaterial>(new TriangleMaterial);
            this->materialCollection->UpdateMaterial(matId, newMaterial);
            this->triangleWidget->UpdatePropertyBrowser(newMaterial.get());
            this->triangleWidget->slotColumnSolutionChanged(solId);
            this->triangleWidget->show();

            this->initialWidget->hide();
            this->quadWidget->hide();
            this->fluidWidget->hide();
        }
        else if(QString("Cohesive Quad (4)") == elemType)
        {
            QSharedPointer<CohesiveMaterial> newMaterial =
                    QSharedPointer<CohesiveMaterial>(new CohesiveMaterial);
            this->materialCollection->UpdateMaterial(matId, newMaterial);
            this->quadWidget->UpdatePropertyBrowser(newMaterial.get());
            this->quadWidget->slotColumnSolutionChanged(solId);
            this->quadWidget->show();

            this->initialWidget->hide();
            this->triangleWidget->hide();
            this->fluidWidget->hide();
        }
        else if(QString("Newtonian Fluid (0)") == elemType)
        {
            QSharedPointer<NTFluidMaterial> newMaterial =
                    QSharedPointer<NTFluidMaterial>(new NTFluidMaterial);
            this->materialCollection->UpdateMaterial(matId, newMaterial);
            this->fluidWidget->UpdatePropertyBrowser(newMaterial.get());
            this->fluidWidget->show();

            this->initialWidget->hide();
            this->triangleWidget->hide();
            this->quadWidget->hide();
        }
    }
    else if(COL_SOLVE == item->column())
    {
        QString solId = item->data(Qt::EditRole).toString();
        this->triangleWidget->slotColumnSolutionChanged(solId);
        this->quadWidget->slotColumnSolutionChanged(solId);
    }
}

void FDEMMaterialWidget::slotSelectionChanged(const QItemSelection &selection, const QItemSelection &)
{
    if(selection.isEmpty()) return;

    QString matId = selection.indexes().at(COL_TITLE).data().toString();
    QString etype = selection.indexes().at(COL_ETYPE).data().toString();
    QString solId = selection.indexes().at(COL_SOLVE).data().toString();

    QSharedPointer<Material> material = this->materialCollection->GetMaterial(matId);
    if(QString("Solid Triangle (3)") == etype)
    {
        TriangleMaterial *triangleMat = static_cast<TriangleMaterial*>(material.get());
        this->triangleWidget->UpdatePropertyBrowser(triangleMat);
        this->triangleWidget->slotColumnSolutionChanged(solId);
        this->triangleWidget->show();

        this->initialWidget->hide();
        this->quadWidget->hide();
        this->fluidWidget->hide();
    }
    else if(QString("Cohesive Quad (4)") == etype)
    {
        CohesiveMaterial *quadMat = static_cast<CohesiveMaterial*>(material.get());
        this->quadWidget->UpdatePropertyBrowser(quadMat);
        this->quadWidget->slotColumnSolutionChanged(solId);
        this->quadWidget->show();

        this->initialWidget->hide();
        this->triangleWidget->hide();
        this->fluidWidget->hide();
    }
    else if(QString("Newtonian Fluid (0)") == etype)
    {
        NTFluidMaterial* fluidMat = static_cast<NTFluidMaterial*>(material.get());
        this->fluidWidget->UpdatePropertyBrowser(fluidMat);
        this->fluidWidget->show();

        this->initialWidget->hide();
        this->triangleWidget->hide();
        this->quadWidget->hide();
    }
}

void FDEMMaterialWidget::slotApplyButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(!selection.empty())
    {
//      QString materialId = selection.indexes().at(COL_TITLE).data().toString();
//      emit this->signalApplyMaterialButtonClicked(materialId);

        // Block Dataset
        QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
        vtkMultiBlockDataSet *blockDataSet = actorCollection->GetBlockActorDataSet();

        // Select GlobalIds
        QString materialId = selection.indexes().at(COL_TITLE).data().toString();
        QSharedPointer<MaterialCollection> materialCollection = MaterialCollection::GetMaterialCollection();
        QSharedPointer<Material> material = materialCollection->GetMaterial(materialId);
        QSharedPointer<ElemSetCollection> groupCollection = ElemSetCollection::GetElemSetCollection();
        QStringList elemSetIds = material->elementSet.split(QChar(','), QString::SkipEmptyParts);

        vtkNew<vtkIdTypeArray> globalIds;
        for(QString id : elemSetIds)
        {
            bool hasGroup = groupCollection->GetGroupMap().contains(id.toInt());
            if(!hasGroup)
            {
                QMessageBox::warning(this, QString("Invalid Operation"),
                                     QString("ElemSet-%1 has not been created. Check out!").arg(id.toInt()));
                return;
            }

            QSharedPointer<Group> group = groupCollection->GetGroup(id.toInt());
            for(int i = 0; i < group->getSize(); i++)
            {
                globalIds->InsertNextValue(group->getGlobalIds()[i]);
            }
        }

        QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
        selectCollection->SetSelectStyle(Selection::NORMAL);
        selectCollection->SetSelectField(Selection::CELL);
        selectCollection->SetInputDataSet(blockDataSet);
        selectCollection->GlobalIDsSelect(globalIds);

        emit this->signalApplyMaterialButtonClicked(materialId);
    }
    else
    {
        QMessageBox::warning(this, QString("Invalid Operation"),
                             QString("Select the material to be mapped. Thanks!"));
    }
}

void FDEMMaterialWidget::slotDefaultButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(!selection.empty())
    {
        QString materialId = selection.indexes().at(COL_TITLE).data().toString();
        QSharedPointer<Material> material = materialCollection->GetMaterial(materialId);

        int type = material->getMaterialType();
        if(Material::TRIANGLE == type)
        {
            QSharedPointer<TriangleMaterial> triangleMat =
                    QSharedPointer<TriangleMaterial>(new TriangleMaterial());
            this->triangleWidget->UpdatePropertyBrowser(triangleMat.get());
        }
        else if(Material::COHESIVE == type)
        {
            QSharedPointer<CohesiveMaterial> quadMat =
                    QSharedPointer<CohesiveMaterial>(new CohesiveMaterial());
            this->quadWidget->UpdatePropertyBrowser(quadMat.get());
        }
        else if(Material::NTSFLUID == type)
        {
            QSharedPointer<NTFluidMaterial> fluidMat =
                    QSharedPointer<NTFluidMaterial>(new NTFluidMaterial());
            this->fluidWidget->UpdatePropertyBrowser(fluidMat.get());
        }
    }
    else
    {
        QMessageBox::warning(this, QString("Invalid Operation"),
                             QString("Select the material to be mapped. Thanks!"));
    }
}

void FDEMMaterialWidget::slotCancelButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(!selection.empty())
    {
        QString materialId = selection.indexes().at(COL_TITLE).data().toString();
        emit this->signalCancelMaterialButtonClicked(materialId);
    }
    else
    {
        QMessageBox::warning(this, QString("Invalid Operation"),
                             QString("Select the material to be canceled. Thanks!"));
    }
}

void FDEMMaterialWidget::slotSolutionCollectionChanged(int size)
{
    FDEMMaterialItemDelegate *delegate = dynamic_cast<FDEMMaterialItemDelegate*>(ui->tableView->itemDelegate());
    if(delegate)
    {
        QStringList names = {"solution-1"};
        for(int i = 1; i < size; i++)
        { names << QString("solution-%1").arg(i + 1);}
        delegate->SetSolutionNames(names);

        for(int i = 0; i < this->tableModel->rowCount(); i++)
        {
            QStandardItem *item = this->tableModel->item(i, COL_SOLVE);
            if(!names.contains(item->data(Qt::EditRole).toString()))
            { item->setText(names.last()); }
        }
    }
}
