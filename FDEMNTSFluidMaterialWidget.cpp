#include "fdemntsfluidmaterialwidget.h"
#include "ui_fdemntsfluidmaterialwidget.h"
#include "material.h"
#include "solution.h"
#include "solutioncollection.h"

#include <QDebug>
#include <QtVariantProperty>

FDEMNTSFluidMaterialWidget::FDEMNTSFluidMaterialWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMNTSFluidMaterialWidget)
{
    ui->setupUi(this);

    this->manager = new QtVariantPropertyManager(this);
    this->factory = new QtVariantEditorFactory(this);
    ui->treeBrowser->setFactoryForManager(this->manager, this->factory);

    ui->treeBrowser->setPropertiesWithoutValueMarked(true);
    ui->treeBrowser->setAlternatingRowColors(true);
    ui->treeBrowser->setHeaderVisible(false);

    this->InitPropertyBrowser();
    this->ConnectSignalSlots();
}

FDEMNTSFluidMaterialWidget::~FDEMNTSFluidMaterialWidget()
{
    delete ui;
}

void FDEMNTSFluidMaterialWidget::InitPropertyBrowser()
{
    this->CreateNewtonianFluidProperty();
}

void FDEMNTSFluidMaterialWidget::ConnectSignalSlots()
{
    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &FDEMNTSFluidMaterialWidget::slotTreeBrowserValueChanged);
}

void FDEMNTSFluidMaterialWidget::UpdatePropertyBrowser(NTFluidMaterial *mat)
{
    this->material = mat;
    this->UpdateNewtonianFluidProperty();

    QSharedPointer<SolutionCollection> solutionCollection = SolutionCollection::GetSolutionCollection();
    this->SetPropertyVisibility(QString("ThermalProperty"), (solutionCollection->GetSolutionModule() & THERMAL));
}

void FDEMNTSFluidMaterialWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    { ui->treeBrowser->setItemVisible(item, visible); }
}

void FDEMNTSFluidMaterialWidget::SetBackgroundColor(const QColor &color)
{
    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    {
        ui->treeBrowser->setBackgroundColor(item, color);
    }
}

void FDEMNTSFluidMaterialWidget::CreateNewtonianFluidProperty()
{
    QtVariantProperty* property;

    QtProperty *physicalGroup = manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Physical Property"));
    physicalGroup->setPropertyId(QString("PhysicalProperty"));
    ui->treeBrowser->addProperty(physicalGroup);

    property = manager->addProperty(QVariant::Double, QString("Density [") + QString(0X03C1) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("Density"));
    physicalGroup->addSubProperty(property);

    property = manager->addProperty(QVariant::String, QString("BulkModulus [K]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("BulkModulus"));
    physicalGroup->addSubProperty(property);

    property = manager->addProperty(QVariant::Double, QString("BulkViscosity [") + QString(0X03B6) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 6);
    property->setPropertyId(QString("BulkViscosity"));
    physicalGroup->addSubProperty(property);

    property = manager->addProperty(QVariant::Double, QString("DynaViscosity [") + QString(0X03B7) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 6);
    property->setPropertyId(QString("DynaViscosity"));
    physicalGroup->addSubProperty(property);

    this->SetPropertyVisibility(QString("PhysicalProperty"), true);
    QtBrowserItem *physicalItem = ui->treeBrowser->topLevelItem(physicalGroup);
    ui->treeBrowser->setBackgroundColor(physicalItem, QColor(191, 255, 255));

    QtProperty *thermalGroup = manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Thermal Property"));
    thermalGroup->setPropertyId(QString("ThermalProperty"));
    ui->treeBrowser->addProperty(thermalGroup);

    property = manager->addProperty(QVariant::String, QString("Conductivity [k]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("Conductivity"));
    thermalGroup->addSubProperty(property);

    property = manager->addProperty(QVariant::Double, QString("HeatCapacity [c]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("HeatCapacity"));
    thermalGroup->addSubProperty(property);

    this->SetPropertyVisibility(QString("ThermalProperty"), false);
    QtBrowserItem *thermalItem = ui->treeBrowser->topLevelItem(thermalGroup);
    ui->treeBrowser->setBackgroundColor(thermalItem, QColor(255, 230, 191));
}

void FDEMNTSFluidMaterialWidget::UpdateNewtonianFluidProperty()
{
    QtVariantProperty *property;

    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("Density")));
    property->setValue(this->material->density);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("BulkModulus")));
    property->setValue(QString::number(this->material->bulkModulus));
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("BulkViscosity")));
    property->setValue(this->material->bulkViscosity);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("DynaViscosity")));
    property->setValue(this->material->dynaViscosity);

    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("Conductivity")));
    property->setValue(QString::number(this->material->conductivity));
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("HeatCapacity")));
    property->setValue(this->material->heatCapacity);
}

void FDEMNTSFluidMaterialWidget::slotTreeBrowserValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("Density"))
    {
        this->material->density = value.toDouble();
    }
    else if(id == QString("BulkModulus"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->material->bulkModulus = val;
    }
    else if(id == QString("BulkViscosity"))
    {
        this->material->bulkViscosity = value.toDouble();
    }
    else if(id == QString("DynaViscosity"))
    {
        this->material->dynaViscosity = value.toDouble();
    }
    else if(id == QString("Conductivity"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->material->conductivity = val;
    }
    else if(id == QString("HeatCapacity"))
    {
        this->material->heatCapacity = value.toDouble();
    }
}

void FDEMNTSFluidMaterialWidget::slotAnalysisModuleChanged(const QVariant &value)
{
    this->SetPropertyVisibility(QString("ThermalProperty"), (THERMAL & value.toInt()));
}
