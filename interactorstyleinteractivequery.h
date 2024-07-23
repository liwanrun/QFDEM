#ifndef INTERACTORSTYLEINTERACTIVEQUERY_H
#define INTERACTORSTYLEINTERACTIVEQUERY_H

#include "selectcollection.h"
#include <QSharedDataPointer>

#include <vtkNew.h>
#include <vtkInteractorStyleTrackballCamera.h>

class vtkPolyData;
class vtkCellPicker;
class vtkPointPicker;
class vtkAbstractArray;
class vtkBalloonWidget;
class vtkBalloonRepresentation;

class InteractorStyleInteractiveQuery : public vtkInteractorStyleTrackballCamera
{
public:
    vtkTypeMacro(InteractorStyleInteractiveQuery, vtkInteractorStyleTrackballCamera);

    static InteractorStyleInteractiveQuery* New();

    void PrintSelf(ostream& os, vtkIndent indent) override;

    void OnChar() override;

    void OnLeftButtonDown() override;

    void OnLeftButtonUp() override;

    void OnMouseMove() override;

protected:
    InteractorStyleInteractiveQuery();

    ~InteractorStyleInteractiveQuery() override;

    void HighlightMovePoint();

    void HighlightMoveCell();

    void QueryMovePoint();

    void QueryMoveCell();

private:
    vtkActor *MoveActor;
    vtkPolyDataMapper* MoveMapper;

    vtkBalloonWidget *balloonWidget;
    vtkNew<vtkBalloonRepresentation> balloonRep;

    vtkNew<vtkPointPicker> PointPicker;
    vtkNew<vtkCellPicker> CellPicker;

    QSharedPointer<SelectCollection> SelectCollection;

private:
  InteractorStyleInteractiveQuery(const InteractorStyleInteractiveQuery &) = delete;

  void operator=(const InteractorStyleInteractiveQuery &) = delete;
};

#endif // INTERACTORSTYLEINTERACTIVEQUERY_H
