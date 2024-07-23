#ifndef FDEMMECHANICALTEMPORALWIDGET_H
#define FDEMMECHANICALTEMPORALWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMMechanicalTemporalWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;
class QTableWidgetItem;

class MechanicalTemporal;
class FDEMMechanicalTemporalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMMechanicalTemporalWidget(QWidget *parent = nullptr);
    ~FDEMMechanicalTemporalWidget();

    void InitPropertyBrowser();

    void ConnectSignalSlots();

    void UpdatePropertyBrowser(MechanicalTemporal* temp);

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
    Ui::FDEMMechanicalTemporalWidget *ui;

    QtVariantPropertyManager *manager;
    QtVariantEditorFactory   *factory;

    MechanicalTemporal *temporal;
};

#endif // FDEMMECHANICALTEMPORALWIDGET_H
