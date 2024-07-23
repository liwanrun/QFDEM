#include "filterselectdialog.h"
#include "ui_filterselectdialog.h"
#include "filterselectdelegate.h"
#include "criterioncollection.h"
#include "criterion.h"
#include "selectcollection.h"
#include "actorcollection.h"

#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItemModel>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkDoubleArray.h>

FilterSelectDialog::FilterSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterSelectDialog),
    path(QString(":/images/resources/")),
    cellType(VTK_TRIANGLE)
{
    ui->setupUi(this);

    this->tableModel = new QStandardItemModel(this);
    ui->tableView->setModel(this->tableModel);
    this->delegate = new FilterSelectDelegate(this);
    ui->tableView->setItemDelegate(this->delegate);

    this->SetButtonsEnabled(false);
    this->InitFindDataDialog();
    this->ConnectSignalSlots();

    this->criterionCollection = CriterionCollection::GetCriterionCollection();
    this->actorCollection = ActorCollection::GetActorCollection();

    this->selectCollection = SelectCollection::GetSelectCollection();
    this->selectCollection->SetSelectField(Selection::POINT);
    this->selectCollection->SetInvertSelect(false);
}

FilterSelectDialog::~FilterSelectDialog()
{
    this->criterionCollection->Clear();
    delete ui;
}

void FilterSelectDialog::SetDefaultRenderer(vtkRenderer *renderer)
{
    this->renderer = renderer;
}

void FilterSelectDialog::InitFindDataDialog()
{
    ui->entityBox->setCurrentText(QString("Points"));
    ui->cellTypeBox->setEnabled(false);

    QFont font(QString("Arial"), -1, QFont::Bold);
    QStringList headers = QStringList() << "Header" << "Filter" << "Select" << "Number" << "Boolean";
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
    ui->tableView->setColumnWidth(COL_HEADER, 120);
    ui->tableView->setColumnWidth(COL_FILTER, 120);
    ui->tableView->setColumnWidth(COL_SELECT, 120);
    ui->tableView->setColumnWidth(COL_NUMBER, 100);
    ui->tableView->setColumnWidth(COL_LOGICS, 100);

    ui->enterButton->setDefault(true);
}

void FilterSelectDialog::ConnectSignalSlots()
{
    connect(ui->appendButton, &QPushButton::clicked,
            this, &FilterSelectDialog::slotAppendButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked,
            this, &FilterSelectDialog::slotRemoveButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked,
            this, &FilterSelectDialog::slotClearButtonClicked);
    connect(ui->entityBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FilterSelectDialog::slotEntityIndexChanged);
    connect(ui->cellTypeBox, &QComboBox::currentTextChanged,
            this, &FilterSelectDialog::slotCellTypeChanged);

    connect(this->tableModel, &QStandardItemModel::itemChanged,
            this, &FilterSelectDialog::slotTableModelItemChanged);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &FilterSelectDialog::slotSelectionChanged);
    connect(ui->tableWidget, &QTableWidget::itemChanged,
            this, &FilterSelectDialog::slotTableWidgetItemChanged);

    connect(ui->invertButton, &QPushButton::clicked,
            this, &FilterSelectDialog::slotInvertButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked,
            this, &FilterSelectDialog::slotResetButtonClicked);
    connect(ui->enterButton, &QPushButton::clicked,
            this, &FilterSelectDialog::slotEnterButtonClicked);
    connect(ui->selectButton, &QPushButton::clicked,
            this, &FilterSelectDialog::slotSelectButtonClicked);
}

void FilterSelectDialog::SetButtonsEnabled(bool arg)
{
    ui->removeButton->setEnabled(arg);
    ui->clearButton->setEnabled(arg);
    ui->invertButton->setEnabled(arg);
    ui->resetButton->setEnabled(arg);
    ui->enterButton->setEnabled(arg);
    ui->selectButton->setEnabled(arg);
}

