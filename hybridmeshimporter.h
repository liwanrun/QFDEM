#ifndef HYBRIDMESHIMPORTER_H
#define HYBRIDMESHIMPORTER_H

#include "fdemmeshimporter.h"

#include <QSharedPointer>
#include <QTextStream>
#include <QPointF>

#include <array>
#include <vector>

class BlockCollection;
class HybridMeshImporter : public FDEMMeshImporter
{
public:
    HybridMeshImporter();
    ~HybridMeshImporter() override;

private:
    void ParseMeshFile() override;

    void CreateConstrainedTriangulation() override;

    void ParseGmshMeshFormat(QTextStream &in);

    void ParseGmshPhysicalNames(QTextStream &in);

    void ParseGmshEntities(QTextStream &in);

    void ParseGmshNodes(QTextStream &in);

    void ParseGmshElements(QTextStream &in);

private:
    QSharedPointer<BlockCollection> blockCollection;
    QMap<int, QString> physicalGroups;

    std::vector<QPointF> points;
    std::vector<std::array<size_t, 3>> triangles;
    std::vector<std::array<size_t, 4>> quads;
};

#endif // HYBRIDMESHIMPORTER_H
