#ifndef FDEMSHAPEFILTER_H
#define FDEMSHAPEFILTER_H

#include <vtkPolyDataAlgorithm.h>

#include <QVector>
#include <QPointF>

class CGALArrangement;

class FDEMShapeClipFilter : public vtkPolyDataAlgorithm
{
public:
    vtkTypeMacro(FDEMShapeClipFilter,vtkPolyDataAlgorithm);

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMShapeClipFilter* New();

protected:
    FDEMShapeClipFilter();

    ~FDEMShapeClipFilter() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

public:
    void SetSeedPoints(vtkDoubleArray *seeds);

private:
    vtkDoubleArray* clip_seeds;

    CGALArrangement* arr_location;
    CGALArrangement* arr_clipping;

private:
    FDEMShapeClipFilter(const FDEMShapeClipFilter&) = delete;

    void operator=(const FDEMShapeClipFilter&) = delete;
};

#endif // FDEMSHAPEFILTER_H
