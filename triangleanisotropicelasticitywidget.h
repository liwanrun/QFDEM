#ifndef TRIANGLEANISOTROPICELASTICITYWIDGET_H
#define TRIANGLEANISOTROPICELASTICITYWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class TriangleAnisotropicElasticityWidget;
}

class AnisotropicElasticityModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class TriangleAnisotropicElasticityWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit TriangleAnisotropicElasticityWidget(QWidget *parent = nullptr);
    ~TriangleAnisotropicElasticityWidget();

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::TriangleAnisotropicElasticityWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    AnisotropicElasticityModel *model;
};

#endif // TRIANGLEANISOTROPICELASTICITYWIDGET_H
