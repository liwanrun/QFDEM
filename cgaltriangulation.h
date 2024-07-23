#ifndef CGALTRIANGULATION_H
#define CGALTRIANGULATION_H

#include "cgaltypedef.h"
#include "cgalmeshinfo.h"

#include <QVector>
#include <QPointF>
#include <QColor>
#include <QSharedPointer>

#include <vtkDoubleArray.h>


class vtkPolyData;
class CGALTriangulation
{
public:
    CGALTriangulation();
    ~CGALTriangulation();

    int getMaxIterationNumber() const;
    void setMaxIterationNumber(int value);

    double getSize() const;
    void setSize(double size);

    double getShape() const;
    void setShape(double shape);

    QColor getColor() const;
    void setColor(const QColor &color);

    const QVector<CDTP::Point>& getSeedPoints() const;
    void setSeedPoints(const QVector<QPointF> &seeds);

    const CDTP &getTriangleMesh() const;

public:
    QVector<CDTP::Constraint_id> InsertConstraints(vtkPolyData *dataset);

    Opt_code RefineDelaunayMesh();

    void InitMeshEntityId();

    void CreateMeshVertexInfo();

    void CreateMeshFaceInfo();

    void UpdateMeshVertexInfo();

    void UpdateMeshFaceInfo();

private:


private:
    int n_iters;
    double m_size;
    double m_shape;
    QColor m_color;
    QVector<CDTP::Point> seeds;

    CDTP mesh;
};

#endif // CGALTRIANGULATION_H
