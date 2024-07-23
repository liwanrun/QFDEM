#include "blockgeometry.h"

#include <QDebug>

#include <vtkDoubleArray.h>
#include <vtkPolyData.h>


Primitive::Primitive()
{
    qDebug() << "Entity()";

    this->_size = 0;
    this->_cols = 0;
    this->_shape = QString();
    this->_color = QColor(0,255,255);
    this->tableData = nullptr;
    this->polyData = nullptr;

    this->is_embedded = false;
}

Primitive::Primitive(QString s, QColor color)
    : _shape(s), _color(color)
{
    qDebug() << "Entity()";

    this->tableData = vtkDoubleArray::New();
    this->InitDefaultDataArray(_shape);
    this->polyData = nullptr;

    this->is_embedded = false;
}

Primitive::~Primitive()
{
    qDebug() << "~Entity()";

    if(this->tableData)
        this->tableData->Delete();

    if(this->polyData)
        this->polyData->Delete();
}

QString Primitive::getShape() const
{
    return this->_shape;
}

void Primitive::setShape(const QString &shape)
{
    if(this->_shape == shape)
        return;

    this->_shape = shape;
    this->InitDefaultDataArray(shape);
}

QColor Primitive::getColor() const
{
    return this->_color;
}

void Primitive::setColor(const QColor &value)
{
    this->_color = value;
}

int Primitive::getSize() const
{
    return this->_size;
}

void Primitive::setSize(const int size)
{
    this->_size = size;
}

int Primitive::getCols() const
{
    return this->_cols;
}

void Primitive::setCols(const int cols)
{
    this->_cols = cols;
}

vtkDoubleArray *Primitive::getTableData()
{
    return this->tableData;
}

void Primitive::setTableData(vtkDoubleArray *array)
{
    this->tableData = array;
}

vtkPolyData *Primitive::getPolyData() const
{
    return polyData;
}

void Primitive::setPolyData(vtkPolyData *poly)
{
    polyData = poly;
}

void Primitive::InitDefaultDataArray(const QString &shape)
{
    if(QString("Polygon") == shape)
    {
        this->_size = 4;
        this->_cols = 3;

        this->tableData->SetNumberOfComponents(this->_cols);
        this->tableData->SetNumberOfTuples(this->_size);
        this->tableData->SetTuple3(0,0.0,0.0,0.0);
        this->tableData->SetTuple3(1,10.0,0.0,0.0);
        this->tableData->SetTuple3(2,10.0,10.0,0.0);
        this->tableData->SetTuple3(3,0.0,10.0,0.0);
    }
    else if(QString("Ellipse") == shape)
    {
        this->_size = 3;
        this->_cols = 3;

        this->tableData->SetNumberOfComponents(this->_cols);
        this->tableData->SetNumberOfTuples(this->_size);
        this->tableData->SetTuple3(0,0.0,0.0,0.0);
        this->tableData->SetTuple3(1,10.0,10.0,10);
        this->tableData->SetTuple3(2,90.0,0.0,0.0);
    }
    else if(QString("Segment") == shape)
    {
        this->_size = 2;
        this->_cols = 4;

        this->tableData->SetNumberOfComponents(this->_cols);
        this->tableData->SetNumberOfTuples(this->_size);
        this->tableData->SetTuple4(0,0.0,0.0,10.0,10.0);
        this->tableData->SetTuple4(1,10.0,0.0,0.0,10.0);
    }
    else if(QString("Point") == shape)
    {
        this->_size = 1;
        this->_cols = 3;

        this->tableData->SetNumberOfComponents(this->_cols);
        this->tableData->SetNumberOfTuples(this->_size);
        this->tableData->SetTuple3(0,0.0,0.0,0.0);
    }
}

bool Primitive::isEmbedded() const
{
    return this->is_embedded;
}

void Primitive::setEmbedded(bool flag)
{
    this->is_embedded = flag;
}
