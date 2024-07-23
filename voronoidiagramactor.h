#ifndef VORONOIDIAGRAMACTOR_H
#define VORONOIDIAGRAMACTOR_H

#include <QSharedPointer>

#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>

class VoronoiDiagram;
class VoronoiDiagramActor
{
public:
    VoronoiDiagramActor();
    ~VoronoiDiagramActor();

    vtkActor *GetVTKRenderActor();

    vtkPolyDataMapper *GetVTKActorMapper();

    void Update(const QSharedPointer<VoronoiDiagram>& vd);

    void SetInputConnection(vtkAlgorithmOutput* input);

    void SetVisibility(bool visible);

private:
    vtkActor *actor;
    vtkPolyDataMapper *mapper;
};

#endif // VORONOIDIAGRAMACTOR_H
