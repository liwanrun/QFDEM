#include "actorcollection.h"
#include "actor.h"
#include "blockcollection.h"
#include "fdemshapesource.h"
#include "fdemmeshsourcefactory.h"
#include "selectcollection.h"
#include "materialcollection.h"
#include "material.h"
#include "boundarycollection.h"
#include "boundary.h"
#include "temporalcollection.h"
#include "temporal.h"

#include <vtkPolyDataAlgorithm.h>
#include <vtkProperty.h>
#include <vtkPolyLineSource.h>
#include <vtkInformation.h>
#include <vtkCompositeDataIterator.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkTupleInterpolator.h>

#include <QSet>
#include <QVector2D>

QSharedPointer<ActorCollection> ActorCollection::actorCollection = nullptr;

QMutex ActorCollection::mutex;

QSharedPointer<ActorCollection> ActorCollection::GetActorCollection()
{
    if(nullptr == actorCollection)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == actorCollection)
        {
            actorCollection = QSharedPointer<ActorCollection>(new ActorCollection);
        }
    }

    return actorCollection;
}

ActorCollection::~ActorCollection()
{
    qDebug() << "~ActorCollection()";

    this->numNodes = 0;
    this->numCells = 0;
    this->numTriangles = 0;
    this->numQuads = 0;
}

const ActorCollection::BlockActorMap &ActorCollection::GetBlockActorMap() const
{
    return this->block_map;
}

vtkMultiBlockDataSet *ActorCollection::GetBlockActorDataSet()
{
    vtkMultiBlockDataSet *Input = vtkMultiBlockDataSet::New();
    unsigned int index = 0;
    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        TriangulationActor *actor = iter.value()->GetTriangulationActor().data();
        if(actor->GetVTKRenderActor()->GetVisibility())
        {
            vtkPolyData* pd = actor->GetVTKActorMapper()->GetInput();
            Input->SetBlock(index, pd);
            Input->GetMetaData(index)->Set(vtkCompositeDataSet::NAME(), iter.key().toStdString());
            index += 1;
        }
    }

    return Input;
}

void ActorCollection::AppendBlockActor(const QString &id)
{
    QSharedPointer<BlockActor> block = QSharedPointer<BlockActor>(new BlockActor);
    this->block_map.insert(id, block);
}

void ActorCollection::RemoveBlockActor(const QString &id)
{
    this->block_map.remove(id);
}

QSharedPointer<BlockActor> ActorCollection::GetBlockActor(const QString &id)
{
    if(!this->block_map.contains(id))
    {
        this->AppendBlockActor(id);
    }
    return this->block_map[id];
}

void ActorCollection::RemoveBlockBoundaryActor(const QString &blockId, const QString &id)
{
    this->block_map[blockId]->RemoveBoundaryActor(id);
}

QSharedPointer<PrimitiveActor> ActorCollection::GetBlockBoundaryActor(const QString &blockId, const QString &id)
{
    return this->GetBlockActor(blockId)->GetBoundaryActor(id);
}

void ActorCollection::RemoveBlockFractureActor(const QString &blockId, const QString &id)
{
    this->block_map[blockId]->RemoveFractureActor(id);
}

QSharedPointer<PrimitiveActor> ActorCollection::GetBlockFractureActor(const QString &blockId, const QString &id)
{
    return this->GetBlockActor(blockId)->GetFractureActor(id);
}

void ActorCollection::RemoveBlockBedplaneActor(const QString &blockId, const QString &id)
{
    this->block_map[blockId]->RemoveBedplaneActor(id);
}

QSharedPointer<PrimitiveActor> ActorCollection::GetBlockClipseedActor(const QString &blockId, const QString &id)
{
    return this->GetBlockActor(blockId)->GetClipseedActor(id);
}

void ActorCollection::RemoveBlockClipseedActor(const QString &blockId, const QString &id)
{
    this->block_map[blockId]->RemoveClipseedActor(id);
}

QSharedPointer<PrimitiveActor> ActorCollection::GetBlockBedplaneActor(const QString &blockId, const QString &id)
{
    return this->GetBlockActor(blockId)->GetBedplaneActor(id);
}

