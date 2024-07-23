#include "constraineddelaunaytriangulation.h"
#include "blockgeometry.h"

#include <set>
#include <QtMath>
#include <QDebug>
#include <vtkPolyData.h>
#include <vtkDoubleArray.h>

ConstrainedDelaunayTriangulation::ConstrainedDelaunayTriangulation()
    : Triangulation()
{
    this->m_shape = 0.125;
    this->m_size = 2.0;
    this->m_iters = 10;
}

ConstrainedDelaunayTriangulation::~ConstrainedDelaunayTriangulation()
{

}

int ConstrainedDelaunayTriangulation::getMaxIterationNumber() const
{
    return this->m_iters;
}

void ConstrainedDelaunayTriangulation::setMaxIterationNumber(int value)
{
    this->m_iters = value;
}

double ConstrainedDelaunayTriangulation::getSize() const
{
    return this->m_size;
}

void ConstrainedDelaunayTriangulation::setSize(double size)
{
    this->m_size = size;
}

double ConstrainedDelaunayTriangulation::getShape() const
{
    return this->m_shape;
}

void ConstrainedDelaunayTriangulation::setShape(double shape)
{
    this->m_shape = shape;
}

const QVector<CDTP::Point>& ConstrainedDelaunayTriangulation::getSeedPoints() const
{
    return this->seeds;
}

void ConstrainedDelaunayTriangulation::setSeedPoints(const QVector<QPointF> &seeds)
{
    this->seeds.clear();
    for(auto iter = seeds.begin(); iter != seeds.end(); ++iter)
    {
        CDTP::Point seed = CDTP::Point((*iter).x(), (*iter).y());
        this->seeds.push_back(seed);
    }
}

CDTP& ConstrainedDelaunayTriangulation::GetCGALTriangulation()
{
    return this->mesh;
}

void ConstrainedDelaunayTriangulation::InsertConstraints(vtkPolyData *dataset, int flag)
{

    vtkPoints *points = dataset->GetPoints();
    vtkCellArray *lines = dataset->GetLines();

    vtkIdType pos = 0;
    vtkIdType npts, *ids;
    double A[3], B[3];

    for (vtkIdType i = 0; i < lines->GetNumberOfCells(); i++)
    {
        lines->GetCell(pos, npts, ids);
        pos = (i + 1) * (npts + 1);

        if(VTK_POLY_LINE == dataset->GetCellType(i))
        {
            for(vtkIdType j = 0; j < npts - 1; j++)
            {
                points->GetPoint(ids[j], A);
                points->GetPoint(ids[j + 1], B);
                Epick::Point_2 src = Epick::Point_2(A[0], A[1]);
                Epick::Point_2 tar = Epick::Point_2(B[0], B[1]);
                CDTP::Constraint_id constraint = this->mesh.insert_constraint(src, tar);
                this->constraintIds.emplace(constraint, flag);
            }
        }
        else if (VTK_LINE == dataset->GetCellType(i))
        {
            points->GetPoint(ids[0], A);
            points->GetPoint(ids[1], B);
            Epick::Point_2 src = Epick::Point_2(A[0], A[1]);
            Epick::Point_2 tar = Epick::Point_2(B[0], B[1]);
            CDTP::Constraint_id constraint = this->mesh.insert_constraint(src, tar);
            this->constraintIds.emplace(constraint, flag);
        }
    }

    qDebug() << "The number of constraints is:" << this->constraintIds.size();
}

std::map<CDTP::Constraint_id, int>& ConstrainedDelaunayTriangulation::GetConstraintIds()
{
    return this->constraintIds;
}

std::set<CDTP::Edge> &ConstrainedDelaunayTriangulation::GetExtrinsicConstrainedEdges()
{
    return this->extrinsicConstrainedEdges;
}

