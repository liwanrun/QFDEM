#ifndef FDEMTHERMALBOUNDARYWIDGET_H
#define FDEMTHERMALBOUNDARYWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMThermalBoundaryWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;
class QTableWidgetItem;

class ThermalBoundary;
class FDEMThermalBoundaryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMThermalBoundaryWidget(QWidget *parent = nullptr);
    ~FDEMThermalBoundaryWidget();

    void InitPropertyBrowser();

    void ConnectSignalSlots();

    void UpdatePropertyBrowser(ThermalBoundary* bnd);

private:
    void SetPropertyVisibility(const QString& id, bool visible);

    void SetPropertyExpanded(const QString& id, bool expanded);

    void CreateBoundaryConditionProperty();

    void CreateBoundarySpatialOptionProperty();

    void CreateBoundaryTemporalOptionProperty();

private:
    void UpdateBoundaryConditionProperty();

    void UpdateBoundarySpatialOptionProperty();

    void UpdateBoundaryTemporalOptionProperty();

public slots:
    void slotThermalBrowserValueChanged(QtProperty *property, const QVariant &value);

    void slotSpatialTableWidgetItemChanged(QTableWidgetItem *item);

    void slotTemporalTableWidgetItemChanged(QTableWidgetItem *item);

private:
    Ui::FDEMThermalBoundaryWidget *ui;

    QtVariantPropertyManager *manager;
    QtVariantEditorFactory   *factory;

    ThermalBoundary *boundary;
};

#endif // FDEMTHERMALBOUNDARYWIDGET_H
