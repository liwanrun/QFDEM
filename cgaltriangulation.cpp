#include "cgaltriangulation.h"

#include<QDebug>

#include <vtkPolyData.h>

#define CGAL_MESH_2_OPTIMIZER_VERBOSE

CGALTriangulation::CGALTriangulation()
{
    this->n_iters = 10;
    this->m_size = 2.0;
    this->m_shape = 0.125;
    this->m_color = QColor(204,204,81);
}

CGALTriangulation::~CGALTriangulation()
{

}

int CGALTriangulation::getMaxIterationNumber() const
{
    return this->n_iters;
}

void CGALTriangulation::setMaxIterationNumber(int value)
{
    this->n_iters = value;
}

double CGALTriangulation::getSize() const
{
    return this->m_size;
}

void CGALTriangulation::setSize(double size)
{
    this->m_size = size;
}

double CGALTriangulation::getShape() const
{
    return this->m_shape;
}

void CGALTriangulation::setShape(double shape)
{
    this->m_shape = shape;
}

QColor CGALTriangulation::getColor() const
{
    return this->m_color;
}

void CGALTriangulation::setColor(const QColor &color)
{
    this->m_color = color;
}

const QVector<CDTP::Point>& CGALTriangulation::getSeedPoints() const
{
    return this->seeds;
}

void CGALTriangulation::setSeedPoints(const QVector<QPointF> &seeds)
{
    this->seeds.clear();
    for(auto iter = seeds.begin(); iter != seeds.end(); ++iter)
    {
        CDTP::Point seed = CDTP::Point((*iter).x(), (*iter).y());
        this->seeds.push_back(seed);
    }
}

const CDTP& CGALTriangulation::getTriangleMesh() const
{
    return this->mesh;
}

QVector<CDTP::Constraint_id> CGALTriangulation::InsertConstraints(vtkPolyData *dataset)
{

    vtkPoints *points = dataset->GetPoints();
    vtkCellArray *lines = dataset->GetLines();

    vtkIdType pos = 0;
    vtkIdType npts, *ids;
    double A[3], B[3];

    QVector<CDTP::Constraint_id> constraints;
    constraints.reserve(dataset->GetNumberOfPoints());

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
                constraints.push_back(constraint);
            }
        }
        else if (VTK_LINE == dataset->GetCellType(i))
        {
            points->GetPoint(ids[0], A);
            points->GetPoint(ids[1], B);
            Epick::Point_2 src = Epick::Point_2(A[0], A[1]);
            Epick::Point_2 tar = Epick::Point_2(B[0], B[1]);
            CDTP::Constraint_id constraint = this->mesh.insert_constraint(src, tar);
            constraints.push_back(constraint);
        }

        constraints.squeeze();
    }

    return constraints;
}

Opt_code CGALTriangulation::RefineDelaunayMesh()
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
        qDebug() << "Triangulation Sub_Constraints number is: " << mesh.number_of_subconstraints();
        qDebug() << "Triangulation Faces number is: " << mesh.number_of_faces();
    }

    // 2.Optimize shape
    return CGAL::lloyd_optimize_mesh_2(mesh, CGAL::parameters::max_iteration_number = n_iters,
                                       CGAL::parameters::seeds_begin = seeds.begin(),
                                       CGAL::parameters::seeds_end = seeds.end());
}

void CGALTriangulation::InitMeshEntityId()
{
    size_t index;
    // 1.VertIds
    index = 0;
    for(auto viter = mesh.finite_vertices_begin(); viter != mesh.finite_vertices_end(); ++viter)
    {
        viter->info().id = index;
        index++;
    }

    // 2.FaceIds
    index = 0;
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
}

void CGALTriangulation::CreateMeshVertexInfo()
{

}

void CGALTriangulation::CreateMeshFaceInfo()
{

}

void CGALTriangulation::UpdateMeshVertexInfo()
{

}

void CGALTriangulation::UpdateMeshFaceInfo()
{

}
