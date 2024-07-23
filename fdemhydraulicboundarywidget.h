#ifndef FDEMHYDRAULICBOUNDARYWIDGET_H
#define FDEMHYDRAULICBOUNDARYWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMHydraulicBoundaryWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;
class QTableWidgetItem;

class HydraulicBoundary;
class FDEMHydraulicBoundaryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMHydraulicBoundaryWidget(QWidget *parent = nullptr);
    ~FDEMHydraulicBoundaryWidget();

    void InitPropertyBrowser();

    void ConnectSignalSlots();

    void UpdatePropertyBrowser(HydraulicBoundary* bnd);

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
    void slotHydraulicBrowserValueChanged(QtProperty *property, const QVariant &value);

    void slotSpatialTableWidgetItemChanged(QTableWidgetItem *item);

    void slotTemporalTableWidgetItemChanged(QTableWidgetItem *item);

private:
    Ui::FDEMHydraulicBoundaryWidget *ui;

    QtVariantPropertyManager *manager;
    QtVariantEditorFactory   *factory;

    HydraulicBoundary *boundary;
};

#endif // FDEMHYDRAULICBOUNDARYWIDGET_H
