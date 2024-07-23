#ifndef THERMALCONSTITUTIVEMODEL_H
#define THERMALCONSTITUTIVEMODEL_H

#include "constitutivemodel.h"

class TriangleThermalModel : public ConstitutiveModel
{
public:
    TriangleThermalModel();
    ~TriangleThermalModel() override;

public:
    friend class MaterialCollection;
    friend class TriangleMaterial;
    friend class FDEMDocumentExporter;
    friend class FDEMSolutionWidget;

protected:
    int    conductivitySeed;
    double conductivityIndex;
    double heatCapacity;
    double heatExchange;
    double heatSource;
    double heatExpansion;
    bool   isConductivityInhomoEnabled;

    double orientationAngle;
    double majorConductivity;
    double minorConductivity;
};

class IsotropicConductModel : public TriangleThermalModel
{
public:
    IsotropicConductModel();
    ~IsotropicConductModel() override;

public:
    friend class TriangleIsotropicHeatConductWidget;

public:
    double isoConductivity;
};

class AnisotropicConductModel : public TriangleThermalModel
{
public:
    AnisotropicConductModel();
    ~AnisotropicConductModel() override;

public:
    friend class TriangleAnisotropicHeatConductWidget;
};

/*******************************************************/

class CohesiveThermalModel : public ConstitutiveModel
{
public:
    CohesiveThermalModel();
    ~CohesiveThermalModel();

public:
    friend class CohesiveMaterial;
    friend class CohesiveHeatExchangeWidget;
    friend class FDEMDocumentExporter;

private:
    double cohesiveExchange;
    double fractureExchange;
};

#endif // THERMALCONSTITUTIVEMODEL_H
