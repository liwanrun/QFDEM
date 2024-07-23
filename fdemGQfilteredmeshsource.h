#ifndef FDEMGQFILTEREDMESHSOURCE_H
#define FDEMGQFILTEREDMESHSOURCE_H

#include "fdemmeshsource.h"

class FDEMGenerateGQFilteredMeshSource : public FDEMMeshSource
{
public:
    vtkTypeMacro(FDEMGenerateGQFilteredMeshSource, FDEMMeshSource);

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMGenerateGQFilteredMeshSource* New();

protected:
    FDEMGenerateGQFilteredMeshSource();

    ~FDEMGenerateGQFilteredMeshSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMGenerateGQFilteredMeshSource(const FDEMGenerateGQFilteredMeshSource&) = delete;

    void operator=(const FDEMGenerateGQFilteredMeshSource&) = delete;
};


class FDEMImportedGQFilteredMeshSource : public FDEMMeshSource
{
public:
    vtkTypeMacro(FDEMImportedGQFilteredMeshSource, FDEMMeshSource);

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMImportedGQFilteredMeshSource* New();

protected:
    FDEMImportedGQFilteredMeshSource();

    ~FDEMImportedGQFilteredMeshSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMImportedGQFilteredMeshSource(const FDEMImportedGQFilteredMeshSource&) = delete;

    void operator=(const FDEMImportedGQFilteredMeshSource&) = delete;
};

#endif // FDEMGQFILTEREDMESHSOURCE_H
