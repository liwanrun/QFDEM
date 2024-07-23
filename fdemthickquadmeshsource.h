#ifndef FDEMTHICKQUADMESHSOURCE_H
#define FDEMTHICKQUADMESHSOURCE_H

#include "fdemmeshsource.h"

class FDEMImportedThickQuadMeshSource : public FDEMMeshSource
{
public:
    vtkTypeMacro(FDEMImportedThickQuadMeshSource, FDEMMeshSource)

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMImportedThickQuadMeshSource *New();

protected:
    FDEMImportedThickQuadMeshSource();

    ~FDEMImportedThickQuadMeshSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *outputVector) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMImportedThickQuadMeshSource(const FDEMImportedThickQuadMeshSource&) = delete;

    void operator=(const FDEMImportedThickQuadMeshSource&) = delete;
};

#endif // FDEMTHICKQUADMESHSOURCE_H
