#include "samplestrategy.h"
#include "multicascadetree.h"

#include <random>
#include <QtMath>
#include <QDebug>
#include <QMessageBox>

QSharedPointer<PointSampleStrategy> PointStrategyFactory::CreateStrategy(const PDFInfo& info)
{
    QSharedPointer<PointSampleStrategy> strategy = nullptr;

    if(0 == info.m_type)   //Uniform PDF
    {
        strategy = QSharedPointer<PointSampleStrategy>(new UniformPointStrategy(info.m_seed));
    }
    else if(1 == info.m_type)  //Fractal PDF
    {
        strategy = QSharedPointer<PointSampleStrategy>
                (new FractalPointStrategy(info.m_seed, info.m_args[0], info.m_args[1], info.m_args[2], info.m_args[3]));
    }
    else if(2 == info.m_type)  //Regular PDF
    {
        strategy = QSharedPointer<PointSampleStrategy>(new RegularPointStrategy(info.m_args[0], info.m_args[1]));
    }

    return strategy;
}

QSharedPointer<ValueSampleStrategy> ValueStrategyFactory::CreateStrategy(const PDFInfo& info)
{
    QSharedPointer<ValueSampleStrategy> strategy = nullptr;

    if(UNIFORM == info.m_type)   //Uniform PDF
    {
        strategy = QSharedPointer<ValueSampleStrategy>(
                    new UniformValueStrategy(info.m_seed, info.m_args[0], info.m_args[1]));
    }
    else if(EXPONENT == info.m_type)  //Exponential PDF
    {
        strategy = QSharedPointer<ValueSampleStrategy>(
                    new ExponentValueStrategy(info.m_seed, info.m_args[0], info.m_args[1], info.m_args[2]));
    }
    else if(NORMAL == info.m_type)  //Normal PDF
    {
        strategy = QSharedPointer<ValueSampleStrategy>(
                    new NormalValueStrategy(info.m_seed, info.m_args[0], info.m_args[1]));
    }
    else if(LOGNORMAL == info.m_type)   //LogNormal PDF
    {
        strategy = QSharedPointer<ValueSampleStrategy>(
                    new LogNormalValueStrategy(info.m_seed, info.m_args[0], info.m_args[1]));
    }
    else if(WEIBULL == info.m_type)   //Weibull PDF
    {
        strategy = QSharedPointer<ValueSampleStrategy>(
                    new WeibullValueStrategy(info.m_seed, info.m_args[0], info.m_args[1]));
    }
    else if(DUALPOWER == info.m_type)   //DualPower PDF
    {
        strategy = QSharedPointer<ValueSampleStrategy>(
                    new DualPowerValueStrategy(info.m_seed, info.m_args[0], info.m_args[1], info.m_args[2]));
    }

    return strategy;
}

UniformPointStrategy::UniformPointStrategy(int seed)
    : randseed(seed)
{

}

std::vector<QPointF> UniformPointStrategy::Sample(QRectF doi, size_t &size)
{
    std::vector<QPointF> points;

    std::random_device rd;
    std::default_random_engine generator((0 == randseed) ? rd() : randseed);
    std::uniform_real_distribution<double> XcoordDistribution(doi.left(), doi.right());
    std::uniform_real_distribution<double> YcoordDistribution(doi.top(), doi.bottom());

    for(int i = 0; i < size; i++)
    {
        double x = XcoordDistribution(generator);
        double y = YcoordDistribution(generator);
        points.emplace_back(QPointF(x, y));
    }
    return points;
}

FractalPointStrategy::FractalPointStrategy(int seed, double alpha, double dim, double power, int depth)
    : randseed(seed), m_alpha(alpha), m_dim(dim), m_power(power), m_depth(depth)

{

}

