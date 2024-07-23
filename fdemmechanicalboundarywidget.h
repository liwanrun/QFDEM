#ifndef FDEMMECHANICALBOUNDARYWIDGET_H
#define FDEMMECHANICALBOUNDARYWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMMechanicalBoundaryWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;
class QTableWidgetItem;

class MechanicalBoundary;
class FDEMMechanicalBoundaryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMMechanicalBoundaryWidget(QWidget *parent = nullptr);
    ~FDEMMechanicalBoundaryWidget();

    void InitPropertyBrowser();

    void ConnectSignalSlots();

    void UpdatePropertyBrowser(MechanicalBoundary* bnd);

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
    void slotPropertyBrowserValueChanged(QtProperty *property, const QVariant &value);

    void slotSpatialTableWidgetItemChanged(QTableWidgetItem *item);

    void slotTemporalTableWidgetItemChanged(QTableWidgetItem *item);

private:
    Ui::FDEMMechanicalBoundaryWidget *ui;

    QtVariantPropertyManager *manager;
    QtVariantEditorFactory   *factory;

    MechanicalBoundary *boundary;
};

#endif // FDEMMECHANICALBOUNDARYWIDGET_H
