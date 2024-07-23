#include "fdemboundarywidget.h"
#include "ui_fdemboundarywidget.h"
#include "fdemboundaryitemdelegate.h"
#include "qttreepropertybrowser.h"
#include "fdemmechanicalboundarywidget.h"
#include "fdemhydraulicboundarywidget.h"
#include "fdemthermalboundarywidget.h"
#include "blockcollection.h"
#include "actorcollection.h"
#include "selectcollection.h"
#include "boundarycollection.h"
#include "boundary.h"
#include "groupcollection.h"
#include "group.h"
#include "globalcollection.h"

#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QFileDialog>

FDEMBoundaryWidget::FDEMBoundaryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMBoundaryWidget)
{
    ui->setupUi(this);

    this->tableModel = new QStandardItemModel(this);
    ui->tableView->setModel(this->tableModel);
    this->delegate = new FDEMBoundaryItemDelegate(this);
    ui->tableView->setItemDelegate(this->delegate);

    this->SetButtonsEnabled(false);
    this->InitBoundaryTableView();
    this->InitBoundaryProperty();
    this->ConnectSignalSlots();

    this->boundaryCollection = BoundaryCollection::GetBoundaryCollection();
}

FDEMBoundaryWidget::~FDEMBoundaryWidget()
{
    delete this->initialWidget;
    delete this->mechanicalWidget;
    delete this->hydraulicWidget;
    delete this->thermalWidget;

    delete this->tableModel;
    delete this->delegate;

    delete ui;
}

void FDEMBoundaryWidget::InitBoundaryTableView()
{
    //Headers
    QFont font(QString("Arial"), -1, QFont::Bold);
    QStringList headers = QStringList() << "Name" << "Boundary Type" << "Solution";
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
    ui->tableView->setColumnWidth(COL_TITLE, 120);
    ui->tableView->setColumnWidth(COL_FIELD, 120);
    ui->tableView->setColumnWidth(COL_SOLVE, 120);
}

void FDEMBoundaryWidget::InitBoundaryProperty()
{
    this->initialWidget = new QtTreePropertyBrowser;
    ui->propertyWidget->layout()->addWidget(this->initialWidget);
    this->initialWidget->show();

    this->mechanicalWidget = new FDEMMechanicalBoundaryWidget;
    ui->propertyWidget->layout()->addWidget(this->mechanicalWidget);
    this->mechanicalWidget->hide();

    this->hydraulicWidget = new FDEMHydraulicBoundaryWidget;
    ui->propertyWidget->layout()->addWidget(this->hydraulicWidget);
    this->hydraulicWidget->hide();

    this->thermalWidget = new FDEMThermalBoundaryWidget;
    ui->propertyWidget->layout()->addWidget(this->thermalWidget);
    this->thermalWidget->hide();
}

void FDEMBoundaryWidget::ConnectSignalSlots()
{
    connect(ui->appendButton, &QPushButton::clicked,
            this, &FDEMBoundaryWidget::slotAppendButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked,
            this, &FDEMBoundaryWidget::slotRemoveButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked,
            this, &FDEMBoundaryWidget::slotClearButtonClicked);

    connect(ui->spaceFunButton, &QPushButton::clicked,
            this, &FDEMBoundaryWidget::slotSiteDependentButtonClicked);
    connect(ui->timeFunButton, &QPushButton::clicked,
            this, &FDEMBoundaryWidget::slotTimeDependentButtonClicked);

    connect(this->tableModel, &QStandardItemModel::itemChanged,
            this, &FDEMBoundaryWidget::slotTableModelItemChanged);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &FDEMBoundaryWidget::slotSelectionChanged);

    connect(ui->applyButton, &QPushButton::clicked,
            this, &FDEMBoundaryWidget::slotApplyButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &FDEMBoundaryWidget::slotCancelButtonClicked);
    connect(ui->defaultButton, &QPushButton::clicked,
            this, &FDEMBoundaryWidget::slotDefaultButtonClicked);
}

void FDEMBoundaryWidget::SetButtonsEnabled(bool arg)
{
    ui->removeButton->setEnabled(arg);
    ui->clearButton->setEnabled(arg);
    ui->applyButton->setEnabled(arg);
    ui->cancelButton->setEnabled(arg);
    ui->defaultButton->setEnabled(arg);
}

