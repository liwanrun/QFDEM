#include "fdemsolutionwidget.h"
#include "ui_fdemsolutionwidget.h"
#include "fdemsolutionitemdelegate.h"
#include "qttreepropertybrowser.h"

#include "blockcollection.h"
#include "actorcollection.h"
#include "material.h"
#include "materialcollection.h"
#include "solution.h"
#include "solutioncollection.h"

#include "mechanicalconstitutivemodel.h"
#include "contactconstitutivemodel.h"
#include "hydraulicconstitutivemodel.h"
#include "thermalconstitutivemodel.h"

#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QtVariantProperty>

FDEMSolutionWidget::FDEMSolutionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMSolutionWidget)
{
    ui->setupUi(this);
    this->solutionCollection = SolutionCollection::GetSolutionCollection();

    this->tableModel = new QStandardItemModel(this);
    ui->tableView->setModel(this->tableModel);
    this->delegate = new FDEMSolutionItemDelegate(this);
    ui->tableView->setItemDelegate(this->delegate);
    this->InitSolutionTableView();

    this->manager = new QtVariantPropertyManager(this);
    this->factory = new QtVariantEditorFactory(this);
    ui->propertyWidget->setFactoryForManager(this->manager, this->factory);
    this->InitPropertyBrowser();

    this->ConnectSignalSlots();
    this->SetButtonsEnabled(false);
}

FDEMSolutionWidget::~FDEMSolutionWidget()
{
    delete this->manager;
    delete this->factory;

    delete this->tableModel;
    delete this->delegate;

    delete ui;
}

void FDEMSolutionWidget::InitSolutionTableView()
{
    //Headers
    QFont font(QString("Arial"), -1, QFont::Bold);
    QStringList headers = QStringList() << "Solution Step" << "Description";
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
    ui->tableView->setColumnWidth(COL_STEP, 180);
    ui->tableView->setColumnWidth(COL_ROLE, 180);
}

void FDEMSolutionWidget::InitPropertyBrowser()
{
    this->CreateSolutionModelProperty();
    this->CreateSolutionTimestepProperty();
    this->CreateSolutionOptionProperty();

    ui->propertyWidget->setPropertiesWithoutValueMarked(true);
    ui->propertyWidget->setAlternatingRowColors(true);
    ui->propertyWidget->setHeaderVisible(false);
    ui->propertyGroup->setVisible(false);

    //for(QtBrowserItem *item : ui->propertyWidget->topLevelItems())
    //{ ui->propertyWidget->setBackgroundColor(item, QColor(255, 255, 191)); }
}

void FDEMSolutionWidget::UpdatePropertyBrowser()
{
    this->UpdateSolutionProperty(QString("I1MMODE"));
    this->UpdateSolutionProperty(QString("I1HMODE"));
    this->UpdateSolutionProperty(QString("I1TMODE"));

    this->UpdateSolutionProperty(QString("I1TSTEP"));
    this->UpdateSolutionProperty(QString("I1COUTF"));
    this->UpdateSolutionProperty(QString("D1MSTEC"));
    this->UpdateSolutionProperty(QString("D1HSTEC"));
    this->UpdateSolutionProperty(QString("D1TSTEC"));

    this->UpdateSolutionProperty(QString("I1CGRAV"));
    this->UpdateSolutionProperty(QString("D1CGRAX"));
    this->UpdateSolutionProperty(QString("D1CGRAY"));
    this->UpdateSolutionProperty(QString("I1CDAMP"));
    this->UpdateSolutionProperty(QString("D1CDAMP"));
    this->UpdateSolutionProperty(QString("I1CCRIT"));
    this->UpdateSolutionProperty(QString("D1CCRIT"));
    this->UpdateSolutionProperty(QString("I1CONTA"));
    this->UpdateSolutionProperty(QString("I1CHEFF"));
    this->UpdateSolutionProperty(QString("I1CSTEX"));

    QSharedPointer<SolutionCollection> solutionCollection = SolutionCollection::GetSolutionCollection();
    int hasMechanical = solutionCollection->GetSolutionModule() & MECHANICAL;
    int hasHydraulic = solutionCollection->GetSolutionModule() & HYDRAULIC;
    int hasThermal = solutionCollection->GetSolutionModule() & THERMAL;
    this->SetPropertyVisibility(QString("I1MMODE"), hasMechanical);
    this->SetPropertyVisibility(QString("D1MSTEC"), hasMechanical);
    this->SetPropertyVisibility(QString("I1HMODE"), hasHydraulic);
    this->SetPropertyVisibility(QString("D1HSTEC"), hasHydraulic);
    this->SetPropertyVisibility(QString("I1CHEFF"), hasHydraulic);
    this->SetPropertyVisibility(QString("I1TMODE"), hasThermal);
    this->SetPropertyVisibility(QString("D1TSTEC"), hasThermal);
}

