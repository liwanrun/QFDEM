#ifndef FDEMMESHSHRINKFACTORY_H
#define FDEMMESHSHRINKFACTORY_H

#include "fdemGQfilteredmeshshrink.h"
#include "fdemLQfilteredmeshshrink.h"
#include "fdemSTfilteredmeshshrink.h"

class FDEMMeshShrinkFactory
{
public:
    virtual vtkSmartPointer<FDEMMeshShrink> CreateInsertGlobalQuadShrink() = 0;

    virtual vtkSmartPointer<FDEMMeshShrink> CreateInsertLocalQuadShrink() = 0;

    virtual vtkSmartPointer<FDEMMeshShrink> CreateSplitTriangulationShrink() = 0;

    static QSharedPointer<FDEMMeshShrinkFactory> CreateFactory(int type);
};

class GenerateMeshShrinkFactory : public FDEMMeshShrinkFactory
{
public:
    vtkSmartPointer<FDEMMeshShrink> CreateInsertGlobalQuadShrink() override;

    vtkSmartPointer<FDEMMeshShrink> CreateInsertLocalQuadShrink() override;

    vtkSmartPointer<FDEMMeshShrink> CreateSplitTriangulationShrink() override;
};

class ImportedMeshShrinkFactory : public FDEMMeshShrinkFactory
{
public:
    vtkSmartPointer<FDEMMeshShrink> CreateInsertGlobalQuadShrink() override;

    vtkSmartPointer<FDEMMeshShrink> CreateInsertLocalQuadShrink() override;

    vtkSmartPointer<FDEMMeshShrink> CreateSplitTriangulationShrink() override;
};
#endif // FDEMMESHSHRINKFACTORY_H
