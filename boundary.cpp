#include "boundary.h"

#include <vtkDoubleArray.h>

Boundary::Boundary(int type)
    : boundaryType(type)
{
    this->loadStep = 1;
    this->nodeSetId = 1;

    this->loadingTypeX = 0;
    this->amplitudeX = 0.0;

    this->isAmplitudeSiteDependent = false;
    this->spatialSample = 0;

    this->isAmplitudeTimeDependent = false;
    this->temporalSample = 0;
}

int Boundary::getBoundaryType()
{
    return this->boundaryType;
}

MechanicalBoundary::MechanicalBoundary()
    : Boundary (MECHANICAL)
{
    this->loadingTypeY = 0;
    this->amplitudeY = 0.0;

    localAxisXX = 1.0;
    localAxisXY = 0.0;
    localAxisYX = 0.0;
    localAxisYY = 1.0;

    isContactBoundaryEnabled = false;
    isArtificialBoundaryEnabled = false;
    artificialBoundary = -1;
}

const char *MechanicalBoundary::GetIndexArrayName()
{
    return "Point Boundary Ids (M)";
}

const char *MechanicalBoundary::GetFieldArrayName()
{
    if(0 == this->loadingTypeX)
    {
        return "Point Boundary Traction (M)";
    }
    else if(1 == this->loadingTypeX)
    {
        return "Point Boundary Force (M)";
    }
    else if(2 == this->loadingTypeX)
    {
        return "Point Boundary Acceleration (M)";
    }
    else
    {
        return "Point Boundary Velocity (M)";
    }
}

int MechanicalBoundary::GetFieldArrayComponents()
{
    return 2;
}

vtkDoubleArray *MechanicalBoundary::GetSpatialFieldAmplitudes()
{
    double factor1 = 1.0;
    double factor2 = 1.0;

    vtkDoubleArray* ampArray = vtkDoubleArray::New();
    ampArray->SetNumberOfComponents(2);
    ampArray->SetNumberOfTuples(2);
    ampArray->InsertTuple2(0, amplitudeX * factor1, amplitudeY * factor1);
    ampArray->InsertTuple2(1, amplitudeX * factor2, amplitudeY * factor2);
    return ampArray;
}

HydraulicBoundary::HydraulicBoundary()
    : Boundary (HYDRAULIC)
{

}

const char *HydraulicBoundary::GetIndexArrayName()
{
    return "Point Boundary Ids (H)";
}

const char *HydraulicBoundary::GetFieldArrayName()
{
    if(0 == this->loadingTypeX)
    {
        return "Point Boundary Pressure (H)";
    }
    else
    {
        return "Point Boundary FlowRate (H)";
    }
}

int HydraulicBoundary::GetFieldArrayComponents()
{
    return 1;
}

vtkDoubleArray *HydraulicBoundary::GetSpatialFieldAmplitudes()
{
    double factor1 = 1.0;
    double factor2 = 1.0;

    vtkDoubleArray* ampArray = vtkDoubleArray::New();
    ampArray->SetNumberOfComponents(1);
    ampArray->SetNumberOfTuples(2);
    ampArray->InsertTuple1(0, amplitudeX * factor1);
    ampArray->InsertTuple1(1, amplitudeX * factor2);
    return ampArray;
}

ThermalBoundary::ThermalBoundary()
    : Boundary (THERMAL)
{

}

const char *ThermalBoundary::GetIndexArrayName()
{
    return "Point Boundary Ids (T)";
}

const char *ThermalBoundary::GetFieldArrayName()
{
    if(0 == this->loadingTypeX)
    {
        return "Point Boundary Temperature (T)";
    }
    else
    {
        return "Point Boundary HeatFlux (T)";
    }
}

int ThermalBoundary::GetFieldArrayComponents()
{
    return 1;
}

vtkDoubleArray *ThermalBoundary::GetSpatialFieldAmplitudes()
{
    double factor1 = 1.0;
    double factor2 = 1.0;

    vtkDoubleArray* ampArray = vtkDoubleArray::New();
    ampArray->SetNumberOfComponents(1);
    ampArray->SetNumberOfTuples(2);
    ampArray->InsertTuple1(0, amplitudeX * factor1);
    ampArray->InsertTuple1(1, amplitudeX * factor2);
    return ampArray;
}
