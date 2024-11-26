#include "fdemcontactwidget.h"
#include "ui_fdemcontactwidget.h"
#include "contact.h"
#include "blockcollection.h"

#include <QDebug>
#include <QtVariantProperty>

FDEMContactWidget::FDEMContactWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMContactWidget)
{
    ui->setupUi(this);

    this->contact = Contact::GetContactProperty();

    this->manager = new QtVariantPropertyManager(this);
    this->factory = new QtVariantEditorFactory(this);
    ui->treeBrowser->setFactoryForManager(this->manager, this->factory);

    ui->treeBrowser->setPropertiesWithoutValueMarked(true);
    ui->treeBrowser->setAlternatingRowColors(true);
    ui->treeBrowser->setHeaderVisible(false);

    this->InitPropertyBrowser();
    this->ConnectSignalSlots();
}

FDEMContactWidget::~FDEMContactWidget()
{
    delete ui;
}

void FDEMContactWidget::InitPropertyBrowser()
{
    QStringList names;
    QtVariantProperty* property;

    //1.Property Initialization
    QtProperty *root = manager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Configuration"));
    ui->treeBrowser->addProperty(root);

    property = manager->addProperty(QVariant::Int, QString("CoupleCapacity"));
    property->setAttribute(QString("minimum"), 1);
    property->setPropertyId(QString("CoupleCapacity"));
    property->setValue(contact->coupleCapacity);
    root->addSubProperty(property);

    property = manager->addProperty(QVariant::Double, QString("BufferZoneSize"));
    property->setAttribute(QString("minimum"), 0.0);
    property->setAttribute(QString("decimals"), 6);
    property->setPropertyId(QString("BufferZoneSize"));
    property->setValue(contact->bufferZoneSize);
    root->addSubProperty(property);

    property = manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("ContactAlgorithm"));
    names.clear();
    names << "Adaptive" << "Ubiquitous" << "Interface";
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ContactAlgorithm"));
    property->setValue(contact->contactAlgorithm);
    root->addSubProperty(property);

    property = manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("ContactDetection"));
    names.clear();
    names << "MunjizaNBS" << "MigrateALG";
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ContactDetection"));
    property->setValue(contact->contactDetection);
    root->addSubProperty(property);

    property = manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("ContactPotential"));
    names.clear();
    names << "Area-based" << "Distance-based";
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("ContactPotential"));
    property->setValue(contact->contactPotential);
    root->addSubProperty(property);

    for(QtBrowserItem *item : ui->treeBrowser->topLevelItems())
    { ui->treeBrowser->setBackgroundColor(item, QColor(255, 255, 191)); }
}

void FDEMContactWidget::ConnectSignalSlots()
{
    connect(ui->updateButton, &QPushButton::clicked,
            this, &FDEMContactWidget::slotUpdateButtonClicked);
    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &FDEMContactWidget::slotTreeBrowserValueChanged);
}

void FDEMContactWidget::slotUpdateButtonClicked()
{
    //percentage of the global minimum characteristic length
    QSharedPointer<BlockCollection> blockCollection = BlockCollection::GetBlockCollection();
    double buffer_size = 0.2*blockCollection->minimumMeshSize;
    QString msg = QString("[>>>] Buffer zone size:      [%1]").arg(buffer_size);
    emit this->signalOutputPromptMessage(msg);
}

void FDEMContactWidget::slotTreeBrowserValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(QString("CoupleCapacity") == id)
    {
        this->contact->coupleCapacity = value.toInt();
    }
    else if(QString("BufferZoneSize") == id)
    {
        this->contact->bufferZoneSize = value.toDouble();
    }
    else if(QString("ContactAlgorithm") == id)
    {
        this->contact->contactAlgorithm = value.toInt();
    }
    else if(QString("ContactDetection") == id)
    {
        this->contact->contactDetection = value.toInt();
    }
    //Interaction
    else if(QString("ContactPotential") == id)
    {
        this->contact->contactPotential = value.toInt();
    }
}
