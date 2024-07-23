#ifndef BOUNDARY_H
#define BOUNDARY_H

#include <QVector>

class vtkDoubleArray;
class Boundary
{
public:
    enum BoundaryType
    {
        MECHANICAL = 0,
        HYDRAULIC,
        THERMAL
    };

public:
    explicit Boundary(int type = MECHANICAL);
    virtual ~Boundary() {}

    virtual const char *GetIndexArrayName() = 0;
    virtual const char *GetFieldArrayName() = 0;
    virtual int GetFieldArrayComponents() = 0;
    virtual vtkDoubleArray *GetSpatialFieldAmplitudes() = 0;

    int getBoundaryType();

public:
    friend class BoundaryCollection;
    friend class FDEMBoundaryWidget;
    friend class TriangulationActor;

protected:
    int boundaryType;
    int loadStep;
    int nodeSetId;

    int loadingTypeX;
    double amplitudeX;

    bool isAmplitudeSiteDependent;
    int spatialSample;
    QVector<QVector<double>> spatialFactor;

    bool isAmplitudeTimeDependent;
    int temporalSample;
    QVector<QVector<double>> temporalFactor;
};


class MechanicalBoundary : public Boundary
{
public:
   MechanicalBoundary();

   const char *GetIndexArrayName() override;
   const char *GetFieldArrayName() override;
   int GetFieldArrayComponents() override;
   vtkDoubleArray *GetSpatialFieldAmplitudes() override;

public:
   friend class FDEMMechanicalBoundaryWidget;
   friend class FDEMDocumentExporter;
   friend class BoundaryCollection;
private:
   int loadingTypeY;
   double amplitudeY;

   double localAxisXX;
   double localAxisXY;
   double localAxisYX;
   double localAxisYY;

   bool isContactBoundaryEnabled;
   bool isArtificialBoundaryEnabled;
   int artificialBoundary;
};

class HydraulicBoundary : public Boundary
{
public:
   HydraulicBoundary();

   const char *GetIndexArrayName() override;
   const char *GetFieldArrayName() override;
   int GetFieldArrayComponents() override;
   vtkDoubleArray *GetSpatialFieldAmplitudes() override;

public:
   friend class FDEMHydraulicBoundaryWidget;
   friend class FDEMDocumentExporter;

private:

};

class ThermalBoundary : public Boundary
{
public:
   ThermalBoundary();

   const char *GetIndexArrayName() override;
   const char *GetFieldArrayName() override;
   int GetFieldArrayComponents() override;
   vtkDoubleArray *GetSpatialFieldAmplitudes() override;

public:
   friend class FDEMThermalBoundaryWidget;
   friend class FDEMDocumentExporter;

private:

};

#endif // BOUNDARY_H
