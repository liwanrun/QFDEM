#ifndef FDEMCOHESIVEMATERIALWIDGET_H
#define FDEMCOHESIVEMATERIALWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMCohesiveMaterialWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;

class CohesiveMaterial;
class ConstitutiveModelWidget;
class FDEMCohesiveMaterialWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMCohesiveMaterialWidget(QWidget *parent = nullptr);
    ~FDEMCohesiveMaterialWidget();

    void InitPropertyBrowser();

    void UpdatePropertyBrowser(CohesiveMaterial* mat);

public:
    void InitMaterialGlobalControlWidget();

    void InitMechanicalConstitutiveWidget();

    void InitHydraulicConstitutiveWidget();

    void InitThermalConstitutiveWidget();

private:
    void UpdateMaterialGlobalControlWidget();

    void UpdateMechanicalConstitutiveWidget();

    void UpdateHydraulicConstitutiveWidget();

    void UpdateThermalConstitutiveWidget();

public slots:
    void slotColumnSolutionChanged(const QString id);

public slots:
    void slotMaterialGlobalValueChanged(QtProperty *property, const QVariant &val);

    void slotMechanicalConstitutiveModelChanged(int index);

    void slotHydraulicConstitutiveModelChanged(int index);

    void slotThermalConstitutiveModelChanged(int index);

private:
    Ui::FDEMCohesiveMaterialWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    QSharedPointer<ConstitutiveModelWidget> mechanicalWidget;
    QSharedPointer<ConstitutiveModelWidget> hydraulicWidget;
    QSharedPointer<ConstitutiveModelWidget> thermalWidget;

    CohesiveMaterial *material;
};

#endif // FDEMCOHESIVEMATERIALWIDGET_H
