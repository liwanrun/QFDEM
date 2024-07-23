#ifndef COHESIVEHEATEXCHANGEWIDGET_H
#define COHESIVEHEATEXCHANGEWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class CohesiveHeatExchangeWidget;
}

class CohesiveThermalModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class CohesiveHeatExchangeWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit CohesiveHeatExchangeWidget(QWidget *parent = nullptr);
    ~CohesiveHeatExchangeWidget();

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::CohesiveHeatExchangeWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    CohesiveThermalModel *model;
};

#endif // COHESIVEHEATEXCHANGEWIDGET_H