QList<QStandardItem *> FilterSelectDialog::CreateCriterionItems(const QString &criId)
{
    // Criterion header
    QStandardItem* header = new QStandardItem(criId);
    header->setTextAlignment(Qt::AlignCenter);
    header->setEditable(false);

    // Criterion select
    QStandardItem* select = new QStandardItem;
    select->setData(QString("Nearest to (~)"), Qt::EditRole);
    select->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Criterion filter
    QStandardItem* filter = new QStandardItem;
    filter->setData(QString("Point(s)"), Qt::EditRole);
    filter->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Criterion number
    QStandardItem* number = new QStandardItem;
    number->setData(1, Qt::EditRole);
    number->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Criterion logics
    QStandardItem* logics = new QStandardItem;
    logics->setData(QString("Normal"), Qt::EditRole);
    logics->setData(QIcon(path + QString("pqSelectNormal.png")), Qt::DecorationRole);
    logics->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QList<QStandardItem *> items;
    items << header << filter << select << number << logics;
    return items;
}

void FilterSelectDialog::UpdateTableWidgetInformation(const Criterion* crit)
{
    ui->tableWidget->setRowCount(crit->numberRows);
    ui->tableWidget->setColumnCount(crit->numberCols);
    ui->tableWidget->setHorizontalHeaderLabels(crit->headLabels);

    vtkDoubleArray *data = crit->filterData;
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        for(int j = 0; j < ui->tableWidget->columnCount(); j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(QString().number(data->GetTuple(i)[j]));
            ui->tableWidget->setItem(i, j, item);
        }
    }
}

void FilterSelectDialog::UpdateCriterionInformation(const QItemSelection &selection)
{
    QString criId = selection.indexes().at(COL_HEADER).data().toString();
    QSharedPointer<Criterion> criterion = this->criterionCollection->GetCriterion(criId);

    try
    {
        criterion->filterData = this->ReadFromTableWidget();
    }
    catch (std::exception& e)
    {
        QMessageBox::warning(this,QString("Invalid Argument"),QString(e.what()));
        return;
    }
}

vtkDoubleArray *FilterSelectDialog::ReadFromTableWidget()
{
    uint rows = ui->tableWidget->rowCount();
    uint cols = ui->tableWidget->columnCount();

    vtkDoubleArray* matrix = vtkDoubleArray::New();
    matrix->SetNumberOfComponents(cols);
    matrix->SetNumberOfTuples(rows);

    double *tuple = new double[cols];
    for(uint i = 0; i < rows; i++)
    {
        for(uint j = 0; j < cols; j++)
        {
            QTableWidgetItem *item = ui->tableWidget->item(i,j);
            if(nullptr == item)
            {
                delete [] tuple;
                throw std::invalid_argument("Incomplete input data. Please check out!");
            }

            QString value = item->text();
            if (value.isNull() || value.isEmpty())
            {
                delete [] tuple;
                throw std::invalid_argument("Incomplete input data. Please check out!");
            }

            tuple[j] = value.toDouble();
        }

        matrix->SetTuple(i,tuple);
    }
    delete [] tuple;

    return matrix;
}

void FilterSelectDialog::slotAppendButtonClicked()
{
    int index = this->tableModel->rowCount() + 1;
    QString criId = QString("criterion-%1").arg(index);

    this->criterionCollection->AppendCriterion(criId);
    this->tableModel->appendRow(this->CreateCriterionItems(criId));

    this->SetButtonsEnabled(true);
}

void FilterSelectDialog::slotRemoveButtonClicked()
{
    int index = this->tableModel->rowCount();
    QString criId = QString("criterion-%1").arg(index);

    this->tableModel->removeRow(this->tableModel->rowCount() - 1);
    this->criterionCollection->RemoveCriterion(criId);

    if(!this->tableModel->rowCount())
    {
        this->slotResetButtonClicked();
    }

    this->SetButtonsEnabled(this->tableModel->rowCount());
    ui->tableView->selectRow(this->tableModel->rowCount() - 1);
}

void FilterSelectDialog::slotClearButtonClicked()
{
    this->slotResetButtonClicked();
    this->tableModel->removeRows(0, this->tableModel->rowCount());
    this->criterionCollection->Clear();
    this->SetButtonsEnabled(false);
}

