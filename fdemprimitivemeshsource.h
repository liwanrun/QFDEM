#ifndef FDEMPRIMITIVEMESHSOURCE_H
#define FDEMPRIMITIVEMESHSOURCE_H

#include "fdemmeshsource.h"

class FDEMGeneratePrimitiveMeshSource : public FDEMMeshSource
{
public:
    vtkTypeMacro(FDEMGeneratePrimitiveMeshSource, FDEMMeshSource)

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMGeneratePrimitiveMeshSource* New();

protected:
    FDEMGeneratePrimitiveMeshSource();

    ~FDEMGeneratePrimitiveMeshSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMGeneratePrimitiveMeshSource(const FDEMGeneratePrimitiveMeshSource&) = delete;

    void operator=(const FDEMGeneratePrimitiveMeshSource&) = delete;
};


class FDEMImportedPrimitiveMeshSource : public FDEMMeshSource
{
public:
    vtkTypeMacro(FDEMImportedPrimitiveMeshSource, FDEMMeshSource)

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMImportedPrimitiveMeshSource* New();

protected:
    FDEMImportedPrimitiveMeshSource();

    ~FDEMImportedPrimitiveMeshSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMImportedPrimitiveMeshSource(const FDEMImportedPrimitiveMeshSource&) = delete;

    void operator=(const FDEMImportedPrimitiveMeshSource&) = delete;
};

#endif // FDEMPRIMITIVEMESHSOURCE_H
