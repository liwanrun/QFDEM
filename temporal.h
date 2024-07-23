#ifndef TEMPORAL_H
#define TEMPORAL_H

#include <QVector>

class vtkDoubleArray;
class Temporal
{
public:
    enum TemporalType
    {
        MECHANICAL = 0,
        HYDRAULIC,
        THERMAL,
        INISTRESS
    };

public:
    explicit Temporal(int type = MECHANICAL);
    int getTemporalType();

    virtual const char *GetIndexArrayName() = 0;
    virtual const char *GetFieldArrayName() = 0;
    virtual int GetFieldArrayComponents() = 0;
    virtual vtkDoubleArray *GetSpatialFieldAmplitudes() = 0;

public:
    friend class TemporalCollection;
    friend class FDEMTemporalWidget;
    friend class TriangulationActor;
    friend class ActorCollection;

protected:
    int temporalType;
    int nodeSetId;

    int initialField;
    double amplitudeX;

    bool isAmplitudeSiteDependent;
    int spatialSample;
    QVector<QVector<double>> spatialFactor;

    int spatialPattern;
    double spatialFactor1;
    double spatialFactor2;
};

class MechanicalTemporal : public Temporal
{
public:
    MechanicalTemporal();

    const char *GetIndexArrayName() override;
    const char *GetFieldArrayName() override;
    int GetFieldArrayComponents() override;
    vtkDoubleArray *GetSpatialFieldAmplitudes() override;

public:
    friend class FDEMMechanicalTemporalWidget;
    friend class TriangulationActor;
    friend class ActorCollection;
    friend class FDEMDocumentExporter;

private:
    double amplitudeY;
};

class HydraulicTemporal : public Temporal
{
public:
    HydraulicTemporal();

    const char *GetIndexArrayName() override;
    const char *GetFieldArrayName() override;
    int GetFieldArrayComponents() override;
    vtkDoubleArray *GetSpatialFieldAmplitudes() override;

public:
    friend class FDEMHydraulicTemporalWidget;
    friend class ActorCollection;
    friend class FDEMDocumentExporter;
};

class ThermalTemporal : public Temporal
{
public:
    ThermalTemporal();

    const char *GetIndexArrayName() override;
    const char *GetFieldArrayName() override;
    int GetFieldArrayComponents() override;
    vtkDoubleArray *GetSpatialFieldAmplitudes() override;

public:
    friend class FDEMThermalTemporalWidget;
    friend class ActorCollection;
    friend class FDEMDocumentExporter;
};

class TemporalStressStrain : public Temporal
{
public:
    TemporalStressStrain();

    const char *GetIndexArrayName() override;
    const char *GetFieldArrayName() override;
    int GetFieldArrayComponents() override;
    vtkDoubleArray *GetSpatialFieldAmplitudes() override;

private:
    double amplitudeXX;
    double amplitudeYY;
    double amplitudeXY;
//    QVector<QVector<double>> gradients;

    double gradientXOfSXX;
    double gradientYOfSXX;
    double gradientXOfSYY;
    double gradientYOfSYY;
    double gradientXOfSXY;
    double gradientYOfSXY;

public:
    friend class FDEMTemporalStressStrainWidget;
    friend class FDEMDocumentExporter;
};

#endif // TEMPORAL_H
