#include "backgroundgriddialog.h"
#include "ui_backgroundgriddialog.h"
#include "fdemvtkrenderwidget.h"

#include <QDebug>
#include <QtVariantProperty>

BackgroundGridDialog::BackgroundGridDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BackgroundGridDialog)
{
    ui->setupUi(this);
    this->bkgGrid = BackgroundGrid::GetBackgroundGrid();

    this->InitPropertyBrowser();
    this->ConnectSignalToSlot();
}

BackgroundGridDialog::~BackgroundGridDialog()
{
    delete this->manager;
    delete this->factory;
    
    delete ui;
}

void BackgroundGridDialog::InitPropertyBrowser()
{
    this->manager = new QtVariantPropertyManager(this);
    this->factory = new QtVariantEditorFactory(this);
    ui->gridWidget->setFactoryForManager(manager, factory);

    ui->cartesianCoordButton->setChecked(BackgroundGrid::RECTANGLE == bkgGrid->coordType);
    ui->polarCoordButton->setChecked(BackgroundGrid::POLARGRID == bkgGrid->coordType);
    
    QStringList enumNames;
    enumNames << "North" << "East" << "South" << "West";

    QtIconMap enumIcons;
    enumIcons[0] = QIcon(":/images/resources/up.png");
    enumIcons[1] = QIcon(":/images/resources/right.png");
    enumIcons[2] = QIcon(":/images/resources/down.png");
    enumIcons[3] = QIcon(":/images/resources/left.png");

    QtVariantProperty *property, *subProperty;

    property = manager->addProperty(QVariant::PointF,QLatin1String("Location"));
    property->setAttribute(QString("decimals"), 6);
    property->setPropertyId(QLatin1String("Location"));
    property->setValue(bkgGrid->location);
    ui->gridWidget->addProperty(property);

    property = manager->addProperty(QtVariantPropertyManager::groupTypeId(),QLatin1String("Orientation"));
    ui->gridWidget->addProperty(property);
    subProperty = manager->addProperty(QtVariantPropertyManager::enumTypeId(),QLatin1String("Direction"));
    subProperty->setAttribute(QLatin1String("enumNames"), enumNames);
    subProperty->setAttribute(QLatin1String("enumIcons"), QVariant::fromValue(enumIcons));
    subProperty->setPropertyId(QLatin1String("Direction"));
    subProperty->setValue(bkgGrid->direction);
    property->addSubProperty(subProperty);

    property = this->manager->addProperty(QVariant::RectF, QString("Bounding"));
    property->setAttribute(QString("decimals"), 6);
    QList<QtProperty*> bndChildren = property->subProperties();
    bndChildren[2]->setPropertyName(0 == bkgGrid->coordType ? QString("Width") : QString("Radius"));
    bndChildren[3]->setPropertyName(0 == bkgGrid->coordType ? QString("Height") : QString("Angle"));
    property->setPropertyId(QString("Bounding"));
    property->setValue(bkgGrid->bounding);
    ui->gridWidget->addProperty(property);

    property = this->manager->addProperty(QVariant::Point, QString("Dimension"));
    QList<QtProperty*> dimChildren = property->subProperties();
    dimChildren[0]->setPropertyName(QString("numX"));
    dimChildren[1]->setPropertyName(QString("numY"));
    property->setAttribute(QString("minimum"), 1);
    property->setPropertyId(QString("Dimension"));
    property->setValue(bkgGrid->dimension);
    ui->gridWidget->addProperty(property);

    ui->buttonBox->button(QDialogButtonBox::Apply)->setDefault(true);
}

void BackgroundGridDialog::ConnectSignalToSlot()
{
    connect(this->manager, &QtVariantPropertyManager::valueChanged,
            this, &BackgroundGridDialog::slotPropertyBrowserValueChanged);
    connect(ui->cartesianCoordButton, &QToolButton::clicked,
            this, &BackgroundGridDialog::slotCartesianCoordClicked);
    connect(ui->polarCoordButton, &QToolButton::clicked,
            this, &BackgroundGridDialog::slotPolarCoordClicked);
    connect(ui->buttonBox, &QDialogButtonBox::clicked,
            this, &BackgroundGridDialog::slotDialogButtonBoxClicked);
}

void BackgroundGridDialog::slotCartesianCoordClicked(bool checked)
{
    if(checked)
    {
        bkgGrid->coordType = BackgroundGrid::RECTANGLE;
        QtProperty *property = this->manager->qtProperty(QString("Bounding"));
        QList<QtProperty*> children = property->subProperties();
        children[2]->setPropertyName(QString("Width"));
        children[3]->setPropertyName(QString("Height"));

        QtVariantProperty* variantProp;
        variantProp = static_cast<QtVariantProperty*>(children[2]);
        variantProp->setValue(1.0);
        variantProp = static_cast<QtVariantProperty*>(children[3]);
        variantProp->setValue(1.0);
    }
}

void BackgroundGridDialog::slotPolarCoordClicked(bool checked)
{
    if(checked)
    {
        bkgGrid->coordType = BackgroundGrid::POLARGRID;
        QtProperty *property = this->manager->qtProperty(QString("Bounding"));
        QList<QtProperty*> children = property->subProperties();
        children[2]->setPropertyName(QString("Radius"));
        children[3]->setPropertyName(QString("Angle"));

        QtVariantProperty* variantProp;
        variantProp = static_cast<QtVariantProperty*>(children[2]);
        variantProp->setValue(1.0);
        variantProp = static_cast<QtVariantProperty*>(children[3]);
        variantProp->setValue(360.0);
    }
}

void BackgroundGridDialog::slotPropertyBrowserValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(QString("Direction") == id)
    {
        bkgGrid->direction = value.toInt();
    }
    else if(QString("Location") == id)
    {
        bkgGrid->location = value.toPointF();
    }
    else if(QString("Bounding") == id)
    {
        bkgGrid->bounding = value.toRectF();
    }
    else if(QString("Dimension") == id)
    {
        bkgGrid->dimension = value.toPoint();
    }
}

void BackgroundGridDialog::slotDialogButtonBoxClicked(QAbstractButton *button)
{
    if(FDEMVTKRenderWidget *parent = static_cast<FDEMVTKRenderWidget*>(this->parentWidget()))
    {
        switch (ui->buttonBox->standardButton(button))
        {
        case QDialogButtonBox::Ok:
        case QDialogButtonBox::Apply:
        {
            parent->UpdateBackgroundGrid();
            break;
        }
        default:
            break;
        }
    }
}
