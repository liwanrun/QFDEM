#include "fdemhistorywidget.h"
#include "ui_fdemhistorywidget.h"
#include "fdemhistoryitemdelegate.h"
#include "fdempointhistorywidget.h"
#include "fdemcellhistorywidget.h"
#include "fdemfieldhistorywidget.h"
#include "selectcollection.h"
#include "historycollection.h"
#include "history.h"

#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QtVariantProperty>
#include <QtTreePropertyBrowser>
#include <QTableWidget>

FDEMHistoryWidget::FDEMHistoryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMHistoryWidget)
{
    ui->setupUi(this);

    this->tableModel = new QStandardItemModel(this);
    ui->tableView->setModel(this->tableModel);
    this->delegate = new FDEMHistoryItemDelegate(this);
    ui->tableView->setItemDelegate(this->delegate);

    this->SetButtonsEnabled(false);
    this->InitHistoryTableView();
    this->InitHistoryProperty();

    this->ConnectSignalSlots();

    this->historyCollection = HistoryCollection::GetHistoryCollection();
}

FDEMHistoryWidget::~FDEMHistoryWidget()
{
    delete this->initialWidget;
    delete this->histPointWidget;
    delete this->histCellWidget;
    delete this->histFieldWidget;

    delete this->tableModel;
    delete this->delegate;

    delete ui;
}

void FDEMHistoryWidget::InitHistoryTableView()
{
    //Headers
    QFont font(QString("Arial"), -1, QFont::Bold);
    QStringList headers = QStringList() << "Name" << "History Type" << "Ready";
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
    ui->tableView->setColumnWidth(0, 120);
    ui->tableView->setColumnWidth(1, 120);
    ui->tableView->setColumnWidth(2, 120);
}

void FDEMHistoryWidget::InitHistoryProperty()
{
    this->initialWidget = new QtTreePropertyBrowser;
    ui->propertyWidget->layout()->addWidget(this->initialWidget);
    this->initialWidget->show();

    this->histFieldWidget = new FDEMFieldHistoryWidget;
    ui->propertyWidget->layout()->addWidget(this->histFieldWidget);
    this->histFieldWidget->hide();

    this->histPointWidget = new FDEMPointHistoryWidget;
    ui->propertyWidget->layout()->addWidget(this->histPointWidget);
    this->histPointWidget->hide();

    this->histCellWidget = new FDEMCellHistoryWidget;
    ui->propertyWidget->layout()->addWidget(this->histCellWidget);
    this->histCellWidget->hide();
}

void FDEMHistoryWidget::ConnectSignalSlots()
{
    connect(ui->appendButton, &QPushButton::clicked,
            this, &FDEMHistoryWidget::slotAppendButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked,
            this, &FDEMHistoryWidget::slotRemoveButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked,
            this, &FDEMHistoryWidget::slotClearButtonClicked);

    connect(this->tableModel, &QStandardItemModel::itemChanged,
            this, &FDEMHistoryWidget::slotTableModelItemChanged);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &FDEMHistoryWidget::slotSelectionChanged);

    connect(ui->applyButton, &QPushButton::clicked,
            this, &FDEMHistoryWidget::slotApplyButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked,
            this, &FDEMHistoryWidget::slotResetButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &FDEMHistoryWidget::slotResetButtonClicked);
}

void FDEMHistoryWidget::SetButtonsEnabled(bool arg)
{
    ui->removeButton->setEnabled(arg);
    ui->clearButton->setEnabled(arg);
    ui->applyButton->setEnabled(arg);
    ui->resetButton->setEnabled(arg);
}

QList<QStandardItem *> FDEMHistoryWidget::CreateHistoryItems(const QString &historyId)
{
    QString path = QString(":/images/resources/");

    //History title
    QStandardItem* title = new QStandardItem(historyId);
    title->setTextAlignment(Qt::AlignCenter);
    title->setEditable(false);

    //History scope
    QStandardItem* scope = new QStandardItem;
    scope->setData(QString("Global"), Qt::EditRole);
    scope->setData(QIcon(path + QString("pqGlobalData.png")), Qt::DecorationRole);
    scope->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    //History state
    QStandardItem* state = new QStandardItem;
    state->setData(false, Qt::DisplayRole);
    state->setTextAlignment(Qt::AlignCenter);
    state->setEditable(false);

    QList<QStandardItem *> items;
    items << title << scope << state;
    return items;
}

