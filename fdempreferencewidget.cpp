
#include "fdempreferencewidget.h"
#include "ui_fdempreferencewidget.h"
#include "solutioncollection.h"
#include <QtVariantProperty>
#include <QDebug>

FDEMPreferenceWidget::FDEMPreferenceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMPreferenceWidget)
{
    ui->setupUi(this);
    this->solutionCollection = SolutionCollection::GetSolutionCollection();

    this->manager = new QtVariantPropertyManager(this);
    this->factory = new QtVariantEditorFactory(this);
    ui->treeBrowser->setFactoryForManager(this->manager, this->factory);

    this->InitPropertyBrowser();
    this->ConnectSignalSlots();
}

FDEMPreferenceWidget::~FDEMPreferenceWidget()
{
    delete this->manager;
    delete this->factory;
    delete ui;
}

void FDEMPreferenceWidget::InitPropertyBrowser()
{
    QStringList names;
    QtVariantProperty *property;

    QtVariantProperty* root = manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Preference"));
    property = manager->addProperty(QtVariantPropertyManager::flagTypeId(), QString("Physical Module"));

    names = QStringList({"Mechanical", "Hydraulic", "Thermal"});
    property->setAttribute(QString("flagNames"), names);
    property->setPropertyId(QString("ICMODE"));
    property->setValue(solutionCollection->solution_module);
    root->addSubProperty(property);
    ui->treeBrowser->addProperty(root);

    this->InitMechanicalProperty();
    this->InitHydraulicProperty();
    this->InitThermalProperty();
    this->InitFieldCoupleProperty();
}

void FDEMPreferenceWidget::ConnectSignalSlots()
{
    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &FDEMPreferenceWidget::slotPropertyManagerValueChanged);
}

