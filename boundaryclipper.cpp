#include "boundaryclipper.h"

#include <QVector>
#include <vtkDoubleArray.h>

BoundaryClipper::BoundaryClipper()
{
    this->pl.attach(this->arr);
}

void BoundaryClipper::SetClippingBoundary(Block::PrimitiveMap &boundaries)
{
    for(auto iter = boundaries.begin(); iter != boundaries.end(); ++iter)
    {
        vtkDoubleArray *da = iter.value()->getTableData();
        QString shape = iter.value()->getShape();
        if(QString("Polygon") == shape)
        {
            this->InsertPolygon(da);
        }
        else if(QString("Ellipse") == shape)
        {
            this->InsertEllipse(da);
        }
    }
}

void BoundaryClipper::SetClippingSentinel(Block::PrimitiveMap &clipseeds)
{
    for(auto iter = clipseeds.begin(); iter != clipseeds.end(); ++iter)
    {
        vtkDoubleArray* da = iter.value()->getTableData();
        for(vtkIdType id = 0; id < da->GetNumberOfTuples(); id++)
        {
            double x[3];
            da->GetTuple(id, x);
            Epeck::Point_2 seed = Epeck::Point_2(x[0], x[1]);
            Result_Type obj = pl.locate(seed);
            if(const Face_const_handle* face = boost::get<Face_const_handle>(&obj))
            {
                this->faces.push_back(*face);
            }
        }
    }
}

vtkDoubleArray* BoundaryClipper::Clip(Primitive *primitive)
{
    QString shape = primitive->getShape();
    vtkDoubleArray* da = primitive->getTableData();

    vtkDoubleArray *fractures = nullptr;
    if(shape == QString("Polygon"))
    {
        fractures = vtkDoubleArray::New();
        fractures = this->ClipPolygon(da);
    }
    else if(shape == QString("Ellipse"))
    {
        fractures = vtkDoubleArray::New();
        fractures = this->ClipEllipse(da);
    }
    else if(shape == QString("Segment"))
    {
        fractures = vtkDoubleArray::New();
        fractures = this->ClipSegment(da);
    }

    return fractures;
}

void BoundaryClipper::InsertPolygon(vtkDoubleArray *da)
{
    vtkIdType numEdges = da->GetNumberOfTuples();

    QVector<Traits::Curve_2> curves;
    curves.reserve(numEdges);
    for(vtkIdType i = 0; i < numEdges; i++)
    {
        double x0[3], x1[3];
        vtkIdType j = i + 1;
        if(j == numEdges) j = 0;
        da->GetTuple(i, x0);
        da->GetTuple(j, x1);

        Epeck::Point_2 src = Epeck::Point_2(x0[0], x0[1]);
        Epeck::Point_2 tar = Epeck::Point_2(x1[0], x1[1]);
        Traits::Curve_2 curve(src, tar);
        curves.push_back(curve);
    }

    CGAL::insert(this->arr, curves.begin(), curves.end());
}

void BoundaryClipper::InsertEllipse(vtkDoubleArray *da)
{
    int numEdges = da->GetTuple3(1)[2];
    double x0 = da->GetTuple3(0)[0];                                // center X of ellipse
    double y0 = da->GetTuple3(0)[1];                                // center Y of ellipse
    double Ra = da->GetTuple3(1)[0];                                // Major semi-radius
    double Rb = da->GetTuple3(1)[1];                                // Minor semi-radius
    double t0 = da->GetTuple3(2)[0] * vtkMath::Pi() / 180.0;        // starting angle
    double theta = 2.0 * vtkMath::Pi() / numEdges;

    QVector<Traits::Curve_2> curves;
    curves.reserve(numEdges);
    for (int i = 0; i < numEdges; i++)
    {
        int j = i + 1;
        if(j == numEdges) j = 0;

        double p[3] = {x0 + Ra * cos(t0 + i * theta), y0 + Rb * sin(t0 + i * theta), 0.0};
        double q[3] = {x0 + Ra * cos(t0 + j * theta), y0 + Rb * sin(t0 + j * theta), 0.0};

        Epeck::Point_2 src = Epeck::Point_2(p[0], p[1]);
        Epeck::Point_2 tar = Epeck::Point_2(q[0], q[1]);
        Traits::Curve_2 curve(src, tar);
        curves.push_back(curve);
    }

    CGAL::insert(this->arr, curves.begin(), curves.end());
}