const ActorCollection::PrimitiveActorMap &ActorCollection::GetBlockBoundaryMap(const QString &blockId) const
{
    return this->block_map[blockId]->GetBoundaryMap();
}

const ActorCollection::PrimitiveActorMap &ActorCollection::GetBlockFractureMap(const QString &blockId) const
{
    return this->block_map[blockId]->GetFractureMap();
}

const ActorCollection::PrimitiveActorMap &ActorCollection::GetBlockBedplaneMap(const QString &blockId) const
{
    return this->block_map[blockId]->GetBedplaneMap();
}

const ActorCollection::PrimitiveActorMap &ActorCollection::GetBlockClipseedMap(const QString &blockId) const
{
    return this->block_map[blockId]->GetClipseedMap();
}

QSharedPointer<TriangulationActor> &ActorCollection::GetBlockTriangulationActor(const QString &blockId)
{
    return this->GetBlockActor(blockId)->GetTriangulationActor();
}

QSharedPointer<VoronoiDiagramActor> &ActorCollection::GetBlockVoronoiDiagramActor(const QString &blockId)
{
    return this->GetBlockActor(blockId)->GetVoronoiDiagramActor();
}

void ActorCollection::Clear()
{
    this->block_map.clear();
}

void ActorCollection::SetTriangulationVisibility(bool arg)
{
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<TriangulationActor> t = iter.value()->GetTriangulationActor();
        t->SetVisibility(arg);
    }
}

void ActorCollection::SetNodeIdsVisibility(bool arg)
{
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<TriangulationActor> mesh = iter.value()->GetTriangulationActor();
        mesh->SetNodeIdsVisibility(arg);
    }
}

void ActorCollection::SetCellIdsVisibility(bool arg)
{
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<TriangulationActor> mesh = iter.value()->GetTriangulationActor();
        mesh->SetCellIdsVisibility(arg);
    }
}

void ActorCollection::SetTriangulationRepresentation(int index)
{
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<TriangulationActor> mesh = iter.value()->GetTriangulationActor();
        mesh->SetRepresentation(index);
    }
}

void ActorCollection::SelectTriangulationColorArray(const QString &name)
{
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<TriangulationActor> mesh = iter.value()->GetTriangulationActor();
        mesh->SelectColorArray(name);
    }
}

double* ActorCollection::GetTriangulationScalarRange(const QString &text, int comp)
{
    vtkDoubleArray* limit = vtkDoubleArray::New();
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<TriangulationActor> t = iter.value()->GetTriangulationActor();
        double *range = t->GetScalarRange(text, comp);
        limit->InsertNextValue(range[0]);
        limit->InsertNextValue(range[1]);
    }
    return limit->GetRange();
}

void ActorCollection::SetTriangulationLookupTable(vtkScalarsToColors *lut)
{
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<TriangulationActor> t = iter.value()->GetTriangulationActor();
        t->SetLookupTable(lut);
    }
}

void ActorCollection::ApplyTriangulationGlobalIds()
{
    vtkIdType numNodeOffset = 0;
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->AppendNodeGlobalIds("Point Global Ids", numNodeOffset);
    }
    this->numNodes = numNodeOffset;

    vtkIdType numCellOffset = 0;
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->AppendCellGlobalIds("Cell Global Ids", numCellOffset);
    }
    this->numCells = numCellOffset;

//    qDebug() << "Total number of nodes: " << numNodeOffset;
//    qDebug() << "Total number of cells: " << numCellOffset;
}

