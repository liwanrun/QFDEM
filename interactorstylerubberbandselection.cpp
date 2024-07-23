#include "interactorstylerubberbandselection.h"
#include "selectcollection.h"

#include <set>

#include <vtkObjectFactory.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkAssemblyPath.h>
#include <vtkProperty.h>
#include <vtkInformation.h>
#include <vtkInformationStringKey.h>
#include <vtkAreaPicker.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkCompositeDataGeometryFilter.h>
#include <vtkCompositeDataIterator.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkProp3DCollection.h>
#include <vtkDoubleArray.h>
#include <vtkUnsignedIntArray.h>

#include <QDebug>
#include <QMessageBox>

vtkStandardNewMacro(InteractorStyleRubberBandSelection);

#define VTKISRBP_ORIENT 0
#define VTKISRBP_SELECT 1

void InteractorStyleRubberBandSelection::PrintSelf(std::ostream &, vtkIndent)
{

}

void InteractorStyleRubberBandSelection::OnChar()
{
    return;
}

void InteractorStyleRubberBandSelection::OnLeftButtonDown()
{
    this->GetInteractor()->Render();

    this->GetInputDataObject();

    if (this->CurrentMode != VTKISRBP_SELECT)
    {
        return vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

    this->Superclass::OnLeftButtonDown();
}

void InteractorStyleRubberBandSelection::OnMouseMove()
{
    if (this->CurrentMode != VTKISRBP_SELECT)
    {
        return vtkInteractorStyleTrackballCamera::OnMouseMove();
    }

    this->Superclass::OnMouseMove();
}

void InteractorStyleRubberBandSelection::InitializeSelectMode()
{
    this->CurrentMode = VTKISRBP_SELECT;
}

void InteractorStyleRubberBandSelection::OnLeftButtonUp()
{
    if (this->CurrentMode != VTKISRBP_SELECT)
    {
        return vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
    }

    this->Superclass::OnLeftButtonUp();

    this->GetClipPoints();

    try
    {
        switch (this->SelectCollection->selectContent)
        {
        case Selection::FRUSTUM:
        {
            this->FrustumSelect();
            break;
        }
        case Selection::BLOCKS:
        {
            this->BlocksSelect();
            break;
        }
        }

        this->GetInteractor()->EndPickCallback();
        this->CurrentMode = VTKISRBP_ORIENT;
    }
    catch (std::exception& e)
    {
        QMessageBox::warning(nullptr, QString("Invalid Geometry Condition"), QString(e.what()));
        this->GetInteractor()->EndPickCallback();
        this->CurrentMode = VTKISRBP_ORIENT;
        return;
    }
}

InteractorStyleRubberBandSelection::InteractorStyleRubberBandSelection()
{
    this->PickColor[0] = 1.0;
    this->PickColor[1] = 0.0;
    this->PickColor[2] = 1.0;

    this->CurrentMode = VTKISRBP_SELECT;
    this->SelectCollection = SelectCollection::GetSelectCollection();
}

InteractorStyleRubberBandSelection::~InteractorStyleRubberBandSelection()
{

}

void InteractorStyleRubberBandSelection::DefineFrustum()
{
    //find rubber band lower left, upper right and center
    int *size = this->Interactor->GetRenderWindow()->GetSize();
    int x0, y0, x1, y1;
    x0 = this->StartPosition[0] <= this->EndPosition[0] ?
                this->StartPosition[0] : this->EndPosition[0];
    if (x0 < 0) { x0 = 0; }
    if (x0 >= size[0]) { x0 = size[0] - 2; }

    y0 = this->StartPosition[1] <= this->EndPosition[1] ?
                this->StartPosition[1] : this->EndPosition[1];
    if (y0 < 0) { y0 = 0; }
    if (y0 >= size[1]) { y0 = size[1] - 2; }

    x1 = this->EndPosition[0] > this->StartPosition[0] ?
                this->EndPosition[0] : this->StartPosition[0];
    if (x1 < 0) { x1 = 0; }
    if (x1 >= size[0]) { x1 = size[0] - 2; }

    y1 = this->EndPosition[1] > this->StartPosition[1] ?
                this->EndPosition[1] : this->StartPosition[1];
    if (y1 < 0) { y1 = 0; }
    if (y1 >= size[1]) { y1 = size[1] - 2; }

    //compute frustum corners
    double X0 = (x0 < x1) ? x0 : x1;
    double Y0 = (y0 < y1) ? y0 : y1;
    double X1 = (x0 > x1) ? x0 : x1;
    double Y1 = (y0 > y1) ? y0 : y1;

    if (X0 == X1)
    {
        X1 += 1.0;
    }
    if (Y0 == Y1)
    {
        Y1 += 1.0;
    }

    this->ClipPoints->SetNumberOfComponents(4);
    this->ClipPoints->SetNumberOfTuples(8);

    //compute world coordinates of the pick volume
    double verts[32];
    this->DefaultRenderer->SetDisplayPoint(X0, Y0, 0);
    this->DefaultRenderer->DisplayToWorld();
    this->DefaultRenderer->GetWorldPoint(&verts[0]);
    this->ClipPoints->InsertTuple(0, &verts[0]);

    this->DefaultRenderer->SetDisplayPoint(X0, Y0, 1);
    this->DefaultRenderer->DisplayToWorld();
    this->DefaultRenderer->GetWorldPoint(&verts[4]);
    this->ClipPoints->InsertTuple(1, &verts[4]);

    this->DefaultRenderer->SetDisplayPoint(X0, Y1, 0);
    this->DefaultRenderer->DisplayToWorld();
    this->DefaultRenderer->GetWorldPoint(&verts[8]);
    this->ClipPoints->InsertTuple(2, &verts[8]);

    this->DefaultRenderer->SetDisplayPoint(X0, Y1, 1);
    this->DefaultRenderer->DisplayToWorld();
    this->DefaultRenderer->GetWorldPoint(&verts[12]);
    this->ClipPoints->InsertTuple(3, &verts[12]);

    this->DefaultRenderer->SetDisplayPoint(X1, Y0, 0);
    this->DefaultRenderer->DisplayToWorld();
    this->DefaultRenderer->GetWorldPoint(&verts[16]);
    this->ClipPoints->InsertTuple(4, &verts[16]);

    this->DefaultRenderer->SetDisplayPoint(X1, Y0, 1);
    this->DefaultRenderer->DisplayToWorld();
    this->DefaultRenderer->GetWorldPoint(&verts[20]);
    this->ClipPoints->InsertTuple(5, &verts[20]);

    this->DefaultRenderer->SetDisplayPoint(X1, Y1, 0);
    this->DefaultRenderer->DisplayToWorld();
    this->DefaultRenderer->GetWorldPoint(&verts[24]);
    this->ClipPoints->InsertTuple(6, &verts[24]);

    this->DefaultRenderer->SetDisplayPoint(X1, Y1, 1);
    this->DefaultRenderer->DisplayToWorld();
    this->DefaultRenderer->GetWorldPoint(&verts[28]);
    this->ClipPoints->InsertTuple(7, &verts[28]);
}

void InteractorStyleRubberBandSelection::FrustumSelect()
{
    int field = (Selection::CELL == this->SelectCollection->selectField) ?
                vtkSelectionNode::CELL : vtkSelectionNode::POINT;

    vtkNew<vtkSelectionNode> node;
    node->SetContentType(vtkSelectionNode::FRUSTUM);
    node->SetSelectionList(this->ClipPoints);
    node->SetFieldType(field);

    vtkNew<vtkSelection> selection;
    selection->AddNode(node);

    vtkNew<vtkExtractSelection> extractor;
    extractor->SetInputData(0, SelectCollection->Input);
    extractor->SetInputData(1, selection);
    extractor->PreserveTopologyOff();
    extractor->Update();

    vtkNew<vtkMultiBlockDataSet> Output;
    Output->ShallowCopy(extractor->GetOutput());

    vtkNew<vtkIdTypeArray> globalIds;
    globalIds->ShallowCopy(SelectCollection->GetExtractGlobalIds(Output, SelectCollection->selectFilter));
    this->SelectCollection->GlobalIDsSelect(globalIds);
    this->SelectCollection->HighlightSelection();
}

void InteractorStyleRubberBandSelection::BlocksSelect()
{
    vtkActor *part;
    vtkProp3D *prop;
    vtkAssemblyPath* path;
    vtkProp3DCollection* pc = this->Picker->GetProp3Ds();
    vtkCollectionSimpleIterator pit;

    vtkNew<vtkIntArray> selectIds;
    for(pc->InitTraversal(pit); (prop = pc->GetNextProp3D(pit));)
    {
        for (prop->InitPathTraversal(); (path = prop->GetNextPath());)
        {
            part = vtkActor::SafeDownCast(path->GetFirstNode()->GetViewProp());
            if(part && part->GetVisibility() && part->GetPickable())
            {
                std::string blockId = part->GetProperty()->GetInformation()->Get(vtkCompositeDataSet::NAME());
                if(blockId.empty()) continue;

                int index = part->GetProperty()->GetInformation()->Get(vtkSelectionNode::COMPOSITE_INDEX());
                selectIds->InsertNextValue(index + 1);      //[Important]: start from 1
            }
        }
    }

    vtkNew<vtkSelectionNode> node;
    node->SetContentType(vtkSelectionNode::BLOCKS);
    node->SetSelectionList(selectIds);

    vtkNew<vtkSelection> selection;
    selection->AddNode(node);

    vtkNew<vtkExtractSelection> extractor;
    extractor->SetInputData(0, SelectCollection->Input);
    extractor->SetInputData(1, selection);
    extractor->PreserveTopologyOff();
    extractor->Update();

    vtkNew<vtkMultiBlockDataSet> Output;
    Output->ShallowCopy(extractor->GetOutput());

    vtkNew<vtkIdTypeArray> globalIds;
    globalIds->ShallowCopy(SelectCollection->GetExtractGlobalIds(Output, SelectCollection->selectFilter));
    this->SelectCollection->GlobalIDsSelect(globalIds);
    this->SelectCollection->HighlightSelection();
}

void InteractorStyleRubberBandSelection::GetInputDataObject()
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
            part = vtkActor::SafeDownCast(path->GetFirstNode()->GetViewProp());
            if(part && part->GetVisibility() && part->GetPickable())
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

void InteractorStyleRubberBandSelection::GetClipPoints()
{
    int *size = this->Interactor->GetRenderWindow()->GetSize();
    int min[2], max[2];
    min[0] = this->StartPosition[0] <= this->EndPosition[0] ?
                this->StartPosition[0] : this->EndPosition[0];
    if (min[0] < 0) { min[0] = 0; }
    if (min[0] >= size[0]) { min[0] = size[0] - 2; }

    min[1] = this->StartPosition[1] <= this->EndPosition[1] ?
                this->StartPosition[1] : this->EndPosition[1];
    if (min[1] < 0) { min[1] = 0; }
    if (min[1] >= size[1]) { min[1] = size[1] - 2; }

    max[0] = this->EndPosition[0] > this->StartPosition[0] ?
                this->EndPosition[0] : this->StartPosition[0];
    if (max[0] < 0) { max[0] = 0; }
    if (max[0] >= size[0]) { max[0] = size[0] - 2; }

    max[1] = this->EndPosition[1] > this->StartPosition[1] ?
                this->EndPosition[1] : this->StartPosition[1];
    if (max[1] < 0) { max[1] = 0; }
    if (max[1] >= size[1]) { max[1] = size[1] - 2; }

    this->Picker->AreaPick(min[0], min[1], max[0], max[1], this->DefaultRenderer);
    this->ClipPoints->SetNumberOfComponents(4);
    this->ClipPoints->SetNumberOfTuples(8);
    for (int id = 0; id < this->ClipPoints->GetNumberOfTuples(); id++)
    {
        double pt[3];
        this->Picker->GetClipPoints()->GetPoint(id, pt);
        this->ClipPoints->SetTuple4(id, pt[0], pt[1], pt[2], 1.0);
    }
}

