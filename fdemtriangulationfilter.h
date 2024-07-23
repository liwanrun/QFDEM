#ifndef FDEMTRIANGULATIONFILTER_H
#define FDEMTRIANGULATIONFILTER_H

#include <QMap>
#include <QSharedPointer>

class Primitive;
class Triangulation;
class FDEMTriangulationFilter
{
public:
    FDEMTriangulationFilter();
    virtual ~FDEMTriangulationFilter();

    void SetTriangulation(QSharedPointer<Triangulation>& t);

    void SetConstraintInfo(QMap<QString, QSharedPointer<Primitive>>& ci);

    virtual void filter();

protected:
    QSharedPointer<Triangulation> mesh;

    QMap<QString, QSharedPointer<Primitive> > constraints;
};


class InsertGlobalQuadFilter : public FDEMTriangulationFilter
{
public:
    InsertGlobalQuadFilter();
    ~InsertGlobalQuadFilter() override;

    void filter() override;
};

class InsertLocalQuadFilter : public FDEMTriangulationFilter
{
public:
    InsertLocalQuadFilter();
    ~InsertLocalQuadFilter() override;

    void filter() override;
};

class SplitTriangulationFilter : public FDEMTriangulationFilter
{
public:
    SplitTriangulationFilter();
    ~SplitTriangulationFilter() override;

    void filter() override;
};

class InsertConstraintFilter : public FDEMTriangulationFilter
{
public:
    InsertConstraintFilter();
    ~InsertConstraintFilter();

    void filter() override;
};

#endif // FDEMTRIANGULATIONFILTER_H
