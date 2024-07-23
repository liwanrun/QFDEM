#include "fracturenetworkdialog.h"
#include "ui_fracturenetworkdialog.h"
#include "fracturenetworkdelegate.h"
#include "fracturenetwork.h"

#include <QMessageBox>
#include <QtVariantProperty>
#include <QStandardItem>
#include <QStandardItemModel>

FractureNetworkDialog::FractureNetworkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FractureNetworkDialog)
{
    ui->setupUi(this);

    this->domainOfInterest = QRectF(0.0, 0.0, 10.0, 10.0);
    this->dfn = QSharedPointer<FractureNetwork>(new FractureNetwork());
    this->factory = new QtVariantEditorFactory(this);


    this->tableModel = new QStandardItemModel(this);
    ui->tableView->setModel(this->tableModel);
    this->delegate = new FractureNetworkDelegate(this);
    ui->tableView->setItemDelegate(this->delegate);
    this->InitFractureTableViewGroup();
    this->InitDomainOfInterestGroup();
    this->InitFractureFunctionGroup();

    this->ConnectSignalSlots();
}

FractureNetworkDialog::~FractureNetworkDialog()
{
    delete tarManager;
    delete srcManager;
    delete dipManager;
    delete modManager;
    delete posManager;
    delete doiManager;

    delete factory;
    delete delegate;
    delete tableModel;

    delete ui;                     
}

void FractureNetworkDialog::InitFractureTableViewGroup()
{
    //Headers
    QFont font(QString("Arial"), -1, QFont::Bold);
    QStringList headers = QStringList() << "Fracture Sets" << "Generate Style" << "Sample Capacity";
    this->tableModel->setHorizontalHeaderLabels(headers);
    for(int col = 0; col < headers.size(); col++)
    {
        QStandardItem *item = this->tableModel->horizontalHeaderItem(col);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFont(font);
    }

    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setColumnWidth(COL_NAME, 150);
    ui->tableView->setColumnWidth(COL_TYPE, 150);
    ui->tableView->setColumnWidth(COL_SIZE, 150);
}

void FractureNetworkDialog::InitDomainOfInterestGroup()
{
    this->doiManager = new QtVariantPropertyManager(this);
    ui->doiWidget->setFactoryForManager(this->doiManager, this->factory);
    ui->doiWidget->setPropertiesWithoutValueMarked(true);
    ui->doiWidget->setAlternatingRowColors(true);
    ui->doiWidget->setHeaderVisible(false);

    QtProperty *doiGroup = doiManager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("Domain Of Interest (DOI)"));
    ui->doiWidget->addProperty(doiGroup);

    QtVariantProperty *domainProperty = doiManager->addProperty(QVariant::RectF, QString("Rectangle"));
    domainProperty->setAttribute(QString("decimals"), 3);
    domainProperty->setPropertyId(QString("DomainofInterest"));
    domainProperty->setValue(this->domainOfInterest);
    doiGroup->addSubProperty(domainProperty);

//  QtBrowserItem *bvpItem = ui->doiWidget->topLevelItem(doiGroup);
//  ui->doiWidget->setBackgroundColor(bvpItem, QColor(255, 255, 191));
}

void FractureNetworkDialog::InitFractureFunctionGroup()
{
    QString caption;
    QStringList names;

    caption = QString("Position Distribution");
    names.clear(); names << "Uniform" << "Fractal" << "Regular";
    this->posManager = new QtVariantPropertyManager(this);
    this->InitFractureDistributionBrowser(ui->posWidget, this->posManager, caption, names, QColor(255, 230, 191));

    caption = QString("Trace Distribution");
    names.clear(); names << "Uniform" << "Exponential" << "Normal" << "LogNormal" << "Weibull" << "DualPower";
    this->modManager = new QtVariantPropertyManager(this);
    this->InitFractureDistributionBrowser(ui->modWidget, this->modManager, caption, names, QColor(191, 255, 191));

    caption = QString("Dip Distribution (degree)");
    names.clear(); names << "Uniform" << "Exponential" << "Normal" <<  "LogNormal" << "Weibull";
    this->dipManager = new QtVariantPropertyManager(this);
    this->InitFractureDistributionBrowser(ui->dipWidget, this->dipManager, caption, names, QColor(191, 255, 255));

    caption = QString("Source Distribution");
    names.clear(); names << "Uniform" << "Fractal" << "Regular";
    this->srcManager = new QtVariantPropertyManager(this);
    this->InitFractureDistributionBrowser(ui->srcWidget, this->srcManager, caption, names, QColor(255, 230, 191));

    caption = QString("Target Distribution");
    names.clear(); names << "Uniform" << "Fractal" << "Regular";
    this->tarManager = new QtVariantPropertyManager(this);
    this->InitFractureDistributionBrowser(ui->tarWidget, this->tarManager, caption, names, QColor(191, 255, 191));

    ui->pointTraceGroup->setVisible(false);
    ui->pointPointGroup->setVisible(false);
}

