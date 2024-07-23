#ifndef ABAQUSMESHIMPORTER_H
#define ABAQUSMESHIMPORTER_H

#include "fdemmeshimporter.h"

#include <QSharedPointer>
#include <QTextStream>
#include <QTransform>
#include <QPointF>

#include <set>
#include <stack>
#include <vector>
using namespace std;

class BlockCollection;
class AbaqusMeshImporter : public FDEMMeshImporter
{
public:
    AbaqusMeshImporter();
    ~AbaqusMeshImporter() override {}

private:
    void ParseMeshFile() override;

    void CreateConstrainedTriangulation() override;

private:
    QString ParseAsteriskNode(QTextStream &in, vector<array<double, 3>> &points);

    QString ParseAsteriskElement(QTextStream &in, vector<array<size_t, 3>> &triangles);

    QString ParseInstanceTransform(QTextStream &in);

private:
    QSharedPointer<BlockCollection> blockCollection;

    //Parts
    QMap<QString, vector<array<double, 3>>>    partGeometryMap;
    QMap<QString, vector<array<size_t, 3>>> partTriangleMap;

    //Instances
    QMap<QString, vector<array<double, 3>>>    instanceGeometryMap;
    QMap<QString, vector<array<size_t, 3>>> instanceTriangleMap;

    stack<QString> parserStack;
    double translation[3];
};

#endif // ABAQUSMESHIMPORTER_H
