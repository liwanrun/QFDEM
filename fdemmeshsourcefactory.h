#ifndef FDEMMESHSOURCEFACTORY_H
#define FDEMMESHSOURCEFACTORY_H

#include "fdemprimitivemeshsource.h"
#include "fdemGQfilteredmeshsource.h"
#include "fdemLQfilteredmeshsource.h"
#include "fdemSTfilteredmeshsource.h"
#include "fdemthickquadmeshsource.h"

class FDEMMeshSourceFactory
{
public:
    virtual ~FDEMMeshSourceFactory() {}

public:
    virtual vtkSmartPointer<FDEMMeshSource> CreatePrimitiveMeshSource() = 0;

    virtual vtkSmartPointer<FDEMMeshSource> CreateInsertGlobalQuadFilter() = 0;

    virtual vtkSmartPointer<FDEMMeshSource> CreateInsertLocalQuadFilter() = 0;

    virtual vtkSmartPointer<FDEMMeshSource> CreateSplitTriangulationFilter() = 0;

    virtual vtkSmartPointer<FDEMMeshSource> CreateThickQuadMeshSource() = 0;

    static QSharedPointer<FDEMMeshSourceFactory> CreateFactory(int type);
};

class GenerateMeshSourceFactory : public FDEMMeshSourceFactory
{
public:
    vtkSmartPointer<FDEMMeshSource> CreatePrimitiveMeshSource() override;

    vtkSmartPointer<FDEMMeshSource> CreateInsertGlobalQuadFilter() override;

    vtkSmartPointer<FDEMMeshSource> CreateInsertLocalQuadFilter() override;

    vtkSmartPointer<FDEMMeshSource> CreateSplitTriangulationFilter() override;

    vtkSmartPointer<FDEMMeshSource> CreateThickQuadMeshSource() override;
};

class ImportedMeshSourceFactory : public FDEMMeshSourceFactory
{
public:
    vtkSmartPointer<FDEMMeshSource> CreatePrimitiveMeshSource() override;

    vtkSmartPointer<FDEMMeshSource> CreateInsertGlobalQuadFilter() override;

    vtkSmartPointer<FDEMMeshSource> CreateInsertLocalQuadFilter() override;

    vtkSmartPointer<FDEMMeshSource> CreateSplitTriangulationFilter() override;

    vtkSmartPointer<FDEMMeshSource> CreateThickQuadMeshSource() override;
};


#endif // FDEMMESHSOURCEFACTORY_H
