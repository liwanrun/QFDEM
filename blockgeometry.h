#ifndef BLOCKGEOMETRY_H
#define BLOCKGEOMETRY_H

#include <QString>
#include <QColor>

class vtkPolyData;
class vtkDoubleArray;
class FDEMShapeSource;
class Primitive
{
public:
    Primitive();
    Primitive(QString _shape, QColor _color);
    ~Primitive();

public:
    QString getShape() const;
    void setShape(const QString &shape);

    QColor getColor() const;
    void setColor(const QColor &color);

    int getSize() const;
    void setSize(const int size);

    int getCols() const;
    void setCols(const int cols);

    vtkDoubleArray* getTableData();
    void setTableData(vtkDoubleArray *array);

    vtkPolyData *getPolyData() const;
    void setPolyData(vtkPolyData *poly);

    bool isEmbedded() const;
    void setEmbedded(bool flag);

private:
    void InitDefaultDataArray(const QString& shape);

private:
    QString _shape;
    QColor _color;
    int _size;
    int _cols;
    vtkDoubleArray *tableData;
    vtkPolyData *polyData;

    bool is_embedded;
};

#endif // BLOCKGEOMETRY_H