void FractureNetworkDialog::ConnectSignalSlots()
{
    connect(ui->appendButton, &QPushButton::clicked,
            this, &FractureNetworkDialog::slotAppendButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked,
            this, &FractureNetworkDialog::slotRemoveButtonClicked);
    connect(ui->clearButton,&QPushButton::clicked,
            this, &FractureNetworkDialog::slotClearButtonClicked);

    connect(this->tableModel, &QStandardItemModel::itemChanged,
            this, &FractureNetworkDialog::slotTableModelItemChanged);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &FractureNetworkDialog::slotSelectionChanged);

    connect(this->doiManager, &QtVariantPropertyManager::valueChanged,
            this, &FractureNetworkDialog::slotDomainOfInterestValueChanged);
    connect(this->posManager, &QtVariantPropertyManager::valueChanged,
            this, &FractureNetworkDialog::slotPositionBrowserValueChanged);
    connect(this->modManager, &QtVariantPropertyManager::valueChanged,
            this, &FractureNetworkDialog::slotTraceBrowserValueChanged);
    connect(this->dipManager, &QtVariantPropertyManager::valueChanged,
            this, &FractureNetworkDialog::slotDipBrowserValueChanged);
    connect(this->srcManager, &QtVariantPropertyManager::valueChanged,
            this, &FractureNetworkDialog::slotSourceBrowserValueChanged);
    connect(this->tarManager, &QtVariantPropertyManager::valueChanged,
            this, &FractureNetworkDialog::slotTargetBrowserValueChanged);
}

void FractureNetworkDialog::UpdatePointTraceFractureBrowser(PointTraceFractureSet *fracSet)
{
    this->UpdateFractureDistributionBrowser(this->posManager, fracSet->posPdf);
    this->UpdateFractureDistributionBrowser(this->modManager, fracSet->modPdf);
    this->UpdateFractureDistributionBrowser(this->dipManager, fracSet->dipPdf);
}

void FractureNetworkDialog::UpdatePointPointFractureBrowser(PointPointFractureSet *fracSet)
{
    this->UpdateFractureDistributionBrowser(this->srcManager, fracSet->srcPdf);
    this->UpdateFractureDistributionBrowser(this->tarManager, fracSet->tarPdf);
}

QList<QStandardItem *> FractureNetworkDialog::CreateFractureItems(const QString &fractureId)
{
    //Fracture name
    QStandardItem* name = new QStandardItem(fractureId);
    name->setTextAlignment(Qt::AlignCenter);
    name->setEditable(false);

    //Fracture type
    QStandardItem* type = new QStandardItem;
    type->setData(QString("Point-Trace (PMD)"), Qt::EditRole);
    type->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    //Fracture size
    QStandardItem* size = new QStandardItem;
    size->setData(10, Qt::EditRole);
    size->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QList<QStandardItem *> items;
    items << name << type << size;
    return items;
}

