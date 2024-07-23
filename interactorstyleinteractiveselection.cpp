#include "interactorstyleinteractiveselection.h"

#include <vtkObjectFactory.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPointPicker.h>
#include <vtkCellPicker.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkAssemblyPath.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkInformation.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkExtractSelection.h>

#include <QDebug>

vtkStandardNewMacro(InteractorStyleInteractiveSelection);

void InteractorStyleInteractiveSelection::PrintSelf(std::ostream &, vtkIndent)
{

}

void InteractorStyleInteractiveSelection::OnChar()
{
    return Superclass::OnChar();
}

void InteractorStyleInteractiveSelection::OnLeftButtonDown()
{
    this->GetUniversalSet();

    Superclass::OnLeftButtonDown();
}

void InteractorStyleInteractiveSelection::OnLeftButtonUp()
{
    if(Selection::POINT == this->SelectCollection->selectField)
    {
        this->SelectMovePoint();
    }
    else if(Selection::CELL == this->SelectCollection->selectField)
    {
        this->SelectMoveCell();
    }
}

void InteractorStyleInteractiveSelection::OnMouseMove()
{
   if(Selection::POINT == this->SelectCollection->selectField)
   {
       this->HighlightMovePoint();
   }
   else if(Selection::CELL == this->SelectCollection->selectField)
   {
       this->HighlightMoveCell();
   }
}

InteractorStyleInteractiveSelection::InteractorStyleInteractiveSelection()
{
    this->MoveActor = nullptr;
    this->MoveMapper = vtkPolyDataMapper::New();

    this->PointPicker->UseCellsOn();
    this->CellPicker->SetTolerance(0.0005);
    this->SelectCollection = SelectCollection::GetSelectCollection();
}

InteractorStyleInteractiveSelection::~InteractorStyleInteractiveSelection()
{
    this->DefaultRenderer->RemoveActor(this->MoveActor);
    this->MoveMapper->Delete();

    if(this->MoveActor)
    {
        this->MoveActor->Delete();
    }
}

void InteractorStyleInteractiveSelection::GetUniversalSet()
{
    vtkNew<vtkMultiBlockDataSet> Input;

    vtkActor *actor, *part;
    vtkAssemblyPath* path;
    vtkActorCollection* ac = this->GetDefaultRenderer()->GetActors();

    unsigned int index = 0;
    vtkCollectionSimpleIterator ait;
    for(ac->InitTraversal(ait); (actor = ac->GetNextActor(ait));)
    {
        for (actor->InitPathTraversal(); (path = actor->GetNextPath());)
        {
            part = static_cast<vtkActor*>(path->GetFirstNode()->GetViewProp());
            if(part->GetVisibility() && part->GetPickable())
            {
                vtkPolyData* pd = vtkPolyData::SafeDownCast(part->GetMapper()->GetInput());
                std::string blockId = part->GetProperty()->GetInformation()->Get(vtkCompositeDataSet::NAME());
                if(blockId.empty()) continue;

                Input->SetBlock(index, pd);
                Input->GetMetaData(index)->Set(vtkCompositeDataSet::NAME(), blockId);
                part->GetProperty()->GetInformation()->Set(vtkSelectionNode::COMPOSITE_INDEX(), index);
                index += 1;
            }
        }
    }

    this->SelectCollection->Input->Initialize();
    this->SelectCollection->Input->ShallowCopy(Input);
}

void InteractorStyleInteractiveSelection::HighlightMovePoint()
{
    int *pos = this->GetInteractor()->GetEventPosition();
    if(this->PointPicker->Pick(pos[0], pos[1], 0, this->DefaultRenderer))
    {
        double picked[3];
        vtkIdType id = this->PointPicker->GetPointId();
        vtkActor* actor = this->PointPicker->GetActor();
        vtkPolyDataMapper *mapper = static_cast<vtkPolyDataMapper*>(actor->GetMapper());
        mapper->GetInput()->GetPoints()->GetPoint(id, picked);

        vtkNew<vtkPoints> newPoints;
        newPoints->InsertNextPoint(picked);

        vtkNew<vtkPolyData> newPolyData;
        newPolyData->SetPoints(newPoints);

        vtkNew<vtkVertexGlyphFilter> filter;
        filter->SetInputData(newPolyData);

        this->MoveMapper->SetInputConnection(filter->GetOutputPort());
        if(nullptr == this->MoveActor)
        {
            this->MoveActor = vtkActor::New();
            this->MoveActor->PickableOff();
            this->MoveActor->DragableOff();
            this->MoveActor->SetMapper(this->MoveMapper);
            this->MoveActor->GetProperty()->SetColor(0.5, 0.0, 1.0);
            this->MoveActor->GetProperty()->SetAmbient(1.0);
            this->MoveActor->GetProperty()->SetDiffuse(0.0);
            this->MoveActor->GetProperty()->SetLighting(false);
        }
        this->MoveActor->GetProperty()->SetRepresentationToPoints();
        this->MoveActor->GetProperty()->SetPointSize(10.0);

        this->DefaultRenderer->AddActor(this->MoveActor);
        this->GetInteractor()->Render();
    }
    else
    {
        this->DefaultRenderer->RemoveActor(this->MoveActor);
        this->GetInteractor()->Render();
    }
}

