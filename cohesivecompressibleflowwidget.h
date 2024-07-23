#ifndef COHESIVECOMPRESSIBLEFLOWWIDGET_H
#define COHESIVECOMPRESSIBLEFLOWWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class CohesiveCompressibleFlowWidget;
}

class CompressibleFlowModel;
class QtVariantEditorFactory;
class QtVariantPropertyManager;
class CohesiveCompressibleFlowWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit CohesiveCompressibleFlowWidget(QWidget *parent = nullptr);
    ~CohesiveCompressibleFlowWidget();

    void CreateConstitutiveProperty(ConstitutiveModel *constitutive) override;

    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

public slots:
    void slotPropertyValueChanged(QtProperty *property, const QVariant& value);

private:
    Ui::CohesiveCompressibleFlowWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    CompressibleFlowModel *model;
};

#endif // COHESIVECOMPRESSIBLEFLOWWIDGET_H