QList<QStandardItem *> FDEMBoundaryWidget::CreateBoundaryItems(const QString &boundaryId)
{
    QString path = QString(":/images/resources/");

    //Boundary title
    QStandardItem* title = new QStandardItem(boundaryId);
    title->setTextAlignment(Qt::AlignCenter);
    title->setEditable(false);

    //Boundary field
    QStandardItem* field = new QStandardItem;
    field->setData(QString("Mechanical"), Qt::EditRole);
    field->setIcon(QIcon(path + QString("pqPointData.png")));
    field->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    //Boundary state
    QStandardItem* solve = new QStandardItem;
    solve->setData(QString("solution-1"), Qt::EditRole);
    solve->setTextAlignment(Qt::AlignCenter);

    QList<QStandardItem *> items;
    items << title << field << solve;
    return items;
}

void FDEMBoundaryWidget::slotAppendButtonClicked()
{
    int index = this->tableModel->rowCount() + 1;
    QString bndId = QString("boundary-%1").arg(index);

    this->boundaryCollection->AppendBoundary(bndId);
    this->tableModel->appendRow(this->CreateBoundaryItems(bndId));

    this->SetButtonsEnabled(true);
}

void FDEMBoundaryWidget::slotRemoveButtonClicked()
{
    int index = this->tableModel->rowCount();
    QString bndId = QString("boundary-%1").arg(index);
    emit this->signalCancelBoundaryButtonClicked(bndId);

    this->tableModel->removeRow(this->tableModel->rowCount() - 1);
    this->boundaryCollection->RemoveBoundary(bndId);
    this->SetButtonsEnabled(this->tableModel->rowCount());

    ui->tableView->selectRow(this->tableModel->rowCount() - 1);
    if(0 == this->tableModel->rowCount())
    {
        this->initialWidget->show();
        this->mechanicalWidget->hide();
        this->hydraulicWidget->hide();
        this->thermalWidget->hide();
        emit this->signalClearBoundaryButtonClicked();
    }
}

void FDEMBoundaryWidget::slotClearButtonClicked()
{
    this->tableModel->removeRows(0, this->tableModel->rowCount());
    this->boundaryCollection->Clear();
    this->SetButtonsEnabled(false);

    if(!this->tableModel->rowCount())
    {
        this->initialWidget->show();
        this->mechanicalWidget->hide();
        this->hydraulicWidget->hide();
        this->thermalWidget->hide();
        emit this->signalClearBoundaryButtonClicked();
    }
}

void FDEMBoundaryWidget::slotSiteDependentButtonClicked()
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

    //GUI display
    QString id = selection.indexes().at(COL_TITLE).data().toString();
    QSharedPointer<Boundary> boundary = this->boundaryCollection->GetBoundary(id);
    boundary->spatialSample = sequence.size();
    boundary->spatialFactor = sequence;

    QModelIndex index = selection.indexes().at(COL_FIELD);
    QString fieldType = index.data().toString();
    if(QString("Mechanical") == fieldType)
    {
        MechanicalBoundary *mechanicalBnd = static_cast<MechanicalBoundary*>(boundary.get());
        this->mechanicalWidget->UpdatePropertyBrowser(mechanicalBnd);
    }
    else if(QString("Hydraulic") == fieldType)
    {
        HydraulicBoundary *hydraulicalBnd = static_cast<HydraulicBoundary*>(boundary.get());
        this->hydraulicWidget->UpdatePropertyBrowser(hydraulicalBnd);
    }
    else if(QString("Thermal") == fieldType)
    {
        ThermalBoundary* thermalBnd = static_cast<ThermalBoundary*>(boundary.get());
        this->thermalWidget->UpdatePropertyBrowser(thermalBnd);
    }
}

