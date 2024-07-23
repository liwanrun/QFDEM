#include "constrainedtriangulation.h"
#include "blockgeometry.h"

#include <set>
#include <QDebug>
#include <QtMath>

#include <vtkDoubleArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCellType.h>

ConstrainedTriangulation::ConstrainedTriangulation()
    : Triangulation()
{
    this->numPoints = 0;
    this->numTriangles = 0;
    this->numQuads = 0;
}

ConstrainedTriangulation::~ConstrainedTriangulation()
{

}

CT &ConstrainedTriangulation::GetCGALTriangulation()
{
    return this->mesh;
}

size_t ConstrainedTriangulation::InitTriangulationNodeIds()
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

size_t ConstrainedTriangulation::InitTriangulationFaceIds()
{
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        fiter->info().ranks.clear();
    }

    size_t index = 0;
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(fiter->info().in_domain)
        {
            fiter->info().id = index;
            for(int i = 0; i < 3; i++)
            {
                fiter->info().verts[i] = fiter->vertex(i)->info().id;
            }
            index++;
        }
    }
    this->numTriangles = index;
    this->numQuads = 0;

    return index;
}

void ConstrainedTriangulation::UpdateGlobalQuadInsertionInfo()
{
    //Restore initial status
    for(auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        viter->info().copyIds.clear();
    }

    size_t index = 0;
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(fiter->info().in_domain)
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

void ConstrainedTriangulation::UpdateLocalQuadInsertionInfo()
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

void ConstrainedTriangulation::UpdateSplitTriangulationInfo()
{
    //Restore initial status
    for(auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        viter->info().copyIds.clear();
    }

    size_t index = 0;
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(fiter->info().in_domain)
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

void ConstrainedTriangulation::UpdateInsertConstraintsInfo(QMap<QString, QSharedPointer<Primitive>> &primitive_map)
{
    for(auto fiter = mesh.all_faces_begin(); fiter != mesh.all_faces_end(); ++fiter)
    {
        fiter->set_constraints(false, false, false);
    }

    for(auto iter = primitive_map.begin(); iter != primitive_map.end(); ++iter)
    {
        if(iter.value()->isEmbedded())
            continue;

        vtkDoubleArray* fractures = iter.value()->getTableData();
        for(vtkIdType i = 0; i < fractures->GetNumberOfTuples(); i++)
        {
            std::set<CT::Vertex_handle> constrainedVerts;
            double *tuple = fractures->GetTuple4(i);
            Epick::Point_2 p = Epick::Point_2(tuple[0], tuple[1]);
            Epick::Point_2 q = Epick::Point_2(tuple[2], tuple[3]);
            Epick::Segment_2 s = Epick::Segment_2(p, q);

            CT::Line_face_circulator f_cur = mesh.line_walk(p, q), f_end(f_cur);
            int id = this->FindNearestVertexToPoint(f_cur, p);
            f_cur->vertex(id)->info().is_constrained = true;
            constrainedVerts.insert(f_cur->vertex(id));
            do
            {
                if(!f_cur->info().in_domain)
                    continue;

                if(!CGAL::do_intersect(s, mesh.triangle(f_cur)))
                    continue;

                // Orientations
                int orients[3];
                for(int i = 0; i < 3; i++)
                {
                    Epick::Point_2 r = f_cur->vertex(i)->point();
                    orients[i] = CGAL::orientation(p, q, r);
                    qDebug() << "Point >> " << r.x() << ", " << r.y();
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

                    CT::Face_handle f_adj = f_cur->neighbor(i);
                    if((constrainedVerts.size() > 1) &&
                       (!f_cur->info().in_domain || !f_adj->info().in_domain))
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
                    }
                }
            } while(++f_cur != f_end);

            for(CT::Vertex_handle vh : constrainedVerts)
            {
                vh->info().is_constrained = false;
            }
        }
    }
}

void ConstrainedTriangulation::RestoreTriangulationStatus()
{
    for(auto eiter = mesh.constrained_edges_begin(); eiter != mesh.constrained_edges_end(); ++eiter)
    {
        CT::Edge ec = (*eiter);
        CT::Edge en = mesh.mirror_edge(ec);
        if(this->intrinsicConstrainedEdges.count(ec) &&
           this->intrinsicConstrainedEdges.count(en))
        {
            int index = eiter->second;
            CT::Face_handle fh = eiter->first;
            CT::Face_handle fn = fh->neighbor(index);
            mesh.remove_constrained_edge(fh, index);
            mesh.remove_constrained_edge(fn, fn->index(fh));
        }
    }
    this->intrinsicConstrainedEdges.clear();
}

void ConstrainedTriangulation::UpdateTriangulationConstraint(vtkUnstructuredGrid *ug)
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

            CT::Point p = CT::Point(x, y);
            CT::Face_handle fh = mesh.locate(p);
            for(int i = 0; i < 3; i++)
            {
                CT::Face_handle fn = fh->neighbor(i);
                if(fh->info().in_domain && fn->info().in_domain)
                {
                    mesh.insert_constraint(fh->vertex(fh->ccw(i)), fh->vertex(fh->cw(i)));
                    this->intrinsicConstrainedEdges.insert(CT::Edge(fh, i));
                    this->intrinsicConstrainedEdges.insert(CT::Edge(fn, fn->index(fh)));
                }
            }
        }
    }
}