void FDEMSolutionWidget::ConnectSignalSlots()
{
    connect(ui->appendButton, &QPushButton::clicked,
            this, &FDEMSolutionWidget::slotAppendButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked,
            this, &FDEMSolutionWidget::slotRemoveButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked,
            this, &FDEMSolutionWidget::slotClearButtonClicked);
    connect(ui->timestepButton, &QPushButton::clicked,
            this, &FDEMSolutionWidget::slotTimeMaximumStepButtonClicked);

    connect(this->tableModel, &QStandardItemModel::itemChanged,
            this, &FDEMSolutionWidget::slotTableModelItemChanged);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &FDEMSolutionWidget::slotSelectionChanged);

    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &FDEMSolutionWidget::slotPropertyManagerValueChanged);
}

void FDEMSolutionWidget::UpdateSolutionProperty(const QString &id)
{
    QtVariantProperty* property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    property->setValue(this->solution->propertyMap[id]);
}

void FDEMSolutionWidget::SetButtonsEnabled(bool arg)
{
    ui->removeButton->setEnabled(arg);
    ui->clearButton->setEnabled(arg);
    ui->timestepButton->setEnabled(arg);
}

QList<QStandardItem *> FDEMSolutionWidget::CreateSolutionItems(const QString &solutionId)
{
    QString path = QString(":/images/resources/");

    //Solution title
    QStandardItem* step = new QStandardItem(solutionId);
    step->setIcon(QIcon(path + QString("connect.png")));
    step->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    step->setEditable(false);

    //Solution action
    QStandardItem* role = new QStandardItem;
    role->setData(QString("initial step"), Qt::EditRole);
    role->setIcon(QIcon(path + QString("record.png")));
    role->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QList<QStandardItem *> items;
    items << step << role;
    return items;
}

void FDEMSolutionWidget::slotAppendButtonClicked()
{
    int index = this->tableModel->rowCount() + 1;
    QString stepId = QString("solution-%1").arg(index);

    this->solutionCollection->AppendSolution(index);
    this->tableModel->appendRow(this->CreateSolutionItems(stepId));

    this->SetButtonsEnabled(true);

    emit this->signalSolutionCollectionChanged(this->tableModel->rowCount());
}

void FDEMSolutionWidget::slotRemoveButtonClicked()
{
    int index = this->tableModel->rowCount();

    this->tableModel->removeRow(index - 1);
    this->solutionCollection->RemoveSolution(index);
    this->SetButtonsEnabled(this->tableModel->rowCount());

    ui->tableView->selectRow(this->tableModel->rowCount() - 1);
    ui->propertyGroup->setVisible(this->tableModel->rowCount());

    emit this->signalSolutionCollectionChanged(this->tableModel->rowCount());
}

void FDEMSolutionWidget::slotClearButtonClicked()
{
    this->tableModel->removeRows(0, this->tableModel->rowCount());
    this->solutionCollection->Clear();
    this->SetButtonsEnabled(false);
    ui->propertyGroup->setVisible(false);

    emit this->signalSolutionCollectionChanged(this->tableModel->rowCount());
}

void FDEMSolutionWidget::slotTableModelItemChanged(QStandardItem *)
{
    return;
}

void FDEMSolutionWidget::slotSelectionChanged(const QItemSelection &selection, const QItemSelection &)
{
    if(selection.isEmpty()) return;

    QString header = selection.indexes().at(COL_STEP).data().toString();
    int id = header.split(QChar('-')).last().toInt();
    this->solution = this->solutionCollection->GetSolution(id);
    this->UpdatePropertyBrowser();
    ui->propertyGroup->setVisible(true);
}