void FDEMBoundaryWidget::slotTimeDependentButtonClicked()
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
    QString fname = QFileDialog::getOpenFileName(this, QString("Select time varying sequence"), directory, filter);
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

    //GUI display
    QString id = selection.indexes().at(COL_TITLE).data().toString();
    QSharedPointer<Boundary> boundary = this->boundaryCollection->GetBoundary(id);
    boundary->temporalSample = sequence.size();
    boundary->temporalFactor = sequence;

    QModelIndex index = selection.indexes().at(COL_FIELD);
    QString fieldType = index.data().toString();
    if(QString("Mechanical") == fieldType)
    {
        MechanicalBoundary *mechanicalBnd = static_cast<MechanicalBoundary*>(boundary.get());
        this->mechanicalWidget->UpdatePropertyBrowser(mechanicalBnd);
    }
    else if(QString("Hydraulic") == fieldType)
    {
        HydraulicBoundary *hydraulicalBnd = static_cast<HydraulicBoundary*>(boundary.get());
        this->hydraulicWidget->UpdatePropertyBrowser(hydraulicalBnd);
    }
    else if(QString("Thermal") == fieldType)
    {
        ThermalBoundary* thermalBnd = static_cast<ThermalBoundary*>(boundary.get());
        this->thermalWidget->UpdatePropertyBrowser(thermalBnd);
    }
}

void FDEMBoundaryWidget::slotTableModelItemChanged(QStandardItem *item)
{
    QString bndId = this->tableModel->item(item->row(), COL_TITLE)->data(Qt::DisplayRole).toString();
    QString solId = this->tableModel->item(item->row(), COL_SOLVE)->data(Qt::EditRole).toString();

    if(COL_FIELD == item->column())
    {
        QString fieldType = item->data(Qt::EditRole).toString();
        if(QString("Mechanical") == fieldType)
        {
            QSharedPointer<MechanicalBoundary> newBoundary =
                    QSharedPointer<MechanicalBoundary>(new MechanicalBoundary);
            this->boundaryCollection->UpdateBoundary(bndId, newBoundary);
            this->mechanicalWidget->UpdatePropertyBrowser(newBoundary.get());
            this->mechanicalWidget->show();

            this->initialWidget->hide();
            this->hydraulicWidget->hide();
            this->thermalWidget->hide();
        }
        else if(QString("Hydraulic") == fieldType)
        {
            QSharedPointer<HydraulicBoundary> newBoundary =
                    QSharedPointer<HydraulicBoundary>(new HydraulicBoundary);
            this->boundaryCollection->UpdateBoundary(bndId, newBoundary);
            this->hydraulicWidget->UpdatePropertyBrowser(newBoundary.get());
            this->hydraulicWidget->show();

            this->initialWidget->hide();
            this->mechanicalWidget->hide();
            this->thermalWidget->hide();
        }
        else if(QString("Thermal") == fieldType)
        {
            QSharedPointer<ThermalBoundary> newBoundary =
                    QSharedPointer<ThermalBoundary>(new ThermalBoundary);
            this->boundaryCollection->UpdateBoundary(bndId, newBoundary);
            this->thermalWidget->UpdatePropertyBrowser(newBoundary.get());
            this->thermalWidget->show();

            this->initialWidget->hide();
            this->mechanicalWidget->hide();
            this->hydraulicWidget->hide();
        }
    }
    else if(COL_SOLVE == item->column())
    {
        int index = solId.split(QChar('-')).last().toInt();
        this->boundaryCollection->GetBoundary(bndId)->loadStep = index;
    }
}

void FDEMBoundaryWidget::slotSelectionChanged(const QItemSelection &selection, const QItemSelection &)
{
    if(selection.isEmpty()) return;

    QModelIndex index = selection.indexes().at(COL_FIELD);
    QString id = selection.indexes().at(COL_TITLE).data().toString();
    QString fieldType = index.data().toString();

    QSharedPointer<Boundary> boundary = this->boundaryCollection->GetBoundary(id);
    if(QString("Mechanical") == fieldType)
    {
        MechanicalBoundary *mechanicalBnd = static_cast<MechanicalBoundary*>(boundary.get());
        this->mechanicalWidget->UpdatePropertyBrowser(mechanicalBnd);
        this->mechanicalWidget->show();

        this->initialWidget->hide();
        this->hydraulicWidget->hide();
        this->thermalWidget->hide();
    }
    else if(QString("Hydraulic") == fieldType)
    {
        HydraulicBoundary *hydraulicalBnd = static_cast<HydraulicBoundary*>(boundary.get());
        this->hydraulicWidget->UpdatePropertyBrowser(hydraulicalBnd);
        this->hydraulicWidget->show();

        this->initialWidget->hide();
        this->mechanicalWidget->hide();
        this->thermalWidget->hide();
    }
    else if(QString("Thermal") == fieldType)
    {
        ThermalBoundary* thermalBnd = static_cast<ThermalBoundary*>(boundary.get());
        this->thermalWidget->UpdatePropertyBrowser(thermalBnd);
        this->thermalWidget->show();

        this->initialWidget->hide();
        this->mechanicalWidget->hide();
        this->hydraulicWidget->hide();
    }
}

