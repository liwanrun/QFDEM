#include "triangleanisotropicseepagewidget.h"
#include "ui_triangleanisotropicseepagewidget.h"
#include "hydraulicconstitutivemodel.h"

#include <QtVariantProperty>

TriangleAnisotropicSeepageWidget::TriangleAnisotropicSeepageWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::TriangleAnisotropicSeepageWidget)
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
            this, &TriangleAnisotropicSeepageWidget::slotPropertyValueChanged);
}

TriangleAnisotropicSeepageWidget::~TriangleAnisotropicSeepageWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void TriangleAnisotropicSeepageWidget::CreateConstitutiveProperty(ConstitutiveModel *constitutive)
{
    this->model = static_cast<AnisotropicSeepageModel*>(constitutive);

    QtVariantProperty* property;

    property = manager->addProperty(QVariant::Double, QString("MajorPermeability [K1]"));
    property->setAttribute(QString("decimals"), 15);
    property->setAttribute(QString("minimum"), 1.0);
    property->setPropertyId(QString("MajorPermeability"));
    property->setValue(model->majorPermeability);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("MinorPermeability [K2]"));
    property->setAttribute(QString("decimals"), 15);
    property->setAttribute(QString("minimum"), 0.0);
    property->setPropertyId(QString("Permeability"));
    property->setValue(model->minorPermeability);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("OrientationAngle [") + QString(0X03B8) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("OrientationAngle"));
    property->setValue(model->orientationAngle);
    ui->treeBrowser->addProperty(property);

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

    property = manager->addProperty(QVariant::Double, QString("PoreModulus [k]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("PoreModulus"));
    property->setValue(model->poreModulus);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("PoreSource [s]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("PoreSource"));
    property->setValue(model->poreSource);
    ui->treeBrowser->addProperty(property);

    this->SetBackgroundColor(QColor(191, 255, 255));
}

void TriangleAnisotropicSeepageWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtProperty *property = this->manager->qtProperty(id);
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    {
        ui->treeBrowser->setItemVisible(item, visible);
    }
}

void TriangleAnisotropicSeepageWidget::SetBackgroundColor(const QColor &color)
{
    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    {
        ui->treeBrowser->setBackgroundColor(item, color);
    }
}

void TriangleAnisotropicSeepageWidget::slotPropertyValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("MajorPermeability"))
    {
        this->model->majorPermeability = value.toDouble();
    }
    else if(id == QString("MinorPermeability"))
    {
        this->model->minorPermeability = value.toDouble();
    }
    else if(id == QString("OrientationAngle"))
    {
        this->model->orientationAngle = value.toDouble();
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
        this->model->poreModulus = value.toDouble();
    }
    else if(id == QString("PoreSource"))
    {
        this->model->poreSource = value.toDouble();
    }
}
