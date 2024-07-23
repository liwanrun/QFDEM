#include "fdemtemporalwidget.h"
#include "ui_fdemtemporalwidget.h"
#include "fdemtemporalitemdelegate.h"
#include "qttreepropertybrowser.h"
#include "fdemmechanicaltemporalwidget.h"
#include "fdemhydraulictemporalwidget.h"
#include "fdemthermaltemporalwidget.h"
#include "fdemtemporalstressstrainwidget.h"
#include "blockcollection.h"
#include "actorcollection.h"
#include "selectcollection.h"
#include "temporalcollection.h"
#include "temporal.h"
#include "groupcollection.h"
#include "group.h"
#include "globalcollection.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItem>
#include <QStandardItemModel>

FDEMTemporalWidget::FDEMTemporalWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMTemporalWidget)
{
    ui->setupUi(this);

    this->tableModel = new QStandardItemModel(this);
    ui->tableView->setModel(this->tableModel);
    this->delegate = new FDEMTemporalItemDelegate(this);
    ui->tableView->setItemDelegate(this->delegate);

    this->SetButtonsEnabled(false);
    this->InitTemporalTableView();
    this->InitTemporalProperty();
    this->ConnectSignalSlots();

    this->temporalCollection = TemporalCollection::GetTemporalCollection();
}

FDEMTemporalWidget::~FDEMTemporalWidget()
{
    delete this->initialWidget;
    delete this->mechanicalWidget;
    delete this->hydraulicWidget;
    delete this->thermalWidget;
    delete this->stressStrainWidget;

    delete this->tableModel;
    delete this->delegate;

    delete ui;
}

void FDEMTemporalWidget::InitTemporalTableView()
{
    //Headers
    QFont font(QString("Arial"), -1, QFont::Bold);
    QStringList headers = QStringList() << "Name" << "Temporal Type" << "Ready";
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
    ui->tableView->setColumnWidth(COL_TITLE, 130);
    ui->tableView->setColumnWidth(COL_FIELD, 130);
    ui->tableView->setColumnWidth(COL_STATE, 120);
}

void FDEMTemporalWidget::InitTemporalProperty()
{
    this->initialWidget = new QtTreePropertyBrowser;
    ui->propertyWidget->layout()->addWidget(this->initialWidget);
    this->initialWidget->show();

    this->mechanicalWidget = new FDEMMechanicalTemporalWidget;
    ui->propertyWidget->layout()->addWidget(this->mechanicalWidget);
    this->mechanicalWidget->hide();

    this->hydraulicWidget = new FDEMHydraulicTemporalWidget;
    ui->propertyWidget->layout()->addWidget(this->hydraulicWidget);
    this->hydraulicWidget->hide();

    this->thermalWidget = new FDEMThermalTemporalWidget;
    ui->propertyWidget->layout()->addWidget(this->thermalWidget);
    this->thermalWidget->hide();

    this->stressStrainWidget = new FDEMTemporalStressStrainWidget;
    ui->propertyWidget->layout()->addWidget(this->stressStrainWidget);
    this->stressStrainWidget->hide();
}

void FDEMTemporalWidget::ConnectSignalSlots()
{
    connect(ui->appendButton, &QPushButton::clicked,
            this, &FDEMTemporalWidget::slotAppendButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked,
            this, &FDEMTemporalWidget::slotRemoveButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked,
            this, &FDEMTemporalWidget::slotClearButtonClicked);
    connect(ui->spaceFunButton, &QPushButton::clicked,
            this, &FDEMTemporalWidget::slotSiteDependentButtonClicked);

    connect(this->tableModel, &QStandardItemModel::itemChanged,
            this, &FDEMTemporalWidget::slotTableModelItemChanged);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &FDEMTemporalWidget::slotSelectionChanged);

    connect(ui->applyButton, &QPushButton::clicked,
            this, &FDEMTemporalWidget::slotApplyButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &FDEMTemporalWidget::slotCancelButtonClicked);
    connect(ui->defaultButton, &QPushButton::clicked,
            this, &FDEMTemporalWidget::slotDefaultButtonClicked);
}

void FDEMTemporalWidget::SetButtonsEnabled(bool arg)
{
    ui->removeButton->setEnabled(arg);
    ui->clearButton->setEnabled(arg);
    ui->applyButton->setEnabled(arg);
    ui->cancelButton->setEnabled(arg);
    ui->defaultButton->setEnabled(arg);
}