void InteractorStyleInteractiveSelection::HighlightMoveCell()
{
    int *pos = this->GetInteractor()->GetEventPosition();
    if(this->CellPicker->Pick(pos[0], pos[1], 0, this->DefaultRenderer))
    {
        vtkIdType npts, *ids;
        vtkIdType id = this->CellPicker->GetCellId();
        vtkActor* actor = this->CellPicker->GetActor();
        vtkPolyData* input = static_cast<vtkPolyData*>(actor->GetMapper()->GetInput());
        vtkPoints *points = input->GetPoints();
        input->GetCellPoints(id, npts, ids);

        vtkNew<vtkPoints> newPoints;
        newPoints->Allocate(npts);

        vtkNew<vtkCellArray> newPolys;
        newPolys->InsertNextCell(npts);

        for(vtkIdType i = 0; i < npts; i++)
        {
            double picked[3];
            points->GetPoint(ids[i], picked);
            newPoints->InsertNextPoint(picked);
            newPolys->InsertCellPoint(i);
        }

        vtkNew<vtkPolyData> newPolyData;
        newPolyData->SetPoints(newPoints);
        newPolyData->SetPolys(newPolys);

        this->MoveMapper->SetInputData(newPolyData);
        if(nullptr == this->MoveActor)
        {
            this->MoveActor = vtkActor::New();
            this->MoveActor->PickableOff();
            this->MoveActor->DragableOff();
            this->MoveActor->SetMapper(this->MoveMapper);
            this->MoveActor->GetProperty()->SetColor(0.5, 0.0, 1.0);
            this->MoveActor->GetProperty()->SetAmbient(1.0);
            this->MoveActor->GetProperty()->SetDiffuse(0.0);
            this->MoveActor->GetProperty()->SetLighting(false);
        }
        this->MoveActor->GetProperty()->SetRepresentationToWireframe();
        this->MoveActor->GetProperty()->SetLineWidth(2.0);

        this->DefaultRenderer->AddActor(this->MoveActor);
        this->GetInteractor()->Render();
    }
    else
    {
        this->DefaultRenderer->RemoveActor(this->MoveActor);
        this->GetInteractor()->Render();
    }
}

void InteractorStyleInteractiveSelection::SelectMovePoint()
{
    int *pos = this->GetInteractor()->GetEventPosition();
    if(this->PointPicker->Pick(pos[0], pos[1], 0, this->DefaultRenderer))
    {
        vtkActor* actor = this->PointPicker->GetActor();
        vtkIdType index = this->PointPicker->GetPointId();

        vtkPolyData* input = static_cast<vtkPolyData*>(actor->GetMapper()->GetInput());
        vtkIdType id = input->GetPointData()->GetGlobalIds()->GetVariantValue(index).ToLongLong();

        vtkNew<vtkIdTypeArray> globalIds;
        globalIds->InsertNextValue(id);

        this->SelectCollection->GlobalIDsSelect(globalIds);
        this->SelectCollection->HighlightSelection();
        this->GetInteractor()->EndPickCallback();
    }
}

void InteractorStyleInteractiveSelection::SelectMoveCell()
{
    int *pos = this->GetInteractor()->GetEventPosition();
    if(this->CellPicker->Pick(pos[0], pos[1], 0, this->DefaultRenderer))
    {
        vtkActor* actor = this->CellPicker->GetActor();
        vtkIdType index = this->CellPicker->GetCellId();

        vtkPolyData* input = static_cast<vtkPolyData*>(actor->GetMapper()->GetInput());
        vtkIdType id = input->GetCellData()->GetGlobalIds()->GetVariantValue(index).ToLongLong();

        vtkNew<vtkIdTypeArray> globalIds;
        globalIds->InsertNextValue(id);

        this->SelectCollection->GlobalIDsSelect(globalIds);
        this->SelectCollection->HighlightSelection();
        this->GetInteractor()->EndPickCallback();
    }
}
