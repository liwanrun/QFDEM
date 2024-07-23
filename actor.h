#ifndef BLOCKACTOR_H
#define BLOCKACTOR_H

#include "primitiveactor.h"
#include "triangulationactor.h"
#include "voronoidiagramactor.h"

#include <QMap>
#include <QString>
#include <QSharedPointer>

class BlockActor
{
public:
    typedef QMap<QString, QSharedPointer<PrimitiveActor>> PrimitiveActorMap;

public:
    BlockActor();
    ~BlockActor();

    QSharedPointer<PrimitiveActor> GetBoundaryActor(const QString& id);
    void RemoveBoundaryActor(const QString& id);

    QSharedPointer<PrimitiveActor> GetFractureActor(const QString& id);
    void RemoveFractureActor(const QString& id);

    QSharedPointer<PrimitiveActor> GetBedplaneActor(const QString& id);
    void RemoveBedplaneActor(const QString& id);

    QSharedPointer<PrimitiveActor> GetClipseedActor(const QString& id);
    void RemoveClipseedActor(const QString& id);

    const PrimitiveActorMap& GetBoundaryMap() const;
    const PrimitiveActorMap& GetFractureMap() const;
    const PrimitiveActorMap& GetBedplaneMap() const;
    const PrimitiveActorMap& GetClipseedMap() const;

public:
    QSharedPointer<TriangulationActor>& GetTriangulationActor();
    QSharedPointer<VoronoiDiagramActor>& GetVoronoiDiagramActor();

private:
    void AppendBoundaryActor(const QString& id);
    void AppendFractureActor(const QString& id);
    void AppendBedplaneActor(const QString& id);
    void AppendClipseedActor(const QString& id);

private:
    void SetTriangulationActorVisibility(bool arg);

    void SetTriangulationNodeIdsVisibility(bool arg);

    void SetTriangulationCellIdsVisibility(bool arg);

private:
    PrimitiveActorMap boundary_map;
    PrimitiveActorMap fracture_map;
    PrimitiveActorMap bedplane_map;
    PrimitiveActorMap clipseed_map;

    QSharedPointer<TriangulationActor> triangle_mesh;
    QSharedPointer<VoronoiDiagramActor> voronoi_diagram;
};

#endif // BLOCKACTOR_H
