#ifndef ACTORREPOSITORY_H
#define ACTORREPOSITORY_H

#include <QMap>
#include <QDebug>
#include <QMutex>
#include <QSharedPointer>
#include <vtkType.h>

class BlockActor;
class PrimitiveActor;
class TriangulationActor;
class VoronoiDiagramActor;
class vtkScalarsToColors;
class vtkMultiBlockDataSet;

class Material;
class Boundary;
class Temporal;
class ActorCollection
{
public:
    typedef QMap<QString, QSharedPointer<BlockActor>> BlockActorMap;
    typedef QMap<QString, QSharedPointer<PrimitiveActor>> PrimitiveActorMap;

public:
    friend class FDEMSolutionWidget;
    friend class FDEMDocumentExporter;

public:
    static QSharedPointer<ActorCollection> GetActorCollection();
    ~ActorCollection();

    const BlockActorMap& GetBlockActorMap() const;
    vtkMultiBlockDataSet *GetBlockActorDataSet();

    QSharedPointer<BlockActor> GetBlockActor(const QString& id);
    void AppendBlockActor(const QString& id);
    void RemoveBlockActor(const QString& id);
    void Clear();

    QSharedPointer<PrimitiveActor> GetBlockBoundaryActor(const QString& blockId, const QString& id);
    void RemoveBlockBoundaryActor(const QString& blockId, const QString& id);

    QSharedPointer<PrimitiveActor> GetBlockFractureActor(const QString& blockId, const QString& id);
    void RemoveBlockFractureActor(const QString& blockId, const QString& id);

    QSharedPointer<PrimitiveActor> GetBlockBedplaneActor(const QString& blockId, const QString& id);
    void RemoveBlockBedplaneActor(const QString& blockId, const QString& id);

    QSharedPointer<PrimitiveActor> GetBlockClipseedActor(const QString& blockId, const QString& id);
    void RemoveBlockClipseedActor(const QString& blockId, const QString& id);

    const PrimitiveActorMap& GetBlockBoundaryMap(const QString& blockId) const;
    const PrimitiveActorMap& GetBlockFractureMap(const QString& blockId) const;
    const PrimitiveActorMap& GetBlockBedplaneMap(const QString& blockId) const;
    const PrimitiveActorMap& GetBlockClipseedMap(const QString& blockId) const;

public:
    QSharedPointer<TriangulationActor>& GetBlockTriangulationActor(const QString& blockId);
    QSharedPointer<VoronoiDiagramActor>& GetBlockVoronoiDiagramActor(const QString& blockId);

public:
    void SetTriangulationVisibility(bool arg);

    void SetNodeIdsVisibility(bool arg);

    void SetCellIdsVisibility(bool arg);

    void SetTriangulationRepresentation(int index);

    void SelectTriangulationColorArray(const QString& name);

    void SetTriangulationLookupTable(vtkScalarsToColors* lut);

    double* GetTriangulationScalarRange(const QString& name, int comp);

public:
    //universal
    void ApplyTriangulationGlobalIds();

    void ApplyTriangulationPedigreeId();

    void ApplyTriangulationCellTypes();

    //material
    void ApplyTriangulationMaterialIndexArray(const QString& id);

    void ApplyTriangulationMaterialFieldArray(const QString& id);

    void ApplyTriangulationMaterialFieldArray(const QString& id, const char *name, Material *material);

    void CancelTriangulationMaterialId(const QString& id);

    void ClearTriangulationMaterialId();

    //boundary
    void ApplyTriangulationBoundaryIndexArray(const QString& id);

    void ApplyTriangulationBoundaryFieldArray(const QString& id);

    void ApplyTriangulationBoundaryFieldArray(const char *name, Boundary *boundary);

    void CancelTriangulationBoundaryFieldArray(const QString &id);

    void ClearTriangulationBoundaryFieldArray();

    //temporal
    void ApplyTriangulationTemporalIndexArray(const QString& id);

    void ApplyTriangulationTemporalFieldArray(const QString& id);

    void ApplyTriangulationTemporalFieldArray(const char *name, Temporal *temporal);

    void CancelTriangulationTemporalFieldArray(const QString& id);

    void ClearTriangulationTemporalFieldArray();

    //history
    std::vector<size_t> GetHistoryIds();

    //transform
    void TranslateTriangulationGeometry(double *comp);

    void RotateTriangulationGeometry(double *comp);

    void ScaleTriangulationGeometry(double *comp);

public:
    vtkIdType GetNumberOfNodes();

    vtkIdType GetNumberOfCells();

    vtkIdType GetNumberOfTriangles();

    vtkIdType GetNumberOfQuads();

private:
    ActorCollection();

private:
    static QSharedPointer<ActorCollection> actorCollection;

    static QMutex mutex;

    BlockActorMap block_map;

    vtkIdType numNodes;
    vtkIdType numCells;
    vtkIdType numTriangles;
    vtkIdType numQuads;
};

#endif // ACTORREPOSITORY_H
