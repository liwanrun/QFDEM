#ifndef REGIONCLIPPER_H
#define REGIONCLIPPER_H

#include "cgaltypedef.h"
#include "block.h"
#include <QVector>

class BoundaryClipper
{
public:
    BoundaryClipper();

    // step 1
    void SetClippingBoundary(Block::PrimitiveMap &boundaries);

    // step 2
    void SetClippingSentinel(Block::PrimitiveMap &clipseeds);

    // step 3
    vtkDoubleArray *Clip(Primitive *primitive);

private:
    void InsertPolygon(vtkDoubleArray* da);

    void InsertEllipse(vtkDoubleArray* da);

    vtkDoubleArray* ClipPolygon(vtkDoubleArray* da);

    vtkDoubleArray* ClipEllipse(vtkDoubleArray* da);

    vtkDoubleArray* ClipSegment(vtkDoubleArray* da);

    void Update(const Arrangement& arr, vtkDoubleArray *da);

private:
    QVector<Face_const_handle> faces;

    Location pl;
    Arrangement arr;
};

#endif // REGIONCLIPPER_H