vtkDoubleArray* BoundaryClipper::ClipPolygon(vtkDoubleArray *da)
{
    vtkDoubleArray *dataArray = vtkDoubleArray::New();
    dataArray->SetNumberOfComponents(4);

    vtkIdType numEdges = da->GetNumberOfTuples();
    for(vtkIdType i = 0; i < numEdges; i++)
    {
        double x0[3], x1[3];
        vtkIdType j = i + 1;
        if(j == numEdges) j = 0;
        da->GetTuple(i, x0);
        da->GetTuple(j, x1);

        Epeck::Point_2 src(x0[0], x0[1]);
        Epeck::Point_2 tar(x1[0], x1[1]);
        Traits::Curve_2 curve(src, tar);
        Arrangement arrangement(this->arr);
        CGAL::insert(arrangement, curve);
        this->Update(arrangement, dataArray);
    }

    dataArray->Squeeze();
    return dataArray;
}

vtkDoubleArray* BoundaryClipper::ClipEllipse(vtkDoubleArray *da)
{
    vtkDoubleArray *dataArray = vtkDoubleArray::New();
    dataArray->SetNumberOfComponents(4);

    double x0 = da->GetTuple3(0)[0];                                // center X of ellipse
    double y0 = da->GetTuple3(0)[1];                                // center Y of ellipse
    double Ra = da->GetTuple3(1)[0];                                // Major semi-radius
    double Rb = da->GetTuple3(1)[1];                                // Minor semi-radius
    double t0 = da->GetTuple3(2)[0] * vtkMath::Pi() / 180.0;        // starting angle
    double theta = 2.0 * vtkMath::Pi() / da->GetTuple3(1)[2];

    int numEdges = da->GetTuple3(1)[2];
    for (int i = 0; i < numEdges; i++)
    {
        int j = i + 1;
        if(j == numEdges) j = 0;

        double p[3] = {x0 + Ra * cos(t0 + i * theta), y0 + Rb * sin(t0 + i * theta), 0.0};
        double q[3] = {x0 + Ra * cos(t0 + j * theta), y0 + Rb * sin(t0 + j * theta), 0.0};

        Epeck::Point_2 src(p[0], p[1]);
        Epeck::Point_2 tar(q[0], q[1]);
        Traits::Curve_2 curve(src, tar);
        Arrangement arrangement(this->arr);
        CGAL::insert(arrangement, curve);
        this->Update(arrangement, dataArray);
    }

    dataArray->Squeeze();
    return dataArray;
}

vtkDoubleArray* BoundaryClipper::ClipSegment(vtkDoubleArray *da)
{
    vtkDoubleArray *dataArray = vtkDoubleArray::New();
    dataArray->SetNumberOfComponents(4);

    for(vtkIdType id = 0; id < da->GetNumberOfTuples(); id++)
    {
        double x[4];
        da->GetTuple(id, x);
        Epeck::Point_2 src(x[0], x[1]);
        Epeck::Point_2 tar(x[2], x[3]);
        Traits::Curve_2 curve(src, tar);
        Arrangement arrangement(this->arr);
        CGAL::insert(arrangement, curve);
        this->Update(arrangement, dataArray);
    }

    dataArray->Squeeze();
    return dataArray;
}

void BoundaryClipper::Update(const Arrangement &arr, vtkDoubleArray *da)
{
    for (auto eiter = arr.edges_begin(); eiter != arr.edges_end(); ++eiter)
    {
        Epeck::Point_2 src = eiter->source()->point();
        Epeck::Point_2 tar = eiter->target()->point();
        Epeck::Point_2 mpt = CGAL::midpoint(src, tar);

        Result_Type obj = this->pl.locate(mpt);
        if(Face_const_handle* face = boost::get<Face_const_handle>(&obj))
        {
            if(this->faces.contains(*face))
            {
                double x0 = CGAL::to_double(src.x());
                double y0 = CGAL::to_double(src.y());
                double x1 = CGAL::to_double(tar.x());
                double y1 = CGAL::to_double(tar.y());
                da->InsertNextTuple4(x0, y0, x1, y1);
            }
        }
    }
}
