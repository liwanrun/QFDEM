#ifndef GEOMETRYDATABASE_H
#define GEOMETRYDATABASE_H

#include <QMap>
#include <QDebug>
#include <QMutex>                   //thread-safe
#include <QSharedPointer>           //memory-safe

class Block;
class Primitive;
class Triangulation;
class VoronoiDiagram;
class BlockCollection
{
public:
    typedef QMap<QString, QSharedPointer<Block>> BlockMap;
    typedef QMap<QString, QSharedPointer<Primitive>> PrimitiveMap;
    typedef QSharedPointer<Triangulation> TriangulationPtr;
    typedef QSharedPointer<VoronoiDiagram> VoronoiDiagramPtr;

public:
    friend class FDEMDocumentExporter;

//3. Create/Get static single instance.
public:
    static QSharedPointer<BlockCollection> GetBlockCollection();
    ~BlockCollection() {qDebug() << "~BlockCollection()";}

    const BlockMap& GetBlockMap() const;

    QSharedPointer<Block> GetBlock(const QString& id);
    void AppendBlock(const QString& id);
    void RemoveBlock(const QString& id);
    void Clear();

    QSharedPointer<Primitive> GetBlockBoundary(const QString& blockId, const QString& id);
    void RemoveBlockBoundary(const QString& blockId, const QString& id);

    QSharedPointer<Primitive> GetBlockFracture(const QString& blockId, const QString& id);
    void RemoveBlockFracture(const QString& blockId, const QString& id);

    QSharedPointer<Primitive> GetBlockBedplane(const QString& blockId, const QString& id);
    void RemoveBlockBedplane(const QString& blockId, const QString& id);

    QSharedPointer<Primitive> GetBlockClipseed(const QString& blockId, const QString& id);
    void RemoveBlockClipseed(const QString& blockId, const QString& id);

    PrimitiveMap& GetBlockBoundaryMap(const QString& blockId) const;
    PrimitiveMap& GetBlockFractureMap(const QString& blockId) const;
    PrimitiveMap& GetBlockBedplaneMap(const QString& blockId) const;
    PrimitiveMap& GetBlockClipseedMap(const QString& blockId) const;

public:
    TriangulationPtr& GetBlockTriangulation(const QString& blockId);
    VoronoiDiagramPtr& GetBlockVoronoiDiagram(const QString& blockId);

    size_t getBlockGrainSize() const;
    void setBlockGrainSize(const size_t &size);

public:
    void UpdateTriangulationConstraint();

public:
    friend class FDEMSolutionWidget;
    friend class FDEMGeometryWidget;
    friend class FDEMContactWidget;
    friend class FDEMMeshWidget;
    friend class FDEMMaterialWidget;

//1. Make the constructor private.
private:
    BlockCollection() : numBlockGrains(0), minimumMeshSize(0.0), minimumMeshArea(0.0)
    { qDebug() << "BlockCollection()"; }

//2. Declare static single instance.
private:
    static QSharedPointer<BlockCollection> blockCollection;

    static QMutex mutex;

    BlockMap block_map;

    size_t numBlockGrains;
    double minimumMeshSize;
    double minimumMeshArea;

};


#endif // GEOMETRYDATABASE_H