void FDEMHistoryWidget::slotAppendButtonClicked()
{
    int index = this->tableModel->rowCount() + 1;
    QString histId = QString("history-%1").arg(index);

    this->historyCollection->AppendHistory(histId);
    this->tableModel->appendRow(this->CreateHistoryItems(histId));

    this->SetButtonsEnabled(true);
}

void FDEMHistoryWidget::slotRemoveButtonClicked()
{
    int index = this->tableModel->rowCount();
    QString histId = QString("history-%1").arg(index);

    this->tableModel->removeRow(this->tableModel->rowCount() - 1);
    this->historyCollection->RemoveHistory(histId);
    this->SetButtonsEnabled(this->tableModel->rowCount());

    ui->tableView->selectRow(this->tableModel->rowCount() - 1);
    if(0 == this->tableModel->rowCount())
    {
        this->initialWidget->show();
        this->histFieldWidget->hide();
        this->histPointWidget->hide();
        this->histCellWidget->hide();
    }
}

void FDEMHistoryWidget::slotClearButtonClicked()
{
    this->tableModel->removeRows(0, this->tableModel->rowCount());
    this->historyCollection->Clear();
    this->SetButtonsEnabled(false);

    if(!this->tableModel->rowCount())
    {
        this->initialWidget->show();
        this->histFieldWidget->hide();
        this->histPointWidget->hide();
        this->histCellWidget->hide();
    }
}

void FDEMHistoryWidget::slotTableModelItemChanged(QStandardItem *item)
{
    QString id = this->tableModel->item(item->row(), 0)->data(Qt::DisplayRole).toString();

    if(COL_SCOPE == item->column())
    {
        QString histType = item->data(Qt::EditRole).toString();

        if(QString("Point(s)") == histType)
        {
            QSharedPointer<PointHistory> newHistory =  QSharedPointer<PointHistory>(new PointHistory);
            this->historyCollection->UpdateHistory(id, newHistory);
            this->histPointWidget->UpdatePropertyBrowser(newHistory.get());
            this->histPointWidget->show();

            this->initialWidget->hide();
            this->histFieldWidget->hide();
            this->histCellWidget->hide();
        }
        else if(QString("Cell(s)") == histType)
        {
            QSharedPointer<CellHistory> newHistory =  QSharedPointer<CellHistory>(new CellHistory);
            this->historyCollection->UpdateHistory(id, newHistory);
            this->histCellWidget->UpdatePropertyBrowser(newHistory.get());
            this->histCellWidget->show();

            this->initialWidget->hide();
            this->histPointWidget->hide();
            this->histFieldWidget->hide();
        }
        else if(QString("Global") == histType)
        {
            QSharedPointer<GlobalHistory> newHistory =  QSharedPointer<GlobalHistory>(new GlobalHistory);
            this->historyCollection->UpdateHistory(id, newHistory);
            this->histFieldWidget->UpdatePropertyBrowser(newHistory.get());
            this->histFieldWidget->show();

            this->initialWidget->hide();
            this->histPointWidget->hide();
            this->histCellWidget->hide();
        }
    }
}

void FDEMHistoryWidget::slotSelectionChanged(const QItemSelection &selection, const QItemSelection &)
{
    if(selection.isEmpty()) return;

    QString id = selection.indexes().at(COL_TITLE).data().toString();
    QSharedPointer<History> history = this->historyCollection->GetHistory(id);

    if(history->historyType == History::POINT)
    {
        PointHistory *pointHist = static_cast<PointHistory*>(history.get());
        this->histPointWidget->UpdatePropertyBrowser(pointHist);
        this->histPointWidget->show();

        this->initialWidget->hide();
        this->histFieldWidget->hide();
        this->histCellWidget->hide();
    }
    else if(history->historyType == History::CELL)
    {
        CellHistory *cellHist = static_cast<CellHistory*>(history.get());
        this->histCellWidget->UpdatePropertyBrowser(cellHist);
        this->histCellWidget->show();

        this->initialWidget->hide();
        this->histPointWidget->hide();
        this->histFieldWidget->hide();
    }
    else if(history->historyType == History::GLOBAL)
    {
        GlobalHistory *fieldHist = static_cast<GlobalHistory*>(history.get());
        this->histFieldWidget->UpdatePropertyBrowser(fieldHist);
        this->histFieldWidget->show();

        this->initialWidget->hide();
        this->histPointWidget->hide();
        this->histCellWidget->hide();
    }
}

