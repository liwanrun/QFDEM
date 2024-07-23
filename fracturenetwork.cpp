#include "fracturenetwork.h"
#include "samplestrategy.h"

#include <QRectF>
#include <QtMath>

FractureNetwork::FractureNetwork()
{

}

void FractureNetwork::Generate(QRectF doi)
{
    for(auto iter = fracture_map.begin(); iter != fracture_map.end(); ++iter)
    {
        iter.value()->Generate(doi);
    }
}

const FractureNetwork::FractureMap &FractureNetwork::GetFractureMap() const
{
    return this->fracture_map;
}

QSharedPointer<FractureSet> FractureNetwork::GetFractureSet(const QString &id)
{
    if(!this->fracture_map.contains(id))
    {
        this->AppendFractureSet(id);
    }
    return this->fracture_map[id];
}

void FractureNetwork::UpdateFractureSet(const QString &id, QSharedPointer<FractureSet> fracSet)
{
    if(this->fracture_map.contains(id))
    {
        this->fracture_map[id] = fracSet;
    }
}

void FractureNetwork::AppendFractureSet(const QString &id)
{
    QSharedPointer<FractureSet> fracSet = QSharedPointer<PointTraceFractureSet>(new PointTraceFractureSet);
    this->fracture_map.insert(id, fracSet);
}

void FractureNetwork::RemoveFractureSet(const QString &id)
{
    this->fracture_map.remove(id);
}

void FractureNetwork::Clear()
{
    this->fracture_map.clear();
}

PointTraceFractureSet::PointTraceFractureSet()
    : FractureSet ()
{
    posPdf = PDFInfo(0, 0);
    modPdf = PDFInfo(0, 1);
    dipPdf = PDFInfo(0, 2);
}

void PointTraceFractureSet::Generate(QRectF doi)
{
    QSharedPointer<PointSampleStrategy> posStrategy = PointStrategyFactory::CreateStrategy(this->posPdf);
    std::vector<QPointF> points = this->GenerateFracturePosition(doi, posStrategy.data());

    QSharedPointer<ValueSampleStrategy> modStrategy = ValueStrategyFactory::CreateStrategy(this->modPdf);
    std::vector<double> traces = this->GenerateFractureTrace(modStrategy.data());

    QSharedPointer<ValueSampleStrategy> dipStrategy = ValueStrategyFactory::CreateStrategy(this->dipPdf);
    std::vector<double> dips = this->GenerateFractureDip(dipStrategy.data());

    for(size_t id = 0; id < this->m_size; id++)
    {
        double x0 = points[id].x() - traces[id] * qCos(qDegreesToRadians(dips[id])) / 2.0;
        double y0 = points[id].y() - traces[id] * qSin(qDegreesToRadians(dips[id])) / 2.0;
        double x1 = points[id].x() + traces[id] * qCos(qDegreesToRadians(dips[id])) / 2.0;
        double y1 = points[id].y() + traces[id] * qSin(qDegreesToRadians(dips[id])) / 2.0;

        this->fractures.emplace_back(QPointF(x0, y0), QPointF(x1, y1));
    }
}

std::vector<QPointF> PointTraceFractureSet::GenerateFracturePosition(QRectF doi, PointSampleStrategy *strategy)
{
    return strategy->Sample(doi, this->m_size);
}

std::vector<double> PointTraceFractureSet::GenerateFractureTrace(ValueSampleStrategy *strategy)
{
    return strategy->Sample(this->m_size);
}

std::vector<double> PointTraceFractureSet::GenerateFractureDip(ValueSampleStrategy *strategy)
{
    return strategy->Sample(this->m_size);
}

PointPointFractureSet::PointPointFractureSet()
    : FractureSet ()
{
    srcPdf = PDFInfo(0, 0);
    tarPdf = PDFInfo(0, 0);
}

void PointPointFractureSet::Generate(QRectF doi)
{
    QSharedPointer<PointSampleStrategy> srcStrategy = PointStrategyFactory::CreateStrategy(this->srcPdf);
    std::vector<QPointF> sources = srcStrategy->Sample(doi, m_size);

    QSharedPointer<PointSampleStrategy> tarStrategy = PointStrategyFactory::CreateStrategy(this->tarPdf);
    std::vector<QPointF> targets = tarStrategy->Sample(doi, m_size);

    for(size_t id = 0; id < this->m_size; id++)
    {
        this->fractures.emplace_back(QLineF(sources[id], targets[id]));
    }
}

std::vector<QPointF> PointPointFractureSet::GenerateFractureSource(QRect doi, PointSampleStrategy *strategy)
{
    return strategy->Sample(doi, this->m_size);
}

std::vector<QPointF> PointPointFractureSet::GenerateFractureTarget(QRect doi, PointSampleStrategy *strategy)
{
    return strategy->Sample(doi, this->m_size);
}

