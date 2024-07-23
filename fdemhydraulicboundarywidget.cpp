#include "fdemhydraulicboundarywidget.h"
#include "ui_fdemhydraulicboundarywidget.h"
#include "boundary.h"

#include <QMessageBox>
#include <QtVariantProperty>

FDEMHydraulicBoundaryWidget::FDEMHydraulicBoundaryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMHydraulicBoundaryWidget)
{
    ui->setupUi(this);

    this->manager = new QtVariantPropertyManager(this);
    this->factory = new QtVariantEditorFactory(this);

    this->InitPropertyBrowser();
    this->ConnectSignalSlots();
}

FDEMHydraulicBoundaryWidget::~FDEMHydraulicBoundaryWidget()
{
    delete this->factory;
    delete this->manager;

    delete ui;
}

void FDEMHydraulicBoundaryWidget::InitPropertyBrowser()
{
    this->CreateBoundaryConditionProperty();
    this->CreateBoundarySpatialOptionProperty();
    this->CreateBoundaryTemporalOptionProperty();
}

void FDEMHydraulicBoundaryWidget::ConnectSignalSlots()
{
    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &FDEMHydraulicBoundaryWidget::slotHydraulicBrowserValueChanged);
}

void FDEMHydraulicBoundaryWidget::UpdatePropertyBrowser(HydraulicBoundary *bnd)
{
    this->boundary = bnd;

    this->UpdateBoundaryConditionProperty();
    this->UpdateBoundarySpatialOptionProperty();
    this->UpdateBoundaryTemporalOptionProperty();
}

void FDEMHydraulicBoundaryWidget::CreateBoundaryConditionProperty()
{
    ui->treeBrowser->setFactoryForManager(this->manager, this->factory);
    ui->treeBrowser->setPropertiesWithoutValueMarked(true);
    ui->treeBrowser->setAlternatingRowColors(true);
    ui->treeBrowser->setHeaderVisible(false);

    QtVariantProperty *property;
    QtVariantProperty *subProperty;
    QtProperty *root = this->manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Boundary Condition"));
    ui->treeBrowser->addProperty(root);

    property = manager->addProperty(QVariant::Int, QString("NodeSetId"));
    property->setToolTip(QString("Only single is allowed"));
    property->setAttribute(QString("minimum"), 1);
    property->setPropertyId(QString("NodeSetId"));
    root->addSubProperty(property);

    QStringList names;
    property = this->manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("LoadingTypeX"));
    names << "Pressure (Dirichlet)" << "Flow Rate (Neumann)";
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("LoadingTypeX"));
    root->addSubProperty(property);

    subProperty = this->manager->addProperty(QVariant::String, QString("ampX"));
    subProperty->setAttribute(QString("decimals"), 6);
    subProperty->setPropertyId(QString("AmplitudeX"));
    property->addSubProperty(subProperty);

    property = this->manager->addProperty(QVariant::Bool, QString("IsAmplitudeSiteDependent"));
    property->setPropertyId(QString("IsAmplitudeSiteDependent"));
    root->addSubProperty(property);

    property = this->manager->addProperty(QVariant::Bool, QString("IsAmplitudeTimeDependent"));
    property->setPropertyId(QString("IsAmplitudeTimeDependent"));
    root->addSubProperty(property);

    QtBrowserItem *topItem = ui->treeBrowser->topLevelItem(root);
    ui->treeBrowser->setBackgroundColor(topItem, QColor(191, 255, 255));
}

void FDEMHydraulicBoundaryWidget::CreateBoundarySpatialOptionProperty()
{
    ui->spaceBrowser->setFactoryForManager(this->manager, this->factory);
    ui->spaceBrowser->setPropertiesWithoutValueMarked(true);
    ui->spaceBrowser->setAlternatingRowColors(true);
    ui->spaceBrowser->setHeaderVisible(false);

    QtVariantProperty *property;
    QtProperty *root = this->manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Spatial Sequence"));
    ui->spaceBrowser->addProperty(root);

    property = this->manager->addProperty(QVariant::Int, QString("SpatialSample"));
    property->setAttribute(QString("minimum"), 0);
    property->setPropertyId(QString("SpatialSample"));
    root->addSubProperty(property);

    //Visibility
    ui->spatialGroup->setVisible(false);

    QtBrowserItem *item = ui->spaceBrowser->topLevelItem(root);
    ui->spaceBrowser->setBackgroundColor(item, QColor(191, 255, 255));
}

void FDEMHydraulicBoundaryWidget::CreateBoundaryTemporalOptionProperty()
{
    ui->timeBrowser->setFactoryForManager(this->manager, this->factory);
    ui->timeBrowser->setPropertiesWithoutValueMarked(true);
    ui->timeBrowser->setAlternatingRowColors(true);
    ui->timeBrowser->setHeaderVisible(false);

    QtVariantProperty *property;
    QtProperty *root = this->manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Temporal Sequence"));
    ui->timeBrowser->addProperty(root);

    property = this->manager->addProperty(QVariant::Int, QString("TemporalSample"));
    property->setAttribute(QString("minimum"), 0);
    property->setPropertyId(QString("TemporalSample"));
    root->addSubProperty(property);

    //Visibility
    ui->temporalGroup->setVisible(false);

    QtBrowserItem *item = ui->timeBrowser->topLevelItem(root);
    ui->timeBrowser->setBackgroundColor(item, QColor(191, 255, 255));
}

void FDEMHydraulicBoundaryWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    { ui->treeBrowser->setItemVisible(item, visible); }
}

