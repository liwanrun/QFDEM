#ifndef FDEMHYDRAULICTEMPORALWIDGET_H
#define FDEMHYDRAULICTEMPORALWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMHydraulicTemporalWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;
class QTableWidgetItem;

class HydraulicTemporal;
class FDEMHydraulicTemporalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMHydraulicTemporalWidget(QWidget *parent = nullptr);
    ~FDEMHydraulicTemporalWidget();

    void InitPropertyBrowser();

    void ConnectSignalSlots();

    void UpdatePropertyBrowser(HydraulicTemporal* temp);

private:
    void SetPropertyVisibility(const QString& id, bool visible);

    void SetPropertyExpanded(const QString& id, bool expanded);

    void CreateInitialConditionProperty();

    void CreateInitialSpatialOptionProperty();

private:
    void UpdateInitialConditionProperty();

    void UpdateInitialSpatialOptionProperty();

public slots:
    void slotPropertyBrowserValueChanged(QtProperty *property, const QVariant &value);

    void slotSpatialTableWidgetItemChanged(QTableWidgetItem *item);

private:
    Ui::FDEMHydraulicTemporalWidget *ui;

    QtVariantPropertyManager *manager;
    QtVariantEditorFactory   *factory;

    HydraulicTemporal *temporal;
};

#endif // FDEMHYDRAULICTEMPORALWIDGET_H
