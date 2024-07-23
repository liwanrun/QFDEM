#ifndef BOUNDARYCOLLECTION_H
#define BOUNDARYCOLLECTION_H

#include <QMap>
#include <QSet>
#include <QMutex>
#include <QSharedPointer>

class Boundary;
class BoundaryCollection
{
public:
    typedef QMap<QString, QSharedPointer<Boundary>> BoundaryMap;

public:
    ~BoundaryCollection();

    static QSharedPointer<BoundaryCollection> GetBoundaryCollection();

    const BoundaryMap& GetBoundaryMap() const;

    QSharedPointer<Boundary> GetBoundary(const QString& id);

    void AppendBoundary(const QString& id);

    void RemoveBoundary(const QString& id);

    void UpdateBoundary(const QString& id, QSharedPointer<Boundary> boundary);

    void Clear();

public:
    void ReshuffleBoundaryId();

    QStringList GetMechanicalBoundaryList() const;

    QStringList GetHydraulicBoundaryList() const;

    QStringList GetThermalBoundaryList() const;

    bool IsArtificialBoundaryEnabled();

    bool IsMechanicalBoundarySiteDependent();

    bool IsMechanicalBoundaryTimeDependent();

    bool IsHydraulicBoundarySiteDependent();

    bool IsHydraulicBoundaryTimeDependent();

    bool IsThermalBoundarySiteDependent();

    bool IsThermalBoundaryTimeDependent();

private:
    BoundaryCollection();

private:
    static QMutex mutex;
    static QSharedPointer<BoundaryCollection> boundaryCollection;

    BoundaryMap boundary_map;

    QStringList mechsBoundaryIds;
    QStringList hydroBoundaryIds;
    QStringList thermBoundaryIds;
};

#endif // BOUNDARYCOLLECTION_H
