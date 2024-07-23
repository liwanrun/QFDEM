#ifndef FDEMTEMPORALSTRESSSTRAINWIDGET_H
#define FDEMTEMPORALSTRESSSTRAINWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMTemporalStressStrainWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;
class QTableWidgetItem;

class TemporalStressStrain;
class FDEMTemporalStressStrainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMTemporalStressStrainWidget(QWidget *parent = nullptr);
    ~FDEMTemporalStressStrainWidget();

    void InitPropertyBrowser();

    void ConnectSignalSlots();

    void UpdatePropertyBrowser(TemporalStressStrain* temp);

private:
    void SetPropertyVisibility(const QString& id, bool visible);

    void SetPropertyExpanded(const QString& id, bool expanded);

    void CreateInitialConditionProperty();

private:
    void UpdateInitialConditionProperty();

    void UpdateInitialSpatialTableWidget();

public slots:
    void slotPropertyBrowserValueChanged(QtProperty *property, const QVariant &value);

    void slotSpatialTableWidgetItemChanged(QTableWidgetItem *item);

private:
    Ui::FDEMTemporalStressStrainWidget *ui;

    QtVariantPropertyManager *manager;
    QtVariantEditorFactory   *factory;

    TemporalStressStrain *temporal;
};

#endif // FDEMTEMPORALSTRESSSTRAINWIDGET_H
