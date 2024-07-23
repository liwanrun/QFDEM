#include "fdemGQfilteredmeshsource.h"
#include "constraineddelaunaytriangulation.h"
#include "constrainedtriangulation.h"
#include "groupcollection.h"

#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include <QDebug>


vtkStandardNewMacro(FDEMGenerateGQFilteredMeshSource);

FDEMGenerateGQFilteredMeshSource::FDEMGenerateGQFilteredMeshSource()
{

}

FDEMGenerateGQFilteredMeshSource::~FDEMGenerateGQFilteredMeshSource()
{

}

int FDEMGenerateGQFilteredMeshSource::RequestData(vtkInformation *vtkNotUsed(request),
                                                  vtkInformationVector **vtkNotUsed(inputVector),
                                                  vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    ConstrainedDelaunayTriangulation* t =
            static_cast<ConstrainedDelaunayTriangulation*>(this->triangulation.get());
    std::set<CDTP::Edge> constrainedEdges = t->GetExtrinsicConstrainedEdges();
    std::map<CDTP::Constraint_id, int> constraintIds = t->GetConstraintIds();

    CDTP& mesh = t->GetCGALTriangulation();
    size_t numPts = mesh.number_of_vertices();
    size_t numEls = mesh.number_of_faces();

    vtkPoints *newPoints;
    vtkCellArray *newPolys;

    newPoints = vtkPoints::New();
    newPoints->Allocate(numPts);

    newPolys = vtkCellArray::New();
    newPolys->Allocate(newPolys->EstimateSize(3*numEls, 4));

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

    std::vector<vtkIdType> constrainedQuads;

    for(auto eiter = mesh.finite_edges_begin(); eiter != mesh.finite_edges_end(); ++eiter)
    {
        if(Triangulation::isConstraintVisible())
        {
            if(mesh.is_constrained(*eiter))
            {
                int ihe = eiter->second;
                CDTP::Face_handle fh = eiter->first;

                //Intrinsic constraints
                CDTP::Vertex_handle va = fh->vertex(fh->ccw(ihe));
                CDTP::Vertex_handle vb = fh->vertex(fh->cw(ihe));
                CDTP::Constraint_id cid = mesh.context(va, vb).id();
                if(constraintIds.count(cid) && Triangulation::FRACTURE == constraintIds.at(cid))
                {
                    continue;
                }

                //Extrinsic constraints
                CDTP::Edge ec = (*eiter);
                CDTP::Edge en = mesh.mirror_edge(ec);
                if(constrainedEdges.count(ec) && constrainedEdges.count(en))
                {
                    continue;
                }
            }
        }

        int ihe = eiter->second;
        CDTP::Face_handle fh = eiter->first;
        CDTP::Face_handle fn = fh->neighbor(ihe);
        int jhe = fn->index(fh);

        if(fh->is_in_domain() && fn->is_in_domain())
        {
            vtkIdType id = newPolys->InsertNextCell(4);
            newPolys->InsertCellPoint(fh->info().verts[fh->cw(ihe)]);
            newPolys->InsertCellPoint(fh->info().verts[fh->ccw(ihe)]);
            newPolys->InsertCellPoint(fn->info().verts[fn->cw(jhe)]);
            newPolys->InsertCellPoint(fn->info().verts[fn->ccw(jhe)]);
            cellBlockIds.emplace_back(fh->info().fid);
            cellTypes.emplace_back(VTK_QUAD);

            if(mesh.is_constrained(*eiter))
            {
                int ihe = eiter->second;
                CDTP::Face_handle fh = eiter->first;

                //Intrinsic constraints
                CDTP::Vertex_handle va = fh->vertex(fh->ccw(ihe));
                CDTP::Vertex_handle vb = fh->vertex(fh->cw(ihe));
                CDTP::Constraint_id cid = mesh.context(va, vb).id();
                if(constraintIds.count(cid) && Triangulation::FRACTURE == constraintIds.at(cid))
                {
                    constrainedQuads.emplace_back(id);
                }

                //Extrinsic constraints
                CDTP::Edge ec = (*eiter);
                CDTP::Edge en = mesh.mirror_edge(ec);
                if(constrainedEdges.count(ec) && constrainedEdges.count(en))
                {
                    constrainedQuads.emplace_back(id);
                }
            }
        }
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
                CDTP::Point pt = fiter->vertex(i)->point();
                newPoints->InsertNextPoint(pt.x(), pt.y(), 0.0);
            }
        }
    }

    newPoints->Squeeze();
    output->SetPoints(newPoints);
    newPoints->Delete();

    //Attributes
    std::set<vtkIdType> boundaryPoints;
    for(auto eiter = mesh.constrained_edges_begin(); eiter != mesh.constrained_edges_end(); ++eiter)
    {
        int index = eiter->second;
        CDTP::Face_handle fh = eiter->first;

        //Intrinsic constraints
        CDTP::Vertex_handle va = fh->vertex(fh->ccw(index));
        CDTP::Vertex_handle vb = fh->vertex(fh->cw(index));
        CDTP::Constraint_id cid = mesh.context(va, vb).id();
        if(constraintIds.count(cid) && Triangulation::BEDPLANE != constraintIds.at(cid))
        {
            for(vtkIdType id : va->info().copyIds)
            {
                boundaryPoints.emplace(id);
            }
            for(vtkIdType id : vb->info().copyIds)
            {
                boundaryPoints.emplace(id);
            }
        }

        //Extrinsic constraints
        CDTP::Edge ec = (*eiter);
        CDTP::Edge en = mesh.mirror_edge(ec);
        if(constrainedEdges.count(ec) && constrainedEdges.count(en))
        {
            for(vtkIdType id : va->info().copyIds)
            {
                boundaryPoints.emplace(id);
            }
            for(vtkIdType id : vb->info().copyIds)
            {
                boundaryPoints.emplace(id);
            }
        }
    }

    this->AddNodeIdLabels(output);
    this->AddCellIdLabels(output);

    this->AddPointBoundaryTags(output, boundaryPoints);
    this->AddCellConstraintTags(output, constrainedQuads);
    this->AddCellScalarArray(output, "Cell Types", cellTypes);
    this->AddCellScalarArray(output, "Cell Block Ids", cellBlockIds);

    return 1;
}

