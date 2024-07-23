#ifndef HYDRAULICCONSTITUTIVEMODEL_H
#define HYDRAULICCONSTITUTIVEMODEL_H

#include "constitutivemodel.h"

class TriangleHydraulicModel : public ConstitutiveModel
{
public:
    TriangleHydraulicModel();
    ~TriangleHydraulicModel() override;

public:
    friend class MaterialCollection;
    friend class TriangleMaterial;
    friend class FDEMDocumentExporter;
    friend class FDEMSolutionWidget;

protected:
    int    permeabilitySeed;
    double permeabilityIndex;
    bool   isPermeabilityInhomoEnabled;

    double biotCoefficient;
    double porePorosity;
    double poreModulus;
    double poreSource;

    double majorPermeability;
    double minorPermeability;
    double orientationAngle;
};

class IsotropicSeepageModel : public TriangleHydraulicModel
{
public:
    IsotropicSeepageModel();
    ~IsotropicSeepageModel() override;

public:
    friend class TriangleIsotropicSeepageWidget;

public:
    double isoPermeability;
};

class AnisotropicSeepageModel : public TriangleHydraulicModel
{
public:
    AnisotropicSeepageModel();
    ~AnisotropicSeepageModel() override;

public:
    friend class TriangleAnisotropicSeepageWidget;
};

/******************************************************/
class CohesiveHydraulicModel : public ConstitutiveModel
{
public:
    CohesiveHydraulicModel();
    ~CohesiveHydraulicModel() override;

public:
    friend class CohesiveMaterial;
    friend class FDEMDocumentExporter;
    friend class FDEMSolutionWidget;

protected:
    double cohesiveExchange;
    double fractureExchange;
    double iniAperture;
    double minAperture;
    double maxAperture;
};

class IncompressibleFlowModel : public CohesiveHydraulicModel
{
public:
    IncompressibleFlowModel();
    ~IncompressibleFlowModel();

public:
    friend class CohesiveIncompressibleFlowWidget;
};

class CompressibleFlowModel : public CohesiveHydraulicModel
{
public:
    CompressibleFlowModel();
    ~CompressibleFlowModel();

public:
    friend class CohesiveCompressibleFlowWidget;
};

#endif // HYDRAULICCONSTITUTIVEMODEL_H
