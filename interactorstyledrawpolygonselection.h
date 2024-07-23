#ifndef INTERACTORSTYLEDRAWPOLYGONSELECTION_H
#define INTERACTORSTYLEDRAWPOLYGONSELECTION_H

#include "selectcollection.h"
#include <QSharedPointer>

#include <vtkInteractorStyleDrawPolygon.h>

class vtkProperty;
class InteractorStyleDrawPolygonSelection : public vtkInteractorStyleDrawPolygon
{
public:
    vtkTypeMacro(InteractorStyleDrawPolygonSelection, vtkInteractorStyleDrawPolygon);

    void PrintSelf(ostream &os, vtkIndent indent) override;

    static InteractorStyleDrawPolygonSelection* New();

    void OnMouseMove() override;

    void OnLeftButtonDown() override;

    void OnLeftButtonUp() override;

protected:
    InteractorStyleDrawPolygonSelection();

    ~InteractorStyleDrawPolygonSelection();

    void GetInputDataObject();

    void BeginSelection();

    void EndSelection();

private:
    QSharedPointer<SelectCollection> SelectCollection;

    vtkNew<vtkProperty> tmpProperty;

private:
    InteractorStyleDrawPolygonSelection(const InteractorStyleDrawPolygonSelection &) = delete;

    void operator=(const InteractorStyleDrawPolygonSelection &) = delete;
};

#endif // INTERACTORSTYLEDRAWPOLYGONSELECTION_H