void FractureNetworkDialog::InitFractureDistributionBrowser(QtTreePropertyBrowser *browser, QtVariantPropertyManager *manager,
                                                            const QString& caption, const QStringList &names, QColor bkgColor)
{
    browser->setFactoryForManager(manager, factory);
    browser->setPropertiesWithoutValueMarked(true);
    browser->setAlternatingRowColors(true);
    browser->setHeaderVisible(false);

    QtVariantProperty* property;
    QtVariantProperty* subProperty;

    QtProperty *pdfGroup = manager->addProperty(QtVariantPropertyManager::groupTypeId(), caption);
    browser->addProperty(pdfGroup);

    property = manager->addProperty(QVariant::Int, QString("RandSeed"));
    property->setAttribute(QString("minimum"), 0);
    property->setPropertyId(QString("RandomSeed"));
    pdfGroup->addSubProperty(property);

    property = manager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("PDF Model"));
    property->setAttribute(QString("enumNames"), names);
    property->setPropertyId(QString("FunctionType"));
    pdfGroup->addSubProperty(property);

    subProperty = manager->addProperty(QVariant::Double, QString("[a]"));
    subProperty->setAttribute(QString("decimals"), 3);
    subProperty->setPropertyId(QString("Argument_1"));
    property->addSubProperty(subProperty);

    subProperty = manager->addProperty(QVariant::Double, QString("[b]"));
    subProperty->setAttribute(QString("decimals"), 3);
    subProperty->setPropertyId(QString("Argument_2"));
    property->addSubProperty(subProperty);

    subProperty = manager->addProperty(QVariant::Double, QString("arg_3"));
    subProperty->setAttribute(QString("decimals"), 3);
    subProperty->setPropertyId(QString("Argument_3"));
    property->addSubProperty(subProperty);
    this->SetPropertyVisibility(browser, subProperty, false);

    subProperty = manager->addProperty(QVariant::Double, QString("arg_4"));
    subProperty->setAttribute(QString("decimals"), 3);
    subProperty->setPropertyId(QString("Argument_4"));
    property->addSubProperty(subProperty);
    this->SetPropertyVisibility(browser, subProperty, false);

    subProperty = manager->addProperty(QVariant::Double, QString("arg_5"));
    subProperty->setAttribute(QString("decimals"), 3);
    subProperty->setPropertyId(QString("Argument_5"));
    property->addSubProperty(subProperty);
    this->SetPropertyVisibility(browser, subProperty, false);

    QtBrowserItem *topItem = browser->topLevelItem(pdfGroup);
    browser->setBackgroundColor(topItem, bkgColor);
}

void FractureNetworkDialog::UpdateFractureDistributionBrowser(QtVariantPropertyManager *manager, const PDFInfo &info)
{
    QtVariantProperty *property;
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("RandomSeed")));
    property->setValue(info.m_seed);
    property = static_cast<QtVariantProperty*>(manager->qtProperty(QString("FunctionType")));
    property->setValue(info.m_type);

    QList<QtProperty *> properties = property->subProperties();
    for(int i = 0; i < properties.size(); i++)
    {
        QtVariantProperty * subProp = static_cast<QtVariantProperty*>(properties[i]);
        subProp->setValue(info.m_args[i]);
    }
}

void FractureNetworkDialog::SetPropertyVisibility(QtTreePropertyBrowser *browser, QtProperty *property, bool visible)
{
    for(QtBrowserItem *item : browser->items(property))
    {
        browser->setItemVisible(item, visible);
    }
}

