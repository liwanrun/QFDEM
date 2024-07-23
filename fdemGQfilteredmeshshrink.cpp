#include "fdemGQfilteredmeshshrink.h"
#include "constraineddelaunaytriangulation.h"
#include "constrainedtriangulation.h"

#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

vtkStandardNewMacro(FDEMGenerateGQFilteredMeshShrink);

void FDEMGenerateGQFilteredMeshShrink::PrintSelf(std::ostream &, vtkIndent)
{

}

FDEMGenerateGQFilteredMeshShrink::FDEMGenerateGQFilteredMeshShrink()
{

}

FDEMGenerateGQFilteredMeshShrink::~FDEMGenerateGQFilteredMeshShrink()
{

}

int FDEMGenerateGQFilteredMeshShrink::RequestData(vtkInformation *vtkNotUsed(request),
                                                  vtkInformationVector **inputVector,
                                                  vtkInformationVector *outputVector)
{
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output= vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
    output->DeepCopy(input);

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
//            CDTP::Point p0 = fiter->vertex(0)->point();
//            CDTP::Point p1 = fiter->vertex(1)->point();
//            CDTP::Point p2 = fiter->vertex(2)->point();
//            CDTP::Point pc = CGAL::centroid(p0, p1, p2);

//            for(int i = 0; i < 3; i++)
//            {
//                CDTP::Point pt =  fiter->vertex(i)->point();
//                pt = pc + t->getShrinkFactor() * (pt - pc);
//                newPoints->InsertNextPoint(pt.x(), pt.y(), 0.0);
//            }

            int numBndEdges = 0;
            for(int i = 0; i < 3; i++)
            {
                CDTP::Face_handle fn = fiter->neighbor(i);
                if(!fn->is_in_domain())
                {
                    numBndEdges += 1;
                }
            }

            CDTP::Point pc = CGAL::centroid(mesh.triangle(fiter));
            switch(numBndEdges)
            {
            case 0:
            {
                for(int i = 0; i < 3; i++)
                {
                    CDTP::Point p = fiter->vertex(i)->point();
                    p = (1 == floor(sf)) ? (p) : (pc + sf * (p - pc));
                    newPoints->InsertNextPoint(p.x(), p.y(), 0.0);
                }

                break;
            }
            case 1:
            {
                int i, j, k;
                for(i = 0; i < 3; i++)
                {
                    CDTP::Face_handle fn = fiter->neighbor(i);
                    if(!fn->is_in_domain())
                    {
                        j = fiter->ccw(i);
                        k = fiter->ccw(j);
                        break;
                    }
                }

                CDTP::Point pts[3];
                pts[i] = fiter->vertex(i)->point();
                pts[j] = fiter->vertex(j)->point();
                pts[k] = fiter->vertex(k)->point();
                CDTP::Point mp = CGAL::midpoint(pts[j], pts[k]);

                pts[i] = (1 == floor(sf)) ? (pts[i]) : (pc + sf * (pts[i] - pc));
                pts[j] = (1 == floor(sf)) ? (pts[j]) : (mp + sf * (pts[j] - mp));
                pts[k] = (1 == floor(sf)) ? (pts[k]) : (mp + sf * (pts[k] - mp));

                newPoints->InsertNextPoint(pts[0].x(), pts[0].y(), 0.0);
                newPoints->InsertNextPoint(pts[1].x(), pts[1].y(), 0.0);
                newPoints->InsertNextPoint(pts[2].x(), pts[2].y(), 0.0);

                break;
            }
            case 2:
            {
                int i, j, k;
                for(i = 0; i < 3; i++)
                {
                    CDTP::Face_handle fn = fiter->neighbor(i);
                    if(fn->is_in_domain())
                    {
                        j = fiter->ccw(i);
                        k = fiter->ccw(j);
                        break;
                    }
                }

                CDTP::Point pts[3];
                pts[i] = fiter->vertex(i)->point();
                pts[j] = fiter->vertex(j)->point();
                pts[k] = fiter->vertex(k)->point();
                CDTP::Point p0 = CGAL::midpoint(pts[i], pts[j]);
                CDTP::Point p1 = CGAL::midpoint(pts[k], pts[i]);

                pts[j] = (1 == floor(sf)) ? (pts[j]) : (p0 + sf * (pts[j] - p0));
                pts[k] = (1 == floor(sf)) ? (pts[k]) : (p1 + sf * (pts[k] - p1));

                newPoints->InsertNextPoint(pts[0].x(), pts[0].y(), 0.0);
                newPoints->InsertNextPoint(pts[1].x(), pts[1].y(), 0.0);
                newPoints->InsertNextPoint(pts[2].x(), pts[2].y(), 0.0);

                break;
            }
            }
        }
    }

    newPoints->Squeeze();
    output->SetPoints(newPoints);
    newPoints->Delete();

    return 1;
}

