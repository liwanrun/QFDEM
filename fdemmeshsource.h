#ifndef FDEMMESHSOURCE_H
#define FDEMMESHSOURCE_H

#include <set>
#include <QSharedPointer>
#include <vtkPolyDataAlgorithm.h>

class Triangulation;
class FDEMMeshSource : public vtkPolyDataAlgorithm
{
public:
    vtkTypeMacro(FDEMMeshSource, vtkPolyDataAlgorithm)

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMMeshSource* New();

    void SetTriangulation(QSharedPointer<Triangulation> t);

    void AddNodeIdLabels(vtkPolyData *pd);

    void AddCellIdLabels(vtkPolyData *pd);

    void AddCellConstraintTags(vtkPolyData *pd, const std::vector<vtkIdType>& ids);

    void AddPointBoundaryTags(vtkPolyData *pd, const std::set<vtkIdType>& ids);

    void AddCellBlockIds(vtkPolyData* pd, const std::vector<vtkIdType>& ids);

    void AddCellGrainIds(vtkPolyData* pd, const std::vector<vtkIdType>& ids);

    void AddCellScalarArray(vtkPolyData* pd, const char* name, const std::vector<vtkIdType>& ids);

protected:
    FDEMMeshSource();

    ~FDEMMeshSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

protected:
    QSharedPointer<Triangulation> triangulation;

private:
    FDEMMeshSource(const FDEMMeshSource&) = delete;

    void operator=(const FDEMMeshSource&) = delete;
};

#endif // FDEMMESHSOURCE_H
