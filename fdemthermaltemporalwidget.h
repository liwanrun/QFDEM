#ifndef FDEMTHERMALTEMPORALWIDGET_H
#define FDEMTHERMALTEMPORALWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMThermalTemporalWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;
class QTableWidgetItem;

class ThermalTemporal;
class FDEMThermalTemporalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMThermalTemporalWidget(QWidget *parent = nullptr);
    ~FDEMThermalTemporalWidget();

    void InitPropertyBrowser();

    void ConnectSignalSlots();

    void UpdatePropertyBrowser(ThermalTemporal* temp);

private:
    void CreateInitialConditionProperty();

    void CreateInitialSpatialOptionProperty();

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetPropertyExpanded(const QString& id, bool expanded);

private:
    void UpdateInitialConditionProperty();

    void UpdateInitialSpatialOptionProperty();

public slots:
    void slotPropertyBrowserValueChanged(QtProperty *property, const QVariant &value);

    void slotSpatialTableWidgetItemChanged(QTableWidgetItem *item);

private:
    Ui::FDEMThermalTemporalWidget *ui;

    QtVariantPropertyManager *manager;
    QtVariantEditorFactory   *factory;

    ThermalTemporal *temporal;
};

#endif // FDEMTHERMALTEMPORALWIDGET_H
