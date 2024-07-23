#include "fractaldimension.h"
#include "blockcollection.h"
#include "block.h"

#include <QList>
#include <vtkDoubleArray.h>

FractalDimension::FractalDimension()
{
    this->blockCollection = BlockCollection::GetBlockCollection();
}

QVector<QPointF> FractalDimension::GenerateFractureMidpoint()
{
    QVector<QPointF> centers;
    BlockCollection::BlockMap block_map = this->blockCollection->GetBlockMap();
    for(auto biter = block_map.begin(); biter != block_map.end(); ++biter)
    {
        Block::PrimitiveMap fracture_map = biter.value()->GetFractureMap();
        for(auto iter = fracture_map.begin(); iter != fracture_map.end(); ++iter)
        {
            vtkDoubleArray *fractures = iter.value()->getTableData();
            for(vtkIdType id = 0; id < fractures->GetNumberOfTuples(); id++)
            {
                double *px = fractures->GetTuple4(id);
                double xc = (px[0] + px[2]) / 2.0;
                double yc = (px[1] + px[3]) / 2.0;
                centers.push_back(QPointF(xc, yc));
            }
        }
    }
    return centers;
}

QVector<QLineF> FractalDimension::GetSpaceSortedFractures()
{
    double eps = 1.0e-10;
    QVector<QLineF> segments;
    BlockCollection::BlockMap block_map = this->blockCollection->GetBlockMap();
    for(auto biter = block_map.begin(); biter != block_map.end(); ++biter)
    {
        Block::PrimitiveMap fracture_map = biter.value()->GetFractureMap();
        for(auto iter = fracture_map.begin(); iter != fracture_map.end(); ++iter)
        {
            vtkDoubleArray *fractures = iter.value()->getTableData();
            for(vtkIdType id = 0; id < fractures->GetNumberOfTuples(); id++)
            {
                double *px = fractures->GetTuple4(id);
                QPointF p = QPointF(px[0], px[1]);
                QPointF q = QPointF(px[2], px[3]);

                if((p.x() > q.x()) || (std::abs(p.x() - q.x()) < eps && p.y() > q.y()))
                {
                    segments.push_back(QLineF(q, p));
                }
                else
                {
                    segments.push_back(QLineF(p, q));
                }
            }
        }
    }
    return segments;
}

BoxCountingDimension::BoxCountingDimension()
{

}

int BoxCountingDimension::Count(QRectF doi, double cl)
{
    double eps = 1.0e-12;
    int cols = std::round(doi.width() / cl);

    QVector<QLineF> fractureVector = this->GetSpaceSortedFractures();

    QSet<int> indexes;
    for(QLineF fracture : fractureVector)
    {
        QLineF s = this->ClipLineByRectangle(fracture, doi);

        int x1 = (int)((s.p1().x() - doi.x() + eps) / cl);
        int y1 = (int)((s.p1().y() - doi.y() + eps) / cl);
        int x2 = (int)((s.p2().x() - doi.x() - eps) / cl);
        int y2 = (int)((s.p2().y() - doi.y() - eps) / cl);
        double dx = s.dx();
        double dy = s.dy();

        indexes.insert(y1 * cols + x1);
        indexes.insert(y2 * cols + x2);
        if(dx * dy > 0)
        {
            int i = x1, j = y1;
            while(i < x2 && j < y2)
            {
                QPointF pt = QPointF((i + 1) * cl, (j + 1) * cl);
                QVector2D u = QVector2D(s.p2() - s.p1());
                QVector2D v = QVector2D(pt - s.p1());
                double cross_dot = u.x() * v.y() - u.y() * v.x();

                if(std::abs(cross_dot) < eps)
                {
                    i = i + 1;
                    j = j + 1;
                }
                else if(cross_dot > 0.0)
                {
                    i = i + 1;
                    j = j + 0;
                }
                else if(cross_dot < 0.0)
                {
                    i = i + 0;
                    j = j + 1;
                }

                indexes.insert(j * cols + i);
            }
        }
        else if(dx * dy < 0)
        {
            int i = x1, j = y1;
            while(i < x2 && j > y2)
            {
                QPointF pt = QPointF((i + 1) * cl, j * cl);
                QVector2D u = QVector2D(s.p2() - s.p1());
                QVector2D v = QVector2D(pt - s.p1());
                double cross_dot = u.x() * v.y() - u.y() * v.x();

                if(std::abs(cross_dot) < eps)
                {
                    i = i + 1;
                    j = j - 1;
                }
                else if(cross_dot < 0.0)
                {
                    i = i + 1;
                    j = j - 0;
                }
                else if(cross_dot > 0.0)
                {
                    i = i - 0;
                    j = j - 1;
                }

                indexes.insert(j * cols + i);
            }
        }
        else if(0 == dy)
        {
            int i = x1, j = y1;
            while(i < x2)
            {
                i++;
                indexes.insert(j * cols + i);
            }
        }
        else if(0 == dx)
        {
            int i = x1, j = y1;
            while(i < y2)
            {
                j++;
                indexes.insert(j * cols + i);
            }
        }
     }

    return indexes.size();
}

