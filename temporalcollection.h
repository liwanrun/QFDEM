#ifndef TEMPORALCOLLECTION_H
#define TEMPORALCOLLECTION_H

#include <QMap>
#include <QMutex>
#include <QSharedPointer>

class Temporal;
class TemporalCollection
{
public:
    typedef  QMap<QString, QSharedPointer<Temporal>> TemporalMap;

public:
    ~TemporalCollection();

    static QSharedPointer<TemporalCollection> GetTemporalCollection();

    const TemporalMap& GetTemporalMap();

    QSharedPointer<Temporal> GetTemporal(const QString& id);

    void AppendTemporal(const QString& id);

    void RemoveTemporal(const QString& id);

    void UpdateTemporal(const QString& id, QSharedPointer<Temporal> temporal);

    void Clear();

public:
    void ReshuffleTemporalId();

    QStringList GetMechanicalTemporalList() const;

    QStringList GetHydraulicTemporalList() const;

    QStringList GetThermalTemporalList() const;

    QStringList GetIniStressTemporalList() const;

    bool IsMechanicalTemporalSiteDependent();

    bool IsHydraulicTemporalSiteDependent();

    bool IsThermalTemporalSiteDependent();

private:
    TemporalCollection();

private:
    static QMutex mutex;
    static QSharedPointer<TemporalCollection> temporalCollection;

    TemporalMap temporal_map;

    QStringList mechsTemporalIds;
    QStringList hydroTemporalIds;
    QStringList thermTemporalIds;
    QStringList sigmaTemporalIds;
};

#endif // TEMPORALCOLLECTION_H
