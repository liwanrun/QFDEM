#include "cohesiveincompressibleflowwidget.h"
#include "ui_cohesiveincompressibleflowwidget.h"
#include "hydraulicconstitutivemodel.h"

#include <QtVariantProperty>

CohesiveIncompressibleFlowWidget::CohesiveIncompressibleFlowWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::CohesiveIncompressibleFlowWidget)
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
            this, &CohesiveIncompressibleFlowWidget::slotPropertyValueChanged);
}

CohesiveIncompressibleFlowWidget::~CohesiveIncompressibleFlowWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void CohesiveIncompressibleFlowWidget::CreateConstitutiveProperty(ConstitutiveModel *constitutive)
{
    this->model = static_cast<IncompressibleFlowModel*>(constitutive);

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

    property = manager->addProperty(QVariant::String, QString("InitialAperture [a0]"));
    property->setAttribute(QString("decimals"), 6);
    property->setPropertyId(QString("InitialAperture"));
    property->setValue(QString::number(model->iniAperture));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::String, QString("MinimumAperture [a1]"));
    property->setAttribute(QString("decimals"), 6);
    property->setPropertyId(QString("MinimumAperture"));
    property->setValue(QString::number(model->minAperture));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::String, QString("MaximumAperture [a2]"));
    property->setAttribute(QString("decimals"), 6);
    property->setPropertyId(QString("MaximumAperture"));
    property->setValue(QString::number(model->maxAperture));
    ui->treeBrowser->addProperty(property);

    this->SetBackgroundColor(QColor(191, 255, 255));
}

void CohesiveIncompressibleFlowWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtProperty *property = this->manager->qtProperty(id);
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    {
        ui->treeBrowser->setItemVisible(item, visible);
    }
}

void CohesiveIncompressibleFlowWidget::SetBackgroundColor(const QColor &color)
{
    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    {
        ui->treeBrowser->setBackgroundColor(item, color);
    }
}

void CohesiveIncompressibleFlowWidget::slotPropertyValueChanged(QtProperty *property, const QVariant &value)
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
    else if(id == QString("InitialAperture"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(val) this->model->iniAperture = val;
    }
    else if(id == QString("MinimumAperture"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->minAperture = val;
    }
    else if(id == QString("MaximumAperture"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->maxAperture = val;
    }
}