Opt_code ConstrainedDelaunayTriangulation::RefineDelaunayMesh(const QString& blockId)
{
    // 1.Refine mesh
    Criteria crit(this->m_shape, this->m_size);
    Mesher mesher(this->mesh, crit);
    mesher.set_seeds(seeds.begin(), seeds.end(), false);
    mesher.refine_mesh();

    if(mesh.is_valid())
    {
        qDebug() << "Triangulation Vertices number is: " << mesh.number_of_vertices();
        qDebug() << "Triangulation Constraints number is: " << mesh.number_of_constraints();
        qDebug() << "Triangulation Faces number is: " << mesh.number_of_faces();
    }

    // 2.Optimize shape
    Opt_code code = CGAL::lloyd_optimize_mesh_2(mesh, CGAL::parameters::max_iteration_number = m_iters,
                                       CGAL::parameters::seeds_begin = seeds.begin(),
                                       CGAL::parameters::seeds_end = seeds.end());

    // 3.Restore blockId
    int idx = blockId.split(QChar('-'), QString::SkipEmptyParts).last().toInt();
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        fiter->info().fid = idx;
    }

    return code;
}

void ConstrainedDelaunayTriangulation::ClearRefinedDelaunayMesh()
{
    if(this->mesh.is_valid())
    {
        this->mesh.clear();
    }
}

size_t ConstrainedDelaunayTriangulation::InitTriangulationNodeIds()
{
    for(auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        viter->info().copyIds.clear();
    }

    size_t index = 0;
    for(auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        viter->info().id = index;
        viter->info().copyIds.push_back(index);
        index++;
    }
    this->numPoints = index;

    return index;
}

size_t ConstrainedDelaunayTriangulation::InitTriangulationFaceIds()
{
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        fiter->info().ranks.clear();
    }

    size_t index = 0;
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(!fiter->is_in_domain())
            continue;

        fiter->info().id = index;
        for(int i = 0; i < 3; i++)
        {
            fiter->info().verts[i] = fiter->vertex(i)->info().id;
        }
        index++;
    }
    this->numTriangles = index;
    this->numQuads = 0;

    return index;
}

void ConstrainedDelaunayTriangulation::UpdateGlobalQuadInsertionInfo()
{
    //Restore initial status
    for(auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        viter->info().copyIds.clear();
    }

    size_t index = 0;
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(fiter->is_in_domain())
        {
            for(int i = 0; i < 3; i++)
            {
                fiter->info().verts[i] = index;
                fiter->vertex(i)->info().id = index;
                fiter->vertex(i)->info().copyIds.push_back(index);
                index += 1;
            }
        }
    }
    this->numPoints = index;
    this->numQuads = ComputeNumberOfQuads();
}

void ConstrainedDelaunayTriangulation::UpdateLocalQuadInsertionInfo()
{
    //Restore initial status
    this->InitTriangulationNodeIds();
    this->InitTriangulationFaceIds();

    this->InitSplitVertices();
    this->CreateTriangulationNodeInfo();
    this->CreateTriangulationFaceInfo();
    this->UpdateTriangulationNodeInfo();
    this->UpdateTriangulationFaceInfo();
}

void ConstrainedDelaunayTriangulation::UpdateSplitTriangulationInfo()
{
    //Restore initial status
    for(auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        viter->info().copyIds.clear();
    }

    size_t index = 0;
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(fiter->is_in_domain())
        {
            for(int i = 0; i < 3; i++)
            {
                fiter->info().verts[i] = index;
                fiter->vertex(i)->info().id = index;
                fiter->vertex(i)->info().copyIds.push_back(index);
                index += 1;
            }
        }
    }
    this->numPoints = index;
}

