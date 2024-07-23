#include "triangleelastoplasticitywidget.h"
#include "ui_triangleelastoplasticitywidget.h"
#include "mechanicalconstitutivemodel.h"

#include <QtVariantProperty>

TriangleElastoPlasticityWidget::TriangleElastoPlasticityWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::TriangleElastoPlasticityWidget)
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
            this, &TriangleElastoPlasticityWidget::slotPropertyValueChanged);
}

TriangleElastoPlasticityWidget::~TriangleElastoPlasticityWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void TriangleElastoPlasticityWidget::CreateConstitutiveProperty(ConstitutiveModel *constitutive)
{
    this->model = static_cast<ElastoPlasticityModel*>(constitutive);

    QtVariantProperty *property;
    QtVariantProperty *subProperty;

    property = manager->addProperty(QVariant::String, QString("YoungModulus [") + QString("E") + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+15);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("YoungModulus"));
    property->setValue(QString::number(model->youngModulus));
    ui->treeBrowser->addProperty(property);

    subProperty = manager->addProperty(QVariant::Int, QString("RandomSeed"));
    subProperty->setAttribute(QString("minimum"), 0);
    subProperty->setPropertyId(QString("YoungModulusSeed"));
    subProperty->setValue(model->youngModulusSeed);
    property->addSubProperty(subProperty);

    subProperty = manager->addProperty(QVariant::Double, QString("InhomoIndex"));
    subProperty->setAttribute(QString("minimum"), 0.0);
    subProperty->setPropertyId(QString("YoungModulusIndex"));
    subProperty->setValue(model->youngModulusIndex);
    property->addSubProperty(subProperty);

    property = manager->addProperty(QVariant::Double, QString("PoissonRatio [") + QString(0X03BD) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("PoissonRatio"));
    property->setValue(model->poissonRatio);
    ui->treeBrowser->addProperty(property);

    subProperty = manager->addProperty(QVariant::Int, QString("RandomSeed"));
    subProperty->setAttribute(QString("minimum"), 0);
    subProperty->setPropertyId(QString("PoissonRatioSeed"));
    subProperty->setValue(model->poissonRatioSeed);
    property->addSubProperty(subProperty);

    subProperty = manager->addProperty(QVariant::Double, QString("InhomoIndex"));
    subProperty->setAttribute(QString("minimum"), 0.0);
    subProperty->setPropertyId(QString("PoissonRatioIndex"));
    subProperty->setValue(model->poissonRatioIndex);
    property->addSubProperty(subProperty);

    property = manager->addProperty(QVariant::String, QString("ViscousDamping [") + QString(0X03B7) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("ViscousDamping"));
    property->setValue(QString::number(model->viscousDamping));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("SoftenStretch [") + QString(0X03BB) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("SoftenStretch"));
    property->setValue(model->softenStretch);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Bool, QString("IsPropertyInhomoEnabled"));
    property->setPropertyId(QString("IsPropertyInhomoEnabled"));
    property->setValue(model->isPropertyInhomoEnabled);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("InhomogeneousOption"));
    QStringList names = {"Element-wise", "Grain-wise"};
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("InhomogeneousOption"));
    property->setValue(model->inhomogeneousOption);
    ui->treeBrowser->addProperty(property);

    this->SetPropertyVisibility(QString("YoungModulusSeed"), model->isPropertyInhomoEnabled);
    this->SetPropertyVisibility(QString("YoungModulusIndex"), model->isPropertyInhomoEnabled);
    this->SetPropertyVisibility(QString("PoissonRatioSeed"), model->isPropertyInhomoEnabled);
    this->SetPropertyVisibility(QString("PoissonRatioIndex"), model->isPropertyInhomoEnabled);
    this->SetPropertyVisibility(QString("InhomogeneousOption"), model->inhomogeneousOption);

    this->SetBackgroundColor(QColor(255, 255, 191));
}

void TriangleElastoPlasticityWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtProperty *property = this->manager->qtProperty(id);
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    {
        ui->treeBrowser->setItemVisible(item, visible);
    }
}

void TriangleElastoPlasticityWidget::SetBackgroundColor(const QColor &color)
{
    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    {
        ui->treeBrowser->setBackgroundColor(item, color);
    }
}

void TriangleElastoPlasticityWidget::slotPropertyValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("YoungModulus"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->youngModulus = val;
    }
    else if(id == QString("YoungModulusSeed"))
    {
        this->model->youngModulusSeed = value.toInt();
    }
    else if(id == QString("YoungModulusIndex"))
    {
        this->model->youngModulusIndex = value.toDouble();
    }
    else if(id == QString("PoissonRatio"))
    {
        this->model->poissonRatio = value.toDouble();
    }
    else if(id == QString("PoissonRatioSeed"))
    {
        this->model->poissonRatioSeed = value.toInt();
    }
    else if(id == QString("PoissonRatioIndex"))
    {
        this->model->poissonRatioIndex = value.toDouble();
    }
    else if(id == QString("ViscousDamping"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->viscousDamping = val;
    }
    else if(id == QString("SoftenStretch"))
    {
        this->model->softenStretch = value.toDouble();
    }
    else if(id == QString("InhomogeneousOption"))
    {
        this->model->inhomogeneousOption = value.toInt();
    }
    else if(id == QString("IsPropertyInhomoEnabled"))
    {
        this->model->isPropertyInhomoEnabled = value.toBool();

        this->SetPropertyVisibility(QString("YoungModulusSeed"), value.toInt());
        this->SetPropertyVisibility(QString("YoungModulusIndex"), value.toInt());
        this->SetPropertyVisibility(QString("PoissonRatioSeed"), value.toInt());
        this->SetPropertyVisibility(QString("PoissonRatioIndex"), value.toInt());
        this->SetPropertyVisibility(QString("InhomogeneousOption"), value.toInt());
    }
}