void FilterSelectDialog::slotTableModelItemChanged(QStandardItem *item)
{
    QString criId = this->tableModel->item(item->row(), COL_HEADER)->data(Qt::DisplayRole).toString();
    QSharedPointer<Criterion> criterion = this->criterionCollection->GetCriterion(criId);

    if(COL_FILTER == item->column())
    {
        ui->tableWidget->clear();
        ui->enterButton->setEnabled(true);

        QString filter = item->data(Qt::EditRole).toString();
        if(QString("Point(s)") == filter)
        {
            QSharedPointer<PointCriterion> pointCriterion =
                    QSharedPointer<PointCriterion>(new PointCriterion);
            criterionCollection->UpdateCriterion(criId, pointCriterion);

            QStandardItem *isItem = this->tableModel->item(item->row(), COL_SELECT);
            isItem->setData(pointCriterion->selectType, Qt::EditRole);

            QStandardItem *szItem = this->tableModel->item(item->row(), COL_NUMBER);
            szItem->setData(pointCriterion->numberRows, Qt::EditRole);

            QStandardItem *lgItem = this->tableModel->item(item->row(), COL_LOGICS);
            lgItem->setData(pointCriterion->logicsType, Qt::EditRole);

            this->UpdateTableWidgetInformation(pointCriterion.data());
        }
        else if(QString("Segment(s)") == filter)
        {
            QSharedPointer<SegmentCriterion> segmentCriterion =
                    QSharedPointer<SegmentCriterion>(new SegmentCriterion);
            criterionCollection->UpdateCriterion(criId, segmentCriterion);

            QStandardItem *isItem = this->tableModel->item(item->row(), COL_SELECT);
            isItem->setData(segmentCriterion->selectType, Qt::EditRole);

            QStandardItem *szItem = this->tableModel->item(item->row(), COL_NUMBER);
            szItem->setData(segmentCriterion->numberRows, Qt::EditRole);

            QStandardItem *lgItem = this->tableModel->item(item->row(), COL_LOGICS);
            lgItem->setData(segmentCriterion->logicsType, Qt::EditRole);

            this->UpdateTableWidgetInformation(segmentCriterion.data());
        }
        else if(QString("Rectangle(s)") == filter)
        {
            QSharedPointer<RectangleCriterion> rectangleCriterion =
                    QSharedPointer<RectangleCriterion>(new RectangleCriterion);
            criterionCollection->UpdateCriterion(criId, rectangleCriterion);

            QStandardItem *isItem = this->tableModel->item(item->row(), COL_SELECT);
            isItem->setData(rectangleCriterion->selectType, Qt::EditRole);

            QStandardItem *szItem = this->tableModel->item(item->row(), COL_NUMBER);
            szItem->setData(rectangleCriterion->numberRows, Qt::EditRole);

            QStandardItem *lgItem = this->tableModel->item(item->row(), COL_LOGICS);
            lgItem->setData(rectangleCriterion->logicsType, Qt::EditRole);

            this->UpdateTableWidgetInformation(rectangleCriterion.data());
        }
        else if(QString("Circle(s)") == filter)
        {
            QSharedPointer<EllipseCriterion> ellipseCriterion =
                    QSharedPointer<EllipseCriterion>(new EllipseCriterion);
            criterionCollection->UpdateCriterion(criId, ellipseCriterion);

            QStandardItem *isItem = this->tableModel->item(item->row(), COL_SELECT);
            isItem->setData(ellipseCriterion->selectType, Qt::EditRole);

            QStandardItem *szItem = this->tableModel->item(item->row(), COL_NUMBER);
            szItem->setData(ellipseCriterion->numberRows, Qt::EditRole);

            QStandardItem *lgItem = this->tableModel->item(item->row(), COL_LOGICS);
            lgItem->setData(ellipseCriterion->logicsType, Qt::EditRole);

            this->UpdateTableWidgetInformation(ellipseCriterion.data());
        }
    }
    else if(COL_SELECT == item->column())
    {
        criterion->selectType = item->data(Qt::EditRole).toString();
        ui->enterButton->setEnabled(true);
    }
    else if(COL_NUMBER == item->column())
    {
        criterion->numberRows = item->data(Qt::EditRole).toInt();
        ui->tableWidget->setRowCount(criterion->numberRows);
        ui->enterButton->setEnabled(true);
    }
    else if(COL_LOGICS == item->column())
    {
        criterion->logicsType = item->data(Qt::EditRole).toString();
        ui->enterButton->setEnabled(true);
    }
}

