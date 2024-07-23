#include "fdemmeshshrinkfactory.h"

vtkSmartPointer<FDEMMeshShrink> GenerateMeshShrinkFactory::CreateInsertGlobalQuadShrink()
{
    vtkSmartPointer<FDEMMeshShrink> flt = vtkSmartPointer<FDEMGenerateGQFilteredMeshShrink>::New();
    return flt;
}

vtkSmartPointer<FDEMMeshShrink> GenerateMeshShrinkFactory::CreateInsertLocalQuadShrink()
{
    vtkSmartPointer<FDEMMeshShrink> flt = vtkSmartPointer<FDEMGenerateLQFilteredMeshShrink>::New();
    return flt;
}

vtkSmartPointer<FDEMMeshShrink> GenerateMeshShrinkFactory::CreateSplitTriangulationShrink()
{
    vtkSmartPointer<FDEMMeshShrink> flt = vtkSmartPointer<FDEMGenerateSTFilteredMeshShrink>::New();
    return flt;
}

vtkSmartPointer<FDEMMeshShrink> ImportedMeshShrinkFactory::CreateInsertGlobalQuadShrink()
{
    vtkSmartPointer<FDEMMeshShrink> flt = vtkSmartPointer<FDEMImportedGQFilteredMeshShrink>::New();
    return flt;
}

vtkSmartPointer<FDEMMeshShrink> ImportedMeshShrinkFactory::CreateInsertLocalQuadShrink()
{
    vtkSmartPointer<FDEMMeshShrink> flt = vtkSmartPointer<FDEMImportedLQFilteredMeshShrink>::New();
    return flt;
}

vtkSmartPointer<FDEMMeshShrink> ImportedMeshShrinkFactory::CreateSplitTriangulationShrink()
{
    vtkSmartPointer<FDEMMeshShrink> flt = vtkSmartPointer<FDEMImportedSTFilteredMeshShrink>::New();
    return flt;
}

QSharedPointer<FDEMMeshShrinkFactory> FDEMMeshShrinkFactory::CreateFactory(int type)
{
    if(0 == type)
    {
        QSharedPointer<FDEMMeshShrinkFactory> factory =
                QSharedPointer<GenerateMeshShrinkFactory>(new GenerateMeshShrinkFactory);

        return factory;
    }
    else if(1 == type)
    {
        QSharedPointer<FDEMMeshShrinkFactory> factory =
                QSharedPointer<ImportedMeshShrinkFactory>(new ImportedMeshShrinkFactory);

        return factory;
    }
    else
    {
        throw std::invalid_argument("Invalid factory argument. Please check out!");
    }
}
