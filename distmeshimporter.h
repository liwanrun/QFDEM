#ifndef DISTMESHIMPORTER_H
#define DISTMESHIMPORTER_H

#include "fdemmeshimporter.h"

#include <QVector>
#include <QSharedPointer>

class BlockCollection;
class DistMeshImporter : public FDEMMeshImporter
{
public:
    DistMeshImporter();
    ~DistMeshImporter() override;

private:
    void ParseMeshFile() override;
    void CreateConstrainedTriangulation() override;

    void CreateBlockTriangles();
    void UpdateBlockConstraints();

private:
    int numVerts;
    int numFaces;
    int numEdges;

    QVector<std::array<double, 3>> polyVerts;
    QVector<std::array<int, 3>> polyFaces;
    QVector<std::array<int, 2>> polyEdges;
    QSharedPointer<BlockCollection> blockCollection;
};

#endif // DISTMESHIMPORTER_H
