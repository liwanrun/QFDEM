#ifndef FDEMCELLHISTORYWIDGET_H
#define FDEMCELLHISTORYWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMCellHistoryWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;

class CellHistory;
class FDEMCellHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMCellHistoryWidget(QWidget *parent = nullptr);
    ~FDEMCellHistoryWidget();

    void InitPropertyBrowser();

    void ConnectSignalSlots();

    void UpdatePropertyBrowser(CellHistory* hist);

    void UpdateTableWidgetColumn(const QVector<size_t>& ids, int col);

public slots:
    void slotTreeBrowserValueChanged(QtProperty *property, const QVariant &value);

private:
    Ui::FDEMCellHistoryWidget *ui;

    QtVariantPropertyManager *manager;
    QtVariantEditorFactory   *factory;

    CellHistory *history;
};

#endif // FDEMCELLHISTORYWIDGET_H