void FractureNetworkDialog::UpdatePointSampleTreeWidget(QtTreePropertyBrowser *browser, QtProperty *property, int index)
{
    QtAbstractPropertyManager *manager = property->propertyManager();
    QtVariantProperty* pdfProperty = static_cast<QtVariantProperty*>(manager->qtProperty(QString("FunctionType")));
    QStringList names = pdfProperty->attributeValue(QString("enumNames")).toStringList();

    if(QString("Uniform") == names[index])
    {
       QtProperty *argProperty;
       argProperty = manager->qtProperty(QString("Argument_1"));
       argProperty->setPropertyName(QString("[a]"));
       this->SetPropertyVisibility(browser, argProperty, true);

       argProperty = manager->qtProperty(QString("Argument_2"));
       argProperty->setPropertyName(QString("[b]"));
       this->SetPropertyVisibility(browser, argProperty, true);

       argProperty = manager->qtProperty(QString("Argument_3"));
       this->SetPropertyVisibility(browser, argProperty, false);

       argProperty = manager->qtProperty(QString("Argument_4"));
       this->SetPropertyVisibility(browser, argProperty, false);

       argProperty = manager->qtProperty(QString("Argument_5"));
       this->SetPropertyVisibility(browser, argProperty, false);
    }
    else if(QString("Fractal") == names[index])
    {
        QtProperty *argProperty;
        argProperty = manager->qtProperty(QString("Argument_1"));
        argProperty->setPropertyName(QString("[") + QString(0X03B1) + QString("]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_2"));
        argProperty->setPropertyName(QString("[D]{1.3, 2.0}"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_3"));
        argProperty->setPropertyName(QString("[a]{1.3, 3.5}"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_4"));
        argProperty->setPropertyName(QString("[N]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_5"));
        this->SetPropertyVisibility(browser, argProperty, false);
    }
    else if(QString("Regular") == names[index])
    {
        QtProperty *argProperty;
        argProperty = manager->qtProperty(QString("Argument_1"));
        argProperty->setPropertyName(QString("[m]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_2"));
        argProperty->setPropertyName(QString("[n]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_3"));
        this->SetPropertyVisibility(browser, argProperty, false);

        argProperty = manager->qtProperty(QString("Argument_4"));
        this->SetPropertyVisibility(browser, argProperty, false);

        argProperty = manager->qtProperty(QString("Argument_5"));
        this->SetPropertyVisibility(browser, argProperty, false);
    }
}

void FractureNetworkDialog::UpdateValueSampleTreeWidget(QtTreePropertyBrowser *browser, QtProperty *property, int index)
{
    QtAbstractPropertyManager *manager = property->propertyManager();
    QtVariantProperty* pdfProperty = static_cast<QtVariantProperty*>(manager->qtProperty(QString("FunctionType")));
    QStringList names = pdfProperty->attributeValue(QString("enumNames")).toStringList();

    if(QString("Uniform") == names[index])
    {
        QtProperty *argProperty;
        argProperty = manager->qtProperty(QString("Argument_1"));
        argProperty->setPropertyName(QString("[a]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_2"));
        argProperty->setPropertyName(QString("[b]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_3"));
        this->SetPropertyVisibility(browser, argProperty, false);

        argProperty = manager->qtProperty(QString("Argument_4"));
        this->SetPropertyVisibility(browser, argProperty, false);
    }
    else if(QString("Exponential") == names[index])
    {
        QtProperty *argProperty;
        argProperty = manager->qtProperty(QString("Argument_1"));
        argProperty->setPropertyName(QString("[") + QString(0x03BB) + QString("]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_2"));
        argProperty->setPropertyName(QString("[Lmin]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_3"));
        argProperty->setPropertyName(QString("[Lmax]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_4"));
        this->SetPropertyVisibility(browser, argProperty, false);

        argProperty = manager->qtProperty(QString("Argument_5"));
        this->SetPropertyVisibility(browser, argProperty, false);
    }
    else if(QString("Normal") == names[index] || QString("LogNormal") == names[index])
    {
        QtProperty *argProperty;
        argProperty = manager->qtProperty(QString("Argument_1"));
        argProperty->setPropertyName(QString("[") + QString(0x03BC) + QString("]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_2"));
        argProperty->setPropertyName(QString("[") + QString(0x03C3) + QString("]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_3"));
        this->SetPropertyVisibility(browser, argProperty, false);

        argProperty = manager->qtProperty(QString("Argument_4"));
        this->SetPropertyVisibility(browser, argProperty, false);

        argProperty = manager->qtProperty(QString("Argument_5"));
        this->SetPropertyVisibility(browser, argProperty, false);
    }
    else if(QString("Weibull") == names[index])
    {
        QtProperty *argProperty;
        argProperty = manager->qtProperty(QString("Argument_1"));
        argProperty->setPropertyName(QString("[m]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_2"));
        argProperty->setPropertyName(QString("[") + QString(0x03BC) + QString("]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_3"));
        this->SetPropertyVisibility(browser, argProperty, false);

        argProperty = manager->qtProperty(QString("Argument_4"));
        this->SetPropertyVisibility(browser, argProperty, false);

        argProperty = manager->qtProperty(QString("Argument_5"));
        this->SetPropertyVisibility(browser, argProperty, false);
    }
    else if(QString("DualPower") == names[index])
    {
        QtProperty *argProperty;
        argProperty = manager->qtProperty(QString("Argument_1"));
        argProperty->setPropertyName(QString("[a]{1.3, 3.5}"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_2"));
        argProperty->setPropertyName(QString("[Lr]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_3"));
        argProperty->setPropertyName(QString("[N]"));
        this->SetPropertyVisibility(browser, argProperty, true);

        argProperty = manager->qtProperty(QString("Argument_4"));
        this->SetPropertyVisibility(browser, argProperty, false);

        argProperty = manager->qtProperty(QString("Argument_5"));
        this->SetPropertyVisibility(browser, argProperty, false);
    }
}

void FractureNetworkDialog::UpdateFractureSetMetaInfo(const QString &id, const QVariant &value, PDFInfo &info)
{
    if(id == QString("RandomSeed"))
    {
        info.m_seed = value.toInt();
    }
    else if(id == QString("FunctionType"))
    {
        info.m_type = value.toInt();
    }
    else if(id == QString("Argument_1"))
    {
        info.m_args[0] = value.toDouble();
    }
    else if(id == QString("Argument_2"))
    {
        info.m_args[1] = value.toDouble();
    }
    else if(id == QString("Argument_3"))
    {
        info.m_args[2] = value.toDouble();
    }
    else if(id == QString("Argument_4"))
    {
        info.m_args[3] = value.toDouble();
    }
    else if(id == QString("Argument_5"))
    {
        info.m_args[4] = value.toDouble();
    }
}

void FractureNetworkDialog::slotAppendButtonClicked()
{
    int index = this->tableModel->rowCount() + 1;
    QString fracId = QString("fracture-%1").arg(index);

    this->dfn->AppendFractureSet(fracId);
    this->tableModel->appendRow(this->CreateFractureItems(fracId));
}

void FractureNetworkDialog::slotRemoveButtonClicked()
{
    int index = this->tableModel->rowCount();
    QString fracId = QString("fracture-%1").arg(index);

    this->tableModel->removeRow(this->tableModel->rowCount() - 1);
    this->dfn->RemoveFractureSet(fracId);

    //Update DFN dialog
    ui->pointTraceGroup->setVisible(this->tableModel->rowCount());
    ui->pointPointGroup->setVisible(this->tableModel->rowCount());
}

void FractureNetworkDialog::slotClearButtonClicked()
{
    this->tableModel->removeRows(0, this->tableModel->rowCount());
    this->dfn->Clear();

    //Update DFN dialog
    ui->pointTraceGroup->setVisible(false);
    ui->pointPointGroup->setVisible(false);
}

void FractureNetworkDialog::slotTableModelItemChanged(QStandardItem *item)
{
    QString id = this->tableModel->item(item->row(), 0)->data(Qt::DisplayRole).toString();

    if(COL_TYPE == item->column())
    {
        QString type = item->data(Qt::EditRole).toString();
        if(QString("Point-Trace (PMD)") == type)
        {
            QSharedPointer<PointTraceFractureSet> fracture =
                    QSharedPointer<PointTraceFractureSet>(new PointTraceFractureSet);
            this->dfn->UpdateFractureSet(id, fracture);
            this->UpdatePointTraceFractureBrowser(fracture.get());

            ui->pointTraceGroup->setVisible(true);
            ui->pointPointGroup->setVisible(false);
        }
        else if(QString("Point-Point (PTP)") == type)
        {
            QSharedPointer<PointPointFractureSet> fracture =
                    QSharedPointer<PointPointFractureSet>(new PointPointFractureSet);
            this->dfn->UpdateFractureSet(id, fracture);
            this->UpdatePointPointFractureBrowser(fracture.get());

            ui->pointTraceGroup->setVisible(false);
            ui->pointPointGroup->setVisible(true);
        }
    }
    else if(COL_SIZE == item->column())
    {
        QSharedPointer<FractureSet> fracture = this->dfn->GetFractureSet(id);
        fracture->m_size = item->data(Qt::EditRole).toULongLong();
    }
}

void FractureNetworkDialog::slotSelectionChanged(const QItemSelection &selection, const QItemSelection &)
{
    if(selection.isEmpty()) return;

    QModelIndex index = selection.indexes().at(COL_TYPE);
    QString type = index.data().toString();
    QString fracId = selection.indexes().at(COL_NAME).data().toString();

    if(QString("Point-Trace (PMD)") == type)
    {
        QSharedPointer<FractureSet> fracture = this->dfn->GetFractureSet(fracId);
        this->UpdatePointTraceFractureBrowser(static_cast<PointTraceFractureSet*>(fracture.data()));

        ui->pointTraceGroup->setVisible(true);
        ui->pointPointGroup->setVisible(false);
    }
    else if(QString("Point-Point (PTP)") == type)
    {
        QSharedPointer<FractureSet> fracture = this->dfn->GetFractureSet(fracId);
        this->UpdatePointPointFractureBrowser(static_cast<PointPointFractureSet*>(fracture.data()));

        ui->pointTraceGroup->setVisible(false);
        ui->pointPointGroup->setVisible(true);
    }
}

void FractureNetworkDialog::slotDomainOfInterestValueChanged(QtProperty *property, const QVariant &value)
{
    QString id = property->propertyId();

    if(id == QString("DomainofInterest"))
    {
        this->domainOfInterest = value.toRectF();
    }
}

void FractureNetworkDialog::slotPositionBrowserValueChanged(QtProperty *property, const QVariant &value)
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QString name = selection.indexes().at(COL_NAME).data().toString();
    QSharedPointer<FractureSet> fracSet = this->dfn->GetFractureSet(name);
    PDFInfo& info = static_cast<PointTraceFractureSet*>(fracSet.data())->posPdf;

    QString id = property->propertyId();
    this->UpdateFractureSetMetaInfo(id, value, info);

    if(id == QString("FunctionType"))
    {
        this->UpdatePointSampleTreeWidget(ui->posWidget, property, value.toInt());

        if(1 == value.toInt())
        {
            QtVariantProperty *property = static_cast<QtVariantProperty *>
                    (this->modManager->qtProperty(QString("FunctionType")));
            property->setValue(DUALPOWER);
        }
    }
}

void FractureNetworkDialog::slotTraceBrowserValueChanged(QtProperty *property, const QVariant &value)
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QString name = selection.indexes().at(COL_NAME).data().toString();
    QSharedPointer<FractureSet> fracSet = this->dfn->GetFractureSet(name);
    PDFInfo& info = static_cast<PointTraceFractureSet*>(fracSet.data())->modPdf;

    QString id = property->propertyId();
    this->UpdateFractureSetMetaInfo(id, value, info);

    if(id == QString("FunctionType"))
    {
        this->UpdateValueSampleTreeWidget(ui->modWidget, property, value.toInt());
    }
}

void FractureNetworkDialog::slotDipBrowserValueChanged(QtProperty *property, const QVariant &value)
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QString name = selection.indexes().at(COL_NAME).data().toString();
    QSharedPointer<FractureSet> fracSet = this->dfn->GetFractureSet(name);
    PDFInfo& info = static_cast<PointTraceFractureSet*>(fracSet.data())->dipPdf;

    QString id = property->propertyId();
    this->UpdateFractureSetMetaInfo(id, value, info);

    if(id == QString("FunctionType"))
    {
        this->UpdateValueSampleTreeWidget(ui->dipWidget, property, value.toInt());
    }
}

void FractureNetworkDialog::slotSourceBrowserValueChanged(QtProperty *property, const QVariant &value)
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QString name = selection.indexes().at(COL_NAME).data().toString();
    QSharedPointer<FractureSet> fracSet = this->dfn->GetFractureSet(name);
    PDFInfo& info = static_cast<PointPointFractureSet*>(fracSet.data())->srcPdf;

    QString id = property->propertyId();
    this->UpdateFractureSetMetaInfo(id, value, info);

    if(id == QString("FunctionType"))
    {
        this->UpdatePointSampleTreeWidget(ui->srcWidget, property, value.toInt());
    }
}

void FractureNetworkDialog::slotTargetBrowserValueChanged(QtProperty *property, const QVariant &value)
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    QString name = selection.indexes().at(COL_NAME).data().toString();
    QSharedPointer<FractureSet> fracSet = this->dfn->GetFractureSet(name);
    PDFInfo& info = static_cast<PointPointFractureSet*>(fracSet.data())->tarPdf;

    QString id = property->propertyId();
    this->UpdateFractureSetMetaInfo(id, value, info);

    if(id == QString("FunctionType"))
    {
        this->UpdatePointSampleTreeWidget(ui->tarWidget, property, value.toInt());
    }
}

QRectF &FractureNetworkDialog::GetDomainOfInterest()
{
    return this->domainOfInterest;
}

QSharedPointer<FractureNetwork> FractureNetworkDialog::GetFractureNetwork()
{
    return this->dfn;
}
