#include "contactconstitutivemodel.h"

#include <QDebug>


ContactConstitutiveModel::ContactConstitutiveModel()
{
    this->normalStiffness = 1.5e+12;
    this->tangentStiffness = 1.5e+12;
    this->contactFriction = 0.1;
    this->yieldStrength = -1.0;
    this->powerExponent = -1.0;
}

ContactConstitutiveModel::~ContactConstitutiveModel()
{

}

ContactElasticityModel::ContactElasticityModel()
    : ContactConstitutiveModel()
{

}

ContactElasticityModel::~ContactElasticityModel()
{

}

ContactPlasticityModel::ContactPlasticityModel()
    : ContactConstitutiveModel()
{
    this->yieldStrength = 3.0e+7;
    this->powerExponent = 1.0;
}

ContactPlasticityModel::~ContactPlasticityModel()
{

}