void FDEMHistoryWidget::slotApplyButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(!selection.isEmpty())
    {
        QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
        QString historyId = selection.indexes().at(COL_TITLE).data().toString();
        QSharedPointer<History> history = this->historyCollection->GetHistory(historyId);

        if(history->historyType != selectCollection->GetSelectField())
        {
            QMessageBox::warning(this, QString("Invalid Operation"),
                                 QString("Select the correct entities. Thanks!"));
            emit this->signalApplyHistoryButtonClicked();
            return;
        }

        if(history->historyType == History::POINT)
        {

            PointHistory *pointHist = static_cast<PointHistory*>(history.get());
            pointHist->globalIds = selectCollection->GetExtractPedigreeIds();
            pointHist->pedigreeIds = selectCollection->GetExtractPedigreeIds();
            this->histPointWidget->UpdateTableWidgetColumn(pointHist->globalIds, 0);
            this->histPointWidget->UpdateTableWidgetColumn(pointHist->pedigreeIds, 1);
            emit this->signalApplyHistoryButtonClicked();
        }
        else if(history->historyType == History::CELL)
        {
            CellHistory *cellHist = static_cast<CellHistory*>(history.get());
            cellHist->globalIds = selectCollection->GetExtractGlobalIds();
            cellHist->pedigreeIds = selectCollection->GetExtractPedigreeIds();
            this->histCellWidget->UpdateTableWidgetColumn(cellHist->globalIds, 0);
            this->histCellWidget->UpdateTableWidgetColumn(cellHist->pedigreeIds, 1);
            emit this->signalApplyHistoryButtonClicked();
        }

        QModelIndex index = selection.indexes().at(COL_STATE);
        this->tableModel->itemFromIndex(index)->setData(true, Qt::EditRole);
    }
    else
    {
        QMessageBox::warning(this, QString("Invalid Operation"),
                             QString("Select the history to be set. Thanks!"));
    }
}

void FDEMHistoryWidget::slotResetButtonClicked()
{
    QItemSelection selection = ui->tableView->selectionModel()->selection();
    if(!selection.isEmpty())
    {
        QString historyId = selection.indexes().at(COL_TITLE).data().toString();
        QSharedPointer<History> history = historyCollection->GetHistory(historyId);

        if(History::POINT == history->getHistoryType())
        {
            QSharedPointer<PointHistory> pointHist = QSharedPointer<PointHistory>(new PointHistory);
            this->histPointWidget->UpdatePropertyBrowser(pointHist.get());

            QModelIndex index = selection.indexes().at(COL_STATE);
            this->tableModel->itemFromIndex(index)->setData(false, Qt::EditRole);
        }
        else if(History::CELL == history->getHistoryType())
        {
            QSharedPointer<CellHistory> cellHist = QSharedPointer<CellHistory>(new CellHistory);
            this->histCellWidget->UpdatePropertyBrowser(cellHist.get());

            QModelIndex index = selection.indexes().at(COL_STATE);
            this->tableModel->itemFromIndex(index)->setData(false, Qt::EditRole);
        }
        else if(History::GLOBAL == history->getHistoryType())
        {
            QSharedPointer<GlobalHistory> fieldHist = QSharedPointer<GlobalHistory>(new GlobalHistory);
            this->histFieldWidget->UpdatePropertyBrowser(fieldHist.get());

            QModelIndex index = selection.indexes().at(COL_STATE);
            this->tableModel->itemFromIndex(index)->setData(false, Qt::EditRole);
        }
    }
    else
    {
        QMessageBox::warning(this, QString("Invalid Operation"),
                             QString("Select the history to be set. Thanks!"));
    }
}
