#include "triangleisotropicseepagewidget.h"
#include "ui_triangleisotropicseepagewidget.h"
#include "hydraulicconstitutivemodel.h"

#include <QtVariantProperty>

TriangleIsotropicSeepageWidget::TriangleIsotropicSeepageWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::TriangleIsotropicSeepageWidget)
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
            this, &TriangleIsotropicSeepageWidget::slotPropertyValueChanged);
}

TriangleIsotropicSeepageWidget::~TriangleIsotropicSeepageWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void TriangleIsotropicSeepageWidget::CreateConstitutiveProperty(ConstitutiveModel *constitutive)
{
    this->model = static_cast<IsotropicSeepageModel*>(constitutive);

    QtVariantProperty* property;
    QtVariantProperty* subProperty;

    property = manager->addProperty(QVariant::String, QString("Permeability [K]"));
    property->setAttribute(QString("decimals"), 15);
    property->setAttribute(QString("minimum"), 0.0);
    property->setPropertyId(QString("Permeability"));
    property->setValue(QString::number(model->isoPermeability));
    ui->treeBrowser->addProperty(property);

    subProperty = manager->addProperty(QVariant::Int, QString("RandomSeed"));
    subProperty->setAttribute(QString("minimum"), 0);
    subProperty->setPropertyId(QString("PermeabilitySeed"));
    subProperty->setValue(model->permeabilitySeed);
    property->addSubProperty(subProperty);

    subProperty = manager->addProperty(QVariant::Double, QString("InhomoIndex"));
    subProperty->setAttribute(QString("minimum"), 0.0);
    subProperty->setPropertyId(QString("PermeabilityIndex"));
    subProperty->setValue(model->permeabilityIndex);
    property->addSubProperty(subProperty);

    property = manager->addProperty(QVariant::Double, QString("BiotCoefficient [") + QString(0X03B1) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("BiotCoefficient"));
    property->setValue(model->biotCoefficient);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("PorePorosity [") + QString(0X03C6) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("PorePorosity"));
    property->setValue(model->porePorosity);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::String, QString("PoreModulus [k]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("PoreModulus"));
    property->setValue(QString::number(model->poreModulus));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("PoreSource [s]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("PoreSource"));
    property->setValue(model->poreSource);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Bool, QString("IsPermeabilityInhomo"));
    property->setPropertyId(QString("IsPermeabilityInhomo"));
    property->setValue(model->isPermeabilityInhomoEnabled);
    ui->treeBrowser->addProperty(property);

    this->SetPropertyVisibility(QString("PermeabilitySeed"), false);
    this->SetPropertyVisibility(QString("PermeabilityIndex"), false);
    this->SetBackgroundColor(QColor(191, 255, 255));
}

void TriangleIsotropicSeepageWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtProperty *property = this->manager->qtProperty(id);
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    {
        ui->treeBrowser->setItemVisible(item, visible);
    }
}

void TriangleIsotropicSeepageWidget::SetBackgroundColor(const QColor &color)
{
    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    {
        ui->treeBrowser->setBackgroundColor(item, color);
    }
}

void TriangleIsotropicSeepageWidget::slotPropertyValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();
    if(id == QString("Permeability"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->isoPermeability = val;

        if(ok) this->model->majorPermeability = val;
        if(ok) this->model->minorPermeability = val;
        this->model->orientationAngle = 0.0;
    }
    else if(id == QString("PermeabilitySeed"))
    {
        this->model->permeabilitySeed = value.toInt();
    }
    else if(id == QString("PermeabilityIndex"))
    {
        this->model->permeabilityIndex = value.toDouble();
    }
    else if(id == QString("BiotCoefficient"))
    {
        this->model->biotCoefficient = value.toDouble();
    }
    else if(id == QString("PorePorosity"))
    {
        this->model->porePorosity = value.toDouble();
    }
    else if(id == QString("PoreModulus"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->poreModulus = val;
    }
    else if(id == QString("PoreSource"))
    {
        this->model->poreSource = value.toDouble();
    }
    else if(id == QString("IsPermeabilityInhomo"))
    {
        this->model->isPermeabilityInhomoEnabled = value.toBool();

        this->SetPropertyVisibility(QString("PermeabilitySeed"), value.toBool());
        this->SetPropertyVisibility(QString("PermeabilityIndex"), value.toBool());
    }
}
