#include "fdemprimitivemeshsource.h"
#include "constraineddelaunaytriangulation.h"
#include "constrainedtriangulation.h"

#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include <QDebug>

vtkStandardNewMacro(FDEMGeneratePrimitiveMeshSource)

FDEMGeneratePrimitiveMeshSource::FDEMGeneratePrimitiveMeshSource()
    : FDEMMeshSource()
{

}

FDEMGeneratePrimitiveMeshSource::~FDEMGeneratePrimitiveMeshSource()
{

}

int FDEMGeneratePrimitiveMeshSource::RequestData(vtkInformation *vtkNotUsed(request),
                                        vtkInformationVector **vtkNotUsed(inputVector),
                                        vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    ConstrainedDelaunayTriangulation* t = dynamic_cast<ConstrainedDelaunayTriangulation*>(this->triangulation.get());
    CDTP& mesh = t->GetCGALTriangulation();
    size_t numPts = mesh.number_of_vertices();
    size_t numEls = mesh.number_of_faces();

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
        newPolys->InsertCellPoint((*fiter).info().verts[0]);
        newPolys->InsertCellPoint((*fiter).info().verts[1]);
        newPolys->InsertCellPoint((*fiter).info().verts[2]);
        cellBlockIds.emplace_back(fiter->info().fid);
        cellTypes.emplace_back(VTK_TRIANGLE);
    }

    output->SetPolys(newPolys);
    newPolys->Delete();

    for (auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        CDTP::Point pt = viter->point();
        newPoints->InsertNextPoint(pt.x(), pt.y(), 0.0);
    }

    output->SetPoints(newPoints);
    newPoints->Delete();

    //Attribute Array
    std::set<vtkIdType> boundaryPoints;
    std::map<CDTP::Constraint_id, int> constraintIdMap = t->GetConstraintIds();
    for (auto eiter = mesh.finite_edges_begin(); eiter != mesh.finite_edges_end(); ++eiter)
    {
        int index = eiter->second;
        CDTP::Face_handle fh = eiter->first;
        CDTP::Face_handle fn = fh->neighbor(index);
        CDTP::Vertex_handle va = fh->vertex(fh->ccw(index));
        CDTP::Vertex_handle vb = fh->vertex(fh->cw(index));

        if(!fh->is_in_domain() || !fn->is_in_domain())
        {
            boundaryPoints.emplace(va->info().id);
            boundaryPoints.emplace(vb->info().id);
        }
    }

    this->AddNodeIdLabels(output);
    this->AddCellIdLabels(output);
    this->AddPointBoundaryTags(output, boundaryPoints);
    this->AddCellScalarArray(output, "Cell Types", cellTypes);
    this->AddCellScalarArray(output, "Cell Block Ids", cellBlockIds);

    return 1;
}

int FDEMGeneratePrimitiveMeshSource::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}

void FDEMGeneratePrimitiveMeshSource::PrintSelf(std::ostream &, vtkIndent)
{

}


vtkStandardNewMacro(FDEMImportedPrimitiveMeshSource)

FDEMImportedPrimitiveMeshSource::FDEMImportedPrimitiveMeshSource()
    : FDEMMeshSource()
{

}

FDEMImportedPrimitiveMeshSource::~FDEMImportedPrimitiveMeshSource()
{

}

int FDEMImportedPrimitiveMeshSource::RequestData(vtkInformation *vtkNotUsed(request),
                                        vtkInformationVector **vtkNotUsed(inputVector),
                                        vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    ConstrainedTriangulation* t = dynamic_cast<ConstrainedTriangulation*>(this->triangulation.get());
    CT& mesh = t->GetCGALTriangulation();
    size_t numPts = mesh.number_of_vertices();
    size_t numEls = mesh.number_of_faces();

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
        newPolys->InsertCellPoint((*fiter).info().verts[0]);
        newPolys->InsertCellPoint((*fiter).info().verts[1]);
        newPolys->InsertCellPoint((*fiter).info().verts[2]);
        cellBlockIds.emplace_back(fiter->info().fid);
        cellGrainIds.emplace_back(fiter->info().gid);
        cellTypes.emplace_back(VTK_TRIANGLE);
    }

    output->SetPolys(newPolys);
    newPolys->Delete();

    for (auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        CT::Point pt = viter->point();
        newPoints->InsertNextPoint(pt.x(), pt.y(), 0.0);
    }

    output->SetPoints(newPoints);
    newPoints->Delete();

    //Attribute Array
    std::set<vtkIdType> boundaryPoints;
    for (auto eiter = mesh.finite_edges_begin(); eiter != mesh.finite_edges_end(); ++eiter)
    {
        int index = eiter->second;
        CT::Face_handle fh = eiter->first;
        CT::Face_handle fn = fh->neighbor(index);
        CT::Vertex_handle va = fh->vertex(fh->ccw(index));
        CT::Vertex_handle vb = fh->vertex(fh->cw(index));

        if(!fh->info().in_domain || !fn->info().in_domain)
        {
            boundaryPoints.emplace(va->info().id);
            boundaryPoints.emplace(vb->info().id);
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

int FDEMImportedPrimitiveMeshSource::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}

void FDEMImportedPrimitiveMeshSource::PrintSelf(std::ostream &, vtkIndent)
{

}
