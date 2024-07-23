#include "fdemmeshimporter.h"
#include "blockcollection.h"
#include "actorcollection.h"

FDEMMeshImporter::FDEMMeshImporter()
{

}

FDEMMeshImporter::~FDEMMeshImporter()
{

}

void FDEMMeshImporter::SetFileName(QString name)
{
    this->filename = name;
}

void FDEMMeshImporter::Import()
{
    this->ParseMeshFile();
    this->CreateConstrainedTriangulation();
}

void FDEMMeshImporter::ParseMeshFile()
{
    return;
}

void FDEMMeshImporter::CreateConstrainedTriangulation()
{
    return;
}