void FDEMBoundaryWidget::slotDefaultButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(!selection.empty())
    {
        QString boundaryId = selection.indexes().at(COL_TITLE).data().toString();
        QSharedPointer<Boundary> boundary = boundaryCollection->GetBoundary(boundaryId);

        if(Boundary::MECHANICAL == boundary->getBoundaryType())
        {
            QSharedPointer<MechanicalBoundary> mechanicalBnd =
                    QSharedPointer<MechanicalBoundary>(new MechanicalBoundary());
            this->mechanicalWidget->UpdatePropertyBrowser(mechanicalBnd.get());
        }
        else if(Boundary::HYDRAULIC == boundary->getBoundaryType())
        {
            QSharedPointer<HydraulicBoundary> hydraulicBnd =
                    QSharedPointer<HydraulicBoundary>(new HydraulicBoundary());
            this->hydraulicWidget->UpdatePropertyBrowser(hydraulicBnd.get());
        }
        else if(Boundary::THERMAL == boundary->getBoundaryType())
        {
            QSharedPointer<ThermalBoundary> thermalBnd =
                    QSharedPointer<ThermalBoundary>(new ThermalBoundary());
            this->thermalWidget->UpdatePropertyBrowser(thermalBnd.get());
        }
    }
    else
    {
        QMessageBox::warning(this, QString("Invalid Operation"),
                             QString("Select the temporal to be set. Thanks!"));
    }
}

void FDEMBoundaryWidget::slotApplyButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(!selection.empty())
    {
//      QString boundaryId = selection.indexes().at(COL_TITLE).data().toString();
//      emit this->signalApplyBoundaryButtonClicked(boundaryId);

        // Block Dataset
        QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
        vtkMultiBlockDataSet *blockDataSet = actorCollection->GetBlockActorDataSet();

        // Select GlobalIds
        QString boundaryId = selection.indexes().at(COL_TITLE).data().toString();
        QSharedPointer<BoundaryCollection> boundaryCollection = BoundaryCollection::GetBoundaryCollection();
        QSharedPointer<Boundary> boundary = boundaryCollection->GetBoundary(boundaryId);
        QSharedPointer<NodeSetCollection> groupCollection = NodeSetCollection::GetNodeSetCollection();

        bool hasGroup = groupCollection->GetGroupMap().contains(boundary->nodeSetId);
        if(!hasGroup)
        {
            QMessageBox::warning(this, QString("Invalid Operation"),
                                 QString("NodeSet-%1 has not been created. Check out!").arg(boundary->nodeSetId));
            return;
        }

        QSharedPointer<Group> group = groupCollection->GetGroup(boundary->nodeSetId);
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

        emit this->signalApplyBoundaryButtonClicked(boundaryId);
    }
    else
    {
        QMessageBox::warning(this, QString("Invalid Operation"),
                             QString("Select the boundary to be mapped. Thanks!"));
    }
}

void FDEMBoundaryWidget::slotCancelButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(!selection.empty())
    {
        QString boundaryId = selection.indexes().at(COL_TITLE).data().toString();
        emit this->signalCancelBoundaryButtonClicked(boundaryId);
    }
    else
    {
        QMessageBox::warning(this, QString("Invalid Operation"),
                             QString("Select the boundary to be canceled. Thanks!"));
    }
}

void FDEMBoundaryWidget::slotSolutionCollectionChanged(int size)
{
    FDEMBoundaryItemDelegate *delegate = dynamic_cast<FDEMBoundaryItemDelegate*>(ui->tableView->itemDelegate());
    if(delegate)
    {
        QStringList names = {"solution-1"};
        for(int i = 1; i < size; i++)
        { names << QString("solution-%1").arg(i + 1);}
        delegate->SetSolutionNames(names);

        for(int i = 0; i < this->tableModel->rowCount(); i++)
        {
            QStandardItem *item = this->tableModel->item(i, COL_SOLVE);
            if(!names.contains(item->data(Qt::EditRole).toString()))
            { item->setText(names.last()); }
        }
    }
}
