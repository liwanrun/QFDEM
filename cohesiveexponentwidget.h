#ifndef COHESIVEEXPONENTWIDGET_H
#define COHESIVEEXPONENTWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class CohesiveExponentWidget;
}

class CohesiveExponentModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class CohesiveExponentWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit CohesiveExponentWidget(QWidget *parent = nullptr);
    ~CohesiveExponentWidget();

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::CohesiveExponentWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    CohesiveExponentModel *model;
};

#endif // COHESIVEEXPONENTWIDGET_H