void ActorCollection::ApplyTriangulationPedigreeId()
{
    //Point Pedigree Ids
    vtkIdType numNodeOffset = 0;
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->AppendNodePedigreeIds("Point Pedigree Ids", numNodeOffset);
    }
    this->numNodes = numNodeOffset;

    //Cell Pedigree Ids
    vtkIdType numCellOffset = 0;
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->AppendCellPedigreeIds("Cell Pedigree Ids", numCellOffset);
    }
    this->numCells = numCellOffset;

    //1.triangle first
    vtkIdType numTriangleOffset = 0;
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();

        vtkPolyData *pd = t->GetVTKActorMapper()->GetInput();
        vtkAbstractArray *cellPedigreeIds = pd->GetCellData()->GetPedigreeIds();
        for(vtkIdType id = 0; id < pd->GetNumberOfCells(); id++)
        {
            if(VTK_TRIANGLE == pd->GetCellType(id))
            {
                cellPedigreeIds->SetVariantValue(id, numTriangleOffset);
                numTriangleOffset += 1;
            }
        }
    }
    this->numTriangles = numTriangleOffset;

    //Quad second
    vtkIdType numQuadOffset = 0;
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();

        vtkPolyData *pd = t->GetVTKActorMapper()->GetInput();
        vtkAbstractArray *cellPedigreeIds = pd->GetCellData()->GetPedigreeIds();
        for(vtkIdType id = 0; id < pd->GetNumberOfCells(); id++)
        {
            if(VTK_QUAD == pd->GetCellType(id))
            {
                cellPedigreeIds->SetVariantValue(id, numQuadOffset + numTriangleOffset);
                numQuadOffset += 1;
            }
        }
    }
    this->numQuads = numQuadOffset;

//    qDebug() << "Number of triangles: " << numTriangleOffset;
//    qDebug() << "Number of quads: " << numQuadOffset;
}

void ActorCollection::ApplyTriangulationCellTypes()
{
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->AppendCellTypeArray("Cell Types");
    }
}

void ActorCollection::ApplyTriangulationMaterialIndexArray(const QString &id)
{
    QSharedPointer<MaterialCollection> materialCollection = MaterialCollection::GetMaterialCollection();
    QSharedPointer<Material> matreial = materialCollection->GetMaterial(id);

    const char *name = Material::GetIndexArrayName();
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->AppendCellDataArray(name, 1, -1.0);
    }

    QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
    vtkCompositeDataIterator *iter = selectCollection->Output->NewIterator();
    for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
        QString blockId = QString::fromStdString(selectCollection->Output->GetMetaData(iter)->Get(vtkCompositeDataSet::NAME()));
        QSharedPointer<TriangulationActor> t = this->block_map[blockId]->GetTriangulationActor();

        if(vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(iter->GetCurrentDataObject()))
        {
            t->ApplyCellMaterialIndexArray(id, output, name, matreial.get());
        }
    }
}

void ActorCollection::ApplyTriangulationMaterialFieldArray(const QString &id)
{
    QSharedPointer<MaterialCollection> materialCollection = MaterialCollection::GetMaterialCollection();
    QSharedPointer<Material> material = materialCollection->GetMaterial(id);

    int comp = 1;
    const char *name = Material::GetYoungModulusArrayName();

    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->AppendCellDataArray(name, comp, 0.0);
    }

    this->ApplyTriangulationMaterialFieldArray(id, name, material.get());
}

void ActorCollection::ApplyTriangulationMaterialFieldArray(const QString &id, const char *name, Material *material)
{
    QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
    vtkCompositeDataIterator *iter = selectCollection->Output->NewIterator();
    for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
        QString blockId = QString::fromStdString(selectCollection->Output->GetMetaData(iter)->Get(vtkCompositeDataSet::NAME()));
        QSharedPointer<TriangulationActor> t = this->block_map[blockId]->GetTriangulationActor();

        if(vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(iter->GetCurrentDataObject()))
        {
            t->ApplyCellMaterialFieldArray(id, output, name, material);
        }
    }
}

void ActorCollection::CancelTriangulationMaterialId(const QString &id)
{
    const char *name = Material::GetIndexArrayName();
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->CancelCellMaterialIds(id, name);
    }
}

void ActorCollection::ClearTriangulationMaterialId()
{
    const char *name = Material::GetIndexArrayName();
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->RemoveCellDataArray(name);
    }
}

void ActorCollection::ApplyTriangulationBoundaryIndexArray(const QString &id)
{
    QSharedPointer<BoundaryCollection> boundaryCollection = BoundaryCollection::GetBoundaryCollection();
    const char* name = boundaryCollection->GetBoundary(id)->GetIndexArrayName();

    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->AppendPointDataArray(name, 1, -1.0);
    }

    QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
    vtkCompositeDataIterator *iter = selectCollection->Output->NewIterator();
    for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
        QString blockId = QString::fromStdString(selectCollection->Output->GetMetaData(iter)->Get(vtkCompositeDataSet::NAME()));
        QSharedPointer<TriangulationActor> t = this->block_map[blockId]->GetTriangulationActor();

        if(vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(iter->GetCurrentDataObject()))
        {
            t->ApplyPointBoundaryIndexArray(id, output, name);
        }
    }
}

