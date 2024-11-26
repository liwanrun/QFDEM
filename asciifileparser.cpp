#include "asciifileparser.h"
#include <vtkSmartPointer.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkDoubleArray.h>
#include <QDebug>

ASCIIFileParser::ASCIIFileParser()
{
   this->array = vtkDoubleArray::New();
}

void ASCIIFileParser::ParseVtkFile(QString fname)
{
    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(fname.toStdString().c_str());
    reader->Update();
    vtkSmartPointer<vtkPolyData> poly = reader->GetOutput();
    this->array->SetNumberOfComponents(4);
    //this->array->SetName("Fractures");

    vtkSmartPointer<vtkPoints> points = poly->GetPoints();
    vtkSmartPointer<vtkCellArray> cells = poly->GetLines();
    vtkSmartPointer<vtkIdList> ids = vtkSmartPointer<vtkIdList>::New();
    cells->InitTraversal();
    while(cells->GetNextCell(ids))
    {
        double p1[3], p2[3];
        points->GetPoint(ids->GetId(0), p1);
        points->GetPoint(ids->GetId(1), p2);
        this->array->InsertNextTuple4(p1[0], p1[1], p2[0], p2[1]);
    }
}

void ASCIIFileParser::ParsePolyFile(QString fname)
{

}

vtkDoubleArray *ASCIIFileParser::GetPointArray()
{
    return this->array;
}
