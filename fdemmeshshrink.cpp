#include "fdemmeshshrink.h"
#include "constraineddelaunaytriangulation.h"
#include "constrainedtriangulation.h"
#include "triangulation.h"


vtkStandardNewMacro(FDEMMeshShrink);

void FDEMMeshShrink::PrintSelf(std::ostream &, vtkIndent)
{

}

FDEMMeshShrink::FDEMMeshShrink()
{

}

FDEMMeshShrink::~FDEMMeshShrink()
{

}

void FDEMMeshShrink::SetTriangulation(QSharedPointer<Triangulation> t)
{
    this->triangulation = t;
}

int FDEMMeshShrink::RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}

int FDEMMeshShrink::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{
    return 1;
}
