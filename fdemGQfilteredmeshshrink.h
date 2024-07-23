#ifndef FDEMGQFILTEREDMESHSHRINK_H
#define FDEMGQFILTEREDMESHSHRINK_H

#include "fdemmeshshrink.h"

class FDEMGenerateGQFilteredMeshShrink : public FDEMMeshShrink
{
public:
    vtkTypeMacro(FDEMGenerateGQFilteredMeshShrink, FDEMMeshShrink);

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMGenerateGQFilteredMeshShrink* New();

protected:
    FDEMGenerateGQFilteredMeshShrink();

    ~FDEMGenerateGQFilteredMeshShrink() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMGenerateGQFilteredMeshShrink(const FDEMGenerateGQFilteredMeshShrink&) = delete;

    void operator=(const FDEMGenerateGQFilteredMeshShrink&) = delete;
};


class FDEMImportedGQFilteredMeshShrink : public FDEMMeshShrink
{
public:
    vtkTypeMacro(FDEMImportedGQFilteredMeshShrink, FDEMMeshShrink);

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMImportedGQFilteredMeshShrink* New();

protected:
    FDEMImportedGQFilteredMeshShrink();

    ~FDEMImportedGQFilteredMeshShrink() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMImportedGQFilteredMeshShrink(const FDEMImportedGQFilteredMeshShrink&) = delete;

    void operator=(const FDEMImportedGQFilteredMeshShrink&) = delete;
};

#endif // FDEMGQFILTEREDMESHSHRINK_H
