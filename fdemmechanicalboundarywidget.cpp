#include "fdemmechanicalboundarywidget.h"
#include "ui_fdemmechanicalboundarywidget.h"
#include "boundary.h"

#include <QMessageBox>
#include <QtVariantProperty>

FDEMMechanicalBoundaryWidget::FDEMMechanicalBoundaryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMMechanicalBoundaryWidget)
{
    ui->setupUi(this);

    this->manager = new QtVariantPropertyManager(this);
    this->factory = new QtVariantEditorFactory(this);

    this->InitPropertyBrowser();
    this->ConnectSignalSlots();
}

FDEMMechanicalBoundaryWidget::~FDEMMechanicalBoundaryWidget()
{
    delete this->factory;
    delete this->manager;

    delete ui;
}

void FDEMMechanicalBoundaryWidget::InitPropertyBrowser()
{
    this->CreateBoundaryConditionProperty();
    this->CreateBoundarySpatialOptionProperty();
    this->CreateBoundaryTemporalOptionProperty();
}

void FDEMMechanicalBoundaryWidget::ConnectSignalSlots()
{
    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &FDEMMechanicalBoundaryWidget::slotPropertyBrowserValueChanged);
    connect(ui->spaceTableWidget, &QTableWidget::itemChanged,
            this, &FDEMMechanicalBoundaryWidget::slotSpatialTableWidgetItemChanged);
    connect(ui->timeTableWidget, &QTableWidget::itemChanged,
            this, &FDEMMechanicalBoundaryWidget::slotTemporalTableWidgetItemChanged);
}

void FDEMMechanicalBoundaryWidget::UpdatePropertyBrowser(MechanicalBoundary *bnd)
{
    this->boundary = bnd;

    this->UpdateBoundaryConditionProperty();
    this->UpdateBoundarySpatialOptionProperty();
    this->UpdateBoundaryTemporalOptionProperty();
}

void FDEMMechanicalBoundaryWidget::SetPropertyVisibility(const QString &id, bool visible)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    for(QtBrowserItem *item : ui->treeBrowser->items(property))
    { ui->treeBrowser->setItemVisible(item, visible); }
}

void FDEMMechanicalBoundaryWidget::SetPropertyExpanded(const QString &id, bool expanded)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(id));
    QtBrowserItem *item = ui->treeBrowser->items(property).first();
    ui->treeBrowser->setExpanded(item, expanded);
}

void FDEMMechanicalBoundaryWidget::CreateBoundaryConditionProperty()
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

    QStringList mbNames;
    mbNames << "Nodal force (0)" << "Acceleration" << "Velocity" << "Surface traction";
    property = this->manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("LoadingTypeX"));
    property->setAttribute(QString("enumNames"), mbNames);
    property->setPropertyId(QString("LoadingTypeX"));
    root->addSubProperty(property);

    subProperty = this->manager->addProperty(QVariant::String, QString("ampX"));
    subProperty->setAttribute(QString("decimals"), 6);
    subProperty->setPropertyId(QString("AmplitudeX"));
    property->addSubProperty(subProperty);

    property = this->manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("LoadingTypeY"));
    property->setAttribute(QString("enumNames"), mbNames);
    property->setPropertyId(QString("LoadingTypeY"));
    root->addSubProperty(property);

    subProperty = this->manager->addProperty(QVariant::String, QString("ampY"));
    subProperty->setAttribute(QString("decimals"), 6);
    subProperty->setPropertyId(QString("AmplitudeY"));
    property->addSubProperty(subProperty);

    property = this->manager->addProperty(QVariant::Bool, QString("IsContactBoundaryEnabled"));
    property->setPropertyId(QString("IsContactBoundaryEnabled"));
    root->addSubProperty(property);

    property = this->manager->addProperty(QVariant::Bool, QString("IsArtificialBoundaryEnabled"));
    property->setPropertyId(QString("IsArtificialBoundaryEnabled"));
    root->addSubProperty(property);

    property = this->manager->addProperty(QVariant::Bool, QString("IsAmplitudeSiteDependent"));
    property->setPropertyId(QString("IsAmplitudeSiteDependent"));
    root->addSubProperty(property);

    property = this->manager->addProperty(QVariant::Bool, QString("IsAmplitudeTimeDependent"));
    property->setPropertyId(QString("IsAmplitudeTimeDependent"));
    root->addSubProperty(property);

    QStringList vbNames;
    subProperty = this->manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("ArtificialBoundary"));
    vbNames << "Viscous BC" << "Viscoelastic BC" << "*Free-field BC*" << "*Static-dynamic BC*";
    subProperty->setAttribute(QString("enumNames"), vbNames);
    subProperty->setPropertyId(QString("ArtificialBoundary"));
    property->addSubProperty(subProperty);
    this->SetPropertyVisibility(QString("ArtificialBoundary"), false);

    QtBrowserItem *topItem = ui->treeBrowser->topLevelItem(root);
    ui->treeBrowser->setBackgroundColor(topItem, QColor(255, 255, 191));
}

