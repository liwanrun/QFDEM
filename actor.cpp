#include "actor.h"
#include "primitiveactor.h"
#include "triangulationactor.h"

BlockActor::BlockActor()
{

}

BlockActor::~BlockActor()
{

}

QSharedPointer<PrimitiveActor> BlockActor::GetBoundaryActor(const QString &id)
{
    if(!this->boundary_map.contains(id))
    {
        this->AppendBoundaryActor(id);
    }
    return this->boundary_map[id];
}

void BlockActor::RemoveBoundaryActor(const QString &id)
{
    this->boundary_map.remove(id);
}

QSharedPointer<PrimitiveActor> BlockActor::GetFractureActor(const QString &id)
{
    if(!this->fracture_map.contains(id))
    {
        this->AppendFractureActor(id);
    }
    return this->fracture_map[id];
}

void BlockActor::RemoveFractureActor(const QString &id)
{
    this->fracture_map.remove(id);
}

QSharedPointer<PrimitiveActor> BlockActor::GetBedplaneActor(const QString &id)
{
    if(!this->bedplane_map.contains(id))
    {
        this->AppendBedplaneActor(id);
    }
    return this->bedplane_map[id];
}

void BlockActor::RemoveBedplaneActor(const QString &id)
{
    this->bedplane_map.remove(id);
}

QSharedPointer<PrimitiveActor> BlockActor::GetClipseedActor(const QString &id)
{
    if(!this->clipseed_map.contains(id))
    {
        this->AppendClipseedActor(id);
    }
    return this->clipseed_map[id];
}

void BlockActor::RemoveClipseedActor(const QString &id)
{
    this->clipseed_map.remove(id);
}

const BlockActor::PrimitiveActorMap &BlockActor::GetBoundaryMap() const
{
    return this->boundary_map;
}

const BlockActor::PrimitiveActorMap &BlockActor::GetFractureMap() const
{
    return this->fracture_map;
}

const BlockActor::PrimitiveActorMap &BlockActor::GetBedplaneMap() const
{
    return this->bedplane_map;
}

const BlockActor::PrimitiveActorMap &BlockActor::GetClipseedMap() const
{
    return this->clipseed_map;
}

QSharedPointer<TriangulationActor> &BlockActor::GetTriangulationActor()
{
    if(nullptr == this->triangle_mesh)
    {
        this->triangle_mesh = QSharedPointer<TriangulationActor>(new TriangulationActor);
    }
    return this->triangle_mesh;
}

QSharedPointer<VoronoiDiagramActor> &BlockActor::GetVoronoiDiagramActor()
{
    if(nullptr == this->voronoi_diagram)
    {
        this->voronoi_diagram = QSharedPointer<VoronoiDiagramActor>(new VoronoiDiagramActor);
    }
    return this->voronoi_diagram;
}

void BlockActor::AppendBoundaryActor(const QString &id)
{
    QSharedPointer<PrimitiveActor> boundary = QSharedPointer<PrimitiveActor>(new PrimitiveActor);
    this->boundary_map.insert(id, boundary);
}

void BlockActor::AppendFractureActor(const QString &id)
{
    QSharedPointer<PrimitiveActor> fracture = QSharedPointer<PrimitiveActor>(new PrimitiveActor);
    this->fracture_map.insert(id, fracture);
}

void BlockActor::AppendBedplaneActor(const QString &id)
{
    QSharedPointer<PrimitiveActor> bedplane = QSharedPointer<PrimitiveActor>(new PrimitiveActor);
    this->bedplane_map.insert(id, bedplane);
}

void BlockActor::AppendClipseedActor(const QString &id)
{
    QSharedPointer<PrimitiveActor> clipseed = QSharedPointer<PrimitiveActor>(new PrimitiveActor);
    this->clipseed_map.insert(id, clipseed);
}

void BlockActor::SetTriangulationActorVisibility(bool arg)
{
    this->triangle_mesh->SetVisibility(arg);
}

void BlockActor::SetTriangulationNodeIdsVisibility(bool arg)
{
    this->triangle_mesh->SetNodeIdsVisibility(arg);
}

void BlockActor::SetTriangulationCellIdsVisibility(bool arg)
{
    this->triangle_mesh->SetCellIdsVisibility(arg);
}
