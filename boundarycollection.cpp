#include "boundarycollection.h"
#include "boundary.h"

#include <QDebug>

QMutex BoundaryCollection::mutex;

QSharedPointer<BoundaryCollection> BoundaryCollection::boundaryCollection = nullptr;

BoundaryCollection::~BoundaryCollection()
{
    qDebug() << "~BoundaryCollection()";
}

QSharedPointer<BoundaryCollection> BoundaryCollection::GetBoundaryCollection()
{
    if(nullptr == boundaryCollection)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == boundaryCollection)
        {
            boundaryCollection = QSharedPointer<BoundaryCollection>(new BoundaryCollection);
        }
    }

    return boundaryCollection;
}

const BoundaryCollection::BoundaryMap &BoundaryCollection::GetBoundaryMap() const
{
    return this->boundary_map;
}

QSharedPointer<Boundary> BoundaryCollection::GetBoundary(const QString &id)
{
    if(!this->boundary_map.contains(id))
    {
        this->AppendBoundary(id);
    }
    return this->boundary_map[id];
}

void BoundaryCollection::AppendBoundary(const QString &id)
{
    QSharedPointer<Boundary> boundary = QSharedPointer<MechanicalBoundary>(new MechanicalBoundary);
    this->boundary_map.insert(id, boundary);
}

void BoundaryCollection::RemoveBoundary(const QString &id)
{
    this->boundary_map.remove(id);
}

void BoundaryCollection::UpdateBoundary(const QString &id, QSharedPointer<Boundary> boundary)
{
    if(this->boundary_map.contains(id))
    {
        this->boundary_map[id] = boundary;
    }
}

void BoundaryCollection::Clear()
{
    this->boundary_map.clear();
}

void BoundaryCollection::ReshuffleBoundaryId()
{
    if(!mechsBoundaryIds.isEmpty()) mechsBoundaryIds.clear();
    if(!hydroBoundaryIds.isEmpty()) hydroBoundaryIds.clear();
    if(!thermBoundaryIds.isEmpty()) thermBoundaryIds.clear();

    QSharedPointer<BoundaryCollection> boundaryCollection = BoundaryCollection::GetBoundaryCollection();
    BoundaryCollection::BoundaryMap boundary_map = boundaryCollection->GetBoundaryMap();

    for(auto iter = boundary_map.begin(); iter != boundary_map.end(); ++iter)
    {
        int type = (iter.value())->getBoundaryType();
        if(Boundary::MECHANICAL == type)
        {
            this->mechsBoundaryIds.push_back(iter.key());
        }
        else if(Boundary::HYDRAULIC == type)
        {
            this->hydroBoundaryIds.push_back(iter.key());
        }
        else if(Boundary::THERMAL == type)
        {
            this->thermBoundaryIds.push_back(iter.key());
        }
    }
}

QStringList BoundaryCollection::GetMechanicalBoundaryList() const
{
    return this->mechsBoundaryIds;
}

QStringList BoundaryCollection::GetHydraulicBoundaryList() const
{
    return this->hydroBoundaryIds;
}

QStringList BoundaryCollection::GetThermalBoundaryList() const
{
    return this->thermBoundaryIds;
}

bool BoundaryCollection::IsArtificialBoundaryEnabled()
{
    for(const QString id : this->mechsBoundaryIds)
    {
        MechanicalBoundary *boundary = static_cast<MechanicalBoundary*>(this->boundary_map[id].data());
        if(boundary->artificialBoundary > -1) { return true; }
    }
    return false;
}

bool BoundaryCollection::IsMechanicalBoundarySiteDependent()
{
    for(const QString id : this->mechsBoundaryIds)
    {
        MechanicalBoundary *boundary = static_cast<MechanicalBoundary*>(this->boundary_map[id].data());
        if(boundary->isAmplitudeSiteDependent) { return true; }
    }
    return false;
}

bool BoundaryCollection::IsMechanicalBoundaryTimeDependent()
{
    for(const QString id : this->mechsBoundaryIds)
    {
        MechanicalBoundary *boundary = static_cast<MechanicalBoundary*>(this->boundary_map[id].data());
        if(boundary->isAmplitudeTimeDependent) { return true; }
    }
    return false;
}

bool BoundaryCollection::IsHydraulicBoundarySiteDependent()
{
    for(const QString id : this->hydroBoundaryIds)
    {
        HydraulicBoundary *boundary = static_cast<HydraulicBoundary*>(this->boundary_map[id].data());
        if(boundary->isAmplitudeSiteDependent) { return true; }
    }
    return false;
}

bool BoundaryCollection::IsHydraulicBoundaryTimeDependent()
{
    for(const QString id : this->hydroBoundaryIds)
    {
        HydraulicBoundary *boundary = static_cast<HydraulicBoundary*>(this->boundary_map[id].data());
        if(boundary->isAmplitudeTimeDependent) { return true; }
    }
    return false;
}

bool BoundaryCollection::IsThermalBoundarySiteDependent()
{
    for(const QString id : this->thermBoundaryIds)
    {
        ThermalBoundary *boundary = static_cast<ThermalBoundary*>(this->boundary_map[id].data());
        if(boundary->isAmplitudeSiteDependent) { return true; }
    }
    return false;
}

bool BoundaryCollection::IsThermalBoundaryTimeDependent()
{
    for(const QString id : this->thermBoundaryIds)
    {
        ThermalBoundary *boundary = static_cast<ThermalBoundary*>(this->boundary_map[id].data());
        if(boundary->isAmplitudeTimeDependent) { return true; }
    }
    return false;
}

BoundaryCollection::BoundaryCollection()
{
    qDebug() << "BoundaryCollection()";
}