void ActorCollection::ApplyTriangulationBoundaryFieldArray(const QString &id)
{
    QSharedPointer<BoundaryCollection> boundaryCollection = BoundaryCollection::GetBoundaryCollection();
    QSharedPointer<Boundary> boundary = boundaryCollection->GetBoundary(id);

    int comp = boundary->GetFieldArrayComponents();
    const char *name = boundary->GetFieldArrayName();

    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->AppendPointDataArray(name, comp);
    }

    this->ApplyTriangulationBoundaryFieldArray(name, boundary.get());
}

void ActorCollection::ApplyTriangulationBoundaryFieldArray(const char *name, Boundary *boundary)
{
    QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
    vtkCompositeDataIterator *iter = selectCollection->Output->NewIterator();
    for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
        QString blockId = QString::fromStdString(selectCollection->Output->GetMetaData(iter)->Get(vtkCompositeDataSet::NAME()));
        QSharedPointer<TriangulationActor> t = this->block_map[blockId]->GetTriangulationActor();

        if(vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(iter->GetCurrentDataObject()))
        {
            t->ApplyPointBoundaryFieldArray(name, output, boundary);
        }
    }
}

void ActorCollection::CancelTriangulationBoundaryFieldArray(const QString& id)
{
    QSharedPointer<BoundaryCollection> boundaryCollection = BoundaryCollection::GetBoundaryCollection();
    QSharedPointer<Boundary> boundary = boundaryCollection->GetBoundary(id);
    const char* name = boundary->GetFieldArrayName();

    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->CancelPointBoundaryFieldArray(id, name, boundary.get());
    }
}

void ActorCollection::ClearTriangulationBoundaryFieldArray()
{
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();

        t->RemovePointDataArray("Point Boundary Ids (M)");
        t->RemovePointDataArray("Point Boundary Traction (M)");
        t->RemovePointDataArray("Point Boundary Force (M)");
        t->RemovePointDataArray("Point Boundary Acceleration (M)");
        t->RemovePointDataArray("Point Boundary Velocity (M)");

        t->RemovePointDataArray("Point Boundary Ids (H)");
        t->RemovePointDataArray("Point Boundary Pressure (H)");
        t->RemovePointDataArray("Point Boundary FlowRate (H)");

        t->RemovePointDataArray("Point Boundary Ids (T)");
        t->RemovePointDataArray("Point Boundary Temperature (T)");
        t->RemovePointDataArray("Point Boundary HeatFlux (T)");
    }
}

void ActorCollection::ApplyTriangulationTemporalIndexArray(const QString &id)
{
    QSharedPointer<TemporalCollection> temporalCollection = TemporalCollection::GetTemporalCollection();
    const char* name = temporalCollection->GetTemporal(id)->GetIndexArrayName();

    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->AppendPointDataArray(name, 1, -1.0);
    }

    QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
    vtkCompositeDataIterator *iter = selectCollection->Output->NewIterator();
    for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
        QString blockId = QString::fromStdString(selectCollection->Output->GetMetaData(iter)->Get(vtkCompositeDataSet::NAME()));
        QSharedPointer<TriangulationActor> t = this->block_map[blockId]->GetTriangulationActor();

        if(vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(iter->GetCurrentDataObject()))
        {
            t->ApplyPointTemporalIndexArray(id, name, output);
        }
    }
}

void ActorCollection::ApplyTriangulationTemporalFieldArray(const QString &id)
{
    QSharedPointer<TemporalCollection> temporalCollection = TemporalCollection::GetTemporalCollection();
    QSharedPointer<Temporal> temporal = temporalCollection->GetTemporal(id);

    int comp = temporal->GetFieldArrayComponents();
    const char* name = temporal->GetFieldArrayName();

    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->AppendPointDataArray(name, comp);
    }

    this->ApplyTriangulationTemporalFieldArray(name, temporal.get());
}

