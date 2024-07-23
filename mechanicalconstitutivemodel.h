#ifndef MECHANICALCONSTITUTIVEMODEL_H
#define MECHANICALCONSTITUTIVEMODEL_H

#include "constitutivemodel.h"

class TriangleMechanicalModel : public ConstitutiveModel
{
public:
    TriangleMechanicalModel();
    ~TriangleMechanicalModel() override;

public:
    friend class MaterialCollection;
    friend class TriangleMaterial;

    friend class TriangulationActor;
    friend class FDEMDocumentExporter;
    friend class FDEMSolutionWidget;
    friend class FDEMMaterialWidget;

protected:
    double youngModulus;
    int    youngModulusSeed;
    double youngModulusIndex;

    double poissonRatio;
    int    poissonRatioSeed;
    double poissonRatioIndex;

    double viscousDamping;
    double softenStretch;

    bool isPropertyInhomoEnabled;
    int  inhomogeneousOption;

    double orientationAngle;
    double majorYoungModulus;
    double minorYoungModulus;
    double majorPoissonRatio;
    double minorPoissonRatio;
    double shearModulus;
};

class CavityModel : public TriangleMechanicalModel
{
public:
    CavityModel();
    ~CavityModel() override;
};

class RigidBodyMotionModel : public TriangleMechanicalModel
{
public:
    RigidBodyMotionModel();
    ~RigidBodyMotionModel() override;
};

class HookeanElasticityModel : public TriangleMechanicalModel
{
public:
    HookeanElasticityModel();
    ~HookeanElasticityModel() override;

public:
    friend class TriangleHookeanElasticityWidget;
};

class AnisotropicElasticityModel : public TriangleMechanicalModel
{
public:
    AnisotropicElasticityModel();
    ~AnisotropicElasticityModel() override;

public:
    friend class TriangleAnisotropicElasticityWidget;
};

class ElastoPlasticityModel : public TriangleMechanicalModel
{
public:
    ElastoPlasticityModel();
    ~ElastoPlasticityModel() override;

public:
    friend class TriangleElastoPlasticityWidget;
};

/******************************************************/
class CohesiveMechanicalModel : public ConstitutiveModel
{
public:
    CohesiveMechanicalModel();
    ~CohesiveMechanicalModel() override;

public:
    friend class MaterialCollection;
    friend class CohesiveMaterial;

    friend class FDEMDocumentExporter;

protected:
    double normalPenalty;
    double tangentPenalty;

    double tension;
    double cohesion;
    double friction;
    double tensionFERR;
    double shearFERR;

    bool isPropertyInhomoEnabled;
    int  inhomogeneousOption;
};

class CohesiveExponentModel : public CohesiveMechanicalModel
{
public:
    CohesiveExponentModel();
    ~CohesiveExponentModel() override;

public:
    friend class CohesiveExponentWidget;
};

class CohesiveBilinearModel : public CohesiveMechanicalModel
{
public:
    CohesiveBilinearModel();
    ~CohesiveBilinearModel() override;

public:
    friend class CohesiveBilinearWidget;
};

#endif // MECHANICALCONSTITUTIVEMODEL_H
