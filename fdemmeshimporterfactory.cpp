#include "fdemmeshimporterfactory.h"
#include <QFileInfo>

FDEMMeshImporterFactory::FDEMMeshImporterFactory()
{

}

QSharedPointer<FDEMMeshImporter> FDEMMeshImporterFactory::Create(QString fname)
{
    QFileInfo fileInfo(fname);
    QString fname_suffix = fileInfo.suffix();
    QSharedPointer<FDEMMeshImporter> importer = nullptr;

    if(QString("msh") == fname_suffix)
    {
        importer = QSharedPointer<GmshMeshImporter>(new GmshMeshImporter);
    }
    else if(QString("msh4") == fname_suffix)
    {
        importer = QSharedPointer<NeperMesh4Importer>(new NeperMesh4Importer);
    }
    else if(QString("ply") == fname_suffix)
    {
        importer = QSharedPointer<DistMeshImporter>(new DistMeshImporter);
    }
    else if(QString("inp") == fname_suffix)
    {
        importer = QSharedPointer<AbaqusMeshImporter>(new AbaqusMeshImporter);
    }
    else if(QString("Neper File (*.msh)") == fname_suffix)
    {
        importer = QSharedPointer<NeperMeshImporter>(new NeperMeshImporter);
    }
    else if(QString("Hybrid Mesh (*.msh)") == fname_suffix)
    {
        importer = QSharedPointer<HybridMeshImporter>(new HybridMeshImporter);
    }

    return importer;
}