int FDEMGenerateGQFilteredMeshSource::RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*)
{
    return 1;
}

void FDEMGenerateGQFilteredMeshSource::PrintSelf(std::ostream &, vtkIndent)
{

}


vtkStandardNewMacro(FDEMImportedGQFilteredMeshSource);

FDEMImportedGQFilteredMeshSource::FDEMImportedGQFilteredMeshSource()
{

}

FDEMImportedGQFilteredMeshSource::~FDEMImportedGQFilteredMeshSource()
{

}

int FDEMImportedGQFilteredMeshSource::RequestData(vtkInformation *vtkNotUsed(request),
                                                  vtkInformationVector **vtkNotUsed(inputVector),
                                                  vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    ConstrainedTriangulation* t = static_cast<ConstrainedTriangulation*>(this->triangulation.get());
    CT& mesh = t->GetCGALTriangulation();
    size_t numPts = t->GetNumberOfPoints();
    size_t numEls = t->GetNumberOfTriangles() + t->GetNumberOfQuads();

    vtkPoints *newPoints;
    vtkCellArray *newPolys;

    newPoints = vtkPoints::New();
    newPoints->Allocate(numPts);

    newPolys = vtkCellArray::New();
    newPolys->Allocate(newPolys->EstimateSize(numEls, 4));

    //1.Topology
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

    std::vector<vtkIdType> constrainedQuads;
    for(auto eiter = mesh.finite_edges_begin(); eiter != mesh.finite_edges_end(); ++eiter)
    {
        if(Triangulation::isConstraintVisible())
        {
            if(mesh.is_constrained(*eiter))
                continue;
        }

        int ihe = eiter->second;
        CT::Face_handle fh = eiter->first;
        CT::Face_handle fn = fh->neighbor(ihe);
        int jhe = fn->index(fh);

        if(fh->info().in_domain && fn->info().in_domain)
        {
            vtkIdType id = newPolys->InsertNextCell(4);
            newPolys->InsertCellPoint(fh->info().verts[fh->cw(ihe)]);
            newPolys->InsertCellPoint(fh->info().verts[fh->ccw(ihe)]);
            newPolys->InsertCellPoint(fn->info().verts[fn->cw(jhe)]);
            newPolys->InsertCellPoint(fn->info().verts[fn->ccw(jhe)]);

            vtkIdType grainId = (fh->info().gid == fn->info().gid) ? fh->info().gid : -1;
            cellBlockIds.emplace_back(fh->info().fid);
            cellGrainIds.emplace_back(grainId);
            cellTypes.emplace_back(VTK_QUAD);

            // Constraint dataArray
            if(mesh.is_constrained(*eiter))
            {
                constrainedQuads.emplace_back(id);
            }
        }
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
                CT::Point pt =  fiter->vertex(i)->point();
                newPoints->InsertNextPoint(pt.x(), pt.y(), 0.0);
            }
        }
    }

    newPoints->Squeeze();
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

        if(mesh.is_constrained(*eiter) || (!fh->info().in_domain) || (!fn->info().in_domain))
        {
            for(vtkIdType id : va->info().copyIds)
            {
                boundaryPoints.emplace(id);
            }

            for(vtkIdType id : vb->info().copyIds)
            {
                boundaryPoints.emplace(id);
            }
        }
    }

    this->AddNodeIdLabels(output);
    this->AddCellIdLabels(output);

    this->AddPointBoundaryTags(output, boundaryPoints);
    this->AddCellConstraintTags(output, constrainedQuads);
    this->AddCellScalarArray(output, "Cell Types", cellTypes);
    this->AddCellScalarArray(output, "Cell Block Ids", cellBlockIds);
    this->AddCellScalarArray(output, "Cell Grain Ids", cellGrainIds);

    return 1;
}

int FDEMImportedGQFilteredMeshSource::RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*)
{
    return 1;
}

void FDEMImportedGQFilteredMeshSource::PrintSelf(std::ostream &, vtkIndent)
{

}
