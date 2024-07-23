#ifndef TRIANGLEHOOKEANELASTICITYWIDGET_H
#define TRIANGLEHOOKEANELASTICITYWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class TriangleHookeanElasticityWidget;
}

class HookeanElasticityModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class TriangleHookeanElasticityWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit TriangleHookeanElasticityWidget(QWidget *parent = nullptr);
    ~TriangleHookeanElasticityWidget();

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::TriangleHookeanElasticityWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    HookeanElasticityModel *model;
};

#endif // TRIANGLEHOOKEANELASTICITYWIDGET_H
