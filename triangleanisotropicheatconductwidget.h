#ifndef TRIANGLEANISOTROPICHEATCONDUCTWIDGET_H
#define TRIANGLEANISOTROPICHEATCONDUCTWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class TriangleAnisotropicHeatConductWidget;
}

class AnisotropicConductModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class TriangleAnisotropicHeatConductWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit TriangleAnisotropicHeatConductWidget(QWidget *parent = nullptr);
    ~TriangleAnisotropicHeatConductWidget();

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::TriangleAnisotropicHeatConductWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    AnisotropicConductModel *model;
};

#endif // TRIANGLEANISOTROPICHEATCONDUCTWIDGET_H