std::vector<QPointF> FractalPointStrategy::Sample(QRectF doi, size_t &size)
{
    if(doi.width() - doi.height() > 1.0e-06)
    {
        QMessageBox::warning(nullptr, QString("Warning"),
                             QString("Please specify square domain of interest.\nThank you!"));

        return std::vector<QPointF>(0);
    }

    /*1. Calculate the number of discontinuities in joint set */
    double Lr = (doi.width() + doi.height()) / 2.0;
    double Lmin = Lr / qPow(2, m_depth - 1);
    size = qRound((m_alpha / (m_power - 1.0) * qPow(Lr, m_dim) * qPow(Lmin, (1.0 - m_power))));

    /*2. Generate accumulative probability sequence(MCP) */
    std::vector<double> likelihoods;
    QSharedPointer<MultiCascadeTree> cascadeTree =              //Create a MCP tree(bottom->top)
            QSharedPointer<MultiCascadeTree>(new MultiCascadeTree(this->m_depth, this->m_dim));
    cascadeTree->UpdateProbability();                           //Recursively update probability
    likelihoods = cascadeTree->CreateProbabilitySequence();     //Probability density sequence(+1)

    /*3. Sample joint position according to probability matrix */
    std::random_device rd;
    std::default_random_engine generator((0 == randseed) ? rd() : randseed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    std::vector<QPointF> points;
    size_t ndivs = qPow(2, m_depth - 1);
    for(size_t i = 0; i < size; i++)
    {
        double rand = distribution(generator);

        size_t left = 0, mid;
        size_t right = likelihoods.size() - 1;
        do
        {
            mid = (left + right) / 2;
            if(rand < likelihoods[mid])
            {
                right = mid;
            }
            else
            {
                left = mid;
            }
        } while(right - left > 1);

        double glb_x = (left >> (m_depth - 1)) * Lmin;
        double lub_x = glb_x + Lmin;
        std::uniform_real_distribution<double> XcoordDistribution(glb_x, lub_x);
        double x = XcoordDistribution(generator);

        double glb_y = (left & (ndivs - 1)) * Lmin;
        double lub_y = glb_y + Lmin;
        std::uniform_real_distribution<double> YcoordDistribution(glb_y, lub_y);
        double y = YcoordDistribution(generator);

        points.emplace_back(QPointF(x, y));
    }

    return points;
}

RegularPointStrategy::RegularPointStrategy(int rows, int cols)
    : m_rows(rows), m_cols(cols)
{

}

std::vector<QPointF> RegularPointStrategy::Sample(QRectF doi, size_t &size)
{
    std::vector<QPointF> points;
    double dx = doi.width() / (this->m_cols + 1);
    double dy = doi.height() / (this->m_rows + 1);

    size = this->m_rows * m_cols;
    for(int i = 1; i < this->m_rows + 1; i++)
    {
        for(int j = 1; j < this->m_cols + 1; j++)
        {
            points.emplace_back(QPointF(doi.x() + i * dx, doi.y() + j * dy));
        }
    }

    return points;
}

UniformValueStrategy::UniformValueStrategy(int seed, double a, double b)
    : ValueSampleStrategy (seed),
      m_lower(a),
      m_upper(b)
{

}

std::vector<double> UniformValueStrategy::Sample(size_t size)
{
    std::vector<double> sequence;
    std::random_device rd;
    std::default_random_engine generator((0 == randseed) ? rd() : randseed);
    std::uniform_real_distribution<double> distribution(m_lower, m_upper);

    for(int i = 0; i < size; i++)
    {
        sequence.emplace_back(distribution(generator));
    }
    return sequence;
}

ExponentValueStrategy::ExponentValueStrategy(int seed, double theta, double min, double max)
    : ValueSampleStrategy (seed), m_lambda(theta), m_lower(min), m_upper(max)
{

}

std::vector<double> ExponentValueStrategy::Sample(size_t size)
{
    std::vector<double> sequence;
    std::random_device rd;
    std::default_random_engine generator((0 == randseed) ? rd() : randseed);
    std::exponential_distribution<double> distribution(m_lambda);

    for(int i = 0; i < size; i++)
    {
        double value;
        do
        {
            value = distribution(generator);
        }
        while(value < m_lower || value > m_upper);
        sequence.emplace_back(value);
    }
    return sequence;
}

NormalValueStrategy::NormalValueStrategy(int seed, double mean, double dev)
    : ValueSampleStrategy (seed), m_mean(mean), m_dev(dev)
{

}

std::vector<double> NormalValueStrategy::Sample(size_t size)
{
    if(m_dev > 0.0)
    {
        std::vector<double> sequence;
        std::random_device rd;
        std::default_random_engine generator((0 == randseed) ? rd() : randseed);
        std::normal_distribution<double> distribution(m_mean, m_dev);

        for(int i = 0; i < size; i++)
        {
            sequence.emplace_back(distribution(generator));
        }
        return sequence;
    }
    else
    {
        return std::vector<double>(size, m_mean);
    }
}

LogNormalValueStrategy::LogNormalValueStrategy(int seed, double mean, double dev)
    : ValueSampleStrategy (seed), m_mean(mean), m_dev(dev)
{

}

std::vector<double> LogNormalValueStrategy::Sample(size_t size)
{
    if(m_dev > 0.0)
    {
        std::vector<double> sequence;
        std::random_device rd;
        std::default_random_engine generator((0 == randseed) ? rd() : randseed);
        std::lognormal_distribution<double> distribution(m_mean, m_dev);

        for(int i = 0; i < size; i++)
        {
            sequence.emplace_back(distribution(generator));
        }
        return sequence;
    }
    else
    {
        return std::vector<double>(size, m_mean);
    }
}

WeibullValueStrategy::WeibullValueStrategy(int seed, double exp, double mean)
    : ValueSampleStrategy (seed), m_exp(exp), m_mean(mean)
{

}

std::vector<double> WeibullValueStrategy::Sample(size_t size)
{
    std::vector<double> sequence;
    std::random_device rd;
    std::default_random_engine generator((0 == randseed) ? rd() : randseed);
    std::weibull_distribution<double> distribution(m_exp, m_mean);

    for(int i = 0; i < size; i++)
    {
        sequence.emplace_back(distribution(generator));
    }
    return sequence;
}

DualPowerValueStrategy::DualPowerValueStrategy(int seed, double exp, double ext, int depth)
    : ValueSampleStrategy (seed), m_exp(exp), m_ext(ext), m_depth(depth)
{

}

std::vector<double> DualPowerValueStrategy::Sample(size_t size)
{
    std::vector<double> sequence;
    std::random_device rd;
    std::default_random_engine generator((0 == randseed) ? rd() : randseed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    double Lmin = m_ext / qPow(2, m_depth - 1);
    for(int i = 0; i < size; i++)
    {
        double rand = distribution(generator);
        double var = std::pow((1.0 - rand), 1.0 / (1.0 - m_exp)) * Lmin;
        sequence.emplace_back(var);
    }
    return sequence;
}

