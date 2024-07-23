#include "fdemshapesource.h"

#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkPolyData.h>

vtkStandardNewMacro(FDEMShapeSource);

FDEMShapeSource::FDEMShapeSource()
{
    this->RawData = vtkDoubleArray::New();
    this->SetNumberOfInputPorts(0);
}

FDEMShapeSource::~FDEMShapeSource()
{

}

void FDEMShapeSource::SetRawData(vtkDoubleArray *data)
{
     this->RawData->DeepCopy(data);
  // this->RawData = data;
}

int FDEMShapeSource::RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}

int FDEMShapeSource::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}

void FDEMShapeSource::PrintSelf(std::ostream &os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
}


//FDEM Polygon Source
vtkStandardNewMacro(FDEMPolygonSource);

FDEMPolygonSource::FDEMPolygonSource()
{
    this->SetNumberOfInputPorts(0);
}

FDEMPolygonSource::~FDEMPolygonSource()
{
    this->RawData->Delete();
}

int FDEMPolygonSource::RequestData(vtkInformation *vtkNotUsed(request),
                                   vtkInformationVector **vtkNotUsed(inputVector),
                                   vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    int numPts = this->RawData->GetNumberOfTuples();
    vtkPoints *newPoints;
    vtkCellArray *newLine;

    newPoints = vtkPoints::New();
    newPoints->Allocate(numPts);

    newLine = vtkCellArray::New();
    newLine->Allocate(newLine->EstimateSize(1,numPts));
    newLine->InsertNextCell(numPts + 1);
    for (int i = 0; i < numPts; i++)
    {
        newLine->InsertCellPoint(i);
    }
    newLine->InsertCellPoint(0);

    output->SetLines(newLine);
    newLine->Delete();

    for (int j = 0; j < numPts; j++)
    {
        double *tuple = this->RawData->GetTuple3(j);
        newPoints->InsertNextPoint(tuple[0],tuple[1],tuple[2]);
    }

    output->SetPoints(newPoints);
    newPoints->Delete();

    return 1;
}

int FDEMPolygonSource::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}

// Ellipse Source
vtkStandardNewMacro(FDEMEllipseSource);

FDEMEllipseSource::FDEMEllipseSource()
{
    this->SetNumberOfInputPorts(0);
}

FDEMEllipseSource::~FDEMEllipseSource()
{
    this->RawData->Delete();
}

int FDEMEllipseSource::RequestData(vtkInformation *vtkNotUsed(request),
                                   vtkInformationVector **vtkNotUsed(inputVector),
                                   vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    int numPts = this->RawData->GetTuple3(1)[2];
    vtkPoints *newPoints;
    //    vtkCellArray *newPoly;
    vtkCellArray *newLine;

    newPoints = vtkPoints::New();
    newPoints->Allocate(numPts);

    newLine = vtkCellArray::New();
    newLine->Allocate(newLine->EstimateSize(1,numPts));
    newLine->InsertNextCell(numPts + 1);
    for (int i = 0; i < numPts; i++)
    {
        newLine->InsertCellPoint(i);
    }
    newLine->InsertCellPoint(0);

    output->SetLines(newLine);
    newLine->Delete();

    double x0 = this->RawData->GetTuple3(0)[0];         // center X of ellipse
    double y0 = this->RawData->GetTuple3(0)[1];         // center Y of ellipse
    double Ra = this->RawData->GetTuple3(1)[0];         // Major semi-radius
    double Rb = this->RawData->GetTuple3(1)[1];         // Minor semi-radius
    double t0 = this->RawData->GetTuple3(2)[0] * vtkMath::Pi() / 180.0;         // starting angle
    double theta = 2.0 * vtkMath::Pi() / numPts;

    for (int j = 0; j < numPts; j++)
    {
        double x = x0 + Ra * cos(t0 + j * theta);
        double y = y0 + Rb * sin(t0 + j * theta);
        double z = 0.0;
        newPoints->InsertNextPoint(x,y,z);
    }

    output->SetPoints(newPoints);
    newPoints->Delete();

    return 1;
}

int FDEMEllipseSource::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;

}

// Segment Source
vtkStandardNewMacro(FDEMSegmentSource);

FDEMSegmentSource::FDEMSegmentSource()
{
    this->SetNumberOfInputPorts(0);
}

FDEMSegmentSource::~FDEMSegmentSource()
{
    this->RawData->Delete();
}

int FDEMSegmentSource::RequestData(vtkInformation *vtkNotUsed(request),
                                   vtkInformationVector **vtkNotUsed(inputVector),
                                   vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    vtkIdType size = this->RawData->GetNumberOfTuples();
    int numPts = 2 * size;
    vtkPoints *newPoints;
    //    vtkCellArray *newPoly;
    vtkCellArray *newLine;

    newPoints = vtkPoints::New();
    newPoints->Allocate(numPts);

    newLine = vtkCellArray::New();
    newLine->Allocate(newLine->EstimateSize(size,2));

    for (uint i = 0; i < size; i++)
    {
        newLine->InsertNextCell(2);
        newLine->InsertCellPoint(2 * i);
        newLine->InsertCellPoint(2 * i + 1);
    }

    output->SetLines(newLine);
    newLine->Delete();

    for (uint j = 0; j < size; j++)
    {
        double *tuple = this->RawData->GetTuple4(j);
        newPoints->InsertNextPoint(tuple[0],tuple[1],0.0);
        newPoints->InsertNextPoint(tuple[2],tuple[3],0.0);
    }

    output->SetPoints(newPoints);
    newPoints->Delete();

    return 1;
}

int FDEMSegmentSource::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}


//Point Source
vtkStandardNewMacro(FDEMPointSource);

FDEMPointSource::FDEMPointSource()
{
    this->SetNumberOfInputPorts(0);
}

FDEMPointSource::~FDEMPointSource()
{
    this->RawData->Delete();
}

int FDEMPointSource::RequestData(vtkInformation *vtkNotUsed(request),
                                 vtkInformationVector **vtkNotUsed(inputVector),
                                 vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

//    vtkIdType size = this->RawData->GetNumberOfTuples();
    vtkIdType numPts = this->RawData->GetNumberOfTuples();
    vtkPoints *newPoints;
    vtkCellArray *newVerts;

    newPoints = vtkPoints::New();
    newPoints->Allocate(numPts);

    newVerts = vtkCellArray::New();
    newVerts->Allocate(newVerts->EstimateSize(numPts,1));

    for (vtkIdType i = 0; i < numPts; i++)
    {
        newVerts->InsertNextCell(1);
        newVerts->InsertCellPoint(i);
    }

    output->SetVerts(newVerts);
    newVerts->Delete();

    for (vtkIdType j = 0; j < numPts; j++)
    {
        double *tuple = this->RawData->GetTuple3(j);
        newPoints->InsertNextPoint(tuple[0],tuple[1],0.0);
    }

    output->SetPoints(newPoints);
    newPoints->Delete();

    return 1;
}

int FDEMPointSource::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}
