#ifndef COHESIVEBILINEARWIDGET_H
#define COHESIVEBILINEARWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class CohesiveBilinearWidget;
}

class CohesiveBilinearModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class CohesiveBilinearWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit CohesiveBilinearWidget(QWidget *parent = nullptr);
    ~CohesiveBilinearWidget();

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::CohesiveBilinearWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    CohesiveBilinearModel *model;
};

#endif // COHESIVEBILINEARWIDGET_H
