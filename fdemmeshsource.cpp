#include "fdemmeshsource.h"
#include "triangulation.h"

#include <vtkStringArray.h>
#include <vtkIdTypeArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

vtkStandardNewMacro(FDEMMeshSource)

void FDEMMeshSource::SetTriangulation(QSharedPointer<Triangulation> t)
{
    this->triangulation = t;
}

void FDEMMeshSource::AddNodeIdLabels(vtkPolyData *pd)
{
    vtkStringArray* nodeLabels = vtkStringArray::New();
    nodeLabels->SetNumberOfValues(pd->GetNumberOfPoints());
    nodeLabels->SetName("NodeIdLabels");

    for (vtkIdType id = 0; id < pd->GetNumberOfPoints(); id++)
    {
        nodeLabels->SetValue(id, std::to_string(id).c_str());
    }

    pd->GetPointData()->AddArray(nodeLabels);
    nodeLabels->Delete();
}

void FDEMMeshSource::AddCellIdLabels(vtkPolyData *pd)
{
    vtkStringArray* cellLabels = vtkStringArray::New();
    cellLabels->SetNumberOfValues(pd->GetNumberOfCells());
    cellLabels->SetName("CellIdLabels");

    for(vtkIdType id = 0; id < pd->GetNumberOfCells(); id++)
    {
        cellLabels->SetValue(id, std::to_string(id).c_str());
    }

    pd->GetCellData()->AddArray(cellLabels);
    cellLabels->Delete();
}

void FDEMMeshSource::AddCellConstraintTags(vtkPolyData *pd, const std::vector<vtkIdType> &ids)
{
    vtkIntArray *array = vtkIntArray::New();
    array->SetName("Cell Constraint Tags");
    array->SetNumberOfComponents(1);
    array->SetNumberOfTuples(pd->GetNumberOfCells());
    array->FillValue(0);

    for(auto iter = ids.begin(); iter != ids.end(); ++iter)
    {
        array->SetTuple1((*iter), 1);
    }

    pd->GetCellData()->AddArray(array);
    array->Delete();
}

void FDEMMeshSource::AddPointBoundaryTags(vtkPolyData *pd, const std::set<vtkIdType> &ids)
{
    vtkIntArray *array = vtkIntArray::New();
    array->SetName("Point Boundary Tags");
    array->SetNumberOfComponents(1);
    array->SetNumberOfTuples(pd->GetNumberOfPoints());
    array->FillValue(0);

    for(auto iter = ids.begin(); iter != ids.end(); ++iter)
    {
        array->SetTuple1((*iter), 1);
    }

    pd->GetPointData()->AddArray(array);
    array->Delete();
}

void FDEMMeshSource::AddCellBlockIds(vtkPolyData *pd, const std::vector<vtkIdType> &ids)
{
    vtkIdTypeArray *array = vtkIdTypeArray::New();
    array->SetName("Cell Block Ids");
    array->SetNumberOfComponents(1);
    array->SetNumberOfTuples(pd->GetNumberOfCells());

    for(vtkIdType i = 0; i < pd->GetNumberOfCells(); i++)
    {
        array->SetValue(i, ids.at(i));
    }

    pd->GetCellData()->AddArray(array);
    array->Delete();
}

void FDEMMeshSource::AddCellGrainIds(vtkPolyData *pd, const std::vector<vtkIdType> &ids)
{
    vtkIdTypeArray *array = vtkIdTypeArray::New();
    array->SetName("Cell Grain Ids");
    array->SetNumberOfComponents(1);
    array->SetNumberOfTuples(pd->GetNumberOfCells());

    for(vtkIdType i = 0; i < pd->GetNumberOfCells(); i++)
    {
        array->SetValue(i, ids.at(i));
    }

    pd->GetCellData()->AddArray(array);
    array->Delete();
}

void FDEMMeshSource::AddCellScalarArray(vtkPolyData *pd, const char *name, const std::vector<vtkIdType> &ids)
{
    vtkIdTypeArray *array = vtkIdTypeArray::New();
    array->SetName(name);
    array->SetNumberOfComponents(1);
    array->SetNumberOfTuples(pd->GetNumberOfCells());

    for(vtkIdType i = 0; i < pd->GetNumberOfCells(); i++)
    {
        array->SetValue(i, ids.at(i));
    }

    pd->GetCellData()->AddArray(array);
    array->Delete();
}

FDEMMeshSource::FDEMMeshSource()
{
    this->triangulation = nullptr;
    this->SetNumberOfInputPorts(0);
}

FDEMMeshSource::~FDEMMeshSource()
{

}

int FDEMMeshSource::RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}

int FDEMMeshSource::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}

void FDEMMeshSource::PrintSelf(std::ostream &, vtkIndent)
{

}



