#ifndef CONSTRAINEDDELAUNAYTRIANGULATION_H
#define CONSTRAINEDDELAUNAYTRIANGULATION_H

#include <map>
#include <set>

#include <QString>
#include <QPointF>
#include <QVector>

#include "triangulation.h"

class vtkPolyData;
class ConstrainedDelaunayTriangulation : public Triangulation
{
public:
    ConstrainedDelaunayTriangulation();
    ~ConstrainedDelaunayTriangulation() override;

    int getMaxIterationNumber() const;
    void setMaxIterationNumber(int value);

    double getSize() const;
    void setSize(double size);

    double getShape() const;
    void setShape(double shape);

    const QVector<CDTP::Point>& getSeedPoints() const;
    void setSeedPoints(const QVector<QPointF> &seeds);

    CDTP &GetCGALTriangulation();

public:
    void InsertConstraints(vtkPolyData *dataset, int flag);

    std::map<CDTP::Constraint_id, int>& GetConstraintIds();

    std::set<CDTP::Edge> &GetExtrinsicConstrainedEdges();

    Opt_code RefineDelaunayMesh(const QString &blocId);

    void ClearRefinedDelaunayMesh();

public:
    size_t InitTriangulationNodeIds() override;

    size_t InitTriangulationFaceIds() override;

    void UpdateGlobalQuadInsertionInfo() override;

    void UpdateLocalQuadInsertionInfo() override;

    void UpdateSplitTriangulationInfo() override;

    void UpdateInsertConstraintsInfo(QMap<QString, QSharedPointer<Primitive>>& primitive_map) override;

    void RestoreTriangulationStatus() override;

    void UpdateTriangulationConstraint(vtkUnstructuredGrid *ug) override;

    double GetTriangulationCharacteristicLength() override;

    double GetTriangulationMinimumSize() override;

    double GetTriangulationMinimumArea() override;

private:
    void InitSplitVertices();

    void CreateTriangulationNodeInfo();

    void CreateTriangulationFaceInfo();

    void UpdateTriangulationNodeInfo();

    void UpdateTriangulationFaceInfo();

private:
    void ConstructConstrainedVertices(const CDTP::Vertex_handle v);

    void ConstructIncidentAngles(const CDTP::Vertex_handle v);

    int  ComputeIncidentFaceRank(const CDTP::Vertex_handle v, CDTP::Face_handle fh);

    size_t ComputeNumberOfTriangles();

    size_t ComputeNumberOfQuads();

    int FindOptimalEdgeIncidentLine(const CDTP::Face_handle &f, const Epick::Line_2 &l);

    int FindNearestVertexToPoint(const CDTP::Face_handle &f, const Epick::Point_2 &p);

private:
    double m_shape;
    double m_size;
    int m_iters;
    QVector<CDTP::Point> seeds;

    CDTP mesh;

private:
    std::map<CDTP::Constraint_id, int> constraintIds;
    std::set<CDTP::Edge> extrinsicConstrainedEdges;

    std::set<CDTP::Vertex_handle> splitVertices;
    QVector<CDTP::Vertex_handle> constrainedVertices;
    QVector<double> incidentAngles;
};

#endif // CONSTRAINEDDELAUNAYTRIANGULATION_H
