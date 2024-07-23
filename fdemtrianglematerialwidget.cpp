#include "fdemtrianglematerialwidget.h"
#include "ui_fdemtrianglematerialwidget.h"
#include "material.h"
#include "solution.h"
#include "solutioncollection.h"
#include "constitutivemodel.h"
#include "constitutivemodelwidget.h"
#include "constitutivemodelfactory.h"
#include "group.h"
#include "groupcollection.h"

#include <QDebug>
#include <QtVariantProperty>
#include <QMessageBox>

FDEMTriangleMaterialWidget::FDEMTriangleMaterialWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMTriangleMaterialWidget)
{
    ui->setupUi(this);

    this->factory = new QtVariantEditorFactory;
    this->manager = new QtVariantPropertyManager;
    ui->treeBrowser->setFactoryForManager(this->manager, this->factory);
    ui->treeBrowser->setPropertiesWithoutValueMarked(true);
    ui->treeBrowser->setAlternatingRowColors(true);
    ui->treeBrowser->setHeaderVisible(false);

    this->InitPropertyBrowser();
}

FDEMTriangleMaterialWidget::~FDEMTriangleMaterialWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void FDEMTriangleMaterialWidget::InitPropertyBrowser()
{
    this->InitMaterialGlobalControlWidget();
    this->InitMechanicalConstitutiveWidget();
    this->InitContactConstitutiveWidget();
    this->InitHydraulicConstitutiveWidget();
    this->InitThermalConstitutiveWidget();

    ui->mechanicalGroup->setVisible(true);
    ui->contactGroup->setVisible(true);
    ui->hydraulicGroup->setVisible(false);
    ui->thermalGroup->setVisible(false);
}

void FDEMTriangleMaterialWidget::UpdatePropertyBrowser(TriangleMaterial *mat)
{
    this->material = mat;

    this->UpateMaterialGlobalControlWidget();
    this->UpdateMechanicalConstitutiveWidget();
    this->UpdateContactConstitutiveWidget();
    this->UpdateHydraulicConstitutiveWidget();
    this->UpdateThermalConstitutiveWidget();
}

void FDEMTriangleMaterialWidget::slotColumnSolutionChanged(const QString id)
{
    int index = id.split(QChar('-')).last().toInt();
    this->material->enableStep = index;

    QSharedPointer<SolutionCollection> solutionCollection = SolutionCollection::GetSolutionCollection();
    SolutionCollection::SolutionMap solution_map = solutionCollection->GetSolutionMap();
    if(solution_map.contains(index))
    {
        QMap<QString, QVariant> properties = solution_map[index]->propertyMap;
        int cmode = solutionCollection->GetSolutionModule();
        int mmode = properties[QString("I1MMODE")].toInt();
        int hmode = properties[QString("I1HMODE")].toInt();
        int tmode = properties[QString("I1TMODE")].toInt();

        ui->contactGroup->setHidden(!(cmode & MECHANICAL));
        material->isMechanicalAnalysisEnabled = (cmode & MECHANICAL) && (mmode & Solution::DEFORMATION);
        ui->mechanicalGroup->setHidden(!material->isMechanicalAnalysisEnabled);
        material->isHydraulicAnalysisEnabled = (cmode & HYDRAULIC) && (hmode & Solution::POROSITYSEEP);
        ui->hydraulicGroup->setHidden(!material->isHydraulicAnalysisEnabled);
        material->isThermalAnalysisEnabled = (cmode & THERMAL) && (tmode & Solution::SOLIDHEATEX);
        ui->thermalGroup->setHidden(!material->isThermalAnalysisEnabled);
    }
}

