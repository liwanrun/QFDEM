#ifndef FDEMPREFERENCEWIDGET_H
#define FDEMPREFERENCEWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMPreferenceWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;

class SolutionCollection;
class FDEMPreferenceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMPreferenceWidget(QWidget *parent = nullptr);
    ~FDEMPreferenceWidget();

    void InitPropertyBrowser();

    void ConnectSignalSlots();

private:
    void InitMechanicalProperty();

    void InitHydraulicProperty();

    void InitThermalProperty();

    void InitFieldCoupleProperty();

private:
    void SetPropertyVisibility(const QString& id, bool visible);

    void SetPropertyEnabled(const QString& id, bool enabled);

public slots:
    void slotPropertyManagerValueChanged(QtProperty *property, const QVariant &value);

signals:
    void signalOutputPromptMessage(const QString msg);

private:
    Ui::FDEMPreferenceWidget *ui;

    QtVariantEditorFactory   *factory;
    QtVariantPropertyManager *manager;

    QSharedPointer<SolutionCollection> solutionCollection;
};

#endif // FDEMPREFERENCEWIDGET_H
