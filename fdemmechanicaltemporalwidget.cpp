#include "fdemmechanicaltemporalwidget.h"
#include "ui_fdemmechanicaltemporalwidget.h"
#include "temporal.h"

#include <QtVariantProperty>

FDEMMechanicalTemporalWidget::FDEMMechanicalTemporalWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMMechanicalTemporalWidget)
{
    ui->setupUi(this);

    this->manager = new QtVariantPropertyManager(this);
    this->factory = new QtVariantEditorFactory(this);

    this->InitPropertyBrowser();
    this->ConnectSignalSlots();
}

FDEMMechanicalTemporalWidget::~FDEMMechanicalTemporalWidget()
{
    delete this->factory;
    delete this->manager;

    delete ui;
}

void FDEMMechanicalTemporalWidget::InitPropertyBrowser()
{
    this->CreateInitialConditionProperty();
    this->CreateInitialSpatialOptionProperty();
}

void FDEMMechanicalTemporalWidget::ConnectSignalSlots()
{
    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &FDEMMechanicalTemporalWidget::slotPropertyBrowserValueChanged);
    connect(ui->spaceTableWidget, &QTableWidget::itemChanged,
            this, &FDEMMechanicalTemporalWidget::slotSpatialTableWidgetItemChanged);
}

void FDEMMechanicalTemporalWidget::UpdatePropertyBrowser(MechanicalTemporal *temp)
{
    this->temporal = temp;
    this->UpdateInitialConditionProperty();
    this->UpdateInitialSpatialOptionProperty();
}

void FDEMMechanicalTemporalWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    { ui->treeBrowser->setItemVisible(item, visible); }
}

void FDEMMechanicalTemporalWidget::SetPropertyExpanded(const QString &id, bool expanded)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    QtBrowserItem *item = ui->treeBrowser->items(property).first();
    ui->treeBrowser->setExpanded(item, expanded);
}

void FDEMMechanicalTemporalWidget::CreateInitialConditionProperty()
{
    ui->treeBrowser->setFactoryForManager(this->manager, this->factory);
    ui->treeBrowser->setPropertiesWithoutValueMarked(true);
    ui->treeBrowser->setAlternatingRowColors(true);
    ui->treeBrowser->setHeaderVisible(false);

    QtVariantProperty *property;
    QtVariantProperty *subProperty;
    QtProperty *root = this->manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Initial Condition"));
    ui->treeBrowser->addProperty(root);

    property = manager->addProperty(QVariant::Int, QString("NodeSetId"));
    property->setToolTip(QString("Only single is allowed"));
    property->setAttribute(QString("minimum"), 1);
    property->setPropertyId(QString("NodeSetId"));
    root->addSubProperty(property);

    QStringList names;
    property = this->manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("InitialField"));
    names << "Velocity" << "Displacement";
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("InitialField"));
    root->addSubProperty(property);

    subProperty = this->manager->addProperty(QVariant::String, QString("ampX"));
    subProperty->setAttribute(QString("decimals"), 6);
    subProperty->setPropertyId(QString("AmplitudeX"));
    property->addSubProperty(subProperty);

    subProperty = this->manager->addProperty(QVariant::String, QString("ampY"));
    subProperty->setAttribute(QString("decimals"), 6);
    subProperty->setPropertyId(QString("AmplitudeY"));
    property->addSubProperty(subProperty);

    property = this->manager->addProperty(QVariant::Bool, QString("IsAmplitudeSiteDependent"));
    property->setPropertyId(QString("IsAmplitudeSiteDependent"));
    root->addSubProperty(property);

    QtBrowserItem *bvpItem = ui->treeBrowser->topLevelItem(root);
    ui->treeBrowser->setBackgroundColor(bvpItem, QColor(255, 255, 191));
}

