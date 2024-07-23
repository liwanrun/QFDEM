#ifndef FDEMSTFILTEREDMESHSOURCE_H
#define FDEMSTFILTEREDMESHSOURCE_H

#include "fdemmeshsource.h"

class FDEMGenerateSTFilteredMeshSource : public FDEMMeshSource
{
public:
    vtkTypeMacro(FDEMGenerateSTFilteredMeshSource, FDEMMeshSource);

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMGenerateSTFilteredMeshSource* New();

protected:
    FDEMGenerateSTFilteredMeshSource();

    ~FDEMGenerateSTFilteredMeshSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMGenerateSTFilteredMeshSource(const FDEMGenerateSTFilteredMeshSource&) = delete;

    void operator=(const FDEMGenerateSTFilteredMeshSource&) = delete;
};


class FDEMImportedSTFilteredMeshSource : public FDEMMeshSource
{
public:
    vtkTypeMacro(FDEMImportedSTFilteredMeshSource, FDEMMeshSource);

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMImportedSTFilteredMeshSource* New();

protected:
    FDEMImportedSTFilteredMeshSource();

    ~FDEMImportedSTFilteredMeshSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMImportedSTFilteredMeshSource(const FDEMImportedSTFilteredMeshSource&) = delete;

    void operator=(const FDEMImportedSTFilteredMeshSource&) = delete;
};

#endif // FDEMSTFILTEREDMESHSOURCE_H
