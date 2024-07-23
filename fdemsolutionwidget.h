#ifndef FDEMSOLUTIONWIDGET_H
#define FDEMSOLUTIONWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMSolutionWidget;
}

class QStandardItem;
class QItemSelection;
class QStandardItemModel;
class QtTreePropertyBrowser;
class FDEMSolutionSubstepWidget;
class FDEMSolutionItemDelegate;

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;

class Solution;
class SolutionCollection;
class FDEMSolutionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMSolutionWidget(QWidget *parent = nullptr);
    ~FDEMSolutionWidget();

    void InitSolutionTableView();

    void InitPropertyBrowser();

    void UpdatePropertyBrowser();

    void ConnectSignalSlots();

private:
    void UpdateSolutionProperty(const QString& id);

    void SetButtonsEnabled(bool arg);

    QList<QStandardItem *> CreateSolutionItems(const QString& solutionId);

public slots:
    void slotAppendButtonClicked();

    void slotRemoveButtonClicked();

    void slotClearButtonClicked();

    void slotTableModelItemChanged(QStandardItem *item);

    void slotSelectionChanged(const QItemSelection &selection, const QItemSelection &);

public slots:
    void slotPropertyManagerValueChanged(QtProperty *property, const QVariant &value);

private:
    void SetPropertyVisibility(const QString& id, bool visible);

    void SetPropertyExpanded(const QString& id, bool expanded);

    void CreateSolutionModelProperty();

    void CreateSolutionTimestepProperty();

    void CreateSolutionOptionProperty();

private:
    double GetMechanicalFEMTimestep();

    double GetMechanicalDEMTimestep();

    double GetHydraulicPorosityTimestep();

    double GetHydraulicFractureTimestep();

    double GetThermalSolidPhaseTimestep();

    double GetThermalFluidPhaseTimestep();

signals:
    void signalOutputPromptMessage(const QString msg);

    void signalSolutionCollectionChanged(int size);

public slots:
    void slotTimeMaximumStepButtonClicked();

private:
    Ui::FDEMSolutionWidget *ui;

    //TableView
    QStandardItemModel* tableModel;
    FDEMSolutionItemDelegate *delegate;

    //QtTreePropertyBrowser
    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    QSharedPointer<Solution> solution;
    QSharedPointer<SolutionCollection> solutionCollection;
};

#endif // FDEMSOLUTIONWIDGET_H
