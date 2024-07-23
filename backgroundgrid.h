#ifndef BACKGROUNDGRID_H
#define BACKGROUNDGRID_H

#include <QMutex>
#include <QRectF>
#include <QPointF>
#include <QSharedPointer>

class vtkStructuredGrid;
class BackgroundGrid
{
public:
    enum CoordType
    {
        RECTANGLE = 0,
        POLARGRID = 1
    };

public:
    static QSharedPointer<BackgroundGrid> GetBackgroundGrid();

    ~BackgroundGrid();

public:
    vtkStructuredGrid *GenerateGrid();

private:
    BackgroundGrid();

    vtkStructuredGrid *GenerateCoordGridLine();

    vtkStructuredGrid *GeneratePolarGridLine();

signals:
    void signalBackgroundGridUpdated();

private:
    static QSharedPointer<BackgroundGrid> backgroundGrid;

    static QMutex mutex;

private:
    int coordType;
    int direction;
    QPointF location;
    QRectF  bounding;
    QPoint dimension;

public:
    friend class BackgroundGridDialog;
};

#endif // BACKGROUNDGRID_H
