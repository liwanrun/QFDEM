#include "cohesiveexponentwidget.h"
#include "ui_cohesiveexponentwidget.h"
#include "mechanicalconstitutivemodel.h"

#include <QtVariantProperty>

CohesiveExponentWidget::CohesiveExponentWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::CohesiveExponentWidget)
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
            this, &CohesiveExponentWidget::slotPropertyValueChanged);
}

CohesiveExponentWidget::~CohesiveExponentWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void CohesiveExponentWidget::CreateConstitutiveProperty(ConstitutiveModel *constitutive)
{
    this->model = static_cast<CohesiveExponentModel*>(constitutive);

    QtVariantProperty *property;

    property = manager->addProperty(QVariant::String, QString("NormalPenalty [Pn]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+15);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("NormalPenalty"));
    property->setValue(QString::number(model->normalPenalty));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::String, QString("TangentPenalty [Pt]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+15);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("TangentPenalty"));
    property->setValue(QString::number(model->tangentPenalty));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::String, QString("Tension [ft]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("Tension"));
    property->setValue(QString::number(model->tension));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::String, QString("Cohesion [c]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0E+12);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("Cohesion"));
    property->setValue(QString::number(model->cohesion));
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("Friction [") + QString(0X03C6) + QString("]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 90.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("Friction"));
    property->setValue(model->friction);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("TensionFERR [GI]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("TensionFERR"));
    property->setValue(model->tensionFERR);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Double, QString("ShearFERR [GII]"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("ShearFERR"));
    property->setValue(model->shearFERR);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QVariant::Bool, QString("IsPropertyInhomoEnabled"));
    property->setPropertyId(QString("IsPropertyInhomoEnabled"));
    property->setValue(model->isPropertyInhomoEnabled);
    ui->treeBrowser->addProperty(property);

    property = manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("InhomogeneousOption"));
    QStringList names = { "Element-wise", "Grain-wise" };
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("InhomogeneousOption"));
    property->setValue(model->inhomogeneousOption);
    ui->treeBrowser->addProperty(property);

    this->SetPropertyVisibility(QString("InhomogeneousOption"), model->inhomogeneousOption);

    this->SetBackgroundColor(QColor(255, 255, 191));
}

void CohesiveExponentWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtProperty *property = this->manager->qtProperty(id);
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    {
        ui->treeBrowser->setItemVisible(item, visible);
    }
}

void CohesiveExponentWidget::SetBackgroundColor(const QColor &color)
{
    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    {
        ui->treeBrowser->setBackgroundColor(item, color);
    }
}

void CohesiveExponentWidget::slotPropertyValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("NormalPenalty"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->normalPenalty = val;
    }
    else if(id == QString("TangentPenalty"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->tangentPenalty = val;
    }
    else if(id == QString("Tension"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->tension = val;
    }
    else if(id == QString("Cohesion"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->model->cohesion = val;
    }
    else if(id == QString("Friction"))
    {
        this->model->friction = value.toDouble();
    }
    else if(id == QString("TensionFERR"))
    {
        this->model->tensionFERR = value.toDouble();
    }
    else if(id == QString("ShearFERR"))
    {
        this->model->shearFERR = value.toDouble();
    }
    else if(id == QString("InhomogeneityOption"))
    {
        this->model->inhomogeneousOption = value.toInt();
    }
    else if(id == QString("IsPropertyInhomoEnabled"))
    {
        this->model->isPropertyInhomoEnabled = value.toBool();
        this->SetPropertyVisibility(QString("InhomogeneousOption"), value.toInt());
    }
}
