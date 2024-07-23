#include "criterioncollection.h"
#include "criterion.h"

#include <QDebug>

QMutex CriterionCollection::mutex;

QSharedPointer<CriterionCollection> CriterionCollection::criterionCollection = nullptr;

CriterionCollection::~CriterionCollection()
{
    qDebug() << "~CriterionCollection()";
}

QSharedPointer<CriterionCollection> CriterionCollection::GetCriterionCollection()
{
    if(nullptr == criterionCollection)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == criterionCollection)
        {
            criterionCollection = QSharedPointer<CriterionCollection>(new CriterionCollection);
        }
    }

    return criterionCollection;
}

const CriterionCollection::CriterionMap &CriterionCollection::GetCriterionMap() const
{
    return this->criterion_map;
}

QSharedPointer<Criterion> CriterionCollection::GetCriterion(const QString &id)
{
    if(!this->criterion_map.contains(id))
    {
        this->AppendCriterion(id);
    }
    return this->criterion_map[id];
}

void CriterionCollection::AppendCriterion(const QString &id)
{
    QSharedPointer<Criterion> criterion = QSharedPointer<PointCriterion>(new PointCriterion);
    this->criterion_map.insert(id, criterion);
}

void CriterionCollection::RemoveCriterion(const QString &id)
{
    this->criterion_map.remove(id);
}

void CriterionCollection::UpdateCriterion(const QString &id, QSharedPointer<Criterion> criterion)
{
    if(this->criterion_map.contains(id))
    {
        this->criterion_map[id] = criterion;
    }
}

void CriterionCollection::Clear()
{
    this->criterion_map.clear();
}

CriterionCollection::CriterionCollection()
{
    qDebug() << "CriterionCollection";
}