void ConstrainedDelaunayTriangulation::UpdateInsertConstraintsInfo(QMap<QString, QSharedPointer<Primitive>> &primitive_map)
{
    for(auto iter = primitive_map.begin(); iter != primitive_map.end(); ++iter)
    {
        if(iter.value()->isEmbedded())
            continue;

        vtkDoubleArray* fractures = iter.value()->getTableData();
        for(vtkIdType i = 0; i < fractures->GetNumberOfTuples(); i++)
        {
            std::set<CDTP::Vertex_handle> constrainedVerts;
            double *tuple = fractures->GetTuple4(i);
            Epick::Point_2 p = Epick::Point_2(tuple[0], tuple[1]);
            Epick::Point_2 q = Epick::Point_2(tuple[2], tuple[3]);
            Epick::Segment_2 s = Epick::Segment_2(p, q);

            CDTP::Line_face_circulator f_cur = mesh.line_walk(p, q), f_end(f_cur);
            int id = this->FindNearestVertexToPoint(f_cur, p);
            f_cur->vertex(id)->info().is_constrained = true;
            constrainedVerts.insert(f_cur->vertex(id));
            do
            {
                if(!f_cur->is_in_domain())
                    continue;

                if(!CGAL::do_intersect(s, mesh.triangle(f_cur)))
                    continue;

                // Orientations
                int orients[3];
                for(int i = 0; i < 3; i++)
                {
                    Epick::Point_2 r = f_cur->vertex(i)->point();
                    orients[i] = CGAL::orientation(p, q, r);
                }

                int index = 0;
                for(int i = 0; i < 3; i++)
                {
                    int j = f_cur.ccw(i);
                    int k = f_cur.ccw(j);

                    if(orients[i] != orients[j] && orients[j] != orients[k] &&
                       orients[k] != orients[i] && CGAL::COLLINEAR == orients[i])
                    {
                        Epick::Point_2 pj = f_cur->vertex(j)->point();
                        Epick::Point_2 pk = f_cur->vertex(k)->point();
                        double dj = CGAL::squared_distance(pj, s.supporting_line());
                        double dk = CGAL::squared_distance(pk, s.supporting_line());
                        index = (dj < dk) ? j : k;
                        break;
                    }
                    else if(orients[j] == orients[k] && orients[i] != orients[j])
                    {
                        index = i;
                        break;
                    }
                }

                if(!f_cur->vertex(index)->info().is_constrained)
                {
                    int i = this->FindOptimalEdgeIncidentLine(f_cur, s.supporting_line());
                    int j = f_cur->ccw(i);
                    int k = f_cur->ccw(j);

                    CDTP::Face_handle f_adj = f_cur->neighbor(i);
                    if((constrainedVerts.size() > 1) &&
                       (!f_cur->is_in_domain() || !f_adj->is_in_domain()))
                    {
                        break;
                    }
                    else
                    {
                        mesh.insert_constraint(f_cur->vertex(j), f_cur->vertex(k));
                        f_cur->vertex(j)->info().is_constrained = true;
                        f_cur->vertex(k)->info().is_constrained = true;
                        constrainedVerts.insert(f_cur->vertex(j));
                        constrainedVerts.insert(f_cur->vertex(k));

                        extrinsicConstrainedEdges.insert(CDTP::Edge(f_cur, i));
                        extrinsicConstrainedEdges.insert(CDTP::Edge(f_adj, f_adj->index(f_cur)));
                    }
                }
            } while(++f_cur != f_end);

            for(CDTP::Vertex_handle vh : constrainedVerts)
            {
                vh->info().is_constrained = false;
            }
        }
    }

    qDebug() << "Number of constraint ids: " << this->constraintIds.size();
    qDebug() << "Number of constrained edges: " << this->extrinsicConstrainedEdges.size();
}

void ConstrainedDelaunayTriangulation::RestoreTriangulationStatus()
{
    for(auto eiter = mesh.constrained_edges_begin(); eiter != mesh.constrained_edges_end(); ++eiter)
    {
        int index = eiter->second;
        CDTP::Face_handle fh = eiter->first;

        CDTP::Vertex_handle va = fh->vertex(fh->ccw(index));
        CDTP::Vertex_handle vb = fh->vertex(fh->cw(index));
        CDTP::Constraint_id cid = mesh.context(va, vb).id();
        if(!constraintIds.count(cid))
        {
            CDTP::Face_handle fn = fh->neighbor(index);
            mesh.remove_constrained_edge(fh, index);
            mesh.remove_constrained_edge(fn, fn->index(fh));
        }
    }
    this->extrinsicConstrainedEdges.clear();
}