void FDEMSolutionWidget::slotPropertyManagerValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();
    this->solution->propertyMap[id] = value;

    if(QString("I1CGRAV") == id)
    {
        for(QtProperty* prop : property->subProperties())
        {
            for(QtBrowserItem *item : ui->propertyWidget->items(prop))
            { ui->propertyWidget->setItemVisible(item, value.toInt()); }
        }
    }
    else if(QString("I1CDAMP") == id)
    {
        for(QtProperty* prop : property->subProperties())
        {
            for(QtBrowserItem *item : ui->propertyWidget->items(prop))
            { ui->propertyWidget->setItemVisible(item, value.toInt()); }
        }
    }
    else if(QString("I1CCRIT") == id)
    {
        for(QtProperty* prop : property->subProperties())
        {
            for(QtBrowserItem *item : ui->propertyWidget->items(prop))
            { ui->propertyWidget->setItemVisible(item, value.toInt()); }
        }
    }
}

void FDEMSolutionWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    for(QtBrowserItem *item : ui->propertyWidget->items(property))
    { ui->propertyWidget->setItemVisible(item, visible); }
}

void FDEMSolutionWidget::SetPropertyExpanded(const QString &id, bool expanded)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    QtBrowserItem *item = ui->propertyWidget->items(property).first();
    ui->propertyWidget->setExpanded(item, expanded);
}

void FDEMSolutionWidget::CreateSolutionModelProperty()
{
    QStringList names;
    QtVariantProperty* property;

    QtProperty *root = manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Component"));
    ui->propertyWidget->addProperty(root);

    property = manager->addProperty(QtVariantPropertyManager::flagTypeId(), QString("Mechanical"));
    names = QStringList({"Finite Deformation", "Cohesive Fracture"});
    property->setAttribute(QString("flagNames"), names);
    property->setPropertyId(QString("I1MMODE"));
    root->addSubProperty(property);

    for(QtBrowserItem *item : ui->propertyWidget->items(property))
    { ui->propertyWidget->setBackgroundColor(item, QColor(255, 255, 191)); }

    property = manager->addProperty(QtVariantPropertyManager::flagTypeId(), QString("Hydraulic"));
    names = QStringList({"Porous Seepage", "Fracture Flow"});
    property->setAttribute(QString("flagNames"), names);
    property->setPropertyId(QString("I1HMODE"));
    root->addSubProperty(property);

    for(QtBrowserItem *item : ui->propertyWidget->items(property))
    { ui->propertyWidget->setBackgroundColor(item, QColor(191, 255, 255)); }

    property = manager->addProperty(QtVariantPropertyManager::flagTypeId(), QString("Thermal"));
    names = QStringList({"Solid Exchange", "Fluid Exchange"});
    property->setAttribute(QString("flagNames"), names);
    property->setPropertyId(QString("I1TMODE"));
    root->addSubProperty(property);

    for(QtBrowserItem *item : ui->propertyWidget->items(property))
    { ui->propertyWidget->setBackgroundColor(item, QColor(255, 230, 191)); }
}

void FDEMSolutionWidget::CreateSolutionTimestepProperty()
{
    QtVariantProperty* property;

    QtProperty *root = manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Timestep"));
    ui->propertyWidget->addProperty(root);

    property = manager->addProperty(QVariant::Int, QString("TotalTimestep"));
    property->setAttribute(QString("minimum"), 10);
    property->setPropertyId(QString("I1TSTEP"));
    root->addSubProperty(property);

    property = manager->addProperty(QVariant::Int, QString("OutputSpacing"));
    property->setAttribute(QString("minimum"), 0);
    property->setPropertyId(QString("I1COUTF"));
    root->addSubProperty(property);

    property = manager->addProperty(QVariant::Double, QString("TimestepSize (M)"));
    property->setAttribute(QString("decimals"), 10);
    property->setPropertyId(QString("D1MSTEC"));
    root->addSubProperty(property);

    property = manager->addProperty(QVariant::Double, QString("TimestepSize (H)"));
    property->setAttribute(QString("decimals"), 10);
    property->setPropertyId(QString("D1HSTEC"));
    root->addSubProperty(property);

    property = manager->addProperty(QVariant::Double, QString("TimestepSize (T)"));
    property->setAttribute(QString("decimals"), 10);
    property->setPropertyId(QString("D1TSTEC"));
    root->addSubProperty(property);

    for(QtBrowserItem *item : ui->propertyWidget->items(root))
    { ui->propertyWidget->setBackgroundColor(item, QColor(191, 250, 191)); }
}

