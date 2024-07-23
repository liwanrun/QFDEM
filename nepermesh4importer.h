#ifndef NEPERMESH4IMPORTER_H
#define NEPERMESH4IMPORTER_H

#include "fdemmeshimporter.h"

#include <QVector>
#include <QTextStream>
#include <QSharedPointer>

class BlockCollection;
class ElemSetCollection;
class NeperMesh4Importer : public FDEMMeshImporter
{
public:
    NeperMesh4Importer();
    ~NeperMesh4Importer() override;

private:
    void ParseMeshFile() override;
    void CreateConstrainedTriangulation() override;

    void ParseGmshMeshFormat(QTextStream &in);
    void ParseGmshPhysicalNames(QTextStream &in);
    void ParseGmshEntities(QTextStream &in);
    void ParseGmshNodes(QTextStream &in);
    void ParseGmshElements(QTextStream &in);

    void CreateBlockTriangles(int index);
    void UpdateBlockConstraints(int index);
    void UpdateBlockGrains(int index);

private:
    QSharedPointer<BlockCollection>   blockCollection;
    QSharedPointer<ElemSetCollection> grainCollection;

    QHash<int, QString>             gmshPhysicalCurves;
    QHash<int, QString>             gmshPhysicalSurfaces;

    QHash<int, GmshEntity>          gmshCurves;
    QHash<int, GmshEntity>          gmshSurfaces;
    std::vector<GmshNode>           gmshNodes;
    std::vector<GmshElement>        gmshElements;
};

#endif // NEPERMESH4IMPORTER_H
