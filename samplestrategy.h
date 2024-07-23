#ifndef SAMPLESTRATEGY_H
#define SAMPLESTRATEGY_H

#include <QRectF>
#include <vector>
#include <QSharedPointer>

#include "fracturenetwork.h"

//Strategy Factory
class PointStrategyFactory
{
public:
    static QSharedPointer<PointSampleStrategy> CreateStrategy(const PDFInfo& info);
};

class ValueStrategyFactory
{
public:
    static QSharedPointer<ValueSampleStrategy> CreateStrategy(const PDFInfo& info);
};


class PointSampleStrategy
{
public:
    PointSampleStrategy() {};
    virtual ~PointSampleStrategy() {};

    virtual std::vector<QPointF> Sample(QRectF doi, size_t& size) = 0;
};

class UniformPointStrategy : public PointSampleStrategy
{
public:
    UniformPointStrategy(int seed);
    ~UniformPointStrategy() override {}

    std::vector<QPointF> Sample(QRectF doi, size_t& size) override;

private:
    int randseed;
};

class FractalPointStrategy : public PointSampleStrategy
{
public:
    FractalPointStrategy(int seed, double alpha, double dim, double power, int depth);
    ~FractalPointStrategy() override {}

    std::vector<QPointF> Sample(QRectF doi, size_t& size) override;

private:
    int randseed;
    double m_alpha;
    double m_dim;
    double m_power;
    int m_depth;
};

class RegularPointStrategy : public PointSampleStrategy
{
public:
    RegularPointStrategy(int rows, int cols);
    ~RegularPointStrategy() override {}

    std::vector<QPointF> Sample(QRectF doi, size_t& size) override;

private:
    int m_rows;
    int m_cols;
};


class ValueSampleStrategy
{
public:
    ValueSampleStrategy(int seed) : randseed(seed) {};
    virtual ~ValueSampleStrategy() {}

    virtual std::vector<double> Sample(size_t size) = 0;

protected:
    int randseed;
};

class UniformValueStrategy : public ValueSampleStrategy
{
public:
    UniformValueStrategy(int seed, double a = 0.0, double b = 1.0);
    ~UniformValueStrategy() override {}

    std::vector<double> Sample(size_t size) override;

private:
    double m_lower;
    double m_upper;
};

class ExponentValueStrategy : public ValueSampleStrategy
{
public:
    ExponentValueStrategy(int seed, double theta, double min = 0.0, double max = 1.0);
    ~ExponentValueStrategy() override {}

    std::vector<double> Sample(size_t size) override;

private:
    double m_lambda;
    double m_lower;
    double m_upper;
};

class NormalValueStrategy : public ValueSampleStrategy
{
public:
    NormalValueStrategy(int seed, double mean, double dev);
    ~NormalValueStrategy() override {}

    std::vector<double> Sample(size_t size) override;

private:
    double m_mean;
    double m_dev;
};

class WeibullValueStrategy : public ValueSampleStrategy
{
public:
    WeibullValueStrategy(int seed, double exp, double mean);
    ~WeibullValueStrategy() override {}

    std::vector<double> Sample(size_t size) override;

private:
    double m_exp;
    double m_mean;
};

class LogNormalValueStrategy : public ValueSampleStrategy
{
public:
    LogNormalValueStrategy(int seed, double mean, double dev);
    ~LogNormalValueStrategy() override {}

    std::vector<double> Sample(size_t size) override;

private:
    double m_mean;
    double m_dev;
};

class DualPowerValueStrategy : public ValueSampleStrategy
{
public:
    DualPowerValueStrategy(int seed, double exp, double ext, int depth);
    ~DualPowerValueStrategy() override {}

    std::vector<double> Sample(size_t size) override;

private:
    double m_exp;
    double m_ext;
    int m_depth;
};



#endif // SAMPLESTRATEGY_H