void FDEMSolutionWidget::CreateSolutionOptionProperty()
{
    QtVariantProperty* property;
    QtVariantProperty* subProperty;

    QtProperty *root = manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Options"));
    ui->propertyWidget->addProperty(root);

    property = manager->addProperty(QVariant::Bool, QString("IsGravityEnabled"));
    property->setPropertyId(QString("I1CGRAV"));
    root->addSubProperty(property);

    subProperty = manager->addProperty(QVariant::Double, QString("gravX"));
    subProperty->setAttribute(QString("decimals"), 3);
    subProperty->setPropertyId(QString("D1CGRAX"));
    property->addSubProperty(subProperty);
    this->SetPropertyVisibility(QString("D1CGRAX"), false);

    subProperty = manager->addProperty(QVariant::Double, QString("gravY"));
    subProperty->setAttribute(QString("decimals"), 3);
    subProperty->setPropertyId(QString("D1CGRAY"));
    property->addSubProperty(subProperty);
    this->SetPropertyVisibility(QString("D1CGRAY"), false);

    property = manager->addProperty(QVariant::Bool, QString("IsLocalDampingEnabled"));
    property->setPropertyId(QString("I1CDAMP"));
    root->addSubProperty(property);

    subProperty = manager->addProperty(QVariant::Double, QString("localDamping"));
    subProperty->setAttribute(QString("decimals"), 3);
    subProperty->setPropertyId(QString("D1CDAMP"));
    property->addSubProperty(subProperty);
    this->SetPropertyVisibility(QString("D1CDAMP"), false);

    //property = manager->addProperty(QVariant::Bool, QString("*IsHystereticDampEnabled"));
    //property->setPropertyId(QString("I1HDAMP"));
    //root->addSubProperty(property);

    //subProperty = manager->addProperty(QVariant::Double, QString("hystereticDamp"));
    //subProperty->setAttribute(QString("decimals"), 3);
    //subProperty->setPropertyId(QString("D1HDAMP"));
    //property->addSubProperty(subProperty);
    //this->SetPropertyVisibility(QString("D1HDAMP"), false);

    property = manager->addProperty(QVariant::Bool, QString("IsConvergeCritEnabled"));
    property->setPropertyId(QString("I1CCRIT"));
    root->addSubProperty(property);

    subProperty = manager->addProperty(QVariant::Double, QString("convergeCrit"));
    subProperty->setAttribute(QString("decimals"), 6);
    subProperty->setPropertyId(QString("D1CCRIT"));
    property->addSubProperty(subProperty);
    this->SetPropertyVisibility(QString("D1CCRIT"), false);

    property = manager->addProperty(QVariant::Bool, QString("IsContactEnabled"));
    property->setPropertyId(QString("I1CONTA"));
    root->addSubProperty(property);

    property = manager->addProperty(QVariant::Bool, QString("IsPoroFlowEnabled"));
    property->setPropertyId(QString("I1CHEFF"));
    root->addSubProperty(property);

    property = manager->addProperty(QVariant::Bool, QString("IsAdaptiveEnabled"));
    property->setPropertyId(QString("I1CSTEX"));
    root->addSubProperty(property);

    for(QtBrowserItem *item : ui->propertyWidget->items(root))
    { ui->propertyWidget->setBackgroundColor(item, QColor(191, 250, 191)); }
}

