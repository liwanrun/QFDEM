#include "voronoidiagramactor.h"
#include "voronoidiagramsource.h"

#include <QDebug>
#include <vtkNamedColors.h>

VoronoiDiagramActor::VoronoiDiagramActor()
{
    qDebug() << "VoronoiDiagramActor()";

    this->mapper = vtkPolyDataMapper::New();

    this->actor = vtkActor::New();
    this->actor->SetMapper(this->mapper);
    this->actor->SetVisibility(false);
    this->actor->SetPickable(false);
    this->actor->SetDragable(false);

    vtkNew<vtkNamedColors> colors;
    double *rgb = colors->GetColor3d("tomato").GetData();
    this->actor->GetProperty()->SetColor(rgb);
    this->actor->GetProperty()->SetLineWidth(2.0);
    this->actor->GetProperty()->SetAmbient(1.0);
    this->actor->GetProperty()->SetDiffuse(0.0);
    this->actor->GetProperty()->SetLighting(false);
}

VoronoiDiagramActor::~VoronoiDiagramActor()
{
    qDebug() << "~VoronoiDiagramActor()";

    this->mapper->Delete();
    this->actor->Delete();
}

vtkActor *VoronoiDiagramActor::GetVTKRenderActor()
{
    return this->actor;
}

vtkPolyDataMapper *VoronoiDiagramActor::GetVTKActorMapper()
{
    return this->mapper;
}

void VoronoiDiagramActor::Update(const QSharedPointer<VoronoiDiagram> &vd)
{
    vtkSmartPointer<VoronoiDiagramSource> source = vtkSmartPointer<VoronoiDiagramSource>::New();
    source->SetVoronoiDiagram(vd);
    this->SetInputConnection(source->GetOutputPort());
}

void VoronoiDiagramActor::SetInputConnection(vtkAlgorithmOutput *input)
{
    this->mapper->SetInputConnection(input);
}

void VoronoiDiagramActor::SetVisibility(bool visible)
{
    this->actor->SetVisibility(visible);
}
