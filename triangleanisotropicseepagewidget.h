#ifndef TRIANGLEANISOTROPICSEEPAGEWIDGET_H
#define TRIANGLEANISOTROPICSEEPAGEWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class TriangleAnisotropicSeepageWidget;
}

class AnisotropicSeepageModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class TriangleAnisotropicSeepageWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit TriangleAnisotropicSeepageWidget(QWidget *parent = nullptr);
    ~TriangleAnisotropicSeepageWidget();

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::TriangleAnisotropicSeepageWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    AnisotropicSeepageModel *model;
};

#endif // TRIANGLEANISOTROPICSEEPAGEWIDGET_H
