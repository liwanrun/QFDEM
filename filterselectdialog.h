#ifndef FILTERSELECTDIALOG_H
#define FILTERSELECTDIALOG_H

#include <QDialog>
#include <vtkSmartPointer.h>

namespace Ui {
class FilterSelectDialog;
}

class vtkRenderer;
class vtkDoubleArray;
class vtkIdTypeArray;
class QStandardItem;
class QItemSelection;
class QStandardItemModel;
class FilterSelectDelegate;
class QTableWidgetItem;

class Criterion;
class ActorCollection;
class SelectCollection;
class CriterionCollection;
class FilterSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterSelectDialog(QWidget *parent = nullptr);
    ~FilterSelectDialog();

    void SetDefaultRenderer(vtkRenderer * renderer);

    void InitFindDataDialog();

    void ConnectSignalSlots();

private:
    void SetButtonsEnabled(bool arg);

    vtkDoubleArray *ReadFromTableWidget();

    QList<QStandardItem *> CreateCriterionItems(const QString& criId);

    void UpdateTableWidgetInformation(const Criterion *crit);

    void UpdateCriterionInformation(const QItemSelection& selection);

public slots:
    void slotAppendButtonClicked();

    void slotRemoveButtonClicked();

    void slotClearButtonClicked();

    void slotTableModelItemChanged(QStandardItem *item);

    void slotTableWidgetItemChanged(QTableWidgetItem *item);

    void slotSelectionChanged(const QItemSelection &selection, const QItemSelection &);

    void slotInvertButtonClicked();

    void slotResetButtonClicked();

    void slotEnterButtonClicked();

    void slotSelectButtonClicked();

    void slotEntityIndexChanged(int index);

    void slotCellTypeChanged(const QString& text);

private:
    Ui::FilterSelectDialog *ui;
    QString path;
    int cellType;

    QStandardItemModel* tableModel;
    FilterSelectDelegate *delegate;

    vtkSmartPointer<vtkRenderer> renderer;

    QSharedPointer<ActorCollection> actorCollection;
    QSharedPointer<SelectCollection> selectCollection;
    QSharedPointer<CriterionCollection> criterionCollection;
};

#endif // FILTERSELECTDIALOG_H
