#ifndef PRIMITIVEACTOR_H
#define PRIMITIVEACTOR_H

#include <QSharedPointer>

#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>

class Primitive;
class PrimitiveActor
{
public:
    PrimitiveActor();
    ~PrimitiveActor();

    vtkActor* GetVTKRenderActor();

    vtkPolyDataMapper *GetVTKActorMapper();

    vtkPolyData *GetInputDataSet();

    void Update(const QSharedPointer<Primitive> &p);

public:
    inline void SetInputConnection(vtkAlgorithmOutput* input);

    inline void SetColor(QColor color);

    inline void SetLineWidth(float _arg);

    inline void SetPickable(int _arg);

    inline void SetPointSize(float _arg);

    inline void SetInformation(vtkInformation* info);

private:
    vtkActor *actor;
    vtkPolyDataMapper *mapper;
};

#endif // PRIMITIVEACTOR_H
