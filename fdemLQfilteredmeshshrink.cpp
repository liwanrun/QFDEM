#include "fdemLQfilteredmeshshrink.h"
#include "constraineddelaunaytriangulation.h"
#include "constrainedtriangulation.h"

#include <vtkInformation.h>
#include <vtkInformationVector.h>

vtkStandardNewMacro(FDEMGenerateLQFilteredMeshShrink);

void FDEMGenerateLQFilteredMeshShrink::PrintSelf(std::ostream &, vtkIndent)
{

}

FDEMGenerateLQFilteredMeshShrink::FDEMGenerateLQFilteredMeshShrink()
{

}

FDEMGenerateLQFilteredMeshShrink::~FDEMGenerateLQFilteredMeshShrink()
{

}

int FDEMGenerateLQFilteredMeshShrink::RequestData(vtkInformation *vtkNotUsed(request),
                                                  vtkInformationVector **inputVector,
                                                  vtkInformationVector *outputVector)
{
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output= vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
    output->DeepCopy(input);

    ConstrainedDelaunayTriangulation* t = static_cast<ConstrainedDelaunayTriangulation*>(this->triangulation.get());
    std::set<CDTP::Edge> constrainedEdges = t->GetExtrinsicConstrainedEdges();
    std::map<CDTP::Constraint_id, int> constraintIds = t->GetConstraintIds();

    CDTP& mesh = t->GetCGALTriangulation();
    double sf = t->getShrinkFactor();
    size_t numPts = mesh.number_of_vertices();

    vtkPoints *newPoints= vtkPoints::New();
    newPoints->Allocate(numPts);

    for(auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        for(size_t index : viter->info().copyIds)
        {
            newPoints->InsertPoint(index, viter->point().x(), viter->point().y(), 0.0);
        }
    }

    for(auto eiter = mesh.constrained_edges_begin(); eiter != mesh.constrained_edges_end(); ++eiter)
    {
        int ihe = eiter->second;
        CDTP::Face_handle fh = eiter->first;
        CDTP::Face_handle fn = fh->neighbor(ihe);
        int jhe = fn->index(fh);

        CDTP::Vertex_handle va = fh->vertex(fh->ccw(ihe));
        CDTP::Vertex_handle vb = fh->vertex(fh->cw(ihe));
        CDTP::Constraint_id cid = mesh.context(va, vb).id();
        if(constraintIds.count(cid) && Triangulation::FRACTURE != constraintIds.at(cid))
        {
            continue;
        }

        if(fh->is_in_domain() && fn->is_in_domain())
        {
            size_t id;
            CDTP::Point pc, pt;

            //1.current face
            pc = CGAL::centroid(mesh.triangle(fh));

            pt = fh->vertex(fh->ccw(ihe))->point();
            id = fh->info().verts[fh->ccw(ihe)];
            pt = (1 == floor(sf)) ? (pt) : (pc + sf * (pt - pc));
            newPoints->SetPoint(id, pt.x(), pt.y(), 0.0);

            pt = fh->vertex(fh->cw(ihe))->point();
            id = fh->info().verts[fh->cw(ihe)];
            pt = (1 == floor(sf)) ? (pt) : (pc + sf * (pt - pc));
            newPoints->SetPoint(id, pt.x(), pt.y(), 0.0);

            //2.neighbour face
            pc = CGAL::centroid(mesh.triangle(fn));

            pt = fn->vertex(fn->ccw(jhe))->point();
            id = fn->info().verts[fn->ccw(jhe)];
            pt = (1 == floor(sf)) ? (pt) : (pc + sf * (pt - pc));
            newPoints->SetPoint(id, pt.x(), pt.y(), 0.0);

            pt = fn->vertex(fn->cw(jhe))->point();
            id = fn->info().verts[fn->cw(jhe)];
            pt = (1 == floor(sf)) ? (pt) : (pc + sf * (pt - pc));
            newPoints->SetPoint(id, pt.x(), pt.y(), 0.0);
        }
    }

    newPoints->Squeeze();
    output->SetPoints(newPoints);
    newPoints->Delete();

    return 1;
}

int FDEMGenerateLQFilteredMeshShrink::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}


vtkStandardNewMacro(FDEMImportedLQFilteredMeshShrink);

void FDEMImportedLQFilteredMeshShrink::PrintSelf(std::ostream &, vtkIndent)
{

}

FDEMImportedLQFilteredMeshShrink::FDEMImportedLQFilteredMeshShrink()
{

}

FDEMImportedLQFilteredMeshShrink::~FDEMImportedLQFilteredMeshShrink()
{

}

int FDEMImportedLQFilteredMeshShrink::RequestData(vtkInformation *vtkNotUsed(request),
                                                  vtkInformationVector **inputVector,
                                                  vtkInformationVector *outputVector)
{
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output= vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
    output->DeepCopy(input);

    ConstrainedTriangulation* t = static_cast<ConstrainedTriangulation*>(this->triangulation.get());
    CT& mesh = t->GetCGALTriangulation();
    double sf = t->getShrinkFactor();
    size_t numPts = mesh.number_of_vertices();

    vtkPoints *newPoints= vtkPoints::New();
    newPoints->Allocate(numPts);

    for(auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        for(size_t index : viter->info().copyIds)
        {
            newPoints->InsertPoint(index, viter->point().x(), viter->point().y(), 0.0);
        }
    }

    for(auto eiter = mesh.constrained_edges_begin(); eiter != mesh.constrained_edges_end(); ++eiter)
    {
        int ihe = eiter->second;
        CT::Face_handle fh = eiter->first;
        CT::Face_handle fn = fh->neighbor(ihe);
        int jhe = fn->index(fh);

        if(fh->info().in_domain && fn->info().in_domain)
        {
            size_t id;
            CT::Point pc, pt;

            //1.current face
            pc = CGAL::centroid(mesh.triangle(fh));

            pt = fh->vertex(fh->ccw(ihe))->point();
            id = fh->info().verts[fh->ccw(ihe)];
            pt = (1 == floor(sf)) ? (pt) : (pc + sf * (pt - pc));
            newPoints->SetPoint(id, pt.x(), pt.y(), 0.0);

            pt = fh->vertex(fh->cw(ihe))->point();
            id = fh->info().verts[fh->cw(ihe)];
            pt = (1 == floor(sf)) ? (pt) : (pc + sf * (pt - pc));
            newPoints->SetPoint(id, pt.x(), pt.y(), 0.0);

            //2.neighbour face
            pc = CGAL::centroid(mesh.triangle(fn));

            pt = fn->vertex(fn->ccw(jhe))->point();
            id = fn->info().verts[fn->ccw(jhe)];
            pt = (1 == floor(sf)) ? (pt) : (pc + sf * (pt - pc));
            newPoints->SetPoint(id, pt.x(), pt.y(), 0.0);

            pt = fn->vertex(fn->cw(jhe))->point();
            id = fn->info().verts[fn->cw(jhe)];
            pt = (1 == floor(sf)) ? (pt) : (pc + sf * (pt - pc));
            newPoints->SetPoint(id, pt.x(), pt.y(), 0.0);
        }
    }

    newPoints->Squeeze();
    output->SetPoints(newPoints);
    newPoints->Delete();

    return 1;
}

int FDEMImportedLQFilteredMeshShrink::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}