void ConstrainedDelaunayTriangulation::UpdateTriangulationConstraint(vtkUnstructuredGrid *ug)
{
    vtkPoints *points = ug->GetPoints();
    for(vtkIdType id = 0; id < ug->GetNumberOfCells(); id++)
    {
        if(VTK_TRIANGLE == ug->GetCellType(id))
        {
            vtkIdList *ptIds = vtkIdList::New();
            ug->GetCellPoints(id, ptIds);

            double pts[3][3];
            points->GetPoint(ptIds->GetId(0), pts[0]);
            points->GetPoint(ptIds->GetId(1), pts[1]);
            points->GetPoint(ptIds->GetId(2), pts[2]);

            double x, y;
            x = (pts[0][0] + pts[1][0] + pts[2][0]) / 3.0;
            y = (pts[0][1] + pts[1][1] + pts[2][1]) / 3.0;

            CDTP::Point p = CDTP::Point(x, y);
            CDTP::Face_handle fh = mesh.locate(p);
            for(int i = 0; i < 3; i++)
            {
                CDTP::Face_handle fn = fh->neighbor(i);
                if(fh->is_in_domain() && fn->is_in_domain())
                {
                    mesh.insert_constraint(fh->vertex(fh->ccw(i)), fh->vertex(fh->cw(i)));
                    this->extrinsicConstrainedEdges.insert(CDTP::Edge(fh, i));
                    this->extrinsicConstrainedEdges.insert(CDTP::Edge(fn, fn->index(fh)));
                }
            }
        }
    }
}

double ConstrainedDelaunayTriangulation::GetTriangulationCharacteristicLength()
{
    double minSize = 1.0e+15;
    CDTP::Finite_faces_iterator fiter;
    for(fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        Epick::Triangle_2 face = mesh.triangle(fiter);
        double area = face.area();

        double length2 = 1.0e-15;
        for (int i = 0; i < 3; i++)
        {
            CDTP::Edge edge(fiter, i);
            Epick::Segment_2 seg = mesh.segment(edge);
            length2 = fmax(length2, seg.squared_length());
        }

        minSize = fmin(minSize, 2*area/sqrt(length2));
    }

    return minSize;
}

double ConstrainedDelaunayTriangulation::GetTriangulationMinimumSize()
{
    double minSize = 1.0e+15;
    CDTP::Finite_edges_iterator eiter;
    for(eiter = mesh.finite_edges_begin(); eiter != mesh.finite_edges_end(); ++eiter)
    {
        Epick::Segment_2 edge = mesh.segment(eiter);
        minSize = fmin(minSize, edge.squared_length());
    }

    return sqrt(minSize);
}

double ConstrainedDelaunayTriangulation::GetTriangulationMinimumArea()
{
    double minArea = 1.0e+15;
    CDTP::Finite_faces_iterator fiter;
    for(fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        Epick::Triangle_2 face = mesh.triangle(fiter);
        minArea = fmin(minArea, face.area());
    }

    return minArea;
}

void ConstrainedDelaunayTriangulation::InitSplitVertices()
{
    this->splitVertices.clear();
    for(auto eiter = mesh.constrained_edges_begin(); eiter != mesh.constrained_edges_end(); ++eiter)
    {
        int index = eiter->second;
        CDTP::Face_handle fh = eiter->first;
        CDTP::Vertex_handle va = fh->vertex(fh->ccw(index));
        CDTP::Vertex_handle vb = fh->vertex(fh->cw(index));

        CDTP::Constraint_id cid = mesh.context(va, vb).id();
        if(constraintIds.count(cid) && FRACTURE != constraintIds.at(cid))
            continue;

        this->splitVertices.emplace(va);
        this->splitVertices.emplace(vb);
    }
}

