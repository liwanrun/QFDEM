#ifndef FDEMTRIANGLEMATERIALWIDGET_H
#define FDEMTRIANGLEMATERIALWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMTriangleMaterialWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;

class TriangleMaterial;
class ConstitutiveModelWidget;
class FDEMTriangleMaterialWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMTriangleMaterialWidget(QWidget *parent = nullptr);

    ~FDEMTriangleMaterialWidget();

    void InitPropertyBrowser();

    void UpdatePropertyBrowser(TriangleMaterial *mat);

public slots:
    void slotColumnSolutionChanged(const QString id);

public:
    void InitMaterialGlobalControlWidget();

    void InitMechanicalConstitutiveWidget();

    void InitContactConstitutiveWidget();

    void InitHydraulicConstitutiveWidget();

    void InitThermalConstitutiveWidget();

private:
    void UpateMaterialGlobalControlWidget();

    void UpdateMechanicalConstitutiveWidget();

    void UpdateContactConstitutiveWidget();

    void UpdateHydraulicConstitutiveWidget();

    void UpdateThermalConstitutiveWidget();

public slots:
    void slotMaterialGlobalValueChanged(QtProperty *property, const QVariant &val);

    void slotMechanicalConstitutiveModelChanged(int index);

    void slotContactConstitutiveModelChanged(int index);

    void slotHydraulicConstitutiveModelChanged(int index);

    void slotThermalConstitutiveModelChanged(int index);

private:
    Ui::FDEMTriangleMaterialWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    QSharedPointer<ConstitutiveModelWidget> contactWidget;
    QSharedPointer<ConstitutiveModelWidget> mechanicalWidget;
    QSharedPointer<ConstitutiveModelWidget> hydraulicWidget;
    QSharedPointer<ConstitutiveModelWidget> thermalWidget;

    TriangleMaterial *material;
};

#endif // FDEMTRIANGLEMATERIALWIDGET_H