double FDEMSolutionWidget::GetMechanicalFEMTimestep()
{
    QSharedPointer<BlockCollection> blockCollection = BlockCollection::GetBlockCollection();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();

    QSharedPointer<MaterialCollection> materialCollection = MaterialCollection::GetMaterialCollection();
    MaterialCollection::MaterialMap material_map = materialCollection->GetMaterialMap();

    double alpha = 1.0e+15;
    for(auto iter = material_map.begin(); iter != material_map.end(); ++iter)
    {
        if(Material::TRIANGLE == iter.value()->getMaterialType())
        {
            TriangleMaterial *material = static_cast<TriangleMaterial*>(iter.value().data());
            if(material->mechanicalModelType)
            {
                TriangleMechanicalModel *model = static_cast<TriangleMechanicalModel*>(material->mechanicalModel.data());
                alpha = fmin(alpha, material->density*(1.0 - model->poissonRatio*model->poissonRatio) / model->youngModulus);
            }
        }
    }

    double dampRatio = 1.0;
    double coeff = sqrt(1.0 + dampRatio*dampRatio) - dampRatio;
    double timestep = actorCollection->numQuads
                    ? 0.1*blockCollection->minimumMeshSize*sqrt(alpha)*coeff        //FDEM timestep
                    : 0.5*blockCollection->minimumMeshSize*sqrt(alpha)*coeff;       //EFEM timestep
    emit this->signalOutputPromptMessage(QString("[>>>] Mechanical FEM timestep:    [%1]").arg(timestep));

    return timestep;
}

double FDEMSolutionWidget::GetMechanicalDEMTimestep()
{
    QSharedPointer<BlockCollection> blockCollection = BlockCollection::GetBlockCollection();
    QSharedPointer<MaterialCollection> materialCollection = MaterialCollection::GetMaterialCollection();
    MaterialCollection::MaterialMap material_map = materialCollection->GetMaterialMap();

    double alpha = 1.0e+15;
    for(auto iter = material_map.begin(); iter != material_map.end(); ++iter)
    {
        if(Material::TRIANGLE == iter.value()->getMaterialType())
        {
            TriangleMaterial *material = static_cast<TriangleMaterial*>(iter.value().data());
            ContactConstitutiveModel *model = static_cast<ContactConstitutiveModel*>(material->mechanicalModel.data());
            alpha = fmin(alpha, (material->density*blockCollection->minimumMeshSize) / model->normalStiffness);
        }
    }

    double timestep = 0.2 * 3.1415926 * sqrt(alpha);
    emit this->signalOutputPromptMessage(QString("[>>>] Mechanical DEM timestep:    [%1]").arg(timestep));

    return timestep;
}

double FDEMSolutionWidget::GetHydraulicPorosityTimestep()
{
    QSharedPointer<BlockCollection> blockCollection = BlockCollection::GetBlockCollection();
    QSharedPointer<MaterialCollection> materialCollection = MaterialCollection::GetMaterialCollection();
    MaterialCollection::MaterialMap material_map = materialCollection->GetMaterialMap();

    double timestep = 1.0e+15;
    for(auto iter = material_map.begin(); iter != material_map.end(); ++iter)
    {
        double Kf = 1000.0;
        double mu = 0.001;
        if(Material::NTSFLUID == iter.value()->getMaterialType())
        {
            NTFluidMaterial *material = static_cast<NTFluidMaterial*>(iter.value().data());
            Kf = material->bulkModulus;
            mu = material->dynaViscosity;
        }

        if(Material::TRIANGLE == iter.value()->getMaterialType())
        {
            TriangleMaterial *material = static_cast<TriangleMaterial*>(iter.value().data());
            TriangleHydraulicModel *model = static_cast<TriangleHydraulicModel*>(material->hydraulicModel.data());

            double alpha = Kf*(model->biotCoefficient-model->porePorosity)*(1.0-model->biotCoefficient);
            double gamma = (Kf/(model->porePorosity + alpha))*((model->majorPermeability)/mu);
            timestep = fmin(timestep, (blockCollection->minimumMeshSize*blockCollection->minimumMeshSize)/(4.0*gamma));
        }
    }

    emit this->signalOutputPromptMessage(QString("[>>>] Porous seepage timestep:    [%1]").arg(timestep));
    return timestep;
}

