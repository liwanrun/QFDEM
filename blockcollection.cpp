#include "block.h"
#include "triangulation.h"
#include "voronoidiagram.h"
#include "blockcollection.h"
#include "selectcollection.h"
#include "constraineddelaunaytriangulation.h"

#include <vtkInformation.h>
#include <vtkCompositeDataIterator.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkUnstructuredGrid.h>

#include <QDebug>

QSharedPointer<BlockCollection> BlockCollection::blockCollection = nullptr;

QMutex BlockCollection::mutex;

QSharedPointer<BlockCollection> BlockCollection::GetBlockCollection()
{
    if(nullptr == blockCollection)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == blockCollection)
        {
            blockCollection = QSharedPointer<BlockCollection>(new BlockCollection);
        }
    }

    return blockCollection;
}

void BlockCollection::AppendBlock(const QString &id)
{
    QSharedPointer<Block> block = QSharedPointer<Block>(new Block);
    this->block_map.insert(id, block);
}

void BlockCollection::RemoveBlock(const QString &id)
{
    this->block_map.remove(id);
}

const BlockCollection::BlockMap &BlockCollection::GetBlockMap() const
{
    return this->block_map;
}

QSharedPointer<Block> BlockCollection::GetBlock(const QString &id)
{
    if(!this->block_map.contains(id))
    {
        this->AppendBlock(id);
    }
    return this->block_map[id];
}

void BlockCollection::RemoveBlockBoundary(const QString &blockId, const QString &id)
{
    this->block_map[blockId]->RemoveBoundary(id);
}

QSharedPointer<Primitive> BlockCollection::GetBlockBoundary(const QString &blockId, const QString &id)
{
    return this->GetBlock(blockId)->GetBoundary(id);
}

void BlockCollection::RemoveBlockFracture(const QString &blockId, const QString &id)
{
    this->block_map[blockId]->RemoveFracture(id);
}

QSharedPointer<Primitive> BlockCollection::GetBlockFracture(const QString &blockId, const QString &id)
{
    return this->GetBlock(blockId)->GetFracture(id);
}

QSharedPointer<Primitive> BlockCollection::GetBlockBedplane(const QString &blockId, const QString &id)
{
    return this->GetBlock(blockId)->GetBedplane(id);
}

void BlockCollection::RemoveBlockBedplane(const QString &blockId, const QString &id)
{
    this->block_map[blockId]->RemoveFracture(id);
}

QSharedPointer<Primitive> BlockCollection::GetBlockClipseed(const QString &blockId, const QString &id)
{
    return this->GetBlock(blockId)->GetClipseed(id);
}

void BlockCollection::RemoveBlockClipseed(const QString &blockId, const QString &id)
{
    this->block_map[blockId]->RemoveClipseed(id);
}

BlockCollection::PrimitiveMap &BlockCollection::GetBlockBoundaryMap(const QString &blockId) const
{
    return this->block_map[blockId]->GetBoundaryMap();
}

BlockCollection::PrimitiveMap &BlockCollection::GetBlockFractureMap(const QString& blockId) const
{
    return this->block_map[blockId]->GetFractureMap();
}

BlockCollection::PrimitiveMap &BlockCollection::GetBlockBedplaneMap(const QString &blockId) const
{
    return this->block_map[blockId]->GetBedplaneMap();
}

BlockCollection::PrimitiveMap &BlockCollection::GetBlockClipseedMap(const QString &blockId) const
{
    return this->block_map[blockId]->GetClipseedMap();
}

BlockCollection::TriangulationPtr &BlockCollection::GetBlockTriangulation(const QString &blockId)
{
    return this->GetBlock(blockId)->GetTriangulation();
}

BlockCollection::VoronoiDiagramPtr &BlockCollection::GetBlockVoronoiDiagram(const QString &blockId)
{
    return this->GetBlock(blockId)->GetVoronoiDiagram();
}

void BlockCollection::UpdateTriangulationConstraint()
{
    QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
    vtkCompositeDataIterator *iter = selectCollection->Output->NewIterator();
    for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
        QString blockId = QString::fromStdString(selectCollection->Output->GetMetaData(iter)->Get(vtkCompositeDataSet::NAME()));
        if(vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(iter->GetCurrentDataObject()))
        {
            QSharedPointer<Triangulation> t = this->block_map[blockId]->GetTriangulation();
            t->UpdateTriangulationConstraint(output);
        }
    }
}

size_t BlockCollection::getBlockGrainSize() const
{
    return this->numBlockGrains;
}

void BlockCollection::setBlockGrainSize(const size_t &size)
{
    this->numBlockGrains = size;
}

void BlockCollection::Clear()
{
    this->block_map.clear();
}
