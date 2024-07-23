#ifndef CONTACTPLASTICITYWIDGET_H
#define CONTACTPLASTICITYWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class ContactPlasticityWidget;
}

class ContactPlasticityModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class ContactPlasticityWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit ContactPlasticityWidget(QWidget *parent = nullptr);
    ~ContactPlasticityWidget();

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::ContactPlasticityWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    ContactPlasticityModel *model;
};

#endif // CONTACTPLASTICITYWIDGET_H
