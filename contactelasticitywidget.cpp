#include "contactelasticitywidget.h"
#include "ui_contactelasticitywidget.h"
#include "contactconstitutivemodel.h"

#include <QtVariantProperty>

ContactElasticityWidget::ContactElasticityWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::ContactElasticityWidget)
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
            this, &ContactElasticityWidget::slotPropertyValueChanged);
}

ContactElasticityWidget::~ContactElasticityWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void ContactElasticityWidget::CreateConstitutiveProperty(ConstitutiveModel *constitutive)
{
    this->model = static_cast<ContactElasticityModel*>(constitutive);

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
    property->setValue(QString::number(this->model->contactFriction));
    ui->treeBrowser->addProperty(property);

    this->SetBackgroundColor(QColor(255, 255, 191));
}

void ContactElasticityWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtProperty *property = this->manager->qtProperty(id);
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    {
        ui->treeBrowser->setItemVisible(item, visible);
    }
}

void ContactElasticityWidget::SetBackgroundColor(const QColor &color)
{
    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    {
        ui->treeBrowser->setBackgroundColor(item, color);
    }
}

void ContactElasticityWidget::slotPropertyValueChanged(QtProperty *property, const QVariant &value)
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
}