QList<QStandardItem *> FDEMTemporalWidget::CreateTemporalItems(const QString &temporalId)
{
    QString path = QString(":/images/resources/");

    //Temporal title
    QStandardItem* title = new QStandardItem(temporalId);
    title->setTextAlignment(Qt::AlignCenter);
    title->setEditable(false);

    //Temporal field
    QStandardItem* field = new QStandardItem;
    field->setData(QString("Mechanical"), Qt::EditRole);
    field->setIcon(QIcon(path + QString("pqPointData.png")));
    field->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    //Temporal state
    QStandardItem* state = new QStandardItem;
    state->setData(false, Qt::EditRole);
    state->setTextAlignment(Qt::AlignCenter);
    state->setEditable(false);

    QList<QStandardItem *> items;
    items << title << field << state;
    return items;
}

void FDEMTemporalWidget::slotAppendButtonClicked()
{
    int index = this->tableModel->rowCount() + 1;
    QString tempId = QString("temporal-%1").arg(index);
    emit this->signalCancelTemporalButtonClicked(tempId);

    this->temporalCollection->AppendTemporal(tempId);
    this->tableModel->appendRow(this->CreateTemporalItems(tempId));

    this->SetButtonsEnabled(true);
}

void FDEMTemporalWidget::slotRemoveButtonClicked()
{
    int index = this->tableModel->rowCount();
    QString tempId = QString("temporal-%1").arg(index);

    this->tableModel->removeRow(this->tableModel->rowCount() - 1);
    this->temporalCollection->RemoveTemporal(tempId);
    this->SetButtonsEnabled(this->tableModel->rowCount());

    ui->tableView->selectRow(this->tableModel->rowCount() - 1);
    if(0 == this->tableModel->rowCount())
    {
        this->initialWidget->show();
        this->mechanicalWidget->hide();
        this->hydraulicWidget->hide();
        this->thermalWidget->hide();
        this->stressStrainWidget->hide();
        emit this->signalClearTemporalButtonClicked();
    }
}

void FDEMTemporalWidget::slotClearButtonClicked()
{
    this->tableModel->removeRows(0, this->tableModel->rowCount());
    this->temporalCollection->Clear();
    this->SetButtonsEnabled(false);

    if(!this->tableModel->rowCount())
    {
        this->initialWidget->show();
        this->mechanicalWidget->hide();
        this->hydraulicWidget->hide();
        this->thermalWidget->hide();
        this->stressStrainWidget->hide();
        emit this->signalClearTemporalButtonClicked();
    }
}

void FDEMTemporalWidget::slotSiteDependentButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(selection.isEmpty())
    {
        QMessageBox::warning(nullptr, QString("Invalid operation"),
                             QString("Please select the boundary you want to change!"));
        return;
    }

    QString directory = GlobalCollection::GetGlobalCollection()->getWorkspace();
    QString filter = QString("Text File (*.txt *.dat *.csv)");
    QString fname = QFileDialog::getOpenFileName(this, QString("Select loading factor sequence"), directory, filter);
    if(fname.isNull() || fname.isEmpty()) return;

    QFile file(fname);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(nullptr,QString("File Opening Error"),
                             fname + QString("\ncannot be opened. \nPlease check it out!"));
        return;
    }

    QVector<QVector<double>> sequence;
    QTextStream in(&file);
    do
    {
        QString lineInfo = in.readLine();

        if(lineInfo.isNull() || lineInfo.isEmpty())
            continue;

        if(lineInfo.startsWith(QLatin1String("#")))
            continue;

        QStringList info = lineInfo.split(QChar(','), QString::SkipEmptyParts);
        sequence.push_back(QVector<double>({info[0].toDouble(), info[1].toDouble()}));

    } while(!in.atEnd());

    file.close();
    sequence.squeeze();

    // GUI display
    QString id = selection.indexes().at(COL_TITLE).data().toString();
    QSharedPointer<Temporal> temporal = this->temporalCollection->GetTemporal(id);
    temporal->spatialSample = sequence.size();
    temporal->spatialFactor = sequence;

    QModelIndex index = selection.indexes().at(COL_FIELD);
    QString fieldType = index.data().toString();
    if(QString("Mechanical") == fieldType)
    {
        MechanicalTemporal *mechanicalTemp = static_cast<MechanicalTemporal*>(temporal.get());
        this->mechanicalWidget->UpdatePropertyBrowser(mechanicalTemp);
    }
    else if(QString("Hydraulic") == fieldType)
    {
        HydraulicTemporal *hydraulicalTemp = static_cast<HydraulicTemporal*>(temporal.get());
        this->hydraulicWidget->UpdatePropertyBrowser(hydraulicalTemp);
    }
    else if(QString("Thermal") == fieldType)
    {
        ThermalTemporal* thermalTemp = static_cast<ThermalTemporal*>(temporal.get());
        this->thermalWidget->UpdatePropertyBrowser(thermalTemp);
    }
    else if(QString("Stress(Strain)") == fieldType)
    {
        TemporalStressStrain* stressStrainTemp = static_cast<TemporalStressStrain*>(temporal.get());
        this->stressStrainWidget->UpdatePropertyBrowser(stressStrainTemp);
    }
}

