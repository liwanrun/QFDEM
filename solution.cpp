#include "solution.h"
#include "globalmacros.h"
#include <QDebug>

Solution::Solution()
{
    propertyMap.insert(QString("I1MMODE"), QVariant(3));
    propertyMap.insert(QString("I1HMODE"), QVariant(2));
    propertyMap.insert(QString("I1TMODE"), QVariant(1));

    propertyMap.insert(QString("I1TSTEP"), QVariant(10001));
    propertyMap.insert(QString("I1COUTF"), QVariant(100));
    propertyMap.insert(QString("D1MSTEC"), QVariant(2.0e-06));
    propertyMap.insert(QString("D1HSTEC"), QVariant(2.0e-06));
    propertyMap.insert(QString("D1TSTEC"), QVariant(2.0e-06));

    propertyMap.insert(QString("I1CGRAV"), QVariant(false));
    propertyMap.insert(QString("D1CGRAX"), QVariant(0.0));
    propertyMap.insert(QString("D1CGRAY"), QVariant(-10.0));
    propertyMap.insert(QString("I1CDAMP"), QVariant(false));
    propertyMap.insert(QString("D1CDAMP"), QVariant(0.8));
    propertyMap.insert(QString("I1CCRIT"), QVariant(false));
    propertyMap.insert(QString("D1CCRIT"), QVariant(1.0e-05));
    propertyMap.insert(QString("I1CONTA"), QVariant(false));
    propertyMap.insert(QString("I1CMCOH"), QVariant(false));
    propertyMap.insert(QString("I1CHEFF"), QVariant(true));
    propertyMap.insert(QString("I1CSTEX"), QVariant(false));
}

Solution::~Solution()
{
    this->propertyMap.clear();
}