void ConstrainedDelaunayTriangulation::CreateTriangulationNodeInfo()
{
    //Clear Vertex Info
    for(auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        viter->info().copyIds.clear();
        viter->info().copyIds.push_back(viter->info().id);
    }

    for(CDTP::Vertex_handle v : splitVertices)
    {
        int degree = 0;

        auto ec = mesh.incident_edges(v), ef(ec);
        do
        {
            if(mesh.is_infinite(*ec))
            {
                v->info().is_boundary = true;
            }
            else if(mesh.is_constrained(*ec))
            {
                int index = ec->second;
                CDTP::Face_handle fh = ec->first;
                CDTP::Vertex_handle va = fh->vertex(fh->ccw(index));
                CDTP::Vertex_handle vb = fh->vertex(fh->cw(index));

                CDTP::Constraint_id cid = mesh.context(va, vb).id();
                if(constraintIds.count(cid) && FRACTURE != constraintIds.at(cid))
                    continue;

                degree += 1;
            }
        } while(++ec != ef);

        v->info().numCopy = v->info().is_boundary ? (degree + 1) : (degree);
        if((1 == v->info().numCopy) && (!Triangulation::isConstraintVisible()))
        {
            std::string info = "Non-persistent discontinuity is detected!";
            throw std::logic_error(info);
        }
    }
}

void ConstrainedDelaunayTriangulation::CreateTriangulationFaceInfo()
{
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        fiter->info().ranks.clear();
    }

    qDebug() << this->splitVertices.size();
    for (CDTP::Vertex_handle v : splitVertices)
    {
//      qDebug()<<"[need split]"<<"("<<v->point().x()<<","<<v->point().y()<<")";
        this->constrainedVertices.clear();
        this->incidentAngles.clear();
        this->ConstructConstrainedVertices(v);
        this->ConstructIncidentAngles(v);

        CDTP::Face_circulator fh = mesh.incident_faces(v), ff(fh);
        do
        {
            if((mesh.is_infinite(fh)) || (!fh->is_in_domain()))
                continue;

            fh->info().ranks.push_back(this->ComputeIncidentFaceRank(v, fh));
//          qDebug() << "id: " << fh->info().id << "; " << "rank: " << this->ComputeIncidentFaceRank(v, fh);
        } while(++fh != ff);
    }
}

void ConstrainedDelaunayTriangulation::UpdateTriangulationNodeInfo()
{
    size_t index = mesh.number_of_vertices();
    for(CDTP::Vertex_handle v : splitVertices)
    {
        if(v->info().numCopy > 1)
        {
            int degree = v->info().numCopy;
            for(int i = 1; i < degree; i++)
            {
                v->info().copyIds.push_back(index);
                index++;
            }
        }
    }

    this->numPoints = index;
}

void ConstrainedDelaunayTriangulation::UpdateTriangulationFaceInfo()
{
    for(CDTP::Vertex_handle v : splitVertices)
    {
        CDTP::Face_circulator fh = mesh.incident_faces(v), ff(fh);
        do
        {
            if((mesh.is_infinite(fh)) || (!fh->is_in_domain()))
                continue;

            int index = fh->index(v);
            int rank = fh->info().ranks.front();
            fh->info().verts[index] = v->info().copyIds[rank];
            fh->info().ranks.erase(fh->info().ranks.begin());
        } while(++fh != ff);
    }

    this->numQuads = this->ComputeNumberOfQuads();
    qDebug() << "The number of quads is: " << this->numQuads;
}

