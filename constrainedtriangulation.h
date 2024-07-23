#ifndef CONSTRAINEDTRIANGULATION_H
#define CONSTRAINEDTRIANGULATION_H

#include <QPointF>
#include <QVector>

#include "triangulation.h"

class ConstrainedTriangulation : public Triangulation
{
public:
    ConstrainedTriangulation();
    ~ConstrainedTriangulation() override;

    CT &GetCGALTriangulation();

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

    std::set<CT::Edge>& GetIntrinsicConstrainedEdges();

private:
    void InitSplitVertices();

    void CreateTriangulationNodeInfo();

    void CreateTriangulationFaceInfo();

    void UpdateTriangulationNodeInfo();

    void UpdateTriangulationFaceInfo();

private:
    void ConstructConstrainedVertices(const CT::Vertex_handle v);

    void ConstructIncidentAngles(const CT::Vertex_handle v);

    int  ComputeIncidentFaceRank(const CT::Vertex_handle v, CT::Face_handle fh);

    size_t ComputeNumberOfTriangles();

    size_t ComputeNumberOfQuads();

    int FindOptimalEdgeIncidentLine(const CT::Face_handle &f, const Epick::Line_2 &l);

    int FindNearestVertexToPoint(const CT::Face_handle &f, const Epick::Point_2 &p);

private:
    CT mesh;

private:
    std::set<CT::Edge> intrinsicConstrainedEdges;

    std::set<CT::Vertex_handle> splitVertices;
    QVector<CT::Vertex_handle> constrainedVertices;
    QVector<double> incidentAngles;
};

#endif // CONSTRAINEDTRIANGULATION_H
