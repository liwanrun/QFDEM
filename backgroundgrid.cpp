#include "backgroundgrid.h"

#include <vtkMath.h>
#include <vtkPoints.h>
#include <vtkStructuredGrid.h>

QMutex BackgroundGrid::mutex;

QSharedPointer<BackgroundGrid> BackgroundGrid::backgroundGrid = nullptr;

QSharedPointer<BackgroundGrid> BackgroundGrid::GetBackgroundGrid()
{
    if(nullptr == backgroundGrid)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == backgroundGrid)
        {
            backgroundGrid = QSharedPointer<BackgroundGrid>(new BackgroundGrid);
        }
    }

    return backgroundGrid;
}

BackgroundGrid::~BackgroundGrid()
{

}

vtkStructuredGrid *BackgroundGrid::GenerateGrid()
{
    vtkStructuredGrid *sgrid = vtkStructuredGrid::New();
    if(RECTANGLE == this->coordType)
    {
        sgrid = this->GenerateCoordGridLine();
    }
    else if(POLARGRID == this->coordType)
    {
        sgrid = this->GeneratePolarGridLine();
    }

    return sgrid;
}

BackgroundGrid::BackgroundGrid()
{
    this->coordType = 0;
    this->direction = 1;
    this->location = QPointF(0.0, 0.0);
    this->bounding = QRectF(0.0, 0.0, 1.0, 1.0);
    this->dimension = QPoint(10, 10);
}

vtkStructuredGrid *BackgroundGrid::GenerateCoordGridLine()
{
    vtkStructuredGrid *sgrid = vtkStructuredGrid::New();

    double deltaX, deltaY, deltaZ;
    deltaX = this->bounding.width() / this->dimension.x();
    deltaY = this->bounding.height() / this->dimension.y();
    deltaZ = 0.0;

    vtkPoints *points = vtkPoints::New();
    int nx = this->dimension.x() + 1;
    int ny = this->dimension.y() + 1;
    points->Allocate(nx * ny);

    double coord[3];
    for (int k = 0; k < 1; k++)
    {
        coord[2] = -0.000 + k * deltaZ;
        int kOffset = k * nx * ny;
        for (int j = 0; j < ny; j++)
        {
            coord[1] = this->bounding.y() + j * deltaY + this->location.y();
            int jOffset = j * nx;
            for (int i = 0; i < nx; i++)
            {
                coord[0] = this->bounding.x() + i * deltaX + this->location.x();
                int offset = i + jOffset + kOffset;
                points->InsertPoint(offset, coord);
            }
        }
    }

    points->Squeeze();
    sgrid->SetDimensions(nx, ny, 1);
    sgrid->SetPoints(points);
    points->Delete();

    return sgrid;
}

vtkStructuredGrid *BackgroundGrid::GeneratePolarGridLine()
{
    vtkStructuredGrid *sgrid = vtkStructuredGrid::New();

    double deltaT, deltaR, deltaZ;
    deltaR = this->bounding.width() / this->dimension.x();
    deltaT = this->bounding.height() / this->dimension.y();
    deltaZ = 0.0;

    vtkPoints *points = vtkPoints::New();
    int nx = this->dimension.x() + 1;
    int ny = this->dimension.y() + 1;
    points->Allocate(nx * ny);

    double coord[3];
    double angles[4] = {90.0, 0.0, 270.0, 180.0};
    for (int k = 0; k < 1; k++)
    {
        coord[2] = -0.000 + k * deltaZ;
        int kOffset = k * nx * ny;
        for (int j = 0; j < ny; j++)
        {
            double theta = this->bounding.y() + j * deltaT + angles[this->direction];
            theta = vtkMath::RadiansFromDegrees(theta);
            int jOffset = j * nx;
            for (int i = 0; i < nx; i++)
            {
                double radius = this->bounding.x() + i * deltaR;
                coord[0] = radius * cos(theta) + this->location.x();
                coord[1] = radius * sin(theta) + this->location.y();

                int offset = i + jOffset + kOffset;
                points->InsertPoint(offset, coord);
            }
        }
    }

    points->Squeeze();
    sgrid->SetDimensions(nx, ny, 1);
    sgrid->SetPoints(points);
    points->Delete();

    return sgrid;
}
