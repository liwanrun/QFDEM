#include "fdemSTfilteredmeshsource.h"
#include "constraineddelaunaytriangulation.h"
#include "constrainedtriangulation.h"

#include <vtkInformation.h>
#include <vtkInformationVector.h>

vtkStandardNewMacro(FDEMGenerateSTFilteredMeshSource);

void FDEMGenerateSTFilteredMeshSource::PrintSelf(std::ostream &, vtkIndent)
{

}

FDEMGenerateSTFilteredMeshSource::FDEMGenerateSTFilteredMeshSource()
{

}

FDEMGenerateSTFilteredMeshSource::~FDEMGenerateSTFilteredMeshSource()
{

}

int FDEMGenerateSTFilteredMeshSource::RequestData(vtkInformation *vtkNotUsed(request),
                                                  vtkInformationVector **vtkNotUsed(inputVector),
                                                  vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    ConstrainedDelaunayTriangulation* t =
            dynamic_cast<ConstrainedDelaunayTriangulation*>(this->triangulation.get());
    CDTP& mesh = t->GetCGALTriangulation();
    size_t numPts = t->GetNumberOfPoints();
    size_t numEls = t->GetNumberOfTriangles();

    vtkPoints *newPoints;
    vtkCellArray *newPolys;

    newPoints = vtkPoints::New();
    newPoints->Allocate(numPts);

    newPolys = vtkCellArray::New();
    newPolys->Allocate(newPolys->EstimateSize(numEls, 3));

    std::vector<vtkIdType> cellTypes;
    std::vector<vtkIdType> cellBlockIds;
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(!fiter->is_in_domain())
            continue;

        newPolys->InsertNextCell(3);
        newPolys->InsertCellPoint(fiter->info().verts[0]);
        newPolys->InsertCellPoint(fiter->info().verts[1]);
        newPolys->InsertCellPoint(fiter->info().verts[2]);
        cellBlockIds.emplace_back(fiter->info().fid);
        cellTypes.emplace_back(VTK_TRIANGLE);
    }

    newPolys->Squeeze();
    output->SetPolys(newPolys);
    newPolys->Delete();

    //2.Geometry
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(fiter->is_in_domain())
        {
            for(int i = 0; i < 3; i++)
            {
                CDTP::Point pt =  fiter->vertex(i)->point();
                newPoints->InsertNextPoint(pt.x(), pt.y(), 0.0);
            }
        }
    }

    newPoints->Squeeze();
    output->SetPoints(newPoints);
    newPoints->Delete();

    //Attribute Array
    std::set<vtkIdType> boundaryPoints;
    for(auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        for(vtkIdType id : viter->info().copyIds)
        {
            boundaryPoints.emplace(id);
        }
    }

    this->AddNodeIdLabels(output);
    this->AddCellIdLabels(output);

    this->AddPointBoundaryTags(output, boundaryPoints);
    this->AddCellScalarArray(output, "Cell Types", cellTypes);
    this->AddCellScalarArray(output, "Cell Block Ids", cellBlockIds);

    return 1;
}

int FDEMGenerateSTFilteredMeshSource::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}


vtkStandardNewMacro(FDEMImportedSTFilteredMeshSource);

void FDEMImportedSTFilteredMeshSource::PrintSelf(std::ostream &, vtkIndent)
{

}

FDEMImportedSTFilteredMeshSource::FDEMImportedSTFilteredMeshSource()
{

}

FDEMImportedSTFilteredMeshSource::~FDEMImportedSTFilteredMeshSource()
{

}

int FDEMImportedSTFilteredMeshSource::RequestData(vtkInformation *vtkNotUsed(request),
                                                  vtkInformationVector **vtkNotUsed(inputVector),
                                                  vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    ConstrainedTriangulation* t =
            dynamic_cast<ConstrainedTriangulation*>(this->triangulation.get());
    CT& mesh = t->GetCGALTriangulation();
    size_t numPts = t->GetNumberOfPoints();
    size_t numEls = t->GetNumberOfTriangles();

    vtkPoints *newPoints;
    vtkCellArray *newPolys;

    newPoints = vtkPoints::New();
    newPoints->Allocate(numPts);

    newPolys = vtkCellArray::New();
    newPolys->Allocate(newPolys->EstimateSize(numEls, 3));

    std::vector<vtkIdType> cellTypes;
    std::vector<vtkIdType> cellBlockIds;
    std::vector<vtkIdType> cellGrainIds;
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(!fiter->info().in_domain)
            continue;

        newPolys->InsertNextCell(3);
        newPolys->InsertCellPoint(fiter->info().verts[0]);
        newPolys->InsertCellPoint(fiter->info().verts[1]);
        newPolys->InsertCellPoint(fiter->info().verts[2]);
        cellBlockIds.emplace_back(fiter->info().fid);
        cellGrainIds.emplace_back(fiter->info().gid);
        cellTypes.emplace_back(VTK_TRIANGLE);
    }

    newPolys->Squeeze();
    output->SetPolys(newPolys);
    newPolys->Delete();

    //2.Geometry
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(fiter->info().in_domain)
        {
            for(int i = 0; i < 3; i++)
            {
                CT::Point pt = fiter->vertex(i)->point();
                newPoints->InsertNextPoint(pt.x(), pt.y(), 0.0);
            }
        }
    }

    newPoints->Squeeze();
    output->SetPoints(newPoints);
    newPoints->Delete();

    //Attribute Array
    std::set<vtkIdType> boundaryPoints;
    for(auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        for(vtkIdType id : viter->info().copyIds)
        {
            boundaryPoints.emplace(id);
        }
    }

    this->AddNodeIdLabels(output);
    this->AddCellIdLabels(output);

    this->AddPointBoundaryTags(output, boundaryPoints);
    this->AddCellScalarArray(output, "Cell Types", cellTypes);
    this->AddCellScalarArray(output, "Cell Block Ids", cellBlockIds);
    this->AddCellScalarArray(output, "Cell Grain Ids", cellGrainIds);

    return 1;
}

int FDEMImportedSTFilteredMeshSource::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}
