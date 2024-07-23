#ifndef GMSHMESHIMPORTER_H
#define GMSHMESHIMPORTER_H

#include "fdemmeshimporter.h"

#include <QSharedPointer>
#include <QTextStream>

#include <vector>

class BlockCollection;
class ElemSetCollection;
class GmshMeshImporter : public FDEMMeshImporter
{
public:
    GmshMeshImporter();
    ~GmshMeshImporter() override;

private:
    void ParseMeshFile() override;
    void CreateConstrainedTriangulation() override;

    void CreateBlockTriangles();
    void UpdateBlockConstraints();
    void UpdateBlockGrains();
    void InitBlockGrainInformation();
    void UpdatePhysicalGroupId();

    void ParseGmshMeshFormat(QTextStream &in);
    void ParseGmshPhysicalNames(QTextStream &in);
    void ParseGmshEntities(QTextStream &in);
    void ParseGmshNodes(QTextStream &in);
    void ParseGmshElements(QTextStream &in);

private:
    QSharedPointer<BlockCollection>     blockCollection;
    QSharedPointer<ElemSetCollection>   grainCollection;

    QHash<int, QString>                 gmshPhysicalCurves;
    QHash<int, QString>                 gmshPhysicalSurfaces;
    QHash<int, QVector<GmshElement>>    physicalCurveToElements;
    QHash<int, QVector<GmshElement>>    physicalSurfaceToElements;

    QHash<int, GmshEntity>              gmshCurves;
    QHash<int, GmshEntity>              gmshSurfaces;
    std::vector<GmshNode>               gmshNodes;
};

#endif // GMSHMESHIMPORTER_H
