#ifndef FDEMMATERIALWIDGET_H
#define FDEMMATERIALWIDGET_H

#include <QWidget>
#include <QItemSelection>

namespace Ui {
class FDEMMaterialWidget;
}

class QStandardItem;
class QStandardItemModel;
class FDEMMaterialItemDelegate;

class QtTreePropertyBrowser;
class FDEMTriangleMaterialWidget;
class FDEMCohesiveMaterialWidget;
class FDEMNTSFluidMaterialWidget;

class MaterialCollection;
class FDEMMaterialWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMMaterialWidget(QWidget *parent = nullptr);
    ~FDEMMaterialWidget();

    void InitMaterialTableView();

    void InitMaterialProperty();

    void ConnectSignalSlots();

private:
    void SetButtonsEnabled(bool arg);

    QList<QStandardItem *> CreateMaterialItems(const QString& materialId);

public slots:
    void slotAppendButtonClicked();

    void slotRemoveButtonClicked();

    void slotClearButtonClicked();

    void slotRescaleButtonClicked();

    void slotTableModelItemChanged(QStandardItem *item);

    void slotSelectionChanged(const QItemSelection &selection, const QItemSelection &deselected);

    void slotApplyButtonClicked();

    void slotDefaultButtonClicked();

    void slotCancelButtonClicked();

public slots:
    void slotSolutionCollectionChanged(int size);

signals:
    void signalOutputPromptMessage(const QString msg);

    void signalAnalysisModuleChanged(const QVariant& value);

    void signalApplyMaterialButtonClicked(const QString& id);

    void signalCancelMaterialButtonClicked(const QString& id);

    void signalClearMaterialButtonClicked();

private:
    Ui::FDEMMaterialWidget *ui;

    QStandardItemModel* tableModel;
    FDEMMaterialItemDelegate *delegate;

    QtTreePropertyBrowser *initialWidget;
    FDEMTriangleMaterialWidget* triangleWidget;
    FDEMCohesiveMaterialWidget * quadWidget;
    FDEMNTSFluidMaterialWidget * fluidWidget;
    
    QSharedPointer<MaterialCollection> materialCollection;
};

#endif // FDEMMATERIALWIDGET_H
