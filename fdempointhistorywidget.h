#ifndef FDEMPOINTHISTORYWIDGET_H
#define FDEMPOINTHISTORYWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMPointHistoryWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;

class PointHistory;
class FDEMPointHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMPointHistoryWidget(QWidget *parent = nullptr);
    ~FDEMPointHistoryWidget();

    void InitPropertyBrowser();

    void ConnectSignalSlots();

    void UpdatePropertyBrowser(PointHistory* hist);

    void UpdateTableWidgetColumn(const QVector<size_t>& ids, int col);

public slots:
    void slotTreeBrowserValueChanged(QtProperty *property, const QVariant &value);

private:
    Ui::FDEMPointHistoryWidget *ui;

    QtVariantPropertyManager *manager;
    QtVariantEditorFactory   *factory;

    PointHistory *history;
};

#endif // FDEMPOINTHISTORYWIDGET_H
