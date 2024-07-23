#ifndef SOLUTION_H
#define SOLUTION_H

#include <QPointF>
#include <QMutex>
#include <QTextStream>
#include <QSharedPointer>

class Solution
{
public:
    enum MechanicalModel
    {
        DEFORMATION = 1, CZMFRACTURE = 2
    };

    enum HydraulicModel
    {
        POROSITYSEEP = 1, FRACTUREFLOW = 2
    };

    enum ThermalModel
    {
        SOLIDHEATEX = 1, FLUIDHEATEX = 2
    };

public:
    Solution();
    ~Solution();

public:
    friend class SolutionCollection;

    friend class FDEMSolutionWidget;
    friend class FDEMSolutionSubstepWidget;
    friend class FDEMTriangleMaterialWidget;
    friend class FDEMCohesiveMaterialWidget;

private:
    QMap<QString, QVariant> propertyMap;
};

#endif // SOLUTION_H
