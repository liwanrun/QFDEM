#ifndef NEPERMESHIMPORTER_H
#define NEPERMESHIMPORTER_H

#include "fdemmeshimporter.h"

#include <QSharedPointer>
#include <QTextStream>
#include <QPointF>
#include <QSet>

#include <vector>
using namespace std;

class BlockCollection;
class NeperMeshImporter : public FDEMMeshImporter
{
public:
    NeperMeshImporter();
    ~NeperMeshImporter() override;

private:
    void ParseMeshFile() override;

    void CreateConstrainedTriangulation() override;

    void ParseNeperMeshFormat(QTextStream &in);

    void ParseNeperNodes(QTextStream &in);

    void ParseNeperElements(QTextStream &in);

    void ParseNeperNodeSets(QTextStream &in);

    void ParseNeperPhysicalNames(QTextStream &in);

    void ParseElsetOrientations(QTextStream &in);

private:
    QSharedPointer<BlockCollection> blockCollection;

    QMap<QString, QSet<size_t>> boundaryNodeSets;
    QMap<int, int>     edgeEntityPhysicalMap;       //entityTag->physicalTag
    QMap<int, int>     faceEntityPhysicalMap;       //entityTag->physicalTag

    vector<QPointF> points;
    QMap<int, vector<array<size_t, 2>>> physicalEdgesMap;     //numEdgePhysicalGroup
    QMap<int, vector<array<size_t, 3>>> physicalFacesMap;     //numFacePhysicalGroup
};

#endif // NEPERMESHIMPORTER_H
