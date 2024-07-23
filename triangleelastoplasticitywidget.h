#ifndef TRIANGLEELASTOPLASTICITYWIDGET_H
#define TRIANGLEELASTOPLASTICITYWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class TriangleElastoPlasticityWidget;
}

class ElastoPlasticityModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class TriangleElastoPlasticityWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit TriangleElastoPlasticityWidget(QWidget *parent = nullptr);
    ~TriangleElastoPlasticityWidget();

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::TriangleElastoPlasticityWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    ElastoPlasticityModel *model;
};

#endif // TRIANGLEELASTOPLASTICITYWIDGET_H