int FDEMGenerateGQFilteredMeshShrink::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}


vtkStandardNewMacro(FDEMImportedGQFilteredMeshShrink);

void FDEMImportedGQFilteredMeshShrink::PrintSelf(std::ostream &, vtkIndent)
{

}

FDEMImportedGQFilteredMeshShrink::FDEMImportedGQFilteredMeshShrink()
{

}

FDEMImportedGQFilteredMeshShrink::~FDEMImportedGQFilteredMeshShrink()
{

}

int FDEMImportedGQFilteredMeshShrink::RequestData(vtkInformation *vtkNotUsed(request),
                                                  vtkInformationVector **inputVector,
                                                  vtkInformationVector *outputVector)
{
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output= vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
    output->DeepCopy(input);

    //New Points
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
//            CT::Point p0 = fiter->vertex(0)->point();
//            CT::Point p1 = fiter->vertex(1)->point();
//            CT::Point p2 = fiter->vertex(2)->point();
//            CT::Point pc = CGAL::centroid(p0, p1, p2);

//            for(int i = 0; i < 3; i++)
//            {
//                CT::Point pt = fiter->vertex(i)->point();
//                pt = pc + t->getShrinkFactor() * (pt - pc);
//                newPoints->InsertNextPoint(pt.x(), pt.y(), 0.0);
//            }

            int numBndEdges = 0;
            for(int i = 0; i < 3; i++)
            {
                CT::Face_handle fn = fiter->neighbor(i);
                if(!fn->info().in_domain)
                {
                    numBndEdges += 1;
                }
            }

            CT::Point pc = CGAL::centroid(mesh.triangle(fiter));
            switch(numBndEdges)
            {
            case 0:
            {
                for(int i = 0; i < 3; i++)
                {
                    CT::Point p = fiter->vertex(i)->point();
                    p = (1 == floor(sf)) ? (p) : (pc + sf * (p - pc));
                    newPoints->InsertNextPoint(p.x(), p.y(), 0.0);
                }

                break;
            }
            case 1:
            {
                int i, j, k;
                for(i = 0; i < 3; i++)
                {
                    CT::Face_handle fn = fiter->neighbor(i);
                    if(!fn->info().in_domain)
                    {
                        j = fiter->ccw(i);
                        k = fiter->ccw(j);
                        break;
                    }
                }

                CT::Point pts[3];
                pts[i] = fiter->vertex(i)->point();
                pts[j] = fiter->vertex(j)->point();
                pts[k] = fiter->vertex(k)->point();
                CT::Point mp = CGAL::midpoint(pts[j], pts[k]);

                pts[i] = (1 == floor(sf)) ? (pts[i]) : (pc + sf * (pts[i] - pc));
                pts[j] = (1 == floor(sf)) ? (pts[j]) : (mp + sf * (pts[j] - mp));
                pts[k] = (1 == floor(sf)) ? (pts[k]) : (mp + sf * (pts[k] - mp));

                newPoints->InsertNextPoint(pts[0].x(), pts[0].y(), 0.0);
                newPoints->InsertNextPoint(pts[1].x(), pts[1].y(), 0.0);
                newPoints->InsertNextPoint(pts[2].x(), pts[2].y(), 0.0);

                break;
            }
            case 2:
            {
                int i, j, k;
                for(i = 0; i < 3; i++)
                {
                    CT::Face_handle fn = fiter->neighbor(i);
                    if(fn->info().in_domain)
                    {
                        j = fiter->ccw(i);
                        k = fiter->ccw(j);
                        break;
                    }
                }

                CT::Point pts[3];
                pts[i] = fiter->vertex(i)->point();
                pts[j] = fiter->vertex(j)->point();
                pts[k] = fiter->vertex(k)->point();
                CT::Point p0 = CGAL::midpoint(pts[i], pts[j]);
                CT::Point p1 = CGAL::midpoint(pts[k], pts[i]);

                pts[j] = (1 == floor(sf)) ? (pts[j]) : (p0 + sf * (pts[j] - p0));
                pts[k] = (1 == floor(sf)) ? (pts[k]) : (p1 + sf * (pts[k] - p1));

                newPoints->InsertNextPoint(pts[0].x(), pts[0].y(), 0.0);
                newPoints->InsertNextPoint(pts[1].x(), pts[1].y(), 0.0);
                newPoints->InsertNextPoint(pts[2].x(), pts[2].y(), 0.0);

                break;
            }
            }
        }
    }

    newPoints->Squeeze();
    output->SetPoints(newPoints);
    newPoints->Delete();

    return 1;
}

int FDEMImportedGQFilteredMeshShrink::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}
