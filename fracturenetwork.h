#ifndef FRACTURENETWORK_H
#define FRACTURENETWORK_H

#include <QMap>
#include <QLineF>
#include <QString>
#include <QSharedPointer>
#include <vector>

enum Distribution
{
    UNIFORM = 0,
    EXPONENT,
    NORMAL,
    LOGNORMAL,
    WEIBULL,
    DUALPOWER
};


class PDFInfo
{
public:
    PDFInfo(int seed = 0, int type = 0)
        : m_seed(seed), m_type(type)
    {
        m_args[0] = 0.0;
        m_args[1] = 0.0;
        m_args[2] = 0.0;
        m_args[3] = 0.0;
        m_args[4] = 0.0;
    }

public:
    int m_seed;
    int m_type;
    double m_args[5];
};

class FractureSet
{
public:
    friend class FractureNetworkDialog;

public:
    FractureSet(size_t size = 10) : m_size(size) {}
    virtual ~FractureSet() {}

    virtual void Generate(QRectF doi) = 0;

    size_t GetNumberOfFractures()
    {
        return this->m_size;
    }

    std::vector<QLineF> GetFractureVector()
    {
        return this->fractures;
    }

protected:
    int pattern;
    size_t m_size;

    std::vector<QLineF> fractures;
};

class PointSampleStrategy;
class ValueSampleStrategy;
class PointTraceFractureSet : public FractureSet
{
public:
    friend class FractureNetworkDialog;

public:
    PointTraceFractureSet();
    ~PointTraceFractureSet() override {}

    void Generate(QRectF doi) override;

protected:
    std::vector<QPointF> GenerateFracturePosition(QRectF doi, PointSampleStrategy *strategy);

    std::vector<double> GenerateFractureTrace(ValueSampleStrategy *strategy);

    std::vector<double> GenerateFractureDip(ValueSampleStrategy *strategy);

private:
    PDFInfo posPdf;
    PDFInfo modPdf;
    PDFInfo dipPdf;
};

class PointPointFractureSet : public FractureSet
{
public:
    friend class FractureNetworkDialog;

public:
    PointPointFractureSet();
    ~PointPointFractureSet() override {}

    void Generate(QRectF doi) override;

private:
    std::vector<QPointF> GenerateFractureSource(QRect doi, PointSampleStrategy* strategy);

    std::vector<QPointF> GenerateFractureTarget(QRect doi, PointSampleStrategy* strategy);

private:
    PDFInfo srcPdf;
    PDFInfo tarPdf;
};

class FractureNetwork
{
public:
    typedef QMap<QString, QSharedPointer<FractureSet>> FractureMap;

public:
    FractureNetwork();

    void Generate(QRectF doi);

    const FractureMap& GetFractureMap() const;

    QSharedPointer<FractureSet> GetFractureSet(const QString& id);

    void UpdateFractureSet(const QString& id, QSharedPointer<FractureSet> fracSet);

    void AppendFractureSet(const QString& id);

    void RemoveFractureSet(const QString& id);

    void Clear();

private:
    FractureMap fracture_map;
};

#endif // FRACTURENETWORK_H