void FDEMHydraulicBoundaryWidget::SetPropertyExpanded(const QString &id, bool expanded)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    QtBrowserItem *item = ui->treeBrowser->items(property).first();
    ui->treeBrowser->setExpanded(item, expanded);
}

void FDEMHydraulicBoundaryWidget::UpdateBoundaryConditionProperty()
{
    QtVariantProperty *property;

    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("NodeSetId")));
    property->setValue(boundary->nodeSetId);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("LoadingTypeX")));
    property->setValue(this->boundary->loadingTypeX);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("AmplitudeX")));
    property->setValue(QString::number(this->boundary->amplitudeX));

    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("IsAmplitudeSiteDependent")));
    property->setValue(this->boundary->isAmplitudeSiteDependent);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("IsAmplitudeTimeDependent")));
    property->setValue(this->boundary->isAmplitudeTimeDependent);
}

void FDEMHydraulicBoundaryWidget::UpdateBoundarySpatialOptionProperty()
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("SpatialSample")));
    property->setValue(this->boundary->spatialSample);

    disconnect(ui->spaceTableWidget, &QTableWidget::itemChanged,
               this, &FDEMHydraulicBoundaryWidget::slotSpatialTableWidgetItemChanged);
    for(int i = 0; i < ui->spaceTableWidget->rowCount(); i++)
    {
        for(int j = 0; j < ui->spaceTableWidget->columnCount(); j++)
        {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(boundary->spatialFactor[i][j]));
            item->setTextAlignment(Qt::AlignVCenter | Qt:: AlignRight);
            ui->spaceTableWidget->setItem(i, j, item);
        }
    }
    connect(ui->spaceTableWidget, &QTableWidget::itemChanged,
            this, &FDEMHydraulicBoundaryWidget::slotSpatialTableWidgetItemChanged);
}

void FDEMHydraulicBoundaryWidget::UpdateBoundaryTemporalOptionProperty()
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("TemporalSample")));
    property->setValue(this->boundary->temporalSample);

    disconnect(ui->timeTableWidget, &QTableWidget::itemChanged,
               this, &FDEMHydraulicBoundaryWidget::slotTemporalTableWidgetItemChanged);
    for(int i = 0; i < ui->timeTableWidget->rowCount(); i++)
    {
        for(int j = 0; j < ui->timeTableWidget->columnCount(); j++)
        {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(boundary->temporalFactor[i][j]));
            item->setTextAlignment(Qt::AlignVCenter | Qt:: AlignRight);
            ui->timeTableWidget->setItem(i, j, item);
        }
    }
    connect(ui->timeTableWidget, &QTableWidget::itemChanged,
               this, &FDEMHydraulicBoundaryWidget::slotTemporalTableWidgetItemChanged);
}

void FDEMHydraulicBoundaryWidget::slotHydraulicBrowserValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("NodeSetId"))
    {
        this->boundary->nodeSetId = value.toInt();
    }
    else if(id == QString("LoadingTypeX"))
    {
        this->boundary->loadingTypeX = value.toInt();
    }
    else if(id == QString("AmplitudeX"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->boundary->amplitudeX = val;
    }
    else if(id == QString("IsAmplitudeSiteDependent"))
    {
        this->boundary->isAmplitudeSiteDependent = value.toBool();
        ui->spatialGroup->setVisible(value.toBool());

        QtVariantProperty *property;
        property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("SpatialSample")));
        if(!value.toBool()) property->setValue(0);
    }
    else if(id == QString("IsAmplitudeTimeDependent"))
    {
        this->boundary->isAmplitudeTimeDependent = value.toBool();
        ui->temporalGroup->setVisible(value.toBool());

        QtVariantProperty *property;
        property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("TemporalSample")));
        if(!value.toBool()) property->setValue(0);
    }
    else if(id == QString("SpatialSample"))
    {
        this->boundary->spatialSample = value.toInt();
        ui->spaceTableWidget->setRowCount(value.toInt());
        if(this->boundary->spatialFactor.size() != value.toInt())
        {
            QVector<double> tuple({0.0, 1.0});
            this->boundary->spatialFactor.fill(tuple, value.toInt());
            this->boundary->spatialFactor.squeeze();
        }
    }
    else if(id == QString("TemporalSample"))
    {
        this->boundary->temporalSample = value.toInt();
        ui->timeTableWidget->setRowCount(value.toInt());
        if(this->boundary->temporalFactor.size() != value.toInt())
        {
            QVector<double> tuple({0.0, 1.0});
            this->boundary->temporalFactor.fill(tuple, value.toInt());
            this->boundary->temporalFactor.squeeze();
        }
    }
}

void FDEMHydraulicBoundaryWidget::slotSpatialTableWidgetItemChanged(QTableWidgetItem *item)
{
    item->setTextAlignment(Qt::AlignVCenter | Qt:: AlignRight);
    if(!item->data(Qt::EditRole).isNull())
    {
        bool ok; double value = item->data(Qt::EditRole).toString().toDouble(&ok);
        this->boundary->spatialFactor[item->row()][item->column()] = ok ? value : 0.0;
    }
}

void FDEMHydraulicBoundaryWidget::slotTemporalTableWidgetItemChanged(QTableWidgetItem *item)
{
    item->setTextAlignment(Qt::AlignVCenter | Qt:: AlignRight);
    if(!item->data(Qt::EditRole).isNull())
    {
        bool ok; double value = item->data(Qt::EditRole).toString().toDouble(&ok);
        this->boundary->temporalFactor[item->row()][item->column()] = ok ? value : 0.0;
    }
}
