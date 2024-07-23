#ifndef CRITERIONCOLLECTION_H
#define CRITERIONCOLLECTION_H

#include <QMap>
#include <QMutex>
#include <QSharedPointer>

class Criterion;
class CriterionCollection
{
public:
    typedef QMap<QString, QSharedPointer<Criterion>> CriterionMap;

public:
    ~CriterionCollection();

    static QSharedPointer<CriterionCollection> GetCriterionCollection();

    const CriterionMap& GetCriterionMap() const;

    QSharedPointer<Criterion> GetCriterion(const QString& id);

    void AppendCriterion(const QString& id);

    void RemoveCriterion(const QString& id);

    void UpdateCriterion(const QString& id, QSharedPointer<Criterion> criterion);

    void Clear();

private:
    CriterionCollection();

private:
    CriterionMap criterion_map;

    static QMutex mutex;
    static QSharedPointer<CriterionCollection> criterionCollection;
};

#endif // CRITERIONCOLLECTION_H
