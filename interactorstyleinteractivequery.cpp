#include "interactorstyleinteractivequery.h"

#include <vtkObjectFactory.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPointPicker.h>
#include <vtkCellPicker.h>
#include <vtkBalloonWidget.h>
#include <vtkBalloonRepresentation.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkCompositeDataSet.h>
#include <vtkTextProperty.h>

vtkStandardNewMacro(InteractorStyleInteractiveQuery);

void InteractorStyleInteractiveQuery::PrintSelf(std::ostream &, vtkIndent)
{

}

void InteractorStyleInteractiveQuery::OnChar()
{
    return Superclass::OnChar();
}

void InteractorStyleInteractiveQuery::OnLeftButtonDown()
{
    return Superclass::OnLeftButtonDown();
}

void InteractorStyleInteractiveQuery::OnLeftButtonUp()
{
    return Superclass::OnLeftButtonUp();
}

void InteractorStyleInteractiveQuery::OnMouseMove()
{
    if(Selection::POINT == this->SelectCollection->selectField)
    {
        this->HighlightMovePoint();
        this->QueryMovePoint();
    }
    else if(Selection::CELL == this->SelectCollection->selectField)
    {
        this->HighlightMoveCell();
        this->QueryMoveCell();
    }
}

InteractorStyleInteractiveQuery::InteractorStyleInteractiveQuery()
{
    this->MoveActor = nullptr;
    this->MoveMapper = vtkPolyDataMapper::New();

    this->balloonWidget =  nullptr;
    this->balloonRep->GetTextProperty()->SetFontFamilyToCourier();
    this->balloonRep->GetTextProperty()->SetJustificationToLeft();

    this->PointPicker->UseCellsOn();
    this->CellPicker->SetTolerance(0.0005);
    this->SelectCollection = SelectCollection::GetSelectCollection();
}

InteractorStyleInteractiveQuery::~InteractorStyleInteractiveQuery()
{
    this->MoveMapper->Delete();
    this->DefaultRenderer->RemoveActor(this->MoveActor);

    if(this->MoveActor)
    {
        this->MoveActor->Delete();
    }

    if(this->balloonWidget)
    {
        this->balloonWidget->Delete();
    }
}

void InteractorStyleInteractiveQuery::HighlightMovePoint()
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

void InteractorStyleInteractiveQuery::HighlightMoveCell()
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

void InteractorStyleInteractiveQuery::QueryMovePoint()
{
    int *pos = this->GetInteractor()->GetEventPosition();
    if(this->PointPicker->Pick(pos[0], pos[1], 0, this->DefaultRenderer))
    {
        vtkActor* actor = this->PointPicker->GetActor();
        vtkIdType index = this->PointPicker->GetPointId();

        vtkPolyData* input = static_cast<vtkPolyData*>(actor->GetMapper()->GetInput());
        vtkIdType globalId = input->GetPointData()->GetGlobalIds()->GetVariantValue(index).ToLongLong();
        vtkIdType pedigreeId = input->GetPointData()->GetPedigreeIds()->GetVariantValue(index).ToLongLong();

        char info[512];
        std::string blockId = actor->GetProperty()->GetInformation()->Get(vtkCompositeDataSet::NAME());
        sprintf(info,"Title: '%s'\nGlobal Id: [%lld]\nGlobal Id: [%lld]\nLocal Index: [%lld]",
                blockId.c_str(), globalId, pedigreeId, index);

        if(nullptr == this->balloonWidget)
        {
            this->balloonWidget = vtkBalloonWidget::New();
            this->balloonWidget->SetRepresentation(balloonRep);
            this->balloonWidget->SetInteractor(this->GetInteractor());
            this->balloonWidget->EnabledOn();
        }

        this->balloonWidget->AddBalloon(actor, info, nullptr);
    }
}

void InteractorStyleInteractiveQuery::QueryMoveCell()
{
    int *pos = this->GetInteractor()->GetEventPosition();
    if(this->CellPicker->Pick(pos[0], pos[1], 0, this->DefaultRenderer))
    {
        vtkActor* actor = this->CellPicker->GetActor();
        vtkIdType index = this->CellPicker->GetCellId();

        vtkPolyData* input = static_cast<vtkPolyData*>(actor->GetMapper()->GetInput());
        vtkIdType globalId = input->GetCellData()->GetGlobalIds()->GetVariantValue(index).ToLongLong();
        vtkIdType pedigreeId = input->GetCellData()->GetPedigreeIds()->GetVariantValue(index).ToLongLong();

        char info[512];
        const char *name = (VTK_QUAD == input->GetCellType(index)) ? "VTK_QUAD" : "VTK_TRIANGLE";
        std::string blockId = actor->GetProperty()->GetInformation()->Get(vtkCompositeDataSet::NAME());
        sprintf(info,"Title: '%s'\nType: '%s'\nGlobal Id: [%lld]\nPrdigree Id: [%lld]\nLocal Index: [%lld]",
                blockId.c_str(), name, globalId, pedigreeId, index);

        if(nullptr == this->balloonWidget)
        {
            this->balloonWidget = vtkBalloonWidget::New();
            this->balloonWidget->SetRepresentation(balloonRep);
            this->balloonWidget->SetInteractor(this->GetInteractor());
            this->balloonWidget->EnabledOn();
        }

        this->balloonWidget->AddBalloon(actor, info, nullptr);
    }
}
