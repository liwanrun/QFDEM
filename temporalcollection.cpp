#include "temporalcollection.h"
#include "temporal.h"

#include <QDebug>

QMutex TemporalCollection::mutex;

QSharedPointer<TemporalCollection> TemporalCollection::temporalCollection = nullptr;

TemporalCollection::~TemporalCollection()
{
    qDebug() << "~TemporalCollection()";
}

QSharedPointer<TemporalCollection> TemporalCollection::GetTemporalCollection()
{
    if(nullptr == temporalCollection)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == temporalCollection)
        {
            temporalCollection = QSharedPointer<TemporalCollection>(new TemporalCollection);
        }
    }

    return temporalCollection;
}

const TemporalCollection::TemporalMap &TemporalCollection::GetTemporalMap()
{
    return this->temporal_map;
}

QSharedPointer<Temporal> TemporalCollection::GetTemporal(const QString &id)
{
    if(!this->temporal_map.contains(id))
    {
        this->AppendTemporal(id);
    }
    return this->temporal_map[id];
}

void TemporalCollection::AppendTemporal(const QString &id)
{
    QSharedPointer<Temporal> temporal = QSharedPointer<MechanicalTemporal>(new MechanicalTemporal);
    this->temporal_map.insert(id, temporal);
}

void TemporalCollection::RemoveTemporal(const QString &id)
{
    this->temporal_map.remove(id);
}

void TemporalCollection::UpdateTemporal(const QString &id, QSharedPointer<Temporal> temporal)
{
    if(this->temporal_map.contains(id))
    {
        this->temporal_map[id] = temporal;
    }
}

void TemporalCollection::Clear()
{
    this->temporal_map.clear();
}

void TemporalCollection::ReshuffleTemporalId()
{
    if(!mechsTemporalIds.isEmpty()) mechsTemporalIds.clear();
    if(!hydroTemporalIds.isEmpty()) hydroTemporalIds.clear();
    if(!thermTemporalIds.isEmpty()) thermTemporalIds.clear();
    if(!sigmaTemporalIds.isEmpty()) sigmaTemporalIds.clear();

    QSharedPointer<TemporalCollection> temporalCollection = TemporalCollection::GetTemporalCollection();
    TemporalCollection::TemporalMap temporal_map = temporalCollection->GetTemporalMap();

    for(auto iter = temporal_map.begin(); iter != temporal_map.end(); ++iter)
    {
        int type = (iter.value())->getTemporalType();
        if(Temporal::MECHANICAL == type)
        {
            this->mechsTemporalIds.push_back(iter.key());
        }
        else if(Temporal::HYDRAULIC == type)
        {
            this->hydroTemporalIds.push_back(iter.key());
        }
        else if(Temporal::THERMAL == type)
        {
            this->thermTemporalIds.push_back(iter.key());
        }
        else if(Temporal::INISTRESS == type)
        {
            this->sigmaTemporalIds.push_back(iter.key());
        }
    }
}

QStringList TemporalCollection::GetMechanicalTemporalList() const
{
    return this->mechsTemporalIds;
}

QStringList TemporalCollection::GetHydraulicTemporalList() const
{
    return this->hydroTemporalIds;
}

QStringList TemporalCollection::GetThermalTemporalList() const
{
    return this->thermTemporalIds;
}

QStringList TemporalCollection::GetIniStressTemporalList() const
{
    return this->sigmaTemporalIds;
}

bool TemporalCollection::IsMechanicalTemporalSiteDependent()
{
    for(const QString id : this->mechsTemporalIds)
    {
        MechanicalTemporal *temporal = static_cast<MechanicalTemporal*>(this->temporal_map[id].data());
        if(temporal->isAmplitudeSiteDependent) { return true; }
    }
    return false;
}

bool TemporalCollection::IsHydraulicTemporalSiteDependent()
{
    for(const QString id : this->hydroTemporalIds)
    {
        HydraulicTemporal *temporal = static_cast<HydraulicTemporal*>(this->temporal_map[id].data());
        if(temporal->isAmplitudeSiteDependent) { return true; }
    }
    return false;
}

bool TemporalCollection::IsThermalTemporalSiteDependent()
{
    for(const QString id : this->thermTemporalIds)
    {
        ThermalTemporal *temporal = static_cast<ThermalTemporal*>(this->temporal_map[id].data());
        if(temporal->isAmplitudeSiteDependent) { return true; }
    }
    return false;
}

TemporalCollection::TemporalCollection()
{
    qDebug() << "TemporalCollection()";
}