void FDEMMechanicalTemporalWidget::CreateInitialSpatialOptionProperty()
{
    ui->spaceBrowser->setFactoryForManager(this->manager, this->factory);
    ui->spaceBrowser->setPropertiesWithoutValueMarked(true);
    ui->spaceBrowser->setAlternatingRowColors(true);
    ui->spaceBrowser->setHeaderVisible(false);

    QtVariantProperty *property;
    QtProperty *root = this->manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Spatial Group"));
    ui->spaceBrowser->addProperty(root);

    property = this->manager->addProperty(QVariant::Int, QString("SpatialSample"));
    property->setAttribute(QString("minimum"), 0);
    property->setPropertyId(QString("SpatialSample"));
    root->addSubProperty(property);

    //Visibility
    ui->spatialGroup->setVisible(false);
    QtBrowserItem *item = ui->spaceBrowser->topLevelItem(root);
    ui->spaceBrowser->setBackgroundColor(item, QColor(255, 255, 191));
}

void FDEMMechanicalTemporalWidget::UpdateInitialConditionProperty()
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("NodeSetId")));
    property->setValue(this->temporal->nodeSetId);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("InitialField")));
    property->setValue(this->temporal->initialField);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("AmplitudeX")));
    property->setValue(QString::number(this->temporal->amplitudeX));
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("AmplitudeY")));
    property->setValue(QString::number(this->temporal->amplitudeY));
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("IsAmplitudeSiteDependent")));
    property->setValue(this->temporal->isAmplitudeSiteDependent);
}

void FDEMMechanicalTemporalWidget::UpdateInitialSpatialOptionProperty()
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("SpatialSample")));
    property->setValue(this->temporal->spatialSample);

    disconnect(ui->spaceTableWidget, &QTableWidget::itemChanged,
               this, &FDEMMechanicalTemporalWidget::slotSpatialTableWidgetItemChanged);
    for(int i = 0; i < ui->spaceTableWidget->rowCount(); i++)
    {
        for(int j = 0; j < ui->spaceTableWidget->columnCount(); j++)
        {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(temporal->spatialFactor[i][j]));
            item->setTextAlignment(Qt::AlignVCenter | Qt:: AlignRight);
            ui->spaceTableWidget->setItem(i, j, item);
        }
    }
    connect(ui->spaceTableWidget, &QTableWidget::itemChanged,
            this, &FDEMMechanicalTemporalWidget::slotSpatialTableWidgetItemChanged);
}

void FDEMMechanicalTemporalWidget::slotPropertyBrowserValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("NodeSetId"))
    {
        this->temporal->nodeSetId = value.toInt();
    }
    else if(id == QString("InitialField"))
    {
        this->temporal->initialField = value.toInt();
    }
    else if(id == QString("AmplitudeX"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->temporal->amplitudeX = val;
    }
    else if(id == QString("AmplitudeY"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->temporal->amplitudeY = val;
    }
    else if(id == QString("IsAmplitudeSiteDependent"))
    {
        this->temporal->isAmplitudeSiteDependent = value.toBool();
        ui->spatialGroup->setVisible(value.toBool());

        QtVariantProperty *property;
        property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("SpatialSample")));
        if(!value.toBool()) property->setValue(0);
    }
    else if(id == QString("SpatialSample"))
    {
        this->temporal->spatialSample = value.toInt();
        ui->spaceTableWidget->setRowCount(value.toInt());

        if(this->temporal->spatialFactor.size() != value.toInt())
        {
            QVector<double> tuple({0.0, 1.0});
            this->temporal->spatialFactor.fill(tuple, value.toInt());
            this->temporal->spatialFactor.squeeze();
        }
    }
}

void FDEMMechanicalTemporalWidget::slotSpatialTableWidgetItemChanged(QTableWidgetItem *item)
{
    item->setTextAlignment(Qt::AlignVCenter | Qt:: AlignRight);
    if(!item->data(Qt::EditRole).isNull())
    {
        bool ok; double value = item->data(Qt::EditRole).toString().toDouble(&ok);
        this->temporal->spatialFactor[item->row()][item->column()] = ok ? value : 0.0;
    }
}
