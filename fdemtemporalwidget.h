#ifndef FDEMTEMPORALWIDGET_H
#define FDEMTEMPORALWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMTemporalWidget;
}

class QStandardItem;
class QItemSelection;
class QStandardItemModel;
class FDEMTemporalItemDelegate;
class QtTreePropertyBrowser;
class FDEMMechanicalTemporalWidget;
class FDEMHydraulicTemporalWidget;
class FDEMThermalTemporalWidget;
class FDEMTemporalStressStrainWidget;

class TemporalCollection;
class FDEMTemporalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMTemporalWidget(QWidget *parent = nullptr);
    ~FDEMTemporalWidget();

    void InitTemporalTableView();

    void InitTemporalProperty();

    void ConnectSignalSlots();

private:
    void SetButtonsEnabled(bool arg);

    QList<QStandardItem *> CreateTemporalItems(const QString& temporalId);

public slots:
    void slotAppendButtonClicked();

    void slotRemoveButtonClicked();

    void slotClearButtonClicked();

    void slotSiteDependentButtonClicked();

    void slotTableModelItemChanged(QStandardItem *item);

    void slotSelectionChanged(const QItemSelection &selection, const QItemSelection &deselected);

    void slotApplyButtonClicked();

    void slotDefaultButtonClicked();

    void slotCancelButtonClicked();

signals:
    void signalOutputPromptMessage(const QString msg);

    void signalApplyTemporalButtonClicked(const QString& id);

    void signalCancelTemporalButtonClicked(const QString& id);

    void signalClearTemporalButtonClicked();

private:
    Ui::FDEMTemporalWidget *ui;

    QStandardItemModel *tableModel;
    FDEMTemporalItemDelegate* delegate;

    QtTreePropertyBrowser*          initialWidget;
    FDEMMechanicalTemporalWidget*   mechanicalWidget;
    FDEMHydraulicTemporalWidget*    hydraulicWidget;
    FDEMThermalTemporalWidget*      thermalWidget;
    FDEMTemporalStressStrainWidget* stressStrainWidget;

    QSharedPointer<TemporalCollection> temporalCollection;
};

#endif // FDEMTEMPORALWIDGET_H
