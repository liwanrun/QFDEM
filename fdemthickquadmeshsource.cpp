#include "fdemthickquadmeshsource.h"
#include "hybridmeshtriangulation.h"

#include <vtkInformation.h>
#include <vtkInformationVector.h>

vtkStandardNewMacro(FDEMImportedThickQuadMeshSource)

FDEMImportedThickQuadMeshSource::FDEMImportedThickQuadMeshSource()
    : FDEMMeshSource()
{

}

FDEMImportedThickQuadMeshSource::~FDEMImportedThickQuadMeshSource()
{

}

int FDEMImportedThickQuadMeshSource::RequestData(vtkInformation *vtkNotUsed(request),
                                                 vtkInformationVector **vtkNotUsed(inputVector),
                                                 vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    HybridMeshTriangulation* mesh = dynamic_cast<HybridMeshTriangulation*>(this->triangulation.get());

    vtkIdType numTriangles = static_cast<vtkIdType>(mesh->triangles.size());
    vtkIdType numQuads = static_cast<vtkIdType>(mesh->quads.size());
    vtkIdType numPts = static_cast<vtkIdType>(mesh->points.size());
    vtkIdType numEls = numTriangles + numQuads;

    vtkCellArray *newPolys = vtkCellArray::New();
    newPolys->Allocate(newPolys->EstimateSize(numEls, 4));
    for(std::array<size_t, 3> triangle : mesh->triangles)
    {
        newPolys->InsertNextCell(3);
        newPolys->InsertCellPoint(vtkIdType(triangle[0]));
        newPolys->InsertCellPoint(vtkIdType(triangle[1]));
        newPolys->InsertCellPoint(vtkIdType(triangle[2]));
    }

    for(std::array<size_t, 4> quad : mesh->quads)
    {
        newPolys->InsertNextCell(4);
        newPolys->InsertCellPoint(vtkIdType(quad[0]));
        newPolys->InsertCellPoint(vtkIdType(quad[1]));
        newPolys->InsertCellPoint(vtkIdType(quad[2]));
        newPolys->InsertCellPoint(vtkIdType(quad[3]));
    }

    newPolys->Squeeze();
    output->SetPolys(newPolys);
    newPolys->Delete();

    vtkPoints *newPoints = vtkPoints::New();
    newPoints->Allocate(numPts);
    for(QPointF pt : mesh->points)
    {
        newPoints->InsertNextPoint(pt.x(), pt.y(), 0.0);
    }

    newPoints->Squeeze();
    output->SetPoints(newPoints);
    newPoints->Delete();

    this->AddNodeIdLabels(output);
    this->AddCellIdLabels(output);

    return 1;
}

int FDEMImportedThickQuadMeshSource::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}

void FDEMImportedThickQuadMeshSource::PrintSelf(std::ostream &, vtkIndent )
{

}
