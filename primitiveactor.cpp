#include "primitiveactor.h"
#include "fdemshapesource.h"
#include "blockgeometry.h"

#include <QDebug>

PrimitiveActor::PrimitiveActor()
{
    qDebug() << "PrimitiveActor()";

    this->actor = vtkActor::New();
    this->mapper = vtkPolyDataMapper::New();
    this->actor->SetMapper(this->mapper);
}

PrimitiveActor::~PrimitiveActor()
{
    qDebug() << "~PrimitiveActor()";

    this->mapper->Delete();
    this->actor->Delete();
}

vtkActor *PrimitiveActor::GetVTKRenderActor()
{
    return this->actor;
}

vtkPolyDataMapper *PrimitiveActor::GetVTKActorMapper()
{
    return this->mapper;
}

vtkPolyData *PrimitiveActor::GetInputDataSet()
{
    return this->mapper->GetInput();
}

void PrimitiveActor::Update(const QSharedPointer<Primitive> &p)
{
    QSharedPointer<ShapeFactory> factory = QSharedPointer<ShapeFactory>(new ShapeFactory);
    vtkSmartPointer<FDEMShapeSource> source = factory->create(p->getShape());
    source->SetRawData(p->getTableData());

    this->SetInputConnection(source->GetOutputPort());
    this->SetColor(p->getColor());
    this->SetLineWidth(2);
    this->SetPickable(false);

    if(QString("Point") == p->getShape())
    {
        this->SetPointSize(5);
    }
}

void PrimitiveActor::SetInputConnection(vtkAlgorithmOutput *input)
{
    this->mapper->SetInputConnection(input);
}

void PrimitiveActor::SetColor(QColor color)
{
    double rgb[3];
    color.getRgbF(&rgb[0], &rgb[1], &rgb[2]);
    this->actor->GetProperty()->SetColor(rgb);
}

void PrimitiveActor::SetLineWidth(float _arg)
{
    this->actor->GetProperty()->SetLineWidth(_arg);
}

void PrimitiveActor::SetPickable(int _arg)
{
    this->actor->SetPickable(_arg);
}

void PrimitiveActor::SetPointSize(float _arg)
{
    this->actor->GetProperty()->SetPointSize(_arg);
}

void PrimitiveActor::SetInformation(vtkInformation *info)
{
    this->actor->GetProperty()->SetInformation(info);
}