void FDEMMechanicalBoundaryWidget::CreateBoundarySpatialOptionProperty()
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
    ui->spaceBrowser->setBackgroundColor(item, QColor(255, 255, 191));
}

void FDEMMechanicalBoundaryWidget::CreateBoundaryTemporalOptionProperty()
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
    ui->timeBrowser->setBackgroundColor(item, QColor(255, 255, 191));
}

void FDEMMechanicalBoundaryWidget::UpdateBoundaryConditionProperty()
{
    QtVariantProperty *property;

    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("NodeSetId")));
    property->setValue(boundary->nodeSetId);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("LoadingTypeX")));
    property->setValue(this->boundary->loadingTypeX);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("AmplitudeX")));
    property->setValue(QString::number(this->boundary->amplitudeX));
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("LoadingTypeY")));
    property->setValue(this->boundary->loadingTypeY);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("AmplitudeY")));
    property->setValue(QString::number(this->boundary->amplitudeY));

    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("IsContactBoundaryEnabled")));
    property->setValue(this->boundary->isContactBoundaryEnabled);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("IsArtificialBoundaryEnabled")));
    property->setValue(this->boundary->isArtificialBoundaryEnabled);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("ArtificialBoundary")));
    property->setValue(this->boundary->artificialBoundary);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("IsAmplitudeSiteDependent")));
    property->setValue(this->boundary->isAmplitudeSiteDependent);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("IsAmplitudeTimeDependent")));
    property->setValue(this->boundary->isAmplitudeTimeDependent);
}

void FDEMMechanicalBoundaryWidget::UpdateBoundarySpatialOptionProperty()
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("SpatialSample")));
    property->setValue(this->boundary->spatialSample);

    disconnect(ui->spaceTableWidget, &QTableWidget::itemChanged,
               this, &FDEMMechanicalBoundaryWidget::slotSpatialTableWidgetItemChanged);
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
            this, &FDEMMechanicalBoundaryWidget::slotSpatialTableWidgetItemChanged);
}

void FDEMMechanicalBoundaryWidget::UpdateBoundaryTemporalOptionProperty()
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("TemporalSample")));
    property->setValue(this->boundary->temporalSample);

    disconnect(ui->timeTableWidget, &QTableWidget::itemChanged,
               this, &FDEMMechanicalBoundaryWidget::slotTemporalTableWidgetItemChanged);
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
               this, &FDEMMechanicalBoundaryWidget::slotTemporalTableWidgetItemChanged);
}

void FDEMMechanicalBoundaryWidget::slotPropertyBrowserValueChanged(QtProperty *property, const QVariant &value)
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
    else if(id == QString("LoadingTypeY"))
    {
        this->boundary->loadingTypeY = value.toInt();
    }
    else if(id == QString("AmplitudeY"))
    {
        bool ok; double val = value.toDouble(&ok);
        if(ok) this->boundary->amplitudeY = val;
    }
    else if(id == QString("IsContactBoundaryEnabled"))
    {
        this->boundary->isContactBoundaryEnabled = value.toBool();
    }
    else if(id == QString("IsArtificialBoundaryEnabled"))
    {
        this->boundary->isArtificialBoundaryEnabled = value.toBool();
        this->boundary->artificialBoundary = value.toBool() ? 0 : -1;
        this->SetPropertyVisibility(QString("ArtificialBoundary"), value.toBool());
    }
    else if(id == QString("ArtificialBoundary"))
    {
        this->boundary->artificialBoundary = value.toInt();
    }
    else if(id == QString("IsAmplitudeSiteDependent"))
    {
        this->boundary->isAmplitudeSiteDependent = value.toBool();
        ui->spatialGroup->setVisible(value.toBool());

        QtVariantProperty *property;
        property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("SpatialSample")));
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
    else if(id == QString("IsAmplitudeTimeDependent"))
    {
        this->boundary->isAmplitudeTimeDependent = value.toBool();
        ui->temporalGroup->setVisible(value.toBool());

        QtVariantProperty *property;
        property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("TemporalSample")));
        if(!value.toBool()) property->setValue(0);
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

void FDEMMechanicalBoundaryWidget::slotSpatialTableWidgetItemChanged(QTableWidgetItem *item)
{
    item->setTextAlignment(Qt::AlignVCenter | Qt:: AlignRight);
    if(!item->data(Qt::EditRole).isNull())
    {
        bool ok; double value = item->data(Qt::EditRole).toString().toDouble(&ok);
        this->boundary->spatialFactor[item->row()][item->column()] = ok ? value : 0.0;
    }
}

void FDEMMechanicalBoundaryWidget::slotTemporalTableWidgetItemChanged(QTableWidgetItem *item)
{
    item->setTextAlignment(Qt::AlignVCenter | Qt:: AlignRight);
    if(!item->data(Qt::EditRole).isNull())
    {
        bool ok; double value = item->data(Qt::EditRole).toString().toDouble(&ok);
        this->boundary->temporalFactor[item->row()][item->column()] = ok ? value : 0.0;
    }
}
