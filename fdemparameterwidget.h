#ifndef FDEMPARAMETERWIDGET_H
#define FDEMPARAMETERWIDGET_H

#include <QWidget>
#include <QPointer>

namespace Ui {
class FDEMParameterWidget;
}

class FDEMWindow;
class FDEMVTKRenderWidget;
class FDEMInformationWidget;

class FDEMPreferenceWidget;
class FDEMGeometryWidget;
class FDEMMeshWidget;
class FDEMGroupWidget;
class FDEMSolutionWidget;
class FDEMContactWidget;
class FDEMMaterialWidget;
class FDEMBoundaryWidget;
class FDEMTemporalWidget;
class FDEMHistoryWidget;

class FDEMParameterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMParameterWidget(QWidget *parent = nullptr);
    ~FDEMParameterWidget();

    void ConnectSignalSlots();

private:
    void ConnectPreferenceSignalSlots();

    void ConnectGeometrySignalSlots();

    void ConnectMeshSignalSlots();

    void ConnectGroupSignalSlots();

    void ConnectSolutionSignalSlots();

    void ConnectContactSignalSlots();

    void ConnectMaterialSignalSlots();

    void ConnectBoundarySignalSlots();

    void ConnectTemporalSignalSlots();

    void ConnectHistorySignalSlots();

public:
    Ui::FDEMParameterWidget *getUi() const;

    QPointer<FDEMVTKRenderWidget> getFdemVTKRenderWidget() const;
    void setFdemVTKRenderWidget(const QPointer<FDEMVTKRenderWidget> &value);

    QPointer<FDEMInformationWidget> getFdemInformationWidget() const;
    void setFdemInformationWidget(const QPointer<FDEMInformationWidget> &value);

    QPointer<FDEMWindow> getFdemWindow() const;
    void setFdemWindow(const QPointer<FDEMWindow> &value);

private:
    Ui::FDEMParameterWidget *ui;

    QPointer<FDEMWindow> fdemWindow;
    QPointer<FDEMVTKRenderWidget> fdemVTKRenderWidget;
    QPointer<FDEMInformationWidget> fdemInformationWidget;

private:
    QPointer<FDEMPreferenceWidget>  fdemPreferenceWidget;
    QPointer<FDEMGeometryWidget>    fdemGeometryWidget;
    QPointer<FDEMMeshWidget>        fdemMeshWidget;
    QPointer<FDEMGroupWidget>       fdemGroupWidget;
    QPointer<FDEMSolutionWidget>    fdemSolutionWidget;
    QPointer<FDEMContactWidget>     fdemContactWidget;
    QPointer<FDEMMaterialWidget>    fdemMaterialWidget;
    QPointer<FDEMBoundaryWidget>    fdemBoundaryWidget;
    QPointer<FDEMTemporalWidget>    fdemTemporalWidget;
    QPointer<FDEMHistoryWidget>     fdemHistoryWidget;
};

#endif // FDEMPARAMETERWIDGET_H
