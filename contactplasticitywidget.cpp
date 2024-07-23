#include "contactplasticitywidget.h"
#include "ui_contactplasticitywidget.h"
#include "contactconstitutivemodel.h"

#include <QtVariantProperty>

ContactPlasticityWidget::ContactPlasticityWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::ContactPlasticityWidget)
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
            this, &ContactPlasticityWidget::slotPropertyValueChanged);
}

ContactPlasticityWidget::~ContactPlasticityWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void ContactPlasticityWidget::CreateConstitutiveProperty(ConstitutiveModel *constitutive)
{
    this->model = static_cast<ContactPlasticityModel*>(constitutive);

    QtVariantProperty *property;

    property = manager->addProperty(QVariant::String, QString("NormalStiffness [") + QString("Kn") + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+15);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("NormalStiffness"));
    property->setValue(QString::number(this->model->normalStiffness));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::String, QString("TangentStiffness [") + QString("Ks") + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+15);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("TangentStiffness"));
    property->setValue(QString::number(this->model->tangentStiffness));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("ContactFriction [") + QString(0X03BC) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("ContactFriction"));
    property->setValue(this->model->contactFriction);
    property->setValue(0.1);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::String, QString("YieldStrength [") + QString("T") + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("YieldStrength"));
    property->setValue(QString::number(this->model->yieldStrength));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("PowerExponent [") + QString("b") + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("PowerExponent"));
    property->setValue(this->model->powerExponent);
    property->setValue(1.0);
    ui->treeBrowser->addProperty(property);

    this->SetBackgroundColor(QColor(255, 255, 191));
}

void ContactPlasticityWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtProperty *property = this->manager->qtProperty(id);
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    {
        ui->treeBrowser->setItemVisible(item, visible);
    }
}

void ContactPlasticityWidget::SetBackgroundColor(const QColor &color)
{
    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    {
        ui->treeBrowser->setBackgroundColor(item, color);
    }
}

void ContactPlasticityWidget::slotPropertyValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("NormalStiffness"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->normalStiffness = val;
    }
    else if(id == QString("TangentStiffness"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->tangentStiffness = val;
    }
    else if(id == QString("ContactFriction"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->contactFriction = val;
    }
    else if(id == QString("YieldStrength"))
    {
        this->model->yieldStrength = value.toDouble();
    }
    else if(id == QString("powerExponent"))
    {
        this->model->powerExponent = value.toDouble();
    }
}
