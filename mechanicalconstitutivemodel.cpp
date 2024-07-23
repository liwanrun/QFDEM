#include "mechanicalconstitutivemodel.h"

#include <QDebug>
#include <QString>
#include <QtVariantProperty>

TriangleMechanicalModel::TriangleMechanicalModel()
{
    this->youngModulus = 1.5e+10;
    this->youngModulusSeed = 0;
    this->youngModulusIndex = 2.0;

    this->poissonRatio = 0.2;
    this->poissonRatioSeed = 0;
    this->poissonRatioIndex = 2.0;
    this->viscousDamping = 7600000.0;

    this->isPropertyInhomoEnabled = false;
    this->inhomogeneousOption = 0;
}

TriangleMechanicalModel::~TriangleMechanicalModel()
{

}

RigidBodyMotionModel::RigidBodyMotionModel() : TriangleMechanicalModel()
{

}

RigidBodyMotionModel::~RigidBodyMotionModel()
{

}

HookeanElasticityModel::HookeanElasticityModel()
    : TriangleMechanicalModel()
{

}

HookeanElasticityModel::~HookeanElasticityModel()
{

}

AnisotropicElasticityModel::AnisotropicElasticityModel()
    : TriangleMechanicalModel()
{
    this->orientationAngle = 0.0;
    this->majorYoungModulus = 1.5e+10;
    this->minorYoungModulus = 1.0e+10;
    this->majorPoissonRatio = 0.2;
    this->minorPoissonRatio = 0.1;
    this->shearModulus = 2.0e+10;
}

AnisotropicElasticityModel::~AnisotropicElasticityModel()
{

}

ElastoPlasticityModel::ElastoPlasticityModel()
    : TriangleMechanicalModel()
{
    this->softenStretch = 15.0;
}

ElastoPlasticityModel::~ElastoPlasticityModel()
{

}

CohesiveMechanicalModel::CohesiveMechanicalModel()
{
    this->normalPenalty = 1.5e+12;
    this->tangentPenalty = 1.5e+12;

    this->tension = 3.0e+6;
    this->cohesion = 1.0e+7;
    this->friction = 30;
    this->tensionFERR = 3.8;
    this->shearFERR = 84.0;

    this->isPropertyInhomoEnabled = false;
    this->inhomogeneousOption = 0;
}

CohesiveMechanicalModel::~CohesiveMechanicalModel()
{

}

CohesiveExponentModel::CohesiveExponentModel()
    : CohesiveMechanicalModel()
{

}

CohesiveExponentModel::~CohesiveExponentModel()
{

}

CohesiveBilinearModel::CohesiveBilinearModel()
    : CohesiveMechanicalModel()
{

}

CohesiveBilinearModel::~CohesiveBilinearModel()
{

}
