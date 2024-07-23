#include "voronoidiagramsource.h"
#include "voronoidiagram.h"

#include <vtkInformation.h>
#include <vtkInformationVector.h>

vtkStandardNewMacro(VoronoiDiagramSource)

void VoronoiDiagramSource::PrintSelf(std::ostream &, vtkIndent)
{

}

void VoronoiDiagramSource::SetVoronoiDiagram(QSharedPointer<VoronoiDiagram> vd)
{
    this->voronoi = vd;
}

VoronoiDiagramSource::VoronoiDiagramSource()
{
    this->voronoi = nullptr;
    this->SetNumberOfInputPorts(0);
}

VoronoiDiagramSource::~VoronoiDiagramSource()
{

}

int VoronoiDiagramSource::RequestData(vtkInformation *vtkNotUsed(request),
                                      vtkInformationVector **vtkNotUsed(inputVector),
                                      vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    std::vector<QLineF> voronoi = this->voronoi->GetCroppedVoronoi();
    vtkIdType numPts = 2 * voronoi.size();
    vtkIdType numEls = 1 * voronoi.size();

    vtkPoints *newPoints;
    vtkCellArray *newLines;

    newPoints = vtkPoints::New();
    newPoints->Allocate(numPts);

    newLines = vtkCellArray::New();
    newLines->Allocate(numEls);

    for(QLineF line : voronoi)
    {
        vtkIdType i = newPoints->InsertNextPoint(line.x1(), line.y1(), 0.0);
        vtkIdType j = newPoints->InsertNextPoint(line.x2(), line.y2(), 0.0);
        newLines->InsertNextCell(2);
        newLines->InsertCellPoint(i);
        newLines->InsertCellPoint(j);
    }

    output->SetPoints(newPoints);
    newPoints->Delete();

    output->SetLines(newLines);
    newLines->Delete();

    return 1;
}

int VoronoiDiagramSource::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}
