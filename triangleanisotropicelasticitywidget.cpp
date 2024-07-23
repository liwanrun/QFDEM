#include "triangleanisotropicelasticitywidget.h"
#include "ui_triangleanisotropicelasticitywidget.h"
#include "mechanicalconstitutivemodel.h"

#include <QtVariantProperty>

TriangleAnisotropicElasticityWidget::TriangleAnisotropicElasticityWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::TriangleAnisotropicElasticityWidget)
{
    ui->setupUi(this);

    this->factory = new QtVariantEditorFactory(this);
    this->manager = new QtVariantPropertyManager(this);

    ui->treeBrowser->setFactoryForManager(this->manager, this->factory);
    ui->treeBrowser->setPropertiesWithoutValueMarked(true);
    ui->treeBrowser->setAlternatingRowColors(true);
    ui->treeBrowser->setHeaderVisible(false);
}

TriangleAnisotropicElasticityWidget::~TriangleAnisotropicElasticityWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void TriangleAnisotropicElasticityWidget::CreateConstitutiveProperty(ConstitutiveModel *constitutive)
{
    this->model = static_cast<AnisotropicElasticityModel*>(constitutive);

    QtVariantProperty *property;

    property = manager->addProperty(QVariant::String, QString("MajorYoungModulus [") + QString("E") + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("MajorYoungModulus"));
    property->setValue(QString::number(model->majorYoungModulus));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::String, QString("MinorYoungModulus [") + QString("E'") + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("MinorYoungModulus"));
    property->setValue(QString::number(model->minorYoungModulus));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("MajorPoissonRatio [") + QString(0X03BD) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("MajorPoissonRatio"));
    property->setValue(model->majorPoissonRatio);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("MinorPoissonRatio [") + QString(0X03BD) + QString("']"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("MinorPoissonRatio"));
    property->setValue(model->minorPoissonRatio);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("OrientationAngle [") + QString(0X03B8) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("OrientationAngle"));
    property->setValue(model->orientationAngle);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::String, QString("ShearModulus [") + QString("G") + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("ShearModulus"));
    property->setValue(QString::number(model->shearModulus));
    ui->treeBrowser->addProperty(property);

    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    { ui->treeBrowser->setBackgroundColor(item, QColor(255, 255, 191)); }

    //Connection
    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &TriangleAnisotropicElasticityWidget::slotPropertyValueChanged);
}

void TriangleAnisotropicElasticityWidget::slotPropertyValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("MajorYoungModulus"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->majorYoungModulus = val;
    }
    else if(id == QString("MinorYoungModulus"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->minorYoungModulus = val;
    }
    else if(id == QString("MajorPoissonRatio"))
    {
        this->model->majorPoissonRatio = value.toDouble();
    }
    else if(id == QString("MinorPoissonRatio"))
    {
        this->model->minorPoissonRatio = value.toDouble();
    }
    else if(id == QString("OrientationAngle"))
    {
        this->model->orientationAngle = value.toDouble();
    }
    else if(id == QString("ShearModulus"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->shearModulus = val;
    }
}
