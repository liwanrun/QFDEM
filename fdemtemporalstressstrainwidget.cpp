#include "fdemtemporalstressstrainwidget.h"
#include "ui_fdemtemporalstressstrainwidget.h"
#include "temporal.h"

#include <QtVariantProperty>

FDEMTemporalStressStrainWidget::FDEMTemporalStressStrainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMTemporalStressStrainWidget)
{
    ui->setupUi(this);

    this->manager = new QtVariantPropertyManager(this);
    this->factory = new QtVariantEditorFactory(this);

    ui->treeBrowser->setFactoryForManager(this->manager, this->factory);
    ui->treeBrowser->setPropertiesWithoutValueMarked(true);
    ui->treeBrowser->setAlternatingRowColors(true);
    ui->treeBrowser->setHeaderVisible(false);

    this->InitPropertyBrowser();
    this->ConnectSignalSlots();
}

FDEMTemporalStressStrainWidget::~FDEMTemporalStressStrainWidget()
{
    delete ui;
}

void FDEMTemporalStressStrainWidget::InitPropertyBrowser()
{
    this->CreateInitialConditionProperty();
    //this->CreateInitialSpatialOptionProperty();
}

void FDEMTemporalStressStrainWidget::ConnectSignalSlots()
{
    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &FDEMTemporalStressStrainWidget::slotPropertyBrowserValueChanged);
}

void FDEMTemporalStressStrainWidget::UpdatePropertyBrowser(TemporalStressStrain *temp)
{
    this->temporal = temp;
    this->UpdateInitialConditionProperty();
    this->UpdateInitialSpatialTableWidget();
}

void FDEMTemporalStressStrainWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    { ui->treeBrowser->setItemVisible(item, visible); }
}

void FDEMTemporalStressStrainWidget::SetPropertyExpanded(const QString &id, bool expanded)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    QtBrowserItem *item = ui->treeBrowser->items(property).first();
    ui->treeBrowser->setExpanded(item, expanded);
}

void FDEMTemporalStressStrainWidget::CreateInitialConditionProperty()
{
    QtVariantProperty *property;
    QtVariantProperty *subProperty;

    //Parameter
    QtProperty *root = this->manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Initial Condition"));
    ui->treeBrowser->addProperty(root);

    property = this->manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("InitialField"));
    QStringList names = { "Initial Stress", "Initial Strain" };
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("InitialField"));
    root->addSubProperty(property);

    subProperty = this->manager->addProperty(QVariant::Double, QString("ampXX"));
    subProperty->setAttribute(QString("decimals"), 6);
    subProperty->setPropertyId(QString("AmplitudeXX"));
    property->addSubProperty(subProperty);

    subProperty = this->manager->addProperty(QVariant::Double, QString("ampYY"));
    subProperty->setAttribute(QString("decimals"), 6);
    subProperty->setPropertyId(QString("AmplitudeYY"));
    property->addSubProperty(subProperty);

    subProperty = this->manager->addProperty(QVariant::Double, QString("ampXY"));
    subProperty->setAttribute(QString("decimals"), 6);
    subProperty->setPropertyId(QString("AmplitudeXY"));
    property->addSubProperty(subProperty);

    property = this->manager->addProperty(QVariant::Bool, QString("IsAmplitudeSiteDependent"));
    property->setPropertyId(QString("IsAmplitudeSiteDependent"));
    root->addSubProperty(property);

    ui->spatialGroup->setVisible(false);
    QtBrowserItem *topItem = ui->treeBrowser->topLevelItem(root);
    ui->treeBrowser->setBackgroundColor(topItem, QColor(255, 255, 191));
}

void FDEMTemporalStressStrainWidget::UpdateInitialConditionProperty()
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("InitialField")));
    property->setValue(this->temporal->initialField);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("AmplitudeXX")));
    property->setValue(this->temporal->amplitudeXX);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("AmplitudeYY")));
    property->setValue(this->temporal->amplitudeYY);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("AmplitudeXY")));
    property->setValue(this->temporal->amplitudeXY);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("IsAmplitudeSiteDependent")));
    property->setValue(this->temporal->isAmplitudeSiteDependent);
}

void FDEMTemporalStressStrainWidget::UpdateInitialSpatialTableWidget()
{
    disconnect(ui->tableWidget, &QTableWidget::itemChanged,
               this, &FDEMTemporalStressStrainWidget::slotSpatialTableWidgetItemChanged);
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        for(int j = 0; j < ui->tableWidget->columnCount(); j++)
        {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(temporal->spatialFactor[i][j]));
            item->setTextAlignment(Qt::AlignVCenter | Qt:: AlignRight);
            ui->tableWidget->setItem(i, j, item);
        }
    }
    connect(ui->tableWidget, &QTableWidget::itemChanged,
            this, &FDEMTemporalStressStrainWidget::slotSpatialTableWidgetItemChanged);
}

void FDEMTemporalStressStrainWidget::slotPropertyBrowserValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("InitialField"))
    {
        this->temporal->initialField = value.toInt();
    }
    else if(id == QString("AmplitudeXX"))
    {
        this->temporal->amplitudeXX = value.toDouble();
    }
    else if(id == QString("AmplitudeYY"))
    {
        this->temporal->amplitudeYY = value.toDouble();
    }
    else if(id == QString("AmplitudeXY"))
    {
        this->temporal->amplitudeXY = value.toDouble();
    }
    else if(id == QString("IsAmplitudeSiteDependent"))
    {
        this->temporal->isAmplitudeSiteDependent = value.toBool();
        ui->spatialGroup->setVisible(value.toBool());
    }
}

void FDEMTemporalStressStrainWidget::slotSpatialTableWidgetItemChanged(QTableWidgetItem *item)
{
    item->setTextAlignment(Qt::AlignVCenter | Qt:: AlignRight);
    if(!item->data(Qt::EditRole).isNull())
    {
        bool ok; double value = item->data(Qt::EditRole).toString().toDouble(&ok);
        this->temporal->spatialFactor[item->row()][item->column()] = ok ? value : 0.0;
    }
}
