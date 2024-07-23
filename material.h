#ifndef MATERIAL_H
#define MATERIAL_H

#include <QPointF>
#include <QSharedPointer>
#include <QVariant>

class Material
{
public:
    enum MaterialType
    {
        TRIANGLE = 0,
        COHESIVE,
        NTSFLUID
    };

public:
    explicit Material(int type = TRIANGLE);
    virtual ~Material();

    int getMaterialType();

    static const char *GetIndexArrayName();
    static const char *GetYoungModulusArrayName();
    static const char *GetPermeabilityArrayName();
    static const char *GetConductivityArrayName();

public:
    friend class MaterialCollection;
    friend class FDEMMaterialWidget;

protected:
    int materialType;
    int enableStep;
    QString elementSet;
};

class ConstitutiveModel;
class TriangleMaterial : public Material
{
 public:
    TriangleMaterial();
    ~TriangleMaterial() override;

public:
    friend class MaterialCollection;
    friend class FDEMTriangleMaterialWidget;

    friend class TriangulationActor;
    friend class FDEMDocumentExporter;
    friend class FDEMSolutionWidget;
    friend class FDEMMaterialWidget;

private:
    int elemStatus;
    double density;

    int mechanicalModelType;
    QSharedPointer<ConstitutiveModel> mechanicalModel;

    int contactModelType;
    QSharedPointer<ConstitutiveModel> contactModel;

    int hydraulicModelType;
    QSharedPointer<ConstitutiveModel> hydraulicModel;

    int thermalModelType;
    QSharedPointer<ConstitutiveModel> thermalModel;

    bool isMechanicalAnalysisEnabled;
    bool isHydraulicAnalysisEnabled;
    bool isThermalAnalysisEnabled;
};


class CohesiveMaterial : public Material
{
public:
    CohesiveMaterial();
    ~CohesiveMaterial() override;

public:
    friend class MaterialCollection;
    friend class FDEMCohesiveMaterialWidget;

    friend class TriangulationActor;
    friend class FDEMDocumentExporter;
    friend class FDEMSolutionWidget;

private:
    int elemStatus;
    int isCohesiveBroken;

    int mechanicalModelType;
    QSharedPointer<ConstitutiveModel> mechanicalModel;

    int hydraulicModelType;
    QSharedPointer<ConstitutiveModel> hydraulicModel;

    int thermalModelType;
    QSharedPointer<ConstitutiveModel> thermalModel;

    bool isMechanicalAnalysisEnabled;
    bool isHydraulicAnalysisEnabled;
    bool isThermalAnalysisEnabled;
};


class NTFluidMaterial : public Material
{
public:
    NTFluidMaterial();
    ~NTFluidMaterial() override;

public:
    friend class MaterialCollection;
    friend class FDEMNTSFluidMaterialWidget;

    friend class FDEMDocumentExporter;
    friend class FDEMSolutionWidget;

private:
    double density;
    double bulkModulus;
    double bulkViscosity;
    double dynaViscosity;

    double conductivity;
    double heatCapacity;
};


#endif // MATERIAL_H
