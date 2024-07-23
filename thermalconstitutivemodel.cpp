#include "thermalconstitutivemodel.h"
#include <QDebug>

TriangleThermalModel::TriangleThermalModel()
{
    this->conductivitySeed = 0;
    this->conductivityIndex = 2.0;
    this->heatCapacity  = 0.2;
    this->heatExchange  = 0.0;
    this->heatSource    = 0.0;
    this->heatExpansion = 0.0;
    this->isConductivityInhomoEnabled = false;

    this->orientationAngle = 0.0;
    this->majorConductivity = 1.0e+02;
    this->minorConductivity = 1.0e+02;
}

TriangleThermalModel::~TriangleThermalModel()
{

}

IsotropicConductModel::IsotropicConductModel()
    : TriangleThermalModel()
{
    this->isoConductivity  = 1.0e+02;
}

IsotropicConductModel::~IsotropicConductModel()
{

}

AnisotropicConductModel::AnisotropicConductModel()
    : TriangleThermalModel()
{
    this->orientationAngle = 0.0;
    this->majorConductivity = 0.0;
    this->minorConductivity = 0.0;
}

AnisotropicConductModel::~AnisotropicConductModel()
{

}

CohesiveThermalModel::CohesiveThermalModel()
{
    this->cohesiveExchange = 0.0;
    this->fractureExchange = 0.0;
}

CohesiveThermalModel::~CohesiveThermalModel()
{

}
