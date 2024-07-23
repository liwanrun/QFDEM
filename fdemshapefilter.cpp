#include "fdemshapefilter.h"
#include "cgalarrangement.h"

#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkPolyData.h>
#include <vtkDoubleArray.h>

#include <QDebug>

vtkStandardNewMacro(FDEMShapeClipFilter);

FDEMShapeClipFilter::FDEMShapeClipFilter()
{
    this->clip_seeds = vtkDoubleArray::New();
    this->arr_location = new CGALArrangement();
    this->arr_clipping = new CGALArrangement();
    this->SetNumberOfInputPorts(2);
}

FDEMShapeClipFilter::~FDEMShapeClipFilter()
{
    delete this->arr_location;
    delete this->arr_clipping;

    this->clip_seeds->Delete();
}

int FDEMShapeClipFilter::RequestData(vtkInformation *vtkNotUsed(request),
                                     vtkInformationVector **inputVector,
                                     vtkInformationVector *outputVector)
{
    vtkInformation *inInfo1 = inputVector[0]->GetInformationObject(0);
    vtkInformation *inInfo2 = inputVector[1]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    vtkPolyData *input1 = vtkPolyData::SafeDownCast(inInfo1->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *input2 = vtkPolyData::SafeDownCast(inInfo2->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    this->arr_location->InsertPlanarCurves(input2);
    this->arr_clipping->InsertPlanarCurves(input1);
    this->arr_clipping->InsertPlanarCurves(input2);

    Location pl(arr_location->GetCGALArrangement());
    Arrangement arr = arr_clipping->GetCGALArrangement();

    QVector<Face_const_handle> holeFaces;
    for (int i = 0; i < this->clip_seeds->GetNumberOfTuples(); i++)
    {
        double *pt = this->clip_seeds->GetTuple3(i);
        Epeck::Point_2 seed = Epeck::Point_2(pt[0], pt[1]);
        Result_Type obj = pl.locate(seed);
        if(const Face_const_handle* face = boost::get<Face_const_handle>(&obj))
        {
            holeFaces.push_back(*face);
        }
    }

    vtkIdType numPts = arr.number_of_vertices();
    vtkPoints *newPoints;
    vtkCellArray *newLines;

    newPoints = vtkPoints::New();
    newPoints->Allocate(numPts);

    newLines = vtkCellArray::New();
    newLines->Allocate(newLines->EstimateSize(arr.number_of_edges(), 2));

    vtkIdType id = 0;
    for (auto eiter = arr.edges_begin(); eiter != arr.edges_end(); ++eiter)
    {
        Epeck::Point_2 src = eiter->source()->point();
        Epeck::Point_2 tar = eiter->target()->point();
        Epeck::Point_2 midpt = CGAL::midpoint(eiter->source()->point(), eiter->target()->point());

        Result_Type obj = pl.locate(midpt);
        if(Face_const_handle* face = boost::get<Face_const_handle>(&obj))
        {
            if(holeFaces.contains(*face))
            {
                newPoints->InsertNextPoint(CGAL::to_double(src.x()), CGAL::to_double(src.y()), 0.0);
                newPoints->InsertNextPoint(CGAL::to_double(tar.x()), CGAL::to_double(tar.y()), 0.0);
                newLines->InsertNextCell(2);
                newLines->InsertCellPoint(2 * id);
                newLines->InsertCellPoint(2 * id + 1);
                id += 1;
            }
        }
    }

    newPoints->Squeeze();
    output->SetPoints(newPoints);
    newPoints->Delete();

    newLines->Squeeze();
    output->SetLines(newLines);
    newLines->Delete();

    return 1;
}

int FDEMShapeClipFilter::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}

void FDEMShapeClipFilter::SetSeedPoints(vtkDoubleArray *seeds)
{
    this->clip_seeds->ShallowCopy(seeds);
}

void FDEMShapeClipFilter::PrintSelf(std::ostream &, vtkIndent)
{

}
