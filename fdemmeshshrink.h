#ifndef FDEMMESHSHRINKFILTER_H
#define FDEMMESHSHRINKFILTER_H

#include <QSharedPointer>
#include <vtkPolyDataAlgorithm.h>

class Triangulation;
class FDEMMeshShrink : public vtkPolyDataAlgorithm
{
public:
    vtkTypeMacro(FDEMMeshShrink, vtkPolyDataAlgorithm);

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMMeshShrink* New();

    void SetTriangulation(QSharedPointer<Triangulation> t);

protected:
    FDEMMeshShrink();

    ~FDEMMeshShrink() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

protected:
    QSharedPointer<Triangulation> triangulation;

private:
    FDEMMeshShrink(const FDEMMeshShrink&) = delete;

    void operator=(const FDEMMeshShrink&) = delete;
};

#endif // FDEMMESHSHRINKFILTER_H
