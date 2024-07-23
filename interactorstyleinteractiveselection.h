#ifndef INTERACTORSTYLEINTERACTIVESELECTION_H
#define INTERACTORSTYLEINTERACTIVESELECTION_H

#include "selectcollection.h"
#include <QSharedDataPointer>

#include <vtkNew.h>
#include <vtkInteractorStyleTrackballCamera.h>

class vtkPolyData;
class vtkCellPicker;
class vtkPointPicker;
class vtkAbstractArray;

class InteractorStyleInteractiveSelection : public vtkInteractorStyleTrackballCamera
{
public:
    vtkTypeMacro(InteractorStyleInteractiveSelection, vtkInteractorStyleTrackballCamera);

    static InteractorStyleInteractiveSelection* New();

    void PrintSelf(ostream& os, vtkIndent indent) override;

    void OnChar() override;

    void OnLeftButtonDown() override;

    void OnLeftButtonUp() override;

    void OnMouseMove() override;

protected:
    InteractorStyleInteractiveSelection();

    ~InteractorStyleInteractiveSelection() override;

    void GetUniversalSet();

    void HighlightMovePoint();

    void HighlightMoveCell();

    void SelectMovePoint();

    void SelectMoveCell();

private:
    vtkActor *MoveActor;
    vtkPolyDataMapper* MoveMapper;

    vtkNew<vtkPointPicker> PointPicker;
    vtkNew<vtkCellPicker> CellPicker;

    QSharedPointer<SelectCollection> SelectCollection;

private:
  InteractorStyleInteractiveSelection(const InteractorStyleInteractiveSelection &) = delete;

  void operator=(const InteractorStyleInteractiveSelection &) = delete;
};

#endif // INTERACTORSTYLEINTERACTIVESELECTION_H
