#include "hydraulicconstitutivemodel.h"
#include <QDebug>

TriangleHydraulicModel::TriangleHydraulicModel()
{
    this->permeabilitySeed = 0;
    this->permeabilityIndex = 2.0;

    this->biotCoefficient = 1.0;
    this->porePorosity = 0.1;
    this->poreModulus = 1.0e+09;
    this->poreSource = 0.0;
    this->isPermeabilityInhomoEnabled = false;

    this->majorPermeability = 0.0;
    this->minorPermeability = 0.0;
    this->orientationAngle = 0.0;
}

TriangleHydraulicModel::~TriangleHydraulicModel()
{

}

IsotropicSeepageModel::IsotropicSeepageModel()
    : TriangleHydraulicModel()
{
    this->isoPermeability = 1.0E-13;
}

IsotropicSeepageModel::~IsotropicSeepageModel()
{

}

AnisotropicSeepageModel::AnisotropicSeepageModel()
    : TriangleHydraulicModel()
{
    this->majorPermeability = 1.0e-13;
    this->minorPermeability = 1.0e-13;
    this->orientationAngle = 0.0;
}

AnisotropicSeepageModel::~AnisotropicSeepageModel()
{

}


CohesiveHydraulicModel::CohesiveHydraulicModel()
{
    this->cohesiveExchange = 1.0E-11;
    this->fractureExchange = 1.0E-11;
    this->iniAperture = -1.0;
    this->minAperture = -1.0;
    this->maxAperture = -1.0;
}

CohesiveHydraulicModel::~CohesiveHydraulicModel()
{

}

IncompressibleFlowModel::IncompressibleFlowModel()
    : CohesiveHydraulicModel()
{
    this->iniAperture = 5.0e-5;
    this->minAperture = 1.0e-5;
    this->maxAperture = 5.0e-4;
}

IncompressibleFlowModel::~IncompressibleFlowModel()
{

}

CompressibleFlowModel::CompressibleFlowModel()
    : CohesiveHydraulicModel()
{

}

CompressibleFlowModel::~CompressibleFlowModel()
{

}