void ConstrainedDelaunayTriangulation::ConstructConstrainedVertices(const CDTP::Vertex_handle v)
{
    bool is_boundary_vertex_inserted = false;
    this->constrainedVertices.reserve(v->info().numCopy);
    CDTP::Edge_circulator ec = mesh.incident_edges(v), ef(ec);
    do
    {
        if(mesh.is_infinite(*ec))
            continue;

        if(mesh.is_constrained(*ec))
        {
            int index = ec->second;
            CDTP::Face_handle fh = ec->first;
            CDTP::Vertex_handle va = fh->vertex(fh->ccw(index));
            CDTP::Vertex_handle vb = fh->vertex(fh->cw(index));

            CDTP::Constraint_id cid = mesh.context(va, vb).id();
            if(constraintIds.count(cid) && BOUNDARY == constraintIds.at(cid))
            {
                CDTP::Face_handle fn = fh->neighbor(index);
                if(mesh.is_infinite(fh) || mesh.is_infinite(fn))
                {
                    if(is_boundary_vertex_inserted)
                        continue;

                    this->constrainedVertices.push_back(va);
                    is_boundary_vertex_inserted = true;
                }
            }
            else if(constraintIds.count(cid) && BEDPLANE == constraintIds.at(cid))
            {
                continue;
            }
            else
            {
                this->constrainedVertices.push_back(va);
            }
        }
    } while(++ec != ef);

    this->constrainedVertices.squeeze();
}

void ConstrainedDelaunayTriangulation::ConstructIncidentAngles(const CDTP::Vertex_handle v)
{
    this->incidentAngles.reserve(v->info().numCopy);
    this->incidentAngles.push_back(0.0);

    Epick::Vector_2 vec1 = v->point() - constrainedVertices[0]->point();
    for(int i = 1; i < constrainedVertices.size(); i++)
    {
        Epick::Vector_2 vec2 = v->point() - constrainedVertices[i]->point();
        double value = CGAL::scalar_product(vec1, vec2) / qSqrt(vec1.squared_length() * vec2.squared_length());
        double rad = qAcos((value < -1.0) ? -1.0 : (value > 1.0 ? 1.0 : value));
        double theta = qRadiansToDegrees(rad);
        if(CGAL::RIGHT_TURN == CGAL::orientation(vec1, vec2))
        {
            theta = 360.0 - theta;
        }
        this->incidentAngles.push_back(theta);
    }
    this->incidentAngles.push_back(360.0);
    this->incidentAngles.squeeze();
}

int ConstrainedDelaunayTriangulation::ComputeIncidentFaceRank(const CDTP::Vertex_handle v, CDTP::Face_handle fh)
{
    Epick::Point_2 cp = CGAL::centroid(mesh.triangle(fh));
    Epick::Vector_2 vec1 = v->point() - constrainedVertices[0]->point();
    Epick::Vector_2 vec2 = v->point() - cp;
    double value = CGAL::scalar_product(vec1, vec2) / qSqrt(vec1.squared_length() * vec2.squared_length());
    double rad = qAcos((value < -1.0) ? -1.0 : (value > 1.0 ? 1.0 : value));
    double theta = qRadiansToDegrees(rad);
    if(CGAL::RIGHT_TURN == CGAL::orientation(vec1, vec2))
    {
        theta = 360.0 - theta;
    }

    //Binary search for sorted vector
    int lower = 0, median = 0;
    int upper = this->incidentAngles.size() - 1;
    while((upper - lower) != 1)
    {
        median = (lower + upper) >> 1;
        if(theta > this->incidentAngles[median])
        {
            lower = median;
        }
        else if(theta < this->incidentAngles[median])
        {
            upper = median;
        }
        else
        {
            return median;
        }
    }
    return lower;
}

size_t ConstrainedDelaunayTriangulation::ComputeNumberOfTriangles()
{
    size_t count = 0;
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(fiter->is_in_domain())
        {
            count++;
        }
    }
    return count;
}

