#ifndef VORONOIDIAGRAMSOURCE_H
#define VORONOIDIAGRAMSOURCE_H

#include <QSharedPointer>
#include <vtkPolyDataAlgorithm.h>

class VoronoiDiagram;
class VoronoiDiagramSource : public vtkPolyDataAlgorithm
{
public:
    vtkTypeMacro(VoronoiDiagramSource, vtkPolyDataAlgorithm)

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static VoronoiDiagramSource* New();

    void SetVoronoiDiagram(QSharedPointer<VoronoiDiagram> vd);

protected:
    VoronoiDiagramSource();

    ~VoronoiDiagramSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

protected:
    QSharedPointer<VoronoiDiagram> voronoi;

private:
    VoronoiDiagramSource(const VoronoiDiagramSource&) = delete;

    void operator=(const VoronoiDiagramSource&) = delete;
};

#endif // VORONOIDIAGRAMSOURCE_H