double FDEMSolutionWidget::GetHydraulicFractureTimestep()
{
    QSharedPointer<BlockCollection> blockCollection = BlockCollection::GetBlockCollection();
    QSharedPointer<MaterialCollection> materialCollection = MaterialCollection::GetMaterialCollection();
    MaterialCollection::MaterialMap material_map = materialCollection->GetMaterialMap();

    double timestep = 1.0e+15;
    for(auto iter = material_map.begin(); iter != material_map.end(); ++iter)
    {
        double Kf = 1000.0;
        double mu = 0.001;
        if(Material::NTSFLUID == iter.value()->getMaterialType())
        {
            NTFluidMaterial *material = static_cast<NTFluidMaterial*>(iter.value().data());
            Kf = material->bulkModulus;
            mu = material->dynaViscosity;
        }

        if(Material::COHESIVE == iter.value()->getMaterialType())
        {
            CohesiveMaterial *material = static_cast<CohesiveMaterial*>(iter.value().data());
            CohesiveHydraulicModel *model = static_cast<CohesiveHydraulicModel*>(material->hydraulicModel.data());

            double alpha = blockCollection->minimumMeshSize/model->iniAperture;
            timestep = fmin(timestep, 1.0*(mu/Kf)*alpha*alpha);
        }
    }

    emit this->signalOutputPromptMessage(QString("[>>>] Fracture seepage timestep:    [%1]").arg(timestep));
    return timestep;
}

double FDEMSolutionWidget::GetThermalSolidPhaseTimestep()
{
    QSharedPointer<BlockCollection> blockCollection = BlockCollection::GetBlockCollection();
    QSharedPointer<MaterialCollection> materialCollection = MaterialCollection::GetMaterialCollection();
    MaterialCollection::MaterialMap material_map = materialCollection->GetMaterialMap();

    double timestep = 1.0e+15;
    for(auto iter = material_map.begin(); iter != material_map.end(); ++iter)
    {
        if(Material::TRIANGLE == iter.value()->getMaterialType())
        {
            TriangleMaterial *material = static_cast<TriangleMaterial*>(iter.value().data());
            TriangleThermalModel *model = static_cast<TriangleThermalModel*>(material->thermalModel.data());

            double alpha = (model->majorConductivity)/(material->density*model->heatCapacity);
            double gamma = (model->heatExchange*blockCollection->minimumMeshSize)/(2.0*model->majorConductivity);
            timestep = fmin(timestep, (blockCollection->minimumMeshSize*blockCollection->minimumMeshSize)/(4.0*alpha*(1.0+gamma)));
        }
    }

    emit this->signalOutputPromptMessage(QString("[>>>] Thermal solid timestep:    [%1]").arg(timestep));
    return timestep;
}

double FDEMSolutionWidget::GetThermalFluidPhaseTimestep()
{
    return 1.0E+10;
}

void FDEMSolutionWidget::slotTimeMaximumStepButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QSharedPointer<MaterialCollection> materialCollection = MaterialCollection::GetMaterialCollection();
    MaterialCollection::MaterialMap material_map = materialCollection->GetMaterialMap();
    if(material_map.isEmpty())
    {
        QMessageBox::warning(this, QString("Solution warning"),
                             QString("There is no materials at all. Check it out!"));
        return;
    }

    QSharedPointer<SolutionCollection> solutionCollection = SolutionCollection::GetSolutionCollection();
    SolutionCollection::SolutionMap solution_map = solutionCollection->GetSolutionMap();

    QString solutionId = selection.indexes().at(COL_STEP).data().toString();
    int stepId = solutionId.split(QChar('-')).last().toInt();
    QSharedPointer<Solution> solution = solution_map[stepId];

    if(MECHANICAL & solutionCollection->GetSolutionModule())
    {
        QString id = QString("D1MSTEC");
        double FEM_timestep = this->GetMechanicalFEMTimestep();
        double DEM_timestep = this->GetMechanicalDEMTimestep();
        solution->propertyMap[id] = fmin(FEM_timestep, DEM_timestep);
    }

    if(HYDRAULIC & solutionCollection->GetSolutionModule())
    {
        QString id = QString("D1HSTEC");
        double porosity_timestep = this->GetHydraulicPorosityTimestep();
        double fracture_timestep = this->GetHydraulicFractureTimestep();
        solution->propertyMap[id] = fmin(porosity_timestep, fracture_timestep);
    }

    if(THERMAL & solutionCollection->GetSolutionModule())
    {
        QString id = QString("D1TSTEC");
        double solid_timestep = this->GetThermalSolidPhaseTimestep();
        double fluid_timestep = this->GetThermalFluidPhaseTimestep();
        solution->propertyMap[id] = fmin(solid_timestep, fluid_timestep);
    }

    this->UpdatePropertyBrowser();
}
