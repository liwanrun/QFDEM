#ifndef FDEMHISTORYWIDGET_H
#define FDEMHISTORYWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMHistoryWidget;
}

class QStandardItem;
class QItemSelection;
class QStandardItemModel;
class FDEMHistoryItemDelegate;
class QtTreePropertyBrowser;
class FDEMFieldHistoryWidget;
class FDEMPointHistoryWidget;
class FDEMCellHistoryWidget;
class QTableWidget;

class History;
class HistoryCollection;
class FDEMHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMHistoryWidget(QWidget *parent = nullptr);
    ~FDEMHistoryWidget();

    void InitHistoryTableView();

    void InitHistoryProperty();

    void ConnectSignalSlots();

private:
    void SetButtonsEnabled(bool arg);

    QList<QStandardItem *> CreateHistoryItems(const QString& historyId);

    void CreateHistoryProperty();

public slots:
    void slotAppendButtonClicked();

    void slotRemoveButtonClicked();

    void slotClearButtonClicked();

    void slotTableModelItemChanged(QStandardItem *item);

    void slotSelectionChanged(const QItemSelection &selection, const QItemSelection &deselected);

    void slotApplyButtonClicked();

    void slotResetButtonClicked();

signals:
    void signalOutputPromptMessage(const QString msg);

    void signalApplyHistoryButtonClicked();

private:
    Ui::FDEMHistoryWidget *ui;

    QStandardItemModel *tableModel;
    FDEMHistoryItemDelegate* delegate;

    QtTreePropertyBrowser*  initialWidget;
    FDEMFieldHistoryWidget* histFieldWidget;
    FDEMPointHistoryWidget* histPointWidget;
    FDEMCellHistoryWidget*  histCellWidget;

    QTableWidget* historyIdTable;

    QSharedPointer<HistoryCollection> historyCollection;
};

#endif // FDEMHISTORYWIDGET_H
