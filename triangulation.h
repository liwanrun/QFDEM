#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include "cgaltypedef.h"
#include "cgalmeshinfo.h"

#include <QColor>
#include <QSharedPointer>

#include <vtkUnstructuredGrid.h>
#include <vtkInformationIntegerKey.h>
#include <vtkInformationStringKey.h>

class Primitive;
class Triangulation
{
public:
    enum ConstraintFlag
    {
        BOUNDARY = 0,
        FRACTURE = 1,
        BEDPLANE = 2
    };

    enum SourceFlag
    {
        GENERATED = 0,
        IMPORTED
    };

    enum FilterFLag
    {
        PRIMITIVE = 0,
        GQHANDLED = 1,
        LQHANDLED = 2,
        STHANDLED = 3,
        THICKQUAD = 4
    };

public:
    Triangulation();
    virtual ~Triangulation();

    int meshSourceFlag();
    void setMeshSourceFlag(int arg);

    int meshFilterFlag();
    void setMeshFilterFlag(int arg);

    static void setConstraintVisible(bool flag);
    static bool isConstraintVisible();

    QColor getFaceColor() const;
    void setFaceColor(const QColor &color);

    QColor getEdgeColor() const;
    void setEdgeColor(const QColor &color);

    double getShrinkFactor() const;
    void setShrinkFactor(double newShrinkFactor);

    size_t GetNumberOfPoints() const;

    size_t GetNumberOfTriangles() const;

    size_t GetNumberOfQuads() const;

public:
    virtual size_t InitTriangulationNodeIds();

    virtual size_t InitTriangulationFaceIds();

    virtual void UpdateGlobalQuadInsertionInfo();

    virtual void UpdateLocalQuadInsertionInfo();

    virtual void UpdateSplitTriangulationInfo();

    virtual void UpdateInsertConstraintsInfo(QMap<QString, QSharedPointer<Primitive>>& constraints);

    virtual void RestoreTriangulationStatus();

    virtual void UpdateTriangulationConstraint(vtkUnstructuredGrid* ug);

    virtual double GetTriangulationCharacteristicLength();

    virtual double GetTriangulationMinimumSize();

    virtual double GetTriangulationMinimumArea();

public:
    friend class GmshMeshImporter;
    friend class NeperMesh4Importer;

private:
    int mesh_source_flag;
    int mesh_filter_flag;
    static bool is_constraint_visible;

protected:
    size_t numPoints;
    size_t numTriangles;
    size_t numQuads;

    size_t numGrains;
    size_t grainOffs;

    double shrinkFactor;
    QColor face_color;
    QColor edge_color;
};

#endif // TRIANGULATION_H
