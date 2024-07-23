#include "fdemSTfilteredmeshshrink.h"
#include "constraineddelaunaytriangulation.h"
#include "constrainedtriangulation.h"

#include <vtkInformation.h>
#include <vtkInformationVector.h>

vtkStandardNewMacro(FDEMGenerateSTFilteredMeshShrink);

void FDEMGenerateSTFilteredMeshShrink::PrintSelf(std::ostream &, vtkIndent)
{

}

FDEMGenerateSTFilteredMeshShrink::FDEMGenerateSTFilteredMeshShrink()
{

}

FDEMGenerateSTFilteredMeshShrink::~FDEMGenerateSTFilteredMeshShrink()
{

}

int FDEMGenerateSTFilteredMeshShrink::RequestData(vtkInformation *vtkNotUsed(request),
                                                  vtkInformationVector **inputVector,
                                                  vtkInformationVector *outputVector)
{
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output= vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    ConstrainedDelaunayTriangulation* t = static_cast<ConstrainedDelaunayTriangulation*>(this->triangulation.get());
    CDTP& mesh = t->GetCGALTriangulation();
    double sf = t->getShrinkFactor();
    size_t numPts = mesh.number_of_vertices();

    vtkPoints *newPoints= vtkPoints::New();
    newPoints->Allocate(numPts);

    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(fiter->is_in_domain())
        {
            CDTP::Point p0 = fiter->vertex(0)->point();
            CDTP::Point p1 = fiter->vertex(1)->point();
            CDTP::Point p2 = fiter->vertex(2)->point();
            CDTP::Point pc = CGAL::centroid(p0, p1, p2);

            for(int i = 0; i < 3; i++)
            {
                CDTP::Point pt =  fiter->vertex(i)->point();
                pt = (1 == floor(sf)) ? (pt) : (pc + sf * (pt - pc));
                newPoints->InsertNextPoint(pt.x(), pt.y(), 0.0);
            }
        }
    }

    newPoints->Squeeze();
    input->SetPoints(newPoints);
    newPoints->Delete();

    output->Initialize();
    output->ShallowCopy(input);

    return 1;
}

int FDEMGenerateSTFilteredMeshShrink::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}


vtkStandardNewMacro(FDEMImportedSTFilteredMeshShrink);

void FDEMImportedSTFilteredMeshShrink::PrintSelf(std::ostream &, vtkIndent)
{

}

FDEMImportedSTFilteredMeshShrink::FDEMImportedSTFilteredMeshShrink()
{

}

FDEMImportedSTFilteredMeshShrink::~FDEMImportedSTFilteredMeshShrink()
{

}

int FDEMImportedSTFilteredMeshShrink::RequestData(vtkInformation *vtkNotUsed(request),
                                                  vtkInformationVector **inputVector,
                                                  vtkInformationVector *outputVector)
{
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output= vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    ConstrainedTriangulation* t = static_cast<ConstrainedTriangulation*>(this->triangulation.get());
    CT& mesh = t->GetCGALTriangulation();
    double sf = t->getShrinkFactor();
    size_t numPts = mesh.number_of_vertices();

    vtkPoints *newPoints= vtkPoints::New();
    newPoints->Allocate(numPts);

    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(fiter->info().in_domain)
        {
            CT::Point pc = CGAL::centroid(mesh.triangle(fiter));
            for(int i = 0; i < 3; i++)
            {
                CT::Point pt = fiter->vertex(i)->point();
                pt = (1 == floor(sf)) ? (pt) : (pc + sf * (pt - pc));
                newPoints->InsertNextPoint(pt.x(), pt.y(), 0.0);
            }
        }
    }

    newPoints->Squeeze();
    input->SetPoints(newPoints);
    newPoints->Delete();

    output->Initialize();
    output->ShallowCopy(input);

    return 1;
}

int FDEMImportedSTFilteredMeshShrink::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}