double ConstrainedTriangulation::GetTriangulationCharacteristicLength()
{
    double minSize = 1.0e+15;
    CT::Finite_faces_iterator fiter;
    for(fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        Epick::Triangle_2 face = mesh.triangle(fiter);
        double area = face.area();

        double length2 = 1.0e-15;
        for (int i = 0; i < 3; i++)
        {
            CT::Edge edge(fiter, i);
            Epick::Segment_2 seg = mesh.segment(edge);
            length2 = fmax(length2, seg.squared_length());
        }

        minSize = fmin(minSize, 2*area/sqrt(length2));
    }

    return minSize;
}

double ConstrainedTriangulation::GetTriangulationMinimumSize()
{
    double minSize = 1.0e+15;
    CT::Finite_edges_iterator eiter;
    for(eiter = mesh.finite_edges_begin(); eiter != mesh.finite_edges_end(); ++eiter)
    {
        Epick::Segment_2 edge = mesh.segment(eiter);
        minSize = fmin(minSize, edge.squared_length());
    }

    return sqrt(minSize);
}

double ConstrainedTriangulation::GetTriangulationMinimumArea()
{
    double minArea = 1.0e+15;
    CT::Finite_faces_iterator fiter;
    for(fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        Epick::Triangle_2 face = mesh.triangle(fiter);
        minArea = fmin(minArea, face.area());
    }

    return minArea;
}

std::set<CT::Edge>& ConstrainedTriangulation::GetIntrinsicConstrainedEdges()
{
    return this->intrinsicConstrainedEdges;
}

void ConstrainedTriangulation::InitSplitVertices()
{
    this->splitVertices.clear();
    for(auto eiter = mesh.constrained_edges_begin(); eiter != mesh.constrained_edges_end(); ++eiter)
    {
        int index = eiter->second;
        CT::Face_handle fh = eiter->first;
        CT::Vertex_handle va = fh->vertex(fh->ccw(index));
        CT::Vertex_handle vb = fh->vertex(fh->cw(index));
        this->splitVertices.emplace(va);
        this->splitVertices.emplace(vb);
    }
}

