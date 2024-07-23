#include "voronoidiagram.h"
#include "triangulation.h"
#include "constrainedtriangulation.h"
#include "constraineddelaunaytriangulation.h"

#include <QDebug>

VoronoiDiagram::VoronoiDiagram()
{
    qDebug() << "VoronoiDiagram";
}

VD VoronoiDiagram::GetVoronoiDiagram() const
{
    return this->vd;
}

std::vector<QLineF> VoronoiDiagram::GetCroppedVoronoi() const
{
    return this->cropped_vd;
}

void VoronoiDiagram::Construct(Triangulation *dt)
{
    this->vd.clear();

    if(Triangulation::GENERATED == dt->meshSourceFlag())
    {
        ConstrainedDelaunayTriangulation *triangulation = static_cast<ConstrainedDelaunayTriangulation*>(dt);
        CDTP& cdt = triangulation->GetCGALTriangulation();
        for(auto viter = cdt.finite_vertices_begin(); viter != cdt.finite_vertices_end(); ++viter)
        {
            AT::Site_2 site = AT::Point_2(viter->point().x(),viter->point().y());
            this->vd.insert(site);
        }
    }
    else if(Triangulation::IMPORTED == dt->meshSourceFlag())
    {
        ConstrainedTriangulation *triangulation = static_cast<ConstrainedTriangulation*>(dt);
        CT& ct = triangulation->GetCGALTriangulation();
        for(auto viter = ct.finite_vertices_begin(); viter != ct.finite_vertices_end(); ++viter)
        {
            AT::Site_2 site = AT::Point_2(viter->point().x(),viter->point().y());
            this->vd.insert(site);
        }
    }
}

void VoronoiDiagram::CropBoundary(double *bounds)
{
    Epick::Iso_rectangle_2 bbox(bounds[0], bounds[2], bounds[1], bounds[3]);
    Cropped_voronoi_from_delaunay clipper(bbox);

    DT dt2 = this->vd.dual();
    dt2.draw_dual(clipper);

    for(Epick::Segment_2 segment : clipper.m_cropped_vd)
    {
        Epick::Point_2 src = segment.source();
        Epick::Point_2 tar = segment.target();

        QLineF line(src.x(), src.y(), tar.x(), tar.y());
        this->cropped_vd.emplace_back(line);
    }
}
