#ifndef BLOCK_H
#define BLOCK_H

#include "blockgeometry.h"
#include "triangulation.h"
#include "voronoidiagram.h"

#include <QMap>
#include <QString>
#include <QSharedPointer>

class Block
{
public:
    typedef QMap<QString, QSharedPointer<Primitive>> PrimitiveMap;
    typedef QSharedPointer<Triangulation> Triangulation;
    typedef QSharedPointer<VoronoiDiagram> VoronoiPtr;

public:
    Block();
    ~Block();

public:
    QSharedPointer<Primitive> GetBoundary(const QString& id);
    void RemoveBoundary(const QString& id);

    QSharedPointer<Primitive> GetFracture(const QString& id);
    void RemoveFracture(const QString& id);

    QSharedPointer<Primitive> GetBedplane(const QString& id);
    void RemoveBedplane(const QString& id);

    QSharedPointer<Primitive> GetClipseed(const QString& id);
    void RemoveClipseed(const QString& id);

    PrimitiveMap& GetBoundaryMap();
    PrimitiveMap& GetFractureMap();
    PrimitiveMap& GetBedplaneMap();
    PrimitiveMap& GetClipseedMap();

private:
    void AppendBoundary(const QString& id);
    void AppendFracture(const QString& id);
    void AppendBedplane(const QString& id);
    void AppendClipseed(const QString& id);

public:
    double GetMeshCharacteristicLength();
    double GetMeshMinimumSize();
    double GetMeshMinimumArea();

    Triangulation &GetTriangulation();
    VoronoiPtr &GetVoronoiDiagram();

private:
    PrimitiveMap boundary_map;
    PrimitiveMap fracture_map;
    PrimitiveMap bedplane_map;
    PrimitiveMap clipseed_map;

    Triangulation triangulation;
    VoronoiPtr voronoiDiagram;
};

#endif // BLOCK_H