void ConstrainedTriangulation::CreateTriangulationNodeInfo()
{
    //Clear Vertex Info
    for(auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        viter->info().copyIds.clear();
        viter->info().copyIds.push_back(viter->info().id);
    }

    for(CT::Vertex_handle v : splitVertices)
    {
        int degree = 0;
        auto ec = mesh.incident_edges(v), ef(ec);
        do
        {
            if(mesh.is_infinite(*ec))
            {
                v->info().is_boundary = true;
                continue;
            }
            else if(mesh.is_constrained(*ec))
            {
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

void ConstrainedTriangulation::CreateTriangulationFaceInfo()
{
    //Clear Face Info
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        fiter->info().ranks.clear();
    }

    qDebug() << this->splitVertices.size();
    for (CT::Vertex_handle v : splitVertices)
    {
//      qDebug()<<"[need split]"<<"("<<v->point().x()<<","<<v->point().y()<<")";
        this->constrainedVertices.clear();
        this->incidentAngles.clear();
        this->ConstructConstrainedVertices(v);
        this->ConstructIncidentAngles(v);

        CT::Face_circulator fh = mesh.incident_faces(v), ff(fh);
        do
        {
            if(fh->info().in_domain)
            {
                fh->info().ranks.push_back(this->ComputeIncidentFaceRank(v, fh));
            }
        } while(++fh != ff);
    }
}

void ConstrainedTriangulation::UpdateTriangulationNodeInfo()
{
    size_t index = mesh.number_of_vertices();
    for (CT::Vertex_handle v : splitVertices)
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

void ConstrainedTriangulation::UpdateTriangulationFaceInfo()
{
    for (CT::Vertex_handle v : splitVertices)
    {
        CT::Face_circulator fh = mesh.incident_faces(v), ff(fh);
        do
        {
            if(fh->info().in_domain)
            {
                int index = fh->index(v);
                int rank = fh->info().ranks.front();
                fh->info().verts[index] = v->info().copyIds[rank];
                fh->info().ranks.erase(fh->info().ranks.begin());
            }
        } while(++fh != ff);
    }

    this->numQuads = this->ComputeNumberOfQuads();
}

void ConstrainedTriangulation::ConstructConstrainedVertices(const CT::Vertex_handle v)
{
    bool is_boundary_vertex_inserted = false;
    this->constrainedVertices.reserve(v->info().numCopy);
    CT::Edge_circulator ec = mesh.incident_edges(v), ef(ec);
    do
    {
        if(mesh.is_infinite(*ec))
            continue;

        int index = ec->second;
        CT::Face_handle fh = ec->first;
        CT::Face_handle fn = fh->neighbor(index);
        CT::Vertex_handle vh = fh->vertex(fh->ccw(index));

        if(mesh.is_constrained(*ec))
        {
            this->constrainedVertices.push_back(vh);
        }
        else if(!fh->info().in_domain || !fn->info().in_domain)
        {
            if(is_boundary_vertex_inserted)
                continue;

            this->constrainedVertices.push_back(vh);
            is_boundary_vertex_inserted = true;
        }
    } while(++ec != ef);

    this->constrainedVertices.squeeze();
}

void ConstrainedTriangulation::ConstructIncidentAngles(const CT::Vertex_handle v)
{
    this->incidentAngles.reserve(v->info().numCopy + 1);
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

int ConstrainedTriangulation::ComputeIncidentFaceRank(const CT::Vertex_handle v, CT::Face_handle fh)
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

size_t ConstrainedTriangulation::ComputeNumberOfTriangles()
{
    size_t count = 0;
    for(auto fiter = mesh.finite_faces_begin(); fiter != mesh.finite_faces_end(); ++fiter)
    {
        if(fiter->info().in_domain)
        {
            count += 1;
        }
    }
    return count;
}

size_t ConstrainedTriangulation::ComputeNumberOfQuads()
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
                    continue;

                int index = eiter->second;
                CT::Face_handle fh = eiter->first;
                CT::Face_handle fn = fh->neighbor(index);
                if(fh->info().in_domain && fn->info().in_domain)
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
                CT::Face_handle fh = eiter->first;
                CT::Face_handle fn = fh->neighbor(index);
                if(fh->info().in_domain && fn->info().in_domain)
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
                CT::Face_handle fh = eiter->first;
                CT::Face_handle fn = fh->neighbor(index);

                if(fh->info().in_domain && fn->info().in_domain)
                {
                    count += 1;
                }
            }
            return count;
        }
    }

    return count;
}

int ConstrainedTriangulation::FindOptimalEdgeIncidentLine(const CT::Face_handle& f, const Epick::Line_2& l)
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

int ConstrainedTriangulation::FindNearestVertexToPoint(const CT::Face_handle &f, const Epick::Point_2 &p)
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
