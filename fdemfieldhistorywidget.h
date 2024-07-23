#ifndef FDEMFIELDHISTORYWIDGET_H
#define FDEMFIELDHISTORYWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMFieldHistoryWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;

class GlobalHistory;
class FDEMFieldHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMFieldHistoryWidget(QWidget *parent = nullptr);
    ~FDEMFieldHistoryWidget();

    void InitPropertyBrowser();

    void ConnectSignalSlots();

    void UpdatePropertyBrowser(GlobalHistory* hist);

public slots:
    void slotTreeBrowserValueChanged(QtProperty *property, const QVariant &value);

private:
    Ui::FDEMFieldHistoryWidget *ui;

    QtVariantPropertyManager *manager;
    QtVariantEditorFactory   *factory;

    GlobalHistory *history;
};

#endif // FDEMFIELDHISTORYWIDGET_H
