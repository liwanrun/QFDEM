#include "fdemcohesivematerialwidget.h"
#include "ui_fdemcohesivematerialwidget.h"
#include "material.h"
#include "solution.h"
#include "solutioncollection.h"
#include "groupcollection.h"
#include "constitutivemodel.h"
#include "constitutivemodelwidget.h"
#include "constitutivemodelfactory.h"


#include <QDebug>
#include <QtVariantProperty>

FDEMCohesiveMaterialWidget::FDEMCohesiveMaterialWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMCohesiveMaterialWidget)
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

FDEMCohesiveMaterialWidget::~FDEMCohesiveMaterialWidget()
{
    delete ui;
}

void FDEMCohesiveMaterialWidget::InitPropertyBrowser()
{
    this->InitMaterialGlobalControlWidget();
    this->InitMechanicalConstitutiveWidget();
    this->InitHydraulicConstitutiveWidget();
    this->InitThermalConstitutiveWidget();

    ui->mechanicalGroup->setVisible(true);
    ui->hydraulicGroup->setVisible(false);
    ui->thermalGroup->setVisible(false);
}

void FDEMCohesiveMaterialWidget::UpdatePropertyBrowser(CohesiveMaterial *mat)
{
    this->material = mat;

    this->UpdateMaterialGlobalControlWidget();
    this->UpdateMechanicalConstitutiveWidget();
    this->UpdateHydraulicConstitutiveWidget();
    this->UpdateThermalConstitutiveWidget();
}

void FDEMCohesiveMaterialWidget::InitMaterialGlobalControlWidget()
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

    property = manager->addProperty(QVariant::Bool, QString("IsCohesiveBroken"));
    property->setPropertyId(QString("IsCohesiveBroken"));
    property->setValue(false);
    root->addSubProperty(property);

    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    { ui->treeBrowser->setBackgroundColor(item, QColor(191, 255, 191)); }

    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &FDEMCohesiveMaterialWidget::slotMaterialGlobalValueChanged);
}

void FDEMCohesiveMaterialWidget::InitMechanicalConstitutiveWidget()
{
    QStringList models;
    models << "Bilinearity Softening" << "Exponential Softening";
    ui->mechanicalModelBox->addItems(models);
    ui->mechanicalModelBox->setCurrentText(QString("Exponential Softening"));

    connect(ui->mechanicalModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMCohesiveMaterialWidget::slotMechanicalConstitutiveModelChanged);
}

void FDEMCohesiveMaterialWidget::InitHydraulicConstitutiveWidget()
{
    QStringList models;
    models << "Incompressible Flow" << "Compressible Flow";
    ui->hydraulicModelBox->addItems(models);
    ui->hydraulicModelBox->setCurrentText(QString("Incompressible Fluid"));

    connect(ui->hydraulicModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMCohesiveMaterialWidget::slotHydraulicConstitutiveModelChanged);
}

void FDEMCohesiveMaterialWidget::InitThermalConstitutiveWidget()
{
    QStringList models;
    models << "Heat Convection" ;
    ui->thermalModelBox->addItems(models);
    ui->thermalModelBox->setCurrentText(QString("Heat Convection"));

    connect(ui->thermalModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMCohesiveMaterialWidget::slotThermalConstitutiveModelChanged);
}

void FDEMCohesiveMaterialWidget::UpdateMaterialGlobalControlWidget()
{
    QtVariantProperty* property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("ElementSet")));
    property->setValue(material->elementSet);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("ElemStatus")));
    property->setValue(material->elemStatus);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("IsCohesiveBroken")));
    property->setValue(material->isCohesiveBroken);
}

void FDEMCohesiveMaterialWidget::UpdateMechanicalConstitutiveWidget()
{
    disconnect(ui->mechanicalModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMCohesiveMaterialWidget::slotMechanicalConstitutiveModelChanged);

    ui->mechanicalModelBox->setCurrentIndex(material->mechanicalModelType);
    this->mechanicalWidget = ConstitutiveModelWidgetFactory::CreateCohesiveModelWidget(material->mechanicalModelType);
    this->mechanicalWidget->CreateConstitutiveProperty(material->mechanicalModel.get());
    ui->mechanicalPropertyBrowser->layout()->addWidget(this->mechanicalWidget.get());

    connect(ui->mechanicalModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMCohesiveMaterialWidget::slotMechanicalConstitutiveModelChanged);
}