QLineF BoxCountingDimension::ClipLineByRectangle(QLineF line, QRectF rect)
{
    QLineF edges[4] =
    {
        QLineF(rect.topLeft(), rect.topRight()),
        QLineF(rect.topRight(), rect.bottomRight()),
        QLineF(rect.bottomRight(), rect.bottomLeft()),
        QLineF(rect.bottomLeft(), rect.topLeft())
    };

    if(rect.contains(line.p1()) && !rect.contains(line.p2()))
    {
        QPointF p2;
        for(int i = 0; i < 4; i++)
        {
            if(QLineF::BoundedIntersection == line.intersect(edges[i], &p2))
            {
                return QLineF(line.p1(), p2);
            }
        }
    }

    if(!rect.contains(line.p1()) && rect.contains(line.p2()))
    {
        QPointF p1;
        for(int i = 0; i < 4; i++)
        {
            if(QLineF::BoundedIntersection == line.intersect(edges[i], &p1))
            {
                return QLineF(p1, line.p2());
            }
        }
    }

    if(!rect.contains(line.p1()) && !rect.contains(line.p2()))
    {
        QList<QPointF> points;
        for(int i = 0; i < 4; i++)
        {
            QPointF pt;
            if(QLineF::BoundedIntersection == line.intersect(edges[i], &pt))
            {
                points.push_back(pt);
            }
        }

        double eps = 1.0e-10;
        QPointF p = points[0];
        for(int i = 1; i < points.size(); i++)
        {
            QPointF q = points[i];
            if(p == q) continue;

            if((p.x() > q.x()) || (std::abs(p.x() - q.x()) < eps && p.y() > q.y()))
            {
                return QLineF(q, p);
            }
            else
            {
                return QLineF(p, q);
            }
        }
    }

    return line;
}

CorrelationDimension::CorrelationDimension()
{

}

