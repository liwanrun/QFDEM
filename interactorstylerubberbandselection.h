#ifndef INTERACTORSTYLERUBBERBANDSELECTION_H
#define INTERACTORSTYLERUBBERBANDSELECTION_H

#include "selectcollection.h"

#include <QSharedPointer>

#include <vtkNew.h>
#include <vtkInteractorStyleRubberBand3D.h>

class vtkAreaPicker;
class vtkDoubleArray;

class InteractorStyleRubberBandSelection : public vtkInteractorStyleRubberBand3D
{
public:
    static InteractorStyleRubberBandSelection* New();

    vtkTypeMacro(InteractorStyleRubberBandSelection, vtkInteractorStyleRubberBand3D);

    void PrintSelf(ostream& os, vtkIndent indent) override;

    void OnChar() override;

    void OnLeftButtonDown() override;

    void OnLeftButtonUp() override;

    void OnMouseMove() override;

    void InitializeSelectMode();

    vtkSetMacro(CurrentMode, int);
    vtkGetMacro(CurrentMode, int);

protected:
    InteractorStyleRubberBandSelection();

    ~InteractorStyleRubberBandSelection() override;

    void DefineFrustum();

    void GetInputDataObject();

    void GetClipPoints();

    void FrustumSelect();

    void BlocksSelect();

private:
    int CurrentMode;

    vtkNew<vtkAreaPicker> Picker;
    vtkNew<vtkDoubleArray> ClipPoints;

    QSharedPointer<SelectCollection> SelectCollection;

private:
  InteractorStyleRubberBandSelection(const InteractorStyleRubberBandSelection &) = delete;

  void operator=(const InteractorStyleRubberBandSelection &) = delete;
};

#endif // INTERACTORSTYLERUBBERBANDSELECTION_H
