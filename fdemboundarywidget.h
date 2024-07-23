#ifndef FDEMBOUNDARYWIDGET_H
#define FDEMBOUNDARYWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMBoundaryWidget;
}

class QStandardItem;
class QItemSelection;
class QStandardItemModel;
class FDEMBoundaryItemDelegate;
class QtTreePropertyBrowser;
class FDEMMechanicalBoundaryWidget;
class FDEMHydraulicBoundaryWidget;
class FDEMThermalBoundaryWidget;

class BoundaryCollection;
class FDEMBoundaryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMBoundaryWidget(QWidget *parent = nullptr);
    ~FDEMBoundaryWidget();

    void InitBoundaryTableView();

    void InitBoundaryProperty();

    void ConnectSignalSlots();

private:
    void SetButtonsEnabled(bool arg);

    QList<QStandardItem *> CreateBoundaryItems(const QString& boundaryId);

public slots:
    void slotAppendButtonClicked();

    void slotRemoveButtonClicked();

    void slotClearButtonClicked();

    void slotSiteDependentButtonClicked();

    void slotTimeDependentButtonClicked();

    void slotTableModelItemChanged(QStandardItem *item);

    void slotSelectionChanged(const QItemSelection &selection, const QItemSelection &deselected);

    void slotDefaultButtonClicked();

    void slotApplyButtonClicked();

    void slotCancelButtonClicked();

public slots:
    void slotSolutionCollectionChanged(int size);

signals:
    void signalOutputPromptMessage(const QString msg);

    void signalAnalysisModuleChanged(const QVariant& value);

    void signalUpdateBoundaryButtonClicked(const QString& id);

    void signalApplyBoundaryButtonClicked(const QString& id);

    void signalCancelBoundaryButtonClicked(const QString& id);

    void signalClearBoundaryButtonClicked();

private:
    Ui::FDEMBoundaryWidget *ui;

    QStandardItemModel *tableModel;
    FDEMBoundaryItemDelegate* delegate;

    QtTreePropertyBrowser*          initialWidget;
    FDEMMechanicalBoundaryWidget*   mechanicalWidget;
    FDEMHydraulicBoundaryWidget*    hydraulicWidget;
    FDEMThermalBoundaryWidget*      thermalWidget;

    QSharedPointer<BoundaryCollection> boundaryCollection;
};

#endif // FDEMBOUNDARYWIDGET_H
