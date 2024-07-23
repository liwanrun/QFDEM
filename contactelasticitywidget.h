#ifndef CONTACTELASTICITYWIDGET_H
#define CONTACTELASTICITYWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class ContactElasticityWidget;
}

class ContactElasticityModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class ContactElasticityWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit ContactElasticityWidget(QWidget *parent = nullptr);
    ~ContactElasticityWidget();

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::ContactElasticityWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    ContactElasticityModel *model;
};

#endif // CONTACTELASTICITYWIDGET_H
