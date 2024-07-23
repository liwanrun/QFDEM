#include "fdemmeshsourcefactory.h"

#include  <exception>


vtkSmartPointer<FDEMMeshSource> GenerateMeshSourceFactory::CreatePrimitiveMeshSource()
{
    vtkSmartPointer<FDEMMeshSource> src = vtkSmartPointer<FDEMGeneratePrimitiveMeshSource>::New();
    return src;
}

vtkSmartPointer<FDEMMeshSource> GenerateMeshSourceFactory::CreateInsertGlobalQuadFilter()
{
    vtkSmartPointer<FDEMMeshSource> src = vtkSmartPointer<FDEMGenerateGQFilteredMeshSource>::New();
    return src;
}

vtkSmartPointer<FDEMMeshSource> GenerateMeshSourceFactory::CreateInsertLocalQuadFilter()
{
    vtkSmartPointer<FDEMMeshSource> src = vtkSmartPointer<FDEMGenerateLQFilteredMeshSource>::New();
    return src;
}

vtkSmartPointer<FDEMMeshSource> GenerateMeshSourceFactory::CreateSplitTriangulationFilter()
{
    vtkSmartPointer<FDEMMeshSource> src = vtkSmartPointer<FDEMGenerateSTFilteredMeshSource>::New();
    return src;
}

vtkSmartPointer<FDEMMeshSource> GenerateMeshSourceFactory::CreateThickQuadMeshSource()
{
    vtkSmartPointer<FDEMMeshSource> src = vtkSmartPointer<FDEMImportedThickQuadMeshSource>::New();
    return src;
}

vtkSmartPointer<FDEMMeshSource> ImportedMeshSourceFactory::CreatePrimitiveMeshSource()
{
    vtkSmartPointer<FDEMMeshSource> src = vtkSmartPointer<FDEMImportedPrimitiveMeshSource>::New();
    return src;
}

vtkSmartPointer<FDEMMeshSource> ImportedMeshSourceFactory::CreateInsertGlobalQuadFilter()
{
    vtkSmartPointer<FDEMMeshSource> src = vtkSmartPointer<FDEMImportedGQFilteredMeshSource>::New();
    return src;
}

vtkSmartPointer<FDEMMeshSource> ImportedMeshSourceFactory::CreateInsertLocalQuadFilter()
{
    vtkSmartPointer<FDEMMeshSource> src = vtkSmartPointer<FDEMImportedLQFilteredMeshSource>::New();
    return src;
}

vtkSmartPointer<FDEMMeshSource> ImportedMeshSourceFactory::CreateSplitTriangulationFilter()
{
    vtkSmartPointer<FDEMMeshSource> src = vtkSmartPointer<FDEMImportedSTFilteredMeshSource>::New();
    return src;
}

vtkSmartPointer<FDEMMeshSource> ImportedMeshSourceFactory::CreateThickQuadMeshSource()
{
    vtkSmartPointer<FDEMMeshSource> src = vtkSmartPointer<FDEMImportedThickQuadMeshSource>::New();
    return src;
}

QSharedPointer<FDEMMeshSourceFactory> FDEMMeshSourceFactory::CreateFactory(int type)
{
    if(0 == type)
    {
        QSharedPointer<FDEMMeshSourceFactory> factory =
                QSharedPointer<GenerateMeshSourceFactory>(new GenerateMeshSourceFactory);

        return factory;
    }
    else if(1 == type)
    {
        QSharedPointer<FDEMMeshSourceFactory> factory =
                QSharedPointer<ImportedMeshSourceFactory>(new ImportedMeshSourceFactory);

        return factory;
    }
    else
    {
        throw std::invalid_argument("Invalid factory argument. Please check out!");
    }
}
