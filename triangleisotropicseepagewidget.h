#ifndef TRIANGLEISOTROPICSEEPAGEWIDGET_H
#define TRIANGLEISOTROPICSEEPAGEWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class TriangleIsotropicSeepageWidget;
}

class IsotropicSeepageModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class TriangleIsotropicSeepageWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit TriangleIsotropicSeepageWidget(QWidget *parent = nullptr);
    ~TriangleIsotropicSeepageWidget() override;

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::TriangleIsotropicSeepageWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    IsotropicSeepageModel *model;
};

#endif // TRIANGLEISOTROPICSEEPAGEWIDGET_H