void FDEMTriangleMaterialWidget::InitMaterialGlobalControlWidget()
{
    QtVariantProperty* property;

    QtProperty *root = manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Control Information"));
    ui->treeBrowser->addProperty(root);

    property = manager->addProperty(QVariant::String, QString("ElementSet"));
    property->setToolTip(QString("Multiple input is allowed (eg. 1,2,3,...)"));
    property->setPropertyId(QString("ElementSet"));
    property->setValue(QString::number(1));
    root->addSubProperty(property);

    property = manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("ElemStatus"));
    QStringList names = { "Dead", "Alive" };
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ElemStatus"));
    property->setValue(1);
    root->addSubProperty(property);

    property = manager->addProperty(QVariant::Double, QString("Density [") + QString(0X03C1) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("Density"));
    property->setValue(2700.0);
    root->addSubProperty(property);

    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    { ui->treeBrowser->setBackgroundColor(item, QColor(191, 255, 191)); }

    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &FDEMTriangleMaterialWidget::slotMaterialGlobalValueChanged);
}

void FDEMTriangleMaterialWidget::InitMechanicalConstitutiveWidget()
{
    QStringList models;
    models << "Rigid Body" << "Hookean Elasticity" << "Anisotropic Elasticity"
           << "Elasto-Plasticity" << "Plane Stress" << "Plane Strain";
    ui->mechanicalModelBox->addItems(models);
    ui->mechanicalModelBox->setCurrentText(QString("Hookean Elasticity"));

    connect(ui->mechanicalModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMTriangleMaterialWidget::slotMechanicalConstitutiveModelChanged);
}

void FDEMTriangleMaterialWidget::InitContactConstitutiveWidget()
{
    QStringList models;
    models << "Complete Elasticity" << "Plastic Dissipation";
    ui->contactModelBox->addItems(models);
    ui->contactModelBox->setCurrentText(QString("Complete Elasticity"));

    connect(ui->contactModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMTriangleMaterialWidget::slotContactConstitutiveModelChanged);
}

void FDEMTriangleMaterialWidget::InitHydraulicConstitutiveWidget()
{
    QStringList models;
    models << "Isotropic Permeability" << "Anisotropic Permeability";
    ui->hydraulicModelBox->addItems(models);
    ui->hydraulicModelBox->setCurrentText(QString("Isotropic Permeability"));

    connect(ui->hydraulicModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMTriangleMaterialWidget::slotHydraulicConstitutiveModelChanged);
}

void FDEMTriangleMaterialWidget::InitThermalConstitutiveWidget()
{
    QStringList models;
    models << "Isotropic Conductivity" << "Anisotropic Conductivity";
    ui->thermalModelBox->addItems(models);
    ui->thermalModelBox->setCurrentText(QString("Isotropic Conductivity"));

    connect(ui->thermalModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMTriangleMaterialWidget::slotThermalConstitutiveModelChanged);
}

void FDEMTriangleMaterialWidget::UpateMaterialGlobalControlWidget()
{
    QtVariantProperty* property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("ElementSet")));
    property->setValue(material->elementSet);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("ElemStatus")));
    property->setValue(material->elemStatus);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("Density")));
    property->setValue(material->density);
}

void FDEMTriangleMaterialWidget::UpdateMechanicalConstitutiveWidget()
{
    disconnect(ui->mechanicalModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMTriangleMaterialWidget::slotMechanicalConstitutiveModelChanged);

    ui->mechanicalModelBox->setCurrentIndex(material->mechanicalModelType);
    this->mechanicalWidget = ConstitutiveModelWidgetFactory::CreateMaterialModelWidget(material->mechanicalModelType);
    this->mechanicalWidget->CreateConstitutiveProperty(material->mechanicalModel.get());
    ui->mechanicalPropertyBrowser->layout()->addWidget(this->mechanicalWidget.get());

    connect(ui->mechanicalModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMTriangleMaterialWidget::slotMechanicalConstitutiveModelChanged);
}

void FDEMTriangleMaterialWidget::UpdateContactConstitutiveWidget()
{
    disconnect(ui->contactModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMTriangleMaterialWidget::slotContactConstitutiveModelChanged);

    ui->contactModelBox->setCurrentIndex(material->contactModelType);
    this->contactWidget = ConstitutiveModelWidgetFactory::CreateContactModelWidget(material->contactModelType);
    this->contactWidget->CreateConstitutiveProperty(material->contactModel.get());
    ui->contactPropertyBrowser->layout()->addWidget(this->contactWidget.get());

    connect(ui->contactModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMTriangleMaterialWidget::slotContactConstitutiveModelChanged);
}

