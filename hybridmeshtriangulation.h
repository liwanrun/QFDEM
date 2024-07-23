#ifndef HYBRIDMESHTRIANGULATION_H
#define HYBRIDMESHTRIANGULATION_H

#include "triangulation.h"

#include <QPointF>

class HybridMeshTriangulation : public Triangulation
{
public:
    HybridMeshTriangulation();
    ~HybridMeshTriangulation() override;

public:
    size_t InitTriangulationNodeIds() override;

    size_t InitTriangulationFaceIds() override;

    void UpdateGlobalQuadInsertionInfo() override;

    void UpdateLocalQuadInsertionInfo() override;

    void UpdateSplitTriangulationInfo() override;

    void UpdateInsertConstraintsInfo(QMap<QString, QSharedPointer<Primitive>>& primitive_map) override;

    void RestoreTriangulationStatus() override;

    void UpdateTriangulationConstraint(vtkUnstructuredGrid *ug) override;

public:
    friend class HybridMeshImporter;
    friend class FDEMImportedThickQuadMeshSource;

private:
    std::vector<QPointF> points;
    std::vector<std::array<size_t, 3>> triangles;
    std::vector<std::array<size_t, 4>> quads;
};

#endif // HYBRIDMESHTRIANGULATION_H