void FilterSelectDialog::slotTableWidgetItemChanged(QTableWidgetItem *)
{
    ui->enterButton->setEnabled(true);
}

void FilterSelectDialog::slotSelectionChanged(const QItemSelection &selection, const QItemSelection &)
{
    if(selection.isEmpty()) return;

    QString criId = selection.indexes().at(COL_HEADER).data().toString();
    QSharedPointer<Criterion> criterion = this->criterionCollection->GetCriterion(criId);
    this->UpdateTableWidgetInformation(criterion.data());
}

void FilterSelectDialog::slotInvertButtonClicked()
{
    int invert = !(this->selectCollection->IsInvertSelect());
    this->selectCollection->InvertSelection(invert);
    this->selectCollection->SetInvertSelect(invert);
    this->selectCollection->SetSelectStyle(Selection::NORMAL);

    this->renderer->AddActor(this->selectCollection->GetSelectActor());
    this->renderer->GetRenderWindow()->Render();
}

void FilterSelectDialog::slotResetButtonClicked()
{
    this->selectCollection->Initialize();
    this->selectCollection->SetSelectField(ui->entityBox->currentIndex() + 1);
    this->renderer->RemoveActor(this->selectCollection->GetSelectActor());
    this->renderer->GetRenderWindow()->Render();
}

void FilterSelectDialog::slotEnterButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(selection.isEmpty()) return;

    this->UpdateCriterionInformation(selection);
    ui->enterButton->setEnabled(false);
}

void FilterSelectDialog::slotSelectButtonClicked()
{
    vtkMultiBlockDataSet *blockDataSet = this->actorCollection->GetBlockActorDataSet();
    this->selectCollection->SetInputDataSet(blockDataSet);

    CriterionCollection::CriterionMap criterion_map = this->criterionCollection->GetCriterionMap();
    for (int i = 0; i < criterion_map.size(); i++)
    {
        const QString entity = ui->entityBox->currentText();
        const QString header = QString("criterion-%1").arg(i + 1);
        QSharedPointer<Criterion> criterion = criterion_map[header];

        vtkIdTypeArray *ids = (QString("Points") == entity)
                            ? criterion->filterNode()
                            : criterion->filterCell(this->cellType);

        vtkNew<vtkIdTypeArray> globalIds;
        for(vtkIdType i = 0; i < ids->GetNumberOfTuples(); i++)
        {
            globalIds->InsertNextValue(ids->GetValue(i));
        }

        if(QString("Normal") == criterion->logicsType)
        {
            this->selectCollection->SetSelectStyle(Selection::NORMAL);
        }
        else if(QString("Union") == criterion->logicsType)
        {
            this->selectCollection->SetSelectStyle(Selection::UNION);
        }
        else if(QString("Minus") == criterion->logicsType)
        {
            this->selectCollection->SetSelectStyle(Selection::MINUS);
        }

        this->selectCollection->GlobalIDsSelect(globalIds);
        this->selectCollection->HighlightSelection();
    }

    this->renderer->AddActor(this->selectCollection->GetSelectActor());
    this->renderer->GetRenderWindow()->Render();
}

void FilterSelectDialog::slotEntityIndexChanged(int index)
{
    this->selectCollection->SetSelectField(index + 1);
    ui->cellTypeBox->setEnabled(index);
}

void FilterSelectDialog::slotCellTypeChanged(const QString &text)
{
    this->cellType = (QString("Triangle") == text) ? VTK_TRIANGLE : VTK_QUAD;
}