void FDEMTriangleMaterialWidget::UpdateHydraulicConstitutiveWidget()
{
    disconnect(ui->hydraulicModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMTriangleMaterialWidget::slotHydraulicConstitutiveModelChanged);

    ui->hydraulicModelBox->setCurrentIndex(material->hydraulicModelType);
    this->hydraulicWidget = ConstitutiveModelWidgetFactory::CreatePorositySeepageModelWidget(material->hydraulicModelType);
    this->hydraulicWidget->CreateConstitutiveProperty(material->hydraulicModel.get());
    ui->hydraulicPropertyBrowser->layout()->addWidget(this->hydraulicWidget.get());

    connect(ui->hydraulicModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMTriangleMaterialWidget::slotHydraulicConstitutiveModelChanged);
}

void FDEMTriangleMaterialWidget::UpdateThermalConstitutiveWidget()
{
    disconnect(ui->thermalModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMTriangleMaterialWidget::slotThermalConstitutiveModelChanged);

    ui->thermalModelBox->setCurrentIndex(material->thermalModelType);
    this->thermalWidget = ConstitutiveModelWidgetFactory::CreateThermalConductModelWidget(material->thermalModelType);
    this->thermalWidget->CreateConstitutiveProperty(material->thermalModel.get());
    ui->thermalPropertyBrowser->layout()->addWidget(this->thermalWidget.get());

    connect(ui->thermalModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMTriangleMaterialWidget::slotThermalConstitutiveModelChanged);
}

void FDEMTriangleMaterialWidget::slotMaterialGlobalValueChanged(QtProperty *property, const QVariant &val)
{
    QString id = property->propertyId();
    if(id == QString("ElementSet"))
    {
        this->material->elementSet = val.toString();
    }
    else if(id == QString("ElemStatus"))
    {
        this->material->elemStatus = val.toInt();
    }
    else if(id == QString("Density"))
    {
        this->material->density = val.toDouble();
    }
}

void FDEMTriangleMaterialWidget::slotMechanicalConstitutiveModelChanged(int index)
{
    this->material->mechanicalModelType = index;
    this->material->mechanicalModel = ConstitutiveModelFactory::CreateMaterialModel(index);
    this->mechanicalWidget = ConstitutiveModelWidgetFactory::CreateMaterialModelWidget(index);
    this->mechanicalWidget->CreateConstitutiveProperty(this->material->mechanicalModel.get());
    ui->mechanicalPropertyBrowser->layout()->addWidget(this->mechanicalWidget.get());
}

void FDEMTriangleMaterialWidget::slotContactConstitutiveModelChanged(int index)
{
    this->material->contactModelType = index;
    this->material->contactModel = ConstitutiveModelFactory::CreateContactModel(index);
    this->contactWidget = ConstitutiveModelWidgetFactory::CreateContactModelWidget(index);
    this->contactWidget->CreateConstitutiveProperty(this->material->contactModel.get());
    ui->contactPropertyBrowser->layout()->addWidget(this->contactWidget.get());
}

void FDEMTriangleMaterialWidget::slotHydraulicConstitutiveModelChanged(int index)
{
    this->material->hydraulicModelType = index;
    this->material->hydraulicModel = ConstitutiveModelFactory::CreatePorositySeepageModel(index);
    this->hydraulicWidget = ConstitutiveModelWidgetFactory::CreatePorositySeepageModelWidget(index);
    this->hydraulicWidget->CreateConstitutiveProperty(this->material->hydraulicModel.get());
    ui->hydraulicPropertyBrowser->layout()->addWidget(this->hydraulicWidget.get());
}

void FDEMTriangleMaterialWidget::slotThermalConstitutiveModelChanged(int index)
{
    this->material->thermalModelType = index;
    this->material->thermalModel = ConstitutiveModelFactory::CreateThermalConductModel(index);
    this->thermalWidget = ConstitutiveModelWidgetFactory::CreateThermalConductModelWidget(index);
    this->thermalWidget->CreateConstitutiveProperty(this->material->thermalModel.get());
    ui->thermalPropertyBrowser->layout()->addWidget(this->thermalWidget.get());
}
