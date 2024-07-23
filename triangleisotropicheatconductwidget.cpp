#include "triangleisotropicheatconductwidget.h"
#include "ui_triangleisotropicheatconductwidget.h"
#include "thermalconstitutivemodel.h"

#include <QtVariantProperty>

TriangleIsotropicHeatConductWidget::TriangleIsotropicHeatConductWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::TriangleIsotropicHeatConductWidget)
{
    ui->setupUi(this);

    this->factory = new QtVariantEditorFactory(this);
    this->manager = new QtVariantPropertyManager(this);

    ui->treeBrowser->setFactoryForManager(this->manager, this->factory);
    ui->treeBrowser->setPropertiesWithoutValueMarked(true);
    ui->treeBrowser->setAlternatingRowColors(true);
    ui->treeBrowser->setHeaderVisible(false);

    //Connection
    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &TriangleIsotropicHeatConductWidget::slotPropertyValueChanged);
}

TriangleIsotropicHeatConductWidget::~TriangleIsotropicHeatConductWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void TriangleIsotropicHeatConductWidget::CreateConstitutiveProperty(ConstitutiveModel *constitutive)
{
    this->model = static_cast<IsotropicConductModel*>(constitutive);

    QtVariantProperty* property;
    QtVariantProperty* subProperty;

    property = manager->addProperty(QVariant::String, QString("Conductivity [K]"));
    property->setAttribute(QString("decimals"), 6);
    property->setPropertyId(QString("Conductivity"));
    property->setValue(QString::number(model->isoConductivity));
    ui->treeBrowser->addProperty(property);

    subProperty = manager->addProperty(QVariant::Int, QString("RandomSeed"));
    subProperty->setAttribute(QString("minimum"), 0);
    subProperty->setPropertyId(QString("ConductivitySeed"));
    subProperty->setValue(model->conductivitySeed);
    property->addSubProperty(subProperty);

    subProperty = manager->addProperty(QVariant::Double, QString("InhomoIndex"));
    subProperty->setAttribute(QString("minimum"), 0.0);
    subProperty->setPropertyId(QString("ConductivityIndex"));
    subProperty->setValue(model->conductivityIndex);
    property->addSubProperty(subProperty);

    property = manager->addProperty(QVariant::Double, QString("HeatCapacity [c]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 6);
    property->setPropertyId(QString("HeatCapacity"));
    property->setValue(model->heatCapacity);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::String, QString("HeatTransfer [h]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 6);
    property->setPropertyId(QString("HeatTransfer"));
    property->setValue(QString::number(model->heatExchange));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("HeatSource [s]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 6);
    property->setPropertyId(QString("HeatSource"));
    property->setValue(model->heatSource);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::String, QString("HeatExpansion [") + QString(0X03B1) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 6);
    property->setPropertyId(QString("HeatExpansion"));
    property->setValue(QString::number(model->heatExpansion));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Bool, QString("IsConductivityInhomo"));
    property->setPropertyId(QString("IsConductivityInhomo"));
    property->setValue(model->isConductivityInhomoEnabled);
    ui->treeBrowser->addProperty(property);

    this->SetPropertyVisibility(QString("ConductivitySeed"), false);
    this->SetPropertyVisibility(QString("ConductivityIndex"), false);
    this->SetBackgroundColor(QColor(255, 230, 191));
}

void TriangleIsotropicHeatConductWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtProperty *property = this->manager->qtProperty(id);
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    {
        ui->treeBrowser->setItemVisible(item, visible);
    }
}

void TriangleIsotropicHeatConductWidget::SetBackgroundColor(const QColor &color)
{
    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    {
        ui->treeBrowser->setBackgroundColor(item, color);
    }
}

void TriangleIsotropicHeatConductWidget::slotPropertyValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();
    if(id == QString("Conductivity"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->isoConductivity = val;

        if(ok) this->model->majorConductivity = val;
        if(ok) this->model->minorConductivity = val;
        this->model->orientationAngle = 0.0;
    }
    else if(id == QString("ConductivitySeed"))
    {
        this->model->conductivitySeed = value.toInt();
    }
    else if(id == QString("ConductivityIndex"))
    {
        this->model->conductivityIndex = value.toDouble();
    }
    else if(id == QString("HeatCapacity"))
    {
        this->model->heatCapacity = value.toDouble();
    }
    else if(id == QString("HeatTransfer"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->heatExchange = val;
    }
    else if(id == QString("HeatSource"))
    {
        this->model->heatSource = value.toDouble();
    }
    else if(id == QString("HeatExpansion"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->heatExpansion = val;
    }
    else if(id == QString("IsConductivityInhomo"))
    {
        this->model->isConductivityInhomoEnabled = value.toBool();

        this->SetPropertyVisibility(QString("ConductivitySeed"), value.toBool());
        this->SetPropertyVisibility(QString("ConductivityIndex"), value.toBool());
    }
}
