#ifndef FDEMMESHIMPORTER_H
#define FDEMMESHIMPORTER_H

#include <QString>
#include <QVector>

enum PhysicalType
{
    CONSTRAINT = 0,
    BLOCK,
    GRAIN
};

struct GmshEntity
{
    int dim;
    QVector<int> physicalTags;
};

struct GmshNode
{
    double x;
    double y;
    double z;
};

struct GmshElement
{
    int elemType;
    int entityTag;
    QVector<size_t> nodeTags;
};

class FDEMMeshImporter
{
public:
    FDEMMeshImporter();
    virtual ~FDEMMeshImporter();

public:
    void SetFileName(QString name);

    void Import();

private:
    virtual void ParseMeshFile();

    virtual void CreateConstrainedTriangulation();

protected:
    QString filename;
};

#endif // FDEMMESHIMPORTER_H