size_t ConstrainedDelaunayTriangulation::ComputeNumberOfQuads()
{
    size_t count = 0;
    if(GQHANDLED == Triangulation::meshFilterFlag())
    {
        if(Triangulation::isConstraintVisible())
        {
            count = 0;
            for(auto eiter = mesh.finite_edges_begin(); eiter != mesh.finite_edges_end(); ++eiter)
            {
                if(mesh.is_constrained(*eiter))
                {
                    int index = eiter->second;
                    CDTP::Face_handle fh = eiter->first;
                    CDTP::Vertex_handle va = fh->vertex(fh->ccw(index));
                    CDTP::Vertex_handle vb = fh->vertex(fh->cw(index));

                    CDTP::Constraint_id cid = mesh.context(va, vb).id();
                    if(constraintIds.count(cid) && BEDPLANE == constraintIds.at(cid))
                    {
                        count += 1;
                    }
                }
                else
                {
                    count += 1;
                }
            }
            return count;
        }
        else
        {
            count = 0;
            for(auto eiter = mesh.finite_edges_begin(); eiter != mesh.finite_edges_end(); ++eiter)
            {
                int index = eiter->second;
                CDTP::Face_handle fh = eiter->first;
                CDTP::Face_handle fn = fh->neighbor(index);
                if((fh->is_in_domain()) && (fn->is_in_domain()))
                {
                    count += 1;
                }
            }
            return count;
        }
    }
    else if(LQHANDLED == Triangulation::meshFilterFlag())
    {
        if(Triangulation::isConstraintVisible())
        {
            return 0;
        }
        else
        {
            count = 0;
            for(auto eiter = mesh.constrained_edges_begin(); eiter != mesh.constrained_edges_end(); ++eiter)
            {
                int index = eiter->second;
                CDTP::Face_handle fh = eiter->first;
                CDTP::Vertex_handle va = fh->vertex(fh->ccw(index));
                CDTP::Vertex_handle vb = fh->vertex(fh->cw(index));

                CDTP::Constraint_id cid = mesh.context(va, vb).id();
                if(constraintIds.count(cid) && FRACTURE != constraintIds.at(cid))
                {
                    continue;
                }

                count += 1;
            }
            return count;
        }
    }

    return count;
}

int ConstrainedDelaunayTriangulation::FindOptimalEdgeIncidentLine(const CDTP::Face_handle &f, const Epick::Line_2 &l)
{
    Epick::Vector_2 e = l.to_vector();
    e = e / std::sqrt(e.squared_length());

    int index = 0;
    for(int i = 0; i < 3; i++)
    {
        int j = f->ccw(i);
        int k = f->ccw(j);

        if(f->vertex(i)->info().is_constrained)
        {
            Epick::Point_2 p0 = f->vertex(i)->point();
            Epick::Point_2 p1 = f->vertex(j)->point();
            Epick::Point_2 p2 = f->vertex(k)->point();

            Epick::Vector_2 u = Epick::Vector_2(p0, p1);
            u = u / std::sqrt(u.squared_length());
            Epick::Vector_2 v = Epick::Vector_2(p0, p2);
            v = v / std::sqrt(v.squared_length());

            // flatter edge index
            double rj = std::acos(CGAL::scalar_product(u, e));
            double rk = std::acos(CGAL::scalar_product(v, e));
            int i_flat = (rj > rk) ? j : k;

            // nearer edge index
            double dj = CGAL::squared_distance(p1, l);
            double dk = CGAL::squared_distance(p2, l);
            int i_near = (dj > dk) ? j : k;

            index = (i_flat == i_near) ? i_near : i_flat;
        }
    }

    return index;
}

int ConstrainedDelaunayTriangulation::FindNearestVertexToPoint(const CDTP::Face_handle &f, const Epick::Point_2 &p)
{
    int index = 0;
    double min_dist = 9.999999e+10;
    for(int i = 0; i < 3; i++)
    {
        Epick::Point_2 q = f->vertex(i)->point();
        double dist = CGAL::squared_distance(p, q);
        if(dist < min_dist)
        {
            index = i;
            min_dist = dist;
        }
    }

    return index;
}