void FDEMCohesiveMaterialWidget::UpdateHydraulicConstitutiveWidget()
{
    disconnect(ui->hydraulicModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMCohesiveMaterialWidget::slotHydraulicConstitutiveModelChanged);

    ui->hydraulicModelBox->setCurrentIndex(material->hydraulicModelType);
    this->hydraulicWidget = ConstitutiveModelWidgetFactory::CreateFractureSeepageModelWidget(material->hydraulicModelType);
    this->hydraulicWidget->CreateConstitutiveProperty(material->hydraulicModel.get());
    ui->hydraulicPropertyBrowser->layout()->addWidget(this->hydraulicWidget.get());

    connect(ui->hydraulicModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMCohesiveMaterialWidget::slotHydraulicConstitutiveModelChanged);
}

void FDEMCohesiveMaterialWidget::UpdateThermalConstitutiveWidget()
{
    disconnect(ui->thermalModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMCohesiveMaterialWidget::slotThermalConstitutiveModelChanged);

    ui->thermalModelBox->setCurrentIndex(material->thermalModelType);
    this->thermalWidget = ConstitutiveModelWidgetFactory::CreateThermalTransferModelWidget(material->thermalModelType);
    this->thermalWidget->CreateConstitutiveProperty(material->thermalModel.get());
    ui->thermalPropertyBrowser->layout()->addWidget(this->thermalWidget.get());

    connect(ui->thermalModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FDEMCohesiveMaterialWidget::slotThermalConstitutiveModelChanged);
}

void FDEMCohesiveMaterialWidget::slotColumnSolutionChanged(const QString id)
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
//      int tmode = properties[QString("I1TMODE")].toInt();

        material->isMechanicalAnalysisEnabled = (cmode & MECHANICAL) && (mmode & Solution::CZMFRACTURE);
        ui->mechanicalGroup->setHidden(!material->isMechanicalAnalysisEnabled);
        material->isHydraulicAnalysisEnabled = (cmode & HYDRAULIC) && (hmode & Solution::FRACTUREFLOW);
        ui->hydraulicGroup->setHidden(!material->isHydraulicAnalysisEnabled);
//      material->isThermalAnalysisEnabled = (cmode & THERMAL) && (tmode & Solution::FLUIDHEATEX);
        material->isThermalAnalysisEnabled = (cmode & THERMAL);
        ui->thermalGroup->setHidden(!material->isThermalAnalysisEnabled);
    }
}

void FDEMCohesiveMaterialWidget::slotMaterialGlobalValueChanged(QtProperty *property, const QVariant &val)
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
    else if(id == QString("IsCohesiveBroken"))
    {
        this->material->isCohesiveBroken = val.toInt();
    }
}

void FDEMCohesiveMaterialWidget::slotMechanicalConstitutiveModelChanged(int index)
{
    this->material->mechanicalModelType = index;
    this->material->mechanicalModel = ConstitutiveModelFactory::CreateCohesiveModel(index);
    this->mechanicalWidget = ConstitutiveModelWidgetFactory::CreateCohesiveModelWidget(index);
    this->mechanicalWidget->CreateConstitutiveProperty(this->material->mechanicalModel.get());
    ui->mechanicalPropertyBrowser->layout()->addWidget(this->mechanicalWidget.get());
}

void FDEMCohesiveMaterialWidget::slotHydraulicConstitutiveModelChanged(int index)
{
    this->material->hydraulicModelType = index;
    this->material->hydraulicModel = ConstitutiveModelFactory::CreateFractureSeepageModel(index);
    this->hydraulicWidget = ConstitutiveModelWidgetFactory::CreateFractureSeepageModelWidget(index);
    this->hydraulicWidget->CreateConstitutiveProperty(this->material->hydraulicModel.get());
    ui->hydraulicPropertyBrowser->layout()->addWidget(this->hydraulicWidget.get());
}

void FDEMCohesiveMaterialWidget::slotThermalConstitutiveModelChanged(int index)
{
    this->material->thermalModelType = index;
    this->material->hydraulicModel = ConstitutiveModelFactory::CreateThermalTransferModel(index);
    this->thermalWidget = ConstitutiveModelWidgetFactory::CreateThermalTransferModelWidget(index);
    this->thermalWidget->CreateConstitutiveProperty(this->material->thermalModel.get());
    ui->thermalPropertyBrowser->layout()->addWidget(this->thermalWidget.get());
}
