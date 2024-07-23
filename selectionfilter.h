#ifndef SELECTIONFILTER_H
#define SELECTIONFILTER_H

#include <QSharedPointer>

class vtkIdTypeArray;
class vtkDataSet;

class SelectionFilter
{
public:
    SelectionFilter() {}

    virtual void filter(vtkDataSet* ds, vtkIdTypeArray* out) = 0;
    virtual void filter(vtkDataSet* ds, vtkIdTypeArray* in, vtkIdTypeArray* out) = 0;
};

class SelectElementFilter : public SelectionFilter
{
public:
    SelectElementFilter() {}

    void filter(vtkDataSet* ds, vtkIdTypeArray* out) override;
    void filter(vtkDataSet* ds, vtkIdTypeArray* in, vtkIdTypeArray* out) override;
};

class SelectTriangleFilter : public SelectionFilter
{
public:
    SelectTriangleFilter() {}

    void filter(vtkDataSet* ds, vtkIdTypeArray* out) override;
    void filter(vtkDataSet* ds, vtkIdTypeArray* in, vtkIdTypeArray* out) override;
};

class SelectQuadFilter : public SelectionFilter
{
public:
    SelectQuadFilter() {}

    void filter(vtkDataSet* ds, vtkIdTypeArray* out) override;
    void filter(vtkDataSet* ds, vtkIdTypeArray* in, vtkIdTypeArray* out) override;
};

class SelectConstrainedQuadFilter : public SelectionFilter
{
public:
    SelectConstrainedQuadFilter() {}

    void filter(vtkDataSet* ds, vtkIdTypeArray* out) override;
    void filter(vtkDataSet* ds, vtkIdTypeArray* in, vtkIdTypeArray* out) override;
};

class SelectUnconstrainedQuadFilter : public SelectionFilter
{
public:
    SelectUnconstrainedQuadFilter() {}

    void filter(vtkDataSet* ds, vtkIdTypeArray* out) override;
    void filter(vtkDataSet* ds, vtkIdTypeArray* in, vtkIdTypeArray* out) override;
};


class SelectionFilterFactory
{
public:
    SelectionFilterFactory() {}

    QSharedPointer<SelectionFilter> Create(QString criterion);
};

#endif // SELECTIONFILTER_H