int CorrelationDimension::Count(QRectF doi, double cl)
{
    QVector<QPointF> coords = this->GenerateFractureMidpoint();

    QVector<QPoint> pixels;
    for(int ielem = 0; ielem < coords.size(); ielem++)
    {
        int ix = std::floor((coords[ielem].x() - doi.x()) / cl) + 1;
        int iy = std::floor((coords[ielem].y() - doi.y()) / cl) + 1;
        pixels.push_back(QPoint(ix, iy));
    }

    int numCouples = 0;
    int nelem = coords.size();
    int nelemd = 2 * nelem;
    int ncelx = std::round(doi.width() / cl) + 2;
    int ncely = std::round(doi.height() / cl) + 2;

    int i1heay[2];
    int i1heax[5];

    int *i2cfx[2];
    i2cfx[0] = new int[ncelx];
    i2cfx[1] = new int[ncelx];
    int *i1cfy = new int[ncely];

    int *i1cny = new int[nelem];
    int *i1cnx = new int[nelem];

    for(int i = 0; i < ncely; i++)
    {
        i1cfy[i] = -1;
    }

    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < ncelx; j++)
        {
            i2cfx[i][j] = -1;
        }
    }

    for(int ielem = 0; ielem < nelem; ielem++)
    {
        i1cny[ielem] = i1cfy[pixels[ielem].y()];
        i1cfy[pixels[ielem].y()] = ielem;
    }

    for(int ielem = 0; ielem < nelem; ielem++)
    {
        int iy = pixels[ielem].y();
        if(i1cfy[iy] < nelem)
        {
            i1heay[0] = i1cfy[iy];
            i1heay[1] = i1cfy[iy - 1];
            if(i1heay[1] > nelem) i1heay[1] = i1heay[1] - nelemd;
            i1cfy[iy] = i1cfy[iy] + nelemd;

            for(int ihy = 0; ihy < 2; ihy++)
            {
                int ielemy = i1heay[ihy];
                while(ielemy >= 0 )
                {
                    i1cnx[ielemy] = i2cfx[ihy][pixels[ielemy].x()];
                    i2cfx[ihy][pixels[ielemy].x()] = ielemy;
                    ielemy = i1cny[ielemy];
                }
            }

            int ielemy = i1heay[0];
            while(ielemy > 0)
            {
                int ix = pixels[ielemy].x();
                if(i2cfx[0][ix] < nelem)
                {
                    i1heax[0] = i2cfx[0][ix];
                    i1heax[1] = i2cfx[0][ix - 1];
                    i1heax[2] = i2cfx[1][ix + 1];
                    i1heax[3] = i2cfx[1][ix];
                    i1heax[4] = i2cfx[1][ix - 1];
                    if(i1heax[1] > nelem) i1heax[1] = i1heax[1] - nelemd;
                    i2cfx[0][ix] = i2cfx[0][ix] + nelemd;

                    int ielemx = i1heax[0];
                    while(ielemx >= 0)
                    {
                        for(int ih = 0; ih < 5; ih++)
                        {
                            int jelemx = i1heax[ih];
                            while(jelemx >= 0)
                            {
                                if(ih != 0 || ielemx > jelemx)
                                {
                                    double dx = coords[jelemx].x() - coords[ielemx].x();
                                    double dy = coords[jelemx].y() - coords[ielemx].y();
                                    double length = std::sqrt(dx*dx + dy*dy);
                                    if(length <= cl)
                                    {
                                        numCouples += 1;
                                    }
                                }
                                jelemx = i1cnx[jelemx];
                            }
                        }
                        ielemx = i1cnx[ielemx];
                    }
                }
                ielemy = i1cny[ielemy];
            }

            for (int ihy = 0; ihy < 2; ihy++)
            {
                ielemy = i1heay[ihy];
                while (ielemy >= 0)
                {
                    i2cfx[ihy][pixels[ielemy].x()] = -1;
                    ielemy = i1cny[ielemy];
                }
            }
        }
    }

    delete [] i1cfy;
    delete [] i2cfx[0];
    delete [] i2cfx[1];
    delete [] i1cnx;
    delete [] i1cny;

    return numCouples;
}

int CorrelationDimension::BruteForceCount(QRectF, double cl)
{
    int numCouples = 0;
    QVector<QPointF> points = this->GenerateFractureMidpoint();

    for(int i = 0; i < points.size() - 1; i++)
    {
        for(int j = i + 1; j < points.size(); j++)
        {
            double dx = points[i].x() - points[j].x();
            double dy = points[i].y() - points[j].y();
            double length = std::sqrt(dx*dx + dy*dy);
            if(length <= cl)
            {
                numCouples += 1;
            }
        }
    }

    return numCouples;
}