void FDEMPreferenceWidget::InitMechanicalProperty()
{
    QStringList names;
    QtVariantProperty *property;

    QtProperty *mechGroup = manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Mechancial Option"));
    mechGroup->setPropertyId(QString("MechanicalOption"));
    ui->treeBrowser->addProperty(mechGroup);

    property = manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("GeostressSolution"));
    names = QStringList({"Contain-CZM", "Without-CZM"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICMEDO"));
    property->setValue(solutionCollection->triMechSolution);
    mechGroup->addSubProperty(property);

    property = manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("CZMInsertSolution"));
    names = QStringList({"Intrinsic-CZM", "Extrinsic-CZM"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICMJDO"));
    property->setValue(solutionCollection->cohMechSolution);
    mechGroup->addSubProperty(property);

    for(QtBrowserItem *item : ui->treeBrowser->items(mechGroup))
    { ui->treeBrowser->setBackgroundColor(item, QColor(255, 255, 191)); }
    this->SetPropertyVisibility(QString("MechanicalOption"), true);
}

void FDEMPreferenceWidget::InitHydraulicProperty()
{
    QStringList names;
    QtVariantProperty *property, *solutionProp;

    QtProperty *hydroGroup = manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Hydraulic Option"));
    hydroGroup->setPropertyId(QString("HydraulicOption"));
    ui->treeBrowser->addProperty(hydroGroup);

    solutionProp= manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("HydraulicSolution"));
    names = QStringList({"Partitioned approach", "Monolithic approach"});
    solutionProp->setAttribute(QString("enumNames"), names);
    solutionProp->setPropertyId(QString("ICHWAY"));
    solutionProp->setValue(solutionCollection->hydraulicSolution);
    hydroGroup->addSubProperty(solutionProp);

    property= manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("MonolithicScheme"));
    names = QStringList({"Explicit-shared", "Implicit-shared", "Explicit-channel", "Implicit-channel", "Implicit-discrete"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICHMOD"));
    property->setValue(solutionCollection->monolithicSchemes);
    hydroGroup->addSubProperty(property);
    this->SetPropertyVisibility(QString("ICHMOD"), false);

    property= manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("LinearSparseSolver"));
    names = QStringList({"Skyline solver", "Pardiso solver", "SuperLU solver", "DiagPcg solver",
                         "Direct Sparse Solver (DSS)", "Iterative Sparse Solver (ISS)"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICHLSS"));
    property->setValue(solutionCollection->linearSparseSolver);
    hydroGroup->addSubProperty(property);
    this->SetPropertyVisibility(QString("ICHLSS"), false);

    property= manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("PorousAssumption"));
    names = QStringList({"Continuous field", "Discontinuous field", "Progressive field"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICHEIF"));
    property->setValue(solutionCollection->porosityAssumption);
    hydroGroup->addSubProperty(property);

    property= manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("PorousFlowModel"));
    names = QStringList({"Pipe-based flow", "Cell-based flow"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICHECM"));
    property->setValue(solutionCollection->porosityFlowModel);
    hydroGroup->addSubProperty(property);

    property= manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("PorousIntegration"));
    names = QStringList({"Explicit method", "Implicit method"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICHEDO"));
    property->setValue(solutionCollection->porosityIntegration);
    hydroGroup->addSubProperty(property);

    property= manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("FracrtureAssumption"));
    names = QStringList({"Incompressible fluid", "Compressible fluid"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICHJIF"));
    property->setValue(solutionCollection->fractureAssumption);
    hydroGroup->addSubProperty(property);

    property= manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("FracrtureFlowModel"));
    names = QStringList({"Uniform", "Network", "Lerp", "Power"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICHJCM"));
    property->setValue(solutionCollection->fractureFlowModel);
    hydroGroup->addSubProperty(property);

    property= manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("FracrtureIntegration"));
    names = QStringList({"Explicit-Euler", "Implicit-Euler", "Explicit-Exact", "Explicit-Frogs"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICHJDO"));
    property->setValue(solutionCollection->fractureIntegration);
    hydroGroup->addSubProperty(property);

    for(QtBrowserItem *item : ui->treeBrowser->items(hydroGroup))
    { ui->treeBrowser->setBackgroundColor(item, QColor(191, 255, 255)); }
    this->SetPropertyVisibility(QString("HydraulicOption"), false);
}

void FDEMPreferenceWidget::InitThermalProperty()
{
    QStringList names;
    QtVariantProperty *property;

    QtProperty *thermalGroup = manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Thermal Option"));
    thermalGroup->setPropertyId(QString("ThermalOption"));
    ui->treeBrowser->addProperty(thermalGroup);

    property= manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("SolidConductStrategy"));
    names = QStringList({"Continuous", "Discontinuous", "Progressive"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICTEIF"));
    property->setValue(solutionCollection->triThermStrategy);
    thermalGroup->addSubProperty(property);

    property= manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("SolidConductSolution"));
    names = QStringList({"UPM Scheme", "FEM Scheme"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICTEDO"));
    property->setValue(solutionCollection->triThermSolution);
    thermalGroup->addSubProperty(property);

    property= manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("FluidConductStrategy"));
    names = QStringList({"Continuous"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICTJIF"));
    property->setValue(solutionCollection->cohThermStrategy);
    thermalGroup->addSubProperty(property);

    property= manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("FluidConductSolution"));
    names = QStringList({"FEM Scheme"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICTJDO"));
    property->setValue(solutionCollection->cohThermSolution);
    thermalGroup->addSubProperty(property);

    for(QtBrowserItem *item : ui->treeBrowser->items(thermalGroup))
    { ui->treeBrowser->setBackgroundColor(item, QColor(255, 230, 191)); }
    this->SetPropertyVisibility(QString("ThermalOption"), false);

    ui->treeBrowser->setPropertiesWithoutValueMarked(true);
    ui->treeBrowser->setAlternatingRowColors(true);
    ui->treeBrowser->setHeaderVisible(false);
}

void FDEMPreferenceWidget::InitFieldCoupleProperty()
{
    QStringList names;
    QtVariantProperty *property;

    QtProperty *couplingGroup = manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Coupling Option"));
    couplingGroup->setPropertyId(QString("CouplingOption"));
    ui->treeBrowser->addProperty(couplingGroup);

    property = manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("CouplingStrategy"));
    names = QStringList({"One-way", "Two-way"});
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ICCOUP"));
    property->setValue(solutionCollection->couplingStrategy);
    couplingGroup->addSubProperty(property);

    property = manager->addProperty(QVariant::Int, QString("MaximumIters"));
    property->setAttribute(QString("minimum"), 0);
    property->setPropertyId(QString("NITERS"));
    property->setValue(solutionCollection->maxNumIterations);
    property->setEnabled(false);
    couplingGroup->addSubProperty(property);

    property = manager->addProperty(QVariant::Double, QString("MechanicalTolerance"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 5);
    property->setPropertyId(QString("DCMTOL"));
    property->setValue(solutionCollection->absMechTolerance);
    property->setEnabled(false);
    couplingGroup->addSubProperty(property);

    property = manager->addProperty(QVariant::Double, QString("HydraulicTolerance"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 5);
    property->setPropertyId(QString("DCHTOL"));
    property->setValue(solutionCollection->absHydroTolerance);
    property->setEnabled(false);
    couplingGroup->addSubProperty(property);

    property = manager->addProperty(QVariant::Double, QString("ThermalTolerance"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 5);
    property->setPropertyId(QString("DCTTOL"));
    property->setValue(solutionCollection->absThermTolerance);
    property->setEnabled(false);
    couplingGroup->addSubProperty(property);

    for(QtBrowserItem *item : ui->treeBrowser->items(couplingGroup))
    { ui->treeBrowser->setBackgroundColor(item, QColor(234, 191, 255)); }
    this->SetPropertyVisibility(QString("CouplingOption"), false);
}

void FDEMPreferenceWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    { ui->treeBrowser->setItemVisible(item, visible); }
}

void FDEMPreferenceWidget::SetPropertyEnabled(const QString &id, bool enabled)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    property->setEnabled(enabled);
}

void FDEMPreferenceWidget::slotPropertyManagerValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("ICMODE"))
    {
        this->solutionCollection->SetSolutionModule(value.toInt());
        this->SetPropertyVisibility(QString("MechanicalOption"), (value.toInt() & MECHANICAL));
        this->SetPropertyVisibility(QString("HydraulicOption"), (value.toInt() & HYDRAULIC));
        this->SetPropertyVisibility(QString("ThermalOption"), (value.toInt() & THERMAL));

        //coupling options
        bool is_decoupling = (value.toInt() == 0) || (value.toInt() == MECHANICAL) ||
                             (value.toInt() == HYDRAULIC) ||(value.toInt() == THERMAL);
        this->SetPropertyVisibility(QString("CouplingOption"), !is_decoupling);
    }
    // Mechanical options
    else if(id == QString("ICMEDO"))
    {
        this->solutionCollection->triMechSolution = value.toInt();
    }
    else if(id == QString("ICMJDO"))
    {
        this->solutionCollection->cohMechSolution = value.toInt();
    }
    // Hydraulic options
    else if(id == QString("ICHWAY"))
    {
        this->solutionCollection->hydraulicSolution = value.toInt();
        this->SetPropertyVisibility(QString("ICHMOD"),  value.toInt());
        this->SetPropertyVisibility(QString("ICHLSS"),  value.toInt());
        this->SetPropertyVisibility(QString("ICHEIF"), !value.toInt());
        this->SetPropertyVisibility(QString("ICHEDO"), !value.toInt());
        this->SetPropertyVisibility(QString("ICHJIF"), !value.toInt());
        this->SetPropertyVisibility(QString("ICHJDO"), !value.toInt());
    }
    else if(id == QString("ICHMOD"))
    {
        this->solutionCollection->monolithicSchemes = value.toInt();
        QtVariantProperty *prop = static_cast<QtVariantProperty*>(manager->qtProperty(id));
        QString item = prop->attributeValue(QString("enumNames")).toStringList()[value.toInt()];
        this->SetPropertyEnabled(QString("ICHLSS"), item.startsWith(QLatin1String("Implicit-")));
    }
    else if(id == QString("ICHLSS"))
    {
        this->solutionCollection->linearSparseSolver = value.toInt();
    }
    else if(id == QString("ICHEIF"))
    {
        this->solutionCollection->porosityAssumption = value.toInt();
    }
    else if(id == QString("ICHECM"))
    {
        this->solutionCollection->porosityFlowModel = value.toInt();
    }
    else if(id == QString("ICHEDO"))
    {
        this->solutionCollection->porosityIntegration = value.toInt();
    }
    else if(id == QString("ICHJIF"))
    {
        this->solutionCollection->fractureAssumption = value.toInt();
        this->SetPropertyEnabled(QString("ICHJDO"), !value.toInt());
    }
    else if(id == QString("ICHJCM"))
    {
        this->solutionCollection->fractureFlowModel = value.toInt();
    }
    else if(id == QString("ICHJDO"))
    {
        this->solutionCollection->fractureIntegration = value.toInt();
    }
    // Thermal options
    else if(id == QString("ICTEIF"))
    {
        this->solutionCollection->triThermStrategy = value.toInt();
    }
    else if(id == QString("ICTEDO"))
    {
        this->solutionCollection->triThermSolution = value.toInt();
    }
    else if(id == QString("ICTJIF"))
    {
        this->solutionCollection->cohThermStrategy = value.toInt();
    }
    else if(id == QString("ICTJDO"))
    {
        this->solutionCollection->cohThermSolution = value.toInt();
    }
    // Coupling options
    else if(id == QString("ICCOUP"))
    {
        this->solutionCollection->couplingStrategy = value.toInt();
        this->SetPropertyEnabled(QString("NITERS"), value.toInt());
        this->SetPropertyEnabled(QString("DCMTOL"), value.toInt());
        this->SetPropertyEnabled(QString("DCHTOL"), value.toInt());
        this->SetPropertyEnabled(QString("DCTTOL"), value.toInt());
    }
    else if(id == QString("NITERS"))
    {
        this->solutionCollection->maxNumIterations = value.toInt();
    }
    else if(id == QString("DCMTOL"))
    {
        this->solutionCollection->absMechTolerance = value.toDouble();
    }
    else if(id == QString("DCHTOL"))
    {
        this->solutionCollection->absHydroTolerance = value.toDouble();
    }
    else if(id == QString("DCTTOL"))
    {
        this->solutionCollection->absThermTolerance = value.toDouble();
    }
}
