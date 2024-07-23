#include "interactorstyledrawpolygonselection.h"
#include "selectionfilter.h"

#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkActorCollection.h>
#include <vtkAssemblyPath.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkProperty.h>
#include <vtkInformation.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkOpenGLHardwareSelector.h>
#include <vtkExtractSelection.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkProperty.h>

#include <set>
#include <QDebug>

vtkStandardNewMacro(InteractorStyleDrawPolygonSelection);

void InteractorStyleDrawPolygonSelection::PrintSelf(std::ostream &, vtkIndent)
{

}

void InteractorStyleDrawPolygonSelection::OnMouseMove()
{
    this->Superclass::OnMouseMove();
}

void InteractorStyleDrawPolygonSelection::OnLeftButtonDown()
{
    this->GetInteractor()->Render();

    this->GetInputDataObject();

    this->Superclass::OnLeftButtonDown();
}

void InteractorStyleDrawPolygonSelection::OnLeftButtonUp()
{
    this->Superclass::OnLeftButtonUp();

    this->BeginSelection();

    //Polygon Selection
    std::vector<vtkVector2i> points = this->GetPolygonPoints();
    if(points.size() < 3) return;

    vtkNew<vtkIntArray> polygonPointsArray;
    polygonPointsArray->SetNumberOfComponents(2);
    polygonPointsArray->SetNumberOfTuples(static_cast<vtkIdType>(points.size()));
    for(unsigned int j = 0; j < points.size(); ++j)
    {
        const vtkVector2i &v = points[j];
        int pos[2] = {v[0], v[1]};
        polygonPointsArray->SetTypedTuple(j, pos);
    }

    vtkNew<vtkOpenGLHardwareSelector> hardSel;
    int* wsize = this->DefaultRenderer->GetSize();
    int* origin = this->DefaultRenderer->GetOrigin();
    hardSel->SetRenderer(this->DefaultRenderer);
    hardSel->SetArea(origin[0], origin[1], origin[0] + wsize[0] - 1, origin[1] + wsize[1] - 1);

    if(Selection::POINT == this->SelectCollection->selectField)
    {
        hardSel->SetFieldAssociation(vtkDataObject::FIELD_ASSOCIATION_POINTS);
    }
    else
    {
        hardSel->SetFieldAssociation(vtkDataObject::FIELD_ASSOCIATION_CELLS);
    }

    if(hardSel->CaptureBuffers())
    {
        vtkSelection* sel = hardSel->GeneratePolygonSelection(
                            polygonPointsArray->GetPointer(0),
                            polygonPointsArray->GetNumberOfTuples()*2);
        vtkSmartPointer<vtkSelection> selection;
        selection.TakeReference(sel);
        hardSel->ClearBuffers();

        //std::set<vtkIdType> unionIdSet;
        vtkNew<vtkIdTypeArray> globalIds;
        for (unsigned int i = 0; i < sel->GetNumberOfNodes(); i++)
        {
            vtkSelectionNode *node = selection->GetNode(i);
            vtkIdTypeArray *indexes = static_cast<vtkIdTypeArray*>(node->GetSelectionList());
            vtkActor *nativeActor = static_cast<vtkActor*>(node->GetProperties()->Get(vtkSelectionNode::PROP()));
            if(nullptr != nativeActor)
            {
                vtkPolyData *pd = static_cast<vtkPolyData*>(nativeActor->GetMapper()->GetInput());
                if(Selection::POINT == SelectCollection->selectField)
                {
                    vtkDataArray *ids = pd->GetPointData()->GetGlobalIds();
                    for (vtkIdType i = 0; i < indexes->GetNumberOfValues(); i++)
                    {
                        vtkIdType index = indexes->GetValue(i);
                        globalIds->InsertNextValue(ids->GetVariantValue(index).ToLongLong());
                    }
                }
                else if(Selection::CELL == SelectCollection->selectField)
                {
                    QSharedPointer<SelectionFilterFactory> factory =
                            QSharedPointer<SelectionFilterFactory>(new SelectionFilterFactory());
                    QSharedPointer<SelectionFilter> cf = factory->Create(SelectCollection->selectFilter);
                    cf->filter(pd, indexes, globalIds);
                }
            }
        }

        this->SelectCollection->GlobalIDsSelect(globalIds);
        this->SelectCollection->HighlightSelection();
    }

    this->EndSelection();

    this->GetInteractor()->EndPickCallback();
}

InteractorStyleDrawPolygonSelection::InteractorStyleDrawPolygonSelection()
{
    this->SelectCollection = SelectCollection::GetSelectCollection();
}

InteractorStyleDrawPolygonSelection::~InteractorStyleDrawPolygonSelection()
{

}

void InteractorStyleDrawPolygonSelection::GetInputDataObject()
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

void InteractorStyleDrawPolygonSelection::BeginSelection()
{
    vtkActorCollection *ac;
    vtkActor *actor, *part;
    vtkAssemblyPath* path;
    ac = this->DefaultRenderer->GetActors();

    bool PreserveProperty = false;
    vtkCollectionSimpleIterator ait;
    for(ac->InitTraversal(ait); (actor = ac->GetNextActor(ait));)
    {
        for(actor->InitPathTraversal(); (path = actor->GetNextPath());)
        {
            part = static_cast<vtkActor *>(path->GetFirstNode()->GetViewProp());
            if(part->GetVisibility() && part->GetPickable())
            {
                if(!PreserveProperty)
                {
                    this->tmpProperty->SetLighting(part->GetProperty()->GetLighting());
                    this->tmpProperty->SetEdgeVisibility(part->GetProperty()->GetEdgeVisibility());
                    this->tmpProperty->SetRepresentation(part->GetProperty()->GetRepresentation());
                    PreserveProperty = true;
                }

                //Hardware Selection
                part->GetProperty()->SetLighting(false);
                part->GetProperty()->SetEdgeVisibility(false);
                part->GetProperty()->SetRepresentationToSurface();
            }
        }
    }

    PreserveProperty = false;
}

void InteractorStyleDrawPolygonSelection::EndSelection()
{
    vtkActorCollection *ac;
    vtkActor *actor, *part;
    vtkAssemblyPath* path;
    ac = this->DefaultRenderer->GetActors();

    vtkCollectionSimpleIterator bit;
    for(ac->InitTraversal(bit); (actor = ac->GetNextActor(bit));)
    {
        for(actor->InitPathTraversal(); (path = actor->GetNextPath());)
        {
            part = static_cast<vtkActor *>(path->GetFirstNode()->GetViewProp());
            if(part->GetVisibility() && part->GetPickable())
            {
                part->GetProperty()->SetLighting(this->tmpProperty->GetLighting());
                part->GetProperty()->SetEdgeVisibility(this->tmpProperty->GetEdgeVisibility());
                part->GetProperty()->SetRepresentation(this->tmpProperty->GetRepresentation());
            }
        }
    }
}
