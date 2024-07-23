#ifndef FRACTALDIMENSION_H
#define FRACTALDIMENSION_H

#include <QSharedPointer>
#include <QRectF>
#include <QLineF>
#include <QPointF>
#include <QVector>

class BlockCollection;
class FractalDimension
{
public:
    FractalDimension();

    virtual int Count(QRectF doi, double cl) = 0;

protected:
    QVector<QPointF> GenerateFractureMidpoint();

    QVector<QLineF> GetSpaceSortedFractures();

protected:
    QSharedPointer<BlockCollection> blockCollection;
};

class BoxCountingDimension : public FractalDimension
{
public:
    BoxCountingDimension();

    int Count(QRectF doi, double cl) override;

private:
    QLineF ClipLineByRectangle(QLineF line, QRectF rect);
};

class CorrelationDimension : public FractalDimension
{
public:
    CorrelationDimension();

    int Count(QRectF doi, double cl) override;

    int BruteForceCount(QRectF doi, double cl);
};

#endif // FRACTALDIMENSION_H
