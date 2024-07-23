#include "triangleanisotropicheatconductwidget.h"
#include "ui_triangleanisotropicheatconductwidget.h"
#include "thermalconstitutivemodel.h"

#include <QtVariantProperty>

TriangleAnisotropicHeatConductWidget::TriangleAnisotropicHeatConductWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::TriangleAnisotropicHeatConductWidget)
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
            this, &TriangleAnisotropicHeatConductWidget::slotPropertyValueChanged);
}

TriangleAnisotropicHeatConductWidget::~TriangleAnisotropicHeatConductWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void TriangleAnisotropicHeatConductWidget::CreateConstitutiveProperty(ConstitutiveModel *constitutive)
{
    this->model = static_cast<AnisotropicConductModel*>(constitutive);

    QtVariantProperty* property;

    property = manager->addProperty(QVariant::Double, QString("MajorConductivity [K1]"));
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("MajorConductivity"));
    property->setValue(model->majorConductivity);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("MinorConductivity [K2]"));
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("MinorConductivity"));
    property->setValue(model->minorConductivity);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("OrientationAngle [") + QString(0X03B8) + QString("]"));
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("OrientationAngle"));
    property->setValue(model->orientationAngle);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("HeatCapacity [c]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("HeatCapacity"));
    property->setValue(model->heatCapacity);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("HeatTransfer [h]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("HeatTransfer"));
    property->setValue(model->heatExchange);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("HeatSource [s]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("HeatSource"));
    property->setValue(model->heatSource);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("HeatExpansion [") + QString(0X03B1) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("HeatExpansion"));
    property->setValue(model->heatExpansion);
    ui->treeBrowser->addProperty(property);

    this->SetBackgroundColor(QColor(255, 230, 191));
}

void TriangleAnisotropicHeatConductWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtProperty *property = this->manager->qtProperty(id);
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    {
        ui->treeBrowser->setItemVisible(item, visible);
    }
}

void TriangleAnisotropicHeatConductWidget::SetBackgroundColor(const QColor &color)
{
    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    {
        ui->treeBrowser->setBackgroundColor(item, color);
    }
}

void TriangleAnisotropicHeatConductWidget::slotPropertyValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("MajorConductivity"))
    {
        this->model->majorConductivity = value.toDouble();
    }
    else if(id == QString("MinorConductivity"))
    {
        this->model->minorConductivity = value.toDouble();
    }
    else if(id == QString("OrientationAngle"))
    {
        this->model->orientationAngle = value.toDouble();
    }
    else if(id == QString("HeatCapacity"))
    {
        this->model->heatCapacity = value.toDouble();
    }
    else if(id == QString("HeatTransfer"))
    {
        this->model->heatExchange = value.toDouble();
    }
    else if(id == QString("HeatSource"))
    {
        this->model->heatSource = value.toDouble();
    }
    else if(id == QString("HeatExpansion"))
    {
        this->model->heatExpansion = value.toDouble();
    }
}
