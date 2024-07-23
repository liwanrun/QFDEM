#include "material.h"
#include "mechanicalconstitutivemodel.h"
#include "contactconstitutivemodel.h"
#include "hydraulicconstitutivemodel.h"
#include "thermalconstitutivemodel.h"

#include <QDebug>

Material::Material(int type)
    : materialType (type),
      enableStep (1),
      elementSet(QString::number(1))
{

}

Material::~Material()
{

}

int Material::getMaterialType()
{
    return this->materialType;
}

const char *Material::GetIndexArrayName()
{
    return "Cell Material Ids";
}

const char *Material::GetYoungModulusArrayName()
{
    return "Cell Material Young Modulus";
}

const char *Material::GetPermeabilityArrayName()
{
    return "Cell Material Permeability";
}

const char *Material::GetConductivityArrayName()
{
    return "Cell Material Conductivity";
}

TriangleMaterial::TriangleMaterial()
    : Material (TRIANGLE),
      isMechanicalAnalysisEnabled(false),
      isHydraulicAnalysisEnabled(false),
      isThermalAnalysisEnabled(false)
{
    this->elemStatus = 1;
    this->density = 2700.0;

    this->mechanicalModelType = 1;
    this->mechanicalModel = QSharedPointer<HookeanElasticityModel>(new HookeanElasticityModel);

    this->contactModelType = 0;
    this->contactModel = QSharedPointer<ContactElasticityModel>(new ContactElasticityModel);

    this->hydraulicModelType = 0;
    this->hydraulicModel = QSharedPointer<IsotropicSeepageModel>(new IsotropicSeepageModel);

    this->thermalModelType = 0;
    this->thermalModel = QSharedPointer<IsotropicConductModel>(new IsotropicConductModel);
}

TriangleMaterial::~TriangleMaterial()
{
    qDebug() << "~SolidTriangleMaterial()";
}

CohesiveMaterial::CohesiveMaterial()
    : Material (COHESIVE),
      isMechanicalAnalysisEnabled(false),
      isHydraulicAnalysisEnabled(false),
      isThermalAnalysisEnabled(false)
{
    this->elemStatus = 1;
    this->isCohesiveBroken = 0;

    this->mechanicalModelType = 1;
    this->mechanicalModel = QSharedPointer<CohesiveExponentModel>(new CohesiveExponentModel);

    this->hydraulicModelType = 0;
    this->hydraulicModel = QSharedPointer<IncompressibleFlowModel>(new IncompressibleFlowModel);

    this->thermalModelType = 0;
    this->thermalModel = QSharedPointer<CohesiveThermalModel>(new CohesiveThermalModel);
}

CohesiveMaterial::~CohesiveMaterial()
{
    qDebug() << "~CohesiveQuadMaterial()";
}

NTFluidMaterial::NTFluidMaterial()
    : Material (NTSFLUID)
{
    this->density = 1000.0;
    this->bulkModulus = 2.25e+9;
    this->bulkViscosity = 0.0;
    this->dynaViscosity = 1.00e-3;

    this->heatCapacity = 4.20e+3;
    this->conductivity = 0.59;
}

NTFluidMaterial::~NTFluidMaterial()
{
    qDebug() << "~NTFluidMaterial()";
}
