#include "cohesivecompressibleflowwidget.h"
#include "ui_cohesivecompressibleflowwidget.h"
#include "hydraulicconstitutivemodel.h"

#include <QtVariantProperty>

CohesiveCompressibleFlowWidget::CohesiveCompressibleFlowWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::CohesiveCompressibleFlowWidget)
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
            this, &CohesiveCompressibleFlowWidget::slotPropertyValueChanged);
}

CohesiveCompressibleFlowWidget::~CohesiveCompressibleFlowWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void CohesiveCompressibleFlowWidget::CreateConstitutiveProperty(ConstitutiveModel *constitutive)
{
    this->model = static_cast<CompressibleFlowModel*>(constitutive);

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

    this->SetBackgroundColor(QColor(191, 255, 255));
}

void CohesiveCompressibleFlowWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtProperty *property = this->manager->qtProperty(id);
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    {
        ui->treeBrowser->setItemVisible(item, visible);
    }
}

void CohesiveCompressibleFlowWidget::SetBackgroundColor(const QColor &color)
{
    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    {
        ui->treeBrowser->setBackgroundColor(item, color);
    }
}

void CohesiveCompressibleFlowWidget::slotPropertyValueChanged(QtProperty *property, const QVariant &value)
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
