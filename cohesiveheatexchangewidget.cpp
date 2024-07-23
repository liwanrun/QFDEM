#include "cohesiveheatexchangewidget.h"
#include "ui_cohesiveheatexchangewidget.h"
#include "thermalconstitutivemodel.h"

#include <QtVariantProperty>

CohesiveHeatExchangeWidget::CohesiveHeatExchangeWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::CohesiveHeatExchangeWidget)
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
            this, &CohesiveHeatExchangeWidget::slotPropertyValueChanged);
}

CohesiveHeatExchangeWidget::~CohesiveHeatExchangeWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void CohesiveHeatExchangeWidget::CreateConstitutiveProperty(ConstitutiveModel *constitutive)
{
    this->model = static_cast<CohesiveThermalModel*>(constitutive);

    QtVariantProperty* property;

    property = manager->addProperty(QVariant::String, QString("CohesiveExchange [hj]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 15);
    property->setPropertyId(QString("CohesiveExchange"));
    property->setValue(QString::number(model->cohesiveExchange));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::String, QString("FractureExchange [hf]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 15);
    property->setPropertyId(QString("FractureExchange"));
    property->setValue(QString::number(model->fractureExchange));
    ui->treeBrowser->addProperty(property);

    this->SetBackgroundColor(QColor(255, 230, 191));
}

void CohesiveHeatExchangeWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtProperty *property = this->manager->qtProperty(id);
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    {
        ui->treeBrowser->setItemVisible(item, visible);
    }
}

void CohesiveHeatExchangeWidget::SetBackgroundColor(const QColor &color)
{
    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    {
        ui->treeBrowser->setBackgroundColor(item, color);
    }
}

void CohesiveHeatExchangeWidget::slotPropertyValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("CohesiveExchange"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->cohesiveExchange = val;
    }
    else if(id == QString("FractureExchange"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->fractureExchange = val;
    }
}
