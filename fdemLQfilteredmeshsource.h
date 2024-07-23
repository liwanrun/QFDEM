#ifndef FDEMLQFILTEREDMESHSOURCE_H
#define FDEMLQFILTEREDMESHSOURCE_H

#include "fdemmeshsource.h"

class FDEMGenerateLQFilteredMeshSource : public FDEMMeshSource
{
public:
    vtkTypeMacro(FDEMGenerateLQFilteredMeshSource, FDEMMeshSource);

    static FDEMGenerateLQFilteredMeshSource* New();

    void PrintSelf(ostream&, vtkIndent) override;

protected:
    FDEMGenerateLQFilteredMeshSource();

    ~FDEMGenerateLQFilteredMeshSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMGenerateLQFilteredMeshSource(const FDEMGenerateLQFilteredMeshSource&) = delete;

    void operator=(const FDEMGenerateLQFilteredMeshSource&) = delete;
};


class FDEMImportedLQFilteredMeshSource : public FDEMMeshSource
{
public:
    vtkTypeMacro(FDEMImportedLQFilteredMeshSource, FDEMMeshSource);

    static FDEMImportedLQFilteredMeshSource* New();

    void PrintSelf(ostream&, vtkIndent) override;

protected:
    FDEMImportedLQFilteredMeshSource();

    ~FDEMImportedLQFilteredMeshSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMImportedLQFilteredMeshSource(const FDEMImportedLQFilteredMeshSource&) = delete;

    void operator=(const FDEMImportedLQFilteredMeshSource&) = delete;
};

#endif // FDEMLQFILTEREDMESHSOURCE_H
