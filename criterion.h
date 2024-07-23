#ifndef CRITERION_H
#define CRITERION_H

#include <QString>
#include <QStringList>
#include <vtkIdTypeArray.h>

#include <array>
#include <vector>
using namespace std;

class vtkCell;
class Criterion
{
public:
    Criterion();
    virtual ~Criterion();

    virtual vtkIdTypeArray* filterNode() = 0;
    virtual vtkIdTypeArray* filterCell(int cellType) = 0;

public:
    friend class FilterSelectDialog;

protected:
    int numberRows;
    int numberCols;
    QString filterType;
    QString selectType;
    QString logicsType;

    QStringList headLabels;
    vtkDoubleArray *filterData;
};

class PointCriterion : public Criterion
{
public:
    PointCriterion();

    vtkIdTypeArray* filterNode() override;
    vtkIdTypeArray* filterCell(int cellType) override;
};

class SegmentCriterion : public Criterion
{
public:
    SegmentCriterion();

    vtkIdTypeArray* filterNode() override;
    vtkIdTypeArray* filterCell(int cellType) override;
};

class RectangleCriterion : public Criterion
{
public:
    RectangleCriterion();

    vtkIdTypeArray* filterNode() override;
    vtkIdTypeArray* filterCell(int cellType) override;

private:
    bool IsCellOverlappedWithRectangle(vtkCell *cell, double *bbox);
};

class EllipseCriterion : public Criterion
{
public:
    EllipseCriterion();

    vtkIdTypeArray* filterNode() override;
    vtkIdTypeArray* filterCell(int cellType) override;

private:
    bool IsCellOverlappedWithCircle(vtkCell *cell, double *x0, double R);
};

#endif // CRITERION_H
