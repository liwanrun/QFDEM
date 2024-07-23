#include "temporal.h"

#include <vtkDoubleArray.h>

Temporal::Temporal(int type)
    : temporalType(type)
{
    this->nodeSetId = 1;
    this->initialField = 0;
    this->amplitudeX = 0.0;

    this->isAmplitudeSiteDependent = false;
    this->spatialSample = 0;

    this->spatialPattern = 0;
    this->spatialFactor1 = 1.0;
    this->spatialFactor2 = 1.0;
}

int Temporal::getTemporalType()
{
    return this->temporalType;
}

MechanicalTemporal::MechanicalTemporal()
    : Temporal (MECHANICAL)
{
    this->amplitudeY = 0.0;
}

const char *MechanicalTemporal::GetIndexArrayName()
{
    return "Point Initial Ids (M)";
}

const char *MechanicalTemporal::GetFieldArrayName()
{
    return "Point Initial Velocity (M)";
}

int MechanicalTemporal::GetFieldArrayComponents()
{
    return 2;
}

vtkDoubleArray *MechanicalTemporal::GetSpatialFieldAmplitudes()
{
    double factor1 = isAmplitudeSiteDependent ? spatialFactor1 : 1.0;
    double factor2 = isAmplitudeSiteDependent ? spatialFactor2 : 1.0;

    vtkDoubleArray* ampArray = vtkDoubleArray::New();
    ampArray->SetNumberOfComponents(2);
    ampArray->SetNumberOfTuples(2);
    ampArray->InsertTuple2(0, amplitudeX * factor1, amplitudeY * factor1);
    ampArray->InsertTuple2(1, amplitudeX * factor2, amplitudeY * factor2);
    return ampArray;
}

HydraulicTemporal::HydraulicTemporal()
    : Temporal (HYDRAULIC)
{

}

const char *HydraulicTemporal::GetIndexArrayName()
{
    return "Point Initial Ids (H)";
}

const char *HydraulicTemporal::GetFieldArrayName()
{
    return "Point Initial Pressure (H)";
}

int HydraulicTemporal::GetFieldArrayComponents()
{
    return 1;
}

vtkDoubleArray *HydraulicTemporal::GetSpatialFieldAmplitudes()
{
    double factor1 = isAmplitudeSiteDependent ? spatialFactor1 : 1.0;
    double factor2 = isAmplitudeSiteDependent ? spatialFactor2 : 1.0;

    vtkDoubleArray* ampArray = vtkDoubleArray::New();
    ampArray->SetNumberOfComponents(1);
    ampArray->SetNumberOfTuples(2);
    ampArray->InsertTuple1(0, amplitudeX * factor1);
    ampArray->InsertTuple1(1, amplitudeX * factor2);
    return ampArray;
}

ThermalTemporal::ThermalTemporal()
    : Temporal (THERMAL)
{

}

const char *ThermalTemporal::GetIndexArrayName()
{
    return "Point Initial Ids (T)";
}

const char *ThermalTemporal::GetFieldArrayName()
{
    return "Point Initial Temperature (T)";
}

int ThermalTemporal::GetFieldArrayComponents()
{
    return 1;
}

vtkDoubleArray *ThermalTemporal::GetSpatialFieldAmplitudes()
{
    double factor1 = isAmplitudeSiteDependent ? spatialFactor1 : 1.0;
    double factor2 = isAmplitudeSiteDependent ? spatialFactor2 : 1.0;

    vtkDoubleArray* ampArray = vtkDoubleArray::New();
    ampArray->SetNumberOfComponents(1);
    ampArray->SetNumberOfTuples(2);
    ampArray->InsertTuple1(0, amplitudeX * factor1);
    ampArray->InsertTuple1(1, amplitudeX * factor2);
    return ampArray;
}

TemporalStressStrain::TemporalStressStrain()
    : Temporal (INISTRESS)
{
    this->amplitudeXX = 0.0;
    this->amplitudeYY = 0.0;
    this->amplitudeXY = 0.0;

    this->isAmplitudeSiteDependent = false;
    QVector<double> tuple({0.0, 0.0, 0.0});
    this->spatialFactor.fill(tuple, 3);

    this->spatialPattern = 0;
    this->gradientXOfSXX = 0.0;
    this->gradientYOfSXX = 0.0;
    this->gradientXOfSYY = 0.0;
    this->gradientYOfSYY = 0.0;
    this->gradientXOfSXY = 0.0;
    this->gradientYOfSXY = 0.0;
}

const char *TemporalStressStrain::GetIndexArrayName()
{
    return "Cell Initial Ids (M)";
}

const char *TemporalStressStrain::GetFieldArrayName()
{
    return "Cell Initial Stress/Strain (M)";
}

int TemporalStressStrain::GetFieldArrayComponents()
{
    return 3;
}

vtkDoubleArray *TemporalStressStrain::GetSpatialFieldAmplitudes()
{
    return nullptr;
}
