#ifndef FDEMNTSFLUIDMATERIALWIDGET_H
#define FDEMNTSFLUIDMATERIALWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMNTSFluidMaterialWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;

class NTFluidMaterial;
class FDEMNTSFluidMaterialWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMNTSFluidMaterialWidget(QWidget *parent = nullptr);

    ~FDEMNTSFluidMaterialWidget();

    void InitPropertyBrowser();

    void ConnectSignalSlots();

    void UpdatePropertyBrowser(NTFluidMaterial *mat);

private:
    void SetPropertyVisibility(const QString& id, bool visible);

    void SetBackgroundColor(const QColor& color);

    void CreateNewtonianFluidProperty();

    void UpdateNewtonianFluidProperty();

public slots:
    void slotTreeBrowserValueChanged(QtProperty *property, const QVariant &value);

    void slotAnalysisModuleChanged(const QVariant& value);

private:
    Ui::FDEMNTSFluidMaterialWidget *ui;

    QtVariantPropertyManager *manager;
    QtVariantEditorFactory   *factory;

    NTFluidMaterial *material;
};

#endif // FDEMNTSFLUIDMATERIALWIDGET_H
