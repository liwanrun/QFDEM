#include "fdemfieldhistorywidget.h"
#include "ui_fdemfieldhistorywidget.h"
#include "history.h"

#include <QtVariantProperty>

FDEMFieldHistoryWidget::FDEMFieldHistoryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMFieldHistoryWidget)
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

FDEMFieldHistoryWidget::~FDEMFieldHistoryWidget()
{
    delete this->manager;
    delete this->factory;

    delete ui;
}

void FDEMFieldHistoryWidget::InitPropertyBrowser()
{
    QStringList names;
    QtVariantProperty *property;

    //Property
    QtProperty *historyGroup = this->manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Parameter"));
    historyGroup->setPropertyId(QString("HistoryParameter"));
    ui->treeBrowser->addProperty(historyGroup);

    property = this->manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("Variable"));
    names.clear();
    names << "Kinetic Energy" << "Acoustic Emission";
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("HistVariable"));
    historyGroup->addSubProperty(property);

    property = this->manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("Operation"));
    names.clear();
    names << "Enumeration" << "Summation" << "Average";
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("HistOperation"));
    historyGroup->addSubProperty(property);

    property = this->manager->addProperty(QVariant::Int, QString("Frequency"));
    property->setAttribute(QString("minimum"), 1);
    property->setPropertyId(QString("HistFrequency"));
    historyGroup->addSubProperty(property);

    property = this->manager->addProperty(QVariant::Double, QString("TimeScaleFactor"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("TimeScaleFactor"));
    historyGroup->addSubProperty(property);

    property = this->manager->addProperty(QVariant::Double, QString("HistScaleFactor"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("maximum"), 1.0);
    property->setAttribute(QString("decimals"), 3);
    property->setPropertyId(QString("HistScaleFactor"));
    historyGroup->addSubProperty(property);

    //Background
    QtBrowserItem *histItem = ui->treeBrowser->topLevelItem(historyGroup);
    ui->treeBrowser->setBackgroundColor(histItem, QColor(255, 255, 191));
}

void FDEMFieldHistoryWidget::ConnectSignalSlots()
{
    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &FDEMFieldHistoryWidget::slotTreeBrowserValueChanged);
}

void FDEMFieldHistoryWidget::UpdatePropertyBrowser(GlobalHistory *hist)
{
    this->history = hist;

    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("HistVariable")));
    property->setValue(this->history->histVariable);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("HistOperation")));
    property->setValue(this->history->histOperation);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("HistFrequency")));
    property->setValue(this->history->histFrequency);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("TimeScaleFactor")));
    property->setValue(this->history->timeScaleFactor);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("HistScaleFactor")));
    property->setValue(this->history->histScaleFactor);
}

void FDEMFieldHistoryWidget::slotTreeBrowserValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("HistVariable"))
    {
        this->history->histVariable = value.toInt();
    }
    else if(id == QString("HistOperation"))
    {
        this->history->histOperation = value.toInt();
    }
    else if(id == QString("HistFrequency"))
    {
        this->history->histFrequency = value.toInt();
    }
    else if(id == QString("TimeScaleFactor"))
    {
        this->history->timeScaleFactor = value.toDouble();
    }
    else if(id == QString("HistScaleFactor"))
    {
        this->history->histScaleFactor = value.toDouble();
    }
}
