#include "block.h"
#include "constraineddelaunaytriangulation.h"
#include "voronoidiagram.h"

Block::Block()
{

}

Block::~Block()
{

}

QSharedPointer<Primitive> Block::GetBoundary(const QString &id)
{
    if(!this->boundary_map.contains(id))
    {
        this->AppendBoundary(id);
    }
    return this->boundary_map[id];
}

void Block::RemoveBoundary(const QString &id)
{
    this->boundary_map.remove(id);
}

QSharedPointer<Primitive> Block::GetFracture(const QString &id)
{
    if(!this->fracture_map.contains(id))
    {
        this->AppendFracture(id);
    }
    return this->fracture_map[id];
}

void Block::RemoveFracture(const QString &id)
{
    this->fracture_map.remove(id);
}

QSharedPointer<Primitive> Block::GetBedplane(const QString &id)
{
    if(!this->bedplane_map.contains(id))
    {
        this->AppendBedplane(id);
    }
    return this->bedplane_map[id];
}

void Block::RemoveBedplane(const QString &id)
{
    this->bedplane_map.remove(id);
}

QSharedPointer<Primitive> Block::GetClipseed(const QString &id)
{
    if(!this->clipseed_map.contains(id))
    {
        this->AppendClipseed(id);
    }
    return this->clipseed_map[id];
}

void Block::RemoveClipseed(const QString &id)
{
    this->clipseed_map.remove(id);
}

Block::PrimitiveMap &Block::GetBoundaryMap()
{
    return this->boundary_map;
}

Block::PrimitiveMap &Block::GetFractureMap()
{
    return this->fracture_map;
}

Block::PrimitiveMap &Block::GetBedplaneMap()
{
    return this->bedplane_map;
}

Block::PrimitiveMap &Block::GetClipseedMap()
{
    return this->clipseed_map;
}

Block::Triangulation &Block::GetTriangulation()
{
    if(nullptr == this->triangulation)
    {
        this->triangulation = QSharedPointer<ConstrainedDelaunayTriangulation>(new ConstrainedDelaunayTriangulation);
    }
    return this->triangulation;
}

Block::VoronoiPtr &Block::GetVoronoiDiagram()
{
    if(nullptr == this->voronoiDiagram)
    {
        this->voronoiDiagram = QSharedPointer<VoronoiDiagram>(new VoronoiDiagram());
    }
    return this->voronoiDiagram;
}

void Block::AppendBoundary(const QString &id)
{
    QString shape = QString("Polygon");
    QColor color = QColor(Qt::lightGray);
    QSharedPointer<Primitive> boundary = QSharedPointer<Primitive>(new Primitive(shape, color));
    this->boundary_map.insert(id, boundary);
}

void Block::AppendFracture(const QString &id)
{
    QString shape = QString("Segment");
    QColor color = QColor(255,99,71);
    QSharedPointer<Primitive> fracture = QSharedPointer<Primitive>(new Primitive(shape, color));
    this->fracture_map.insert(id, fracture);
}

void Block::AppendBedplane(const QString &id)
{
    QString shape = QString("Segment");
    QColor color = QColor(0,255,255);
    QSharedPointer<Primitive> bedplane = QSharedPointer<Primitive>(new Primitive(shape, color));
    this->bedplane_map.insert(id, bedplane);
}

void Block::AppendClipseed(const QString &id)
{
    QString shape = QString("Point");
    QColor color = QColor(204,204,81);
    QSharedPointer<Primitive> clipseed = QSharedPointer<Primitive>(new Primitive(shape, color));
    this->clipseed_map.insert(id, clipseed);
}

double Block::GetMeshCharacteristicLength()
{
    return this->triangulation->GetTriangulationCharacteristicLength();
}

double Block::GetMeshMinimumSize()
{
    return this->triangulation->GetTriangulationMinimumSize();
}

double Block::GetMeshMinimumArea()
{
    return this->triangulation->GetTriangulationMinimumArea();
}