void ActorCollection::ApplyTriangulationTemporalFieldArray(const char *name, Temporal *temporal)
{
    QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
    vtkCompositeDataIterator *iter = selectCollection->Output->NewIterator();
    for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
        QString blockId = QString::fromStdString(selectCollection->Output->GetMetaData(iter)->Get(vtkCompositeDataSet::NAME()));
        QSharedPointer<TriangulationActor> t = this->block_map[blockId]->GetTriangulationActor();

        if(vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(iter->GetCurrentDataObject()))
        {
            t->ApplyPointTemporalFieldArray(name, output, temporal);
        }
    }
}

void ActorCollection::CancelTriangulationTemporalFieldArray(const QString &id)
{
    QSharedPointer<TemporalCollection> temporalCollection = TemporalCollection::GetTemporalCollection();
    QSharedPointer<Temporal> temporal = temporalCollection->GetTemporal(id);
    const char* name = temporal->GetFieldArrayName();

    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<BlockActor> blockActor = iter.value();
        QSharedPointer<TriangulationActor> t = blockActor->GetTriangulationActor();
        t->CancelPointTemporalFieldArray(id, name, temporal.get());
    }
}

void ActorCollection::ClearTriangulationTemporalFieldArray()
{
    for(auto iter = this->block_map.begin(); iter != this->block_map.end(); ++iter)
    {
        QSharedPointer<TriangulationActor> t = (iter.value())->GetTriangulationActor();

        t->RemovePointDataArray("Point Initial Ids (M)");
        t->RemovePointDataArray("Point Initial Velocity (M)");
        t->RemovePointDataArray("Point Initial Ids (H)");
        t->RemovePointDataArray("Point Initial Pressure (H)");
        t->RemovePointDataArray("Point Initial Ids (T)");
        t->RemovePointDataArray("Point Initial Temperature (T)");
    }
}

std::vector<size_t> ActorCollection::GetHistoryIds()
{
    return std::vector<size_t>{0};
}

void ActorCollection::TranslateTriangulationGeometry(double *comp)
{
    QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
    vtkCompositeDataIterator *iter = selectCollection->Output->NewIterator();
    for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
        QString blockId = QString::fromStdString(selectCollection->Output->GetMetaData(iter)->Get(vtkCompositeDataSet::NAME()));
        QSharedPointer<TriangulationActor> t = this->block_map[blockId]->GetTriangulationActor();

        if(vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(iter->GetCurrentDataObject()))
        {
            t->TranslateTriangulationGeometry(output, comp);
        }
    }
}

void ActorCollection::RotateTriangulationGeometry(double *comp)
{
    QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
    vtkCompositeDataIterator *iter = selectCollection->Output->NewIterator();
    for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
        QString blockId = QString::fromStdString(selectCollection->Output->GetMetaData(iter)->Get(vtkCompositeDataSet::NAME()));
        QSharedPointer<TriangulationActor> t = this->block_map[blockId]->GetTriangulationActor();

        if(vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(iter->GetCurrentDataObject()))
        {
            t->RotateTriangulationGeometry(output, comp);
        }
    }
}

void ActorCollection::ScaleTriangulationGeometry(double *comp)
{
    QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
    vtkCompositeDataIterator *iter = selectCollection->Output->NewIterator();
    for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
        QString blockId = QString::fromStdString(selectCollection->Output->GetMetaData(iter)->Get(vtkCompositeDataSet::NAME()));
        QSharedPointer<TriangulationActor> t = this->block_map[blockId]->GetTriangulationActor();

        if(vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(iter->GetCurrentDataObject()))
        {
            t->ScaleTriangulationGeometry(output, comp);
        }
    }
}

vtkIdType ActorCollection::GetNumberOfNodes()
{
    return this->numNodes;
}

vtkIdType ActorCollection::GetNumberOfCells()
{
    return this->numCells;
}

vtkIdType ActorCollection::GetNumberOfTriangles()
{
    return this->numTriangles;
}

vtkIdType ActorCollection::GetNumberOfQuads()
{
    return this->numQuads;
}

ActorCollection::ActorCollection()
{
    qDebug() << "ActorCollection()";
}
