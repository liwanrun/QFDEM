#ifndef COHESIVEINCOMPRESSIBLEFLOWWIDGET_H
#define COHESIVEINCOMPRESSIBLEFLOWWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class CohesiveIncompressibleFlowWidget;
}

class IncompressibleFlowModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class CohesiveIncompressibleFlowWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit CohesiveIncompressibleFlowWidget(QWidget *parent = nullptr);
    ~CohesiveIncompressibleFlowWidget();

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::CohesiveIncompressibleFlowWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    IncompressibleFlowModel *model;
};

#endif // COHESIVEINCOMPRESSIBLEFLOWWIDGET_H