void FDEMTemporalWidget::slotTableModelItemChanged(QStandardItem *item)
{
    QString id = this->tableModel->item(item->row(), 0)->data(Qt::DisplayRole).toString();

    if(COL_FIELD == item->column())
    {
        QString fieldType = item->data(Qt::EditRole).toString();
        if(QString("Mechanical") == fieldType)
        {
            QSharedPointer<MechanicalTemporal> newTemporal =
                    QSharedPointer<MechanicalTemporal>(new MechanicalTemporal);
            this->temporalCollection->UpdateTemporal(id, newTemporal);
            this->mechanicalWidget->UpdatePropertyBrowser(newTemporal.get());
            this->mechanicalWidget->show();

            this->initialWidget->hide();
            this->hydraulicWidget->hide();
            this->thermalWidget->hide();
            this->stressStrainWidget->hide();
        }
        else if(QString("Hydraulic") == fieldType)
        {
            QSharedPointer<HydraulicTemporal> newTemporal =
                    QSharedPointer<HydraulicTemporal>(new HydraulicTemporal);
            this->temporalCollection->UpdateTemporal(id, newTemporal);
            this->hydraulicWidget->UpdatePropertyBrowser(newTemporal.get());
            this->hydraulicWidget->show();

            this->initialWidget->hide();
            this->mechanicalWidget->hide();
            this->thermalWidget->hide();
            this->stressStrainWidget->hide();
        }
        else if(QString("Thermal") == fieldType)
        {
            QSharedPointer<ThermalTemporal> newTemporal =
                    QSharedPointer<ThermalTemporal>(new ThermalTemporal);
            this->temporalCollection->UpdateTemporal(id, newTemporal);
            this->thermalWidget->UpdatePropertyBrowser(newTemporal.get());
            this->thermalWidget->show();

            this->initialWidget->hide();
            this->mechanicalWidget->hide();
            this->hydraulicWidget->hide();
            this->stressStrainWidget->hide();
        }
        else if(QString("Stress(Strain)") == fieldType)
        {
            QSharedPointer<TemporalStressStrain> newTemporal =
                    QSharedPointer<TemporalStressStrain>(new TemporalStressStrain);
            this->temporalCollection->UpdateTemporal(id, newTemporal);
            this->stressStrainWidget->UpdatePropertyBrowser(newTemporal.get());
            this->stressStrainWidget->show();

            this->initialWidget->hide();
            this->mechanicalWidget->hide();
            this->hydraulicWidget->hide();
            this->thermalWidget->hide();
        }
    }
}

void FDEMTemporalWidget::slotSelectionChanged(const QItemSelection &selection, const QItemSelection &)
{
    if(selection.isEmpty()) return;

    QModelIndex index = selection.indexes().at(COL_FIELD);
    QString id = selection.indexes().at(COL_TITLE).data().toString();
    QString fieldType = index.data().toString();

    QSharedPointer<Temporal> temporal = this->temporalCollection->GetTemporal(id);
    if(QString("Mechanical") == fieldType)
    {
        MechanicalTemporal *mechanicalTemp = static_cast<MechanicalTemporal*>(temporal.get());
        this->mechanicalWidget->UpdatePropertyBrowser(mechanicalTemp);
        this->mechanicalWidget->show();

        this->initialWidget->hide();
        this->hydraulicWidget->hide();
        this->thermalWidget->hide();
        this->stressStrainWidget->hide();
    }
    else if(QString("Hydraulic") == fieldType)
    {
        HydraulicTemporal *hydraulicalTemp = static_cast<HydraulicTemporal*>(temporal.get());
        this->hydraulicWidget->UpdatePropertyBrowser(hydraulicalTemp);
        this->hydraulicWidget->show();

        this->initialWidget->hide();
        this->mechanicalWidget->hide();
        this->thermalWidget->hide();
        this->stressStrainWidget->hide();
    }
    else if(QString("Thermal") == fieldType)
    {
        ThermalTemporal* thermalTemp = static_cast<ThermalTemporal*>(temporal.get());
        this->thermalWidget->UpdatePropertyBrowser(thermalTemp);
        this->thermalWidget->show();

        this->initialWidget->hide();
        this->mechanicalWidget->hide();
        this->hydraulicWidget->hide();
        this->stressStrainWidget->hide();
    }
    else if(QString("Stress(Strain)") == fieldType)
    {
        TemporalStressStrain* stressStrainTemp = static_cast<TemporalStressStrain*>(temporal.get());
        this->stressStrainWidget->UpdatePropertyBrowser(stressStrainTemp);
        this->stressStrainWidget->show();

        this->initialWidget->hide();
        this->mechanicalWidget->hide();
        this->hydraulicWidget->hide();
        this->thermalWidget->hide();
    }
}

