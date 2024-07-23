#ifndef FDEMSTFILTEREDMESHSHRINK_H
#define FDEMSTFILTEREDMESHSHRINK_H

#include "fdemmeshshrink.h"

class FDEMGenerateSTFilteredMeshShrink : public FDEMMeshShrink
{
public:
    vtkTypeMacro(FDEMGenerateSTFilteredMeshShrink, FDEMMeshShrink);

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMGenerateSTFilteredMeshShrink* New();

protected:
    FDEMGenerateSTFilteredMeshShrink();

    ~FDEMGenerateSTFilteredMeshShrink() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMGenerateSTFilteredMeshShrink(const FDEMGenerateSTFilteredMeshShrink&) = delete;

    void operator=(const FDEMGenerateSTFilteredMeshShrink&) = delete;
};


class FDEMImportedSTFilteredMeshShrink : public FDEMMeshShrink
{
public:
    vtkTypeMacro(FDEMImportedSTFilteredMeshShrink, FDEMMeshShrink);

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMImportedSTFilteredMeshShrink* New();

protected:
    FDEMImportedSTFilteredMeshShrink();

    ~FDEMImportedSTFilteredMeshShrink() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMImportedSTFilteredMeshShrink(const FDEMImportedSTFilteredMeshShrink&) = delete;

    void operator=(const FDEMImportedSTFilteredMeshShrink&) = delete;
};

#endif // FDEMSTFILTEREDMESHSHRINK_H
