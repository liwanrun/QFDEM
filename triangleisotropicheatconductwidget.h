#ifndef TRIANGLEISOTROPICHEATCONDUCTWIDGET_H
#define TRIANGLEISOTROPICHEATCONDUCTWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class TriangleIsotropicHeatConductWidget;
}

class IsotropicConductModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class TriangleIsotropicHeatConductWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit TriangleIsotropicHeatConductWidget(QWidget *parent = nullptr);
    ~TriangleIsotropicHeatConductWidget() override;

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::TriangleIsotropicHeatConductWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    IsotropicConductModel *model;
};

#endif // TRIANGLEISOTROPICHEATCONDUCTWIDGET_H
