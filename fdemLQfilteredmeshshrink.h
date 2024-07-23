#ifndef FDEMLQFILTEREDMESHSHRINK_H
#define FDEMLQFILTEREDMESHSHRINK_H

#include "fdemmeshshrink.h"

class FDEMGenerateLQFilteredMeshShrink : public FDEMMeshShrink
{
public:
    vtkTypeMacro(FDEMGenerateLQFilteredMeshShrink, FDEMMeshShrink);

    static FDEMGenerateLQFilteredMeshShrink* New();

    void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
    FDEMGenerateLQFilteredMeshShrink();

    ~FDEMGenerateLQFilteredMeshShrink() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMGenerateLQFilteredMeshShrink(const FDEMGenerateLQFilteredMeshShrink&) = delete;

    void operator=(const FDEMGenerateLQFilteredMeshShrink&) = delete;
};


class FDEMImportedLQFilteredMeshShrink : public FDEMMeshShrink
{
public:
    vtkTypeMacro(FDEMImportedLQFilteredMeshShrink, FDEMMeshShrink);

    static FDEMImportedLQFilteredMeshShrink* New();

    void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
    FDEMImportedLQFilteredMeshShrink();

    ~FDEMImportedLQFilteredMeshShrink() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMImportedLQFilteredMeshShrink(const FDEMImportedLQFilteredMeshShrink&) = delete;

    void operator=(const FDEMImportedLQFilteredMeshShrink&) = delete;
};

#endif // FDEMLQFILTEREDMESHSHRINK_H