void FDEMTemporalWidget::slotApplyButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(!selection.empty())
    {
//      QString temporalId = selection.indexes().at(COL_TITLE).data().toString();
//      emit this->signalApplyTemporalButtonClicked(temporalId);

        // Block Dataset
        QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
        vtkMultiBlockDataSet *blockDataSet = actorCollection->GetBlockActorDataSet();

        // Select GlobalIds
        QString temporalId = selection.indexes().at(COL_TITLE).data().toString();
        QSharedPointer<TemporalCollection> temporalCollection = TemporalCollection::GetTemporalCollection();
        QSharedPointer<Temporal> temporal = temporalCollection->GetTemporal(temporalId);
        QSharedPointer<NodeSetCollection> groupCollection = NodeSetCollection::GetNodeSetCollection();

        bool hasGroup = groupCollection->GetGroupMap().contains(temporal->nodeSetId);
        if(!hasGroup)
        {
            QMessageBox::warning(this, QString("Invalid Operation"),
                                 QString("NodeSet-%1 has not been created. Check out!").arg(temporal->nodeSetId));
            return;
        }

        QSharedPointer<Group> group = groupCollection->GetGroup(temporal->nodeSetId);
        vtkNew<vtkIdTypeArray> globalIds;
        for(int i = 0; i < group->getSize(); i++)
        {
            globalIds->InsertNextValue(group->getGlobalIds()[i]);
        }

        QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
        selectCollection->SetSelectStyle(Selection::NORMAL);
        selectCollection->SetSelectField(Selection::POINT);
        selectCollection->SetInputDataSet(blockDataSet);
        selectCollection->GlobalIDsSelect(globalIds);

        emit this->signalApplyTemporalButtonClicked(temporalId);
        QModelIndex index = selection.indexes().at(COL_STATE);
        this->tableModel->itemFromIndex(index)->setData(true, Qt::EditRole);
    }
    else
    {
        QMessageBox::warning(this, QString("Invalid Operation"),
                             QString("Select the temporal to be set. Thanks!"));
    }
}

void FDEMTemporalWidget::slotDefaultButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(!selection.empty())
    {
        QString temporalId = selection.indexes().at(COL_TITLE).data().toString();
        QSharedPointer<Temporal> temporal = temporalCollection->GetTemporal(temporalId);

        if(Temporal::MECHANICAL == temporal->getTemporalType())
        {
            QSharedPointer<MechanicalTemporal> mechanicalTemp =
                    QSharedPointer<MechanicalTemporal>(new MechanicalTemporal);
            this->mechanicalWidget->UpdatePropertyBrowser(mechanicalTemp.get());
        }
        else if(Temporal::HYDRAULIC == temporal->getTemporalType())
        {
            QSharedPointer<HydraulicTemporal> hydraulicalTemp =
                    QSharedPointer<HydraulicTemporal>(new HydraulicTemporal);
            this->hydraulicWidget->UpdatePropertyBrowser(hydraulicalTemp.get());
        }
        else if(Temporal::THERMAL == temporal->getTemporalType())
        {
            QSharedPointer<ThermalTemporal> thermalTemp =
                    QSharedPointer<ThermalTemporal>(new ThermalTemporal);
            this->thermalWidget->UpdatePropertyBrowser(thermalTemp.get());
        }
        else if(Temporal::INISTRESS == temporal->getTemporalType())
        {
            QSharedPointer<TemporalStressStrain> stressStrainTemp =
                    QSharedPointer<TemporalStressStrain>(new TemporalStressStrain);
            this->stressStrainWidget->UpdatePropertyBrowser(stressStrainTemp.get());
        }
    }
    else
    {
        QMessageBox::warning(this, QString("Invalid Operation"),
                             QString("Select the temporal to be set. Thanks!"));
    }
}

void FDEMTemporalWidget::slotCancelButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(!selection.empty())
    {
        QString temporalId = selection.indexes().at(COL_TITLE).data().toString();
        emit this->signalCancelTemporalButtonClicked(temporalId);

        QModelIndex index = selection.indexes().at(COL_STATE);
        this->tableModel->itemFromIndex(index)->setData(false, Qt::EditRole);
    }
    else
    {
        QMessageBox::warning(this, QString("Invalid Operation"),
                             QString("Select the temporal to be set. Thanks!"));
    }
}
