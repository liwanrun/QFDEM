#ifndef SOLUTIONCOLLECTION_H
#define SOLUTIONCOLLECTION_H

#include <QMap>
#include <QMutex>
#include <QSharedPointer>
#include <QTextStream>

enum SolutionModule
{
    MECHANICAL = 1, HYDRAULIC = 2, THERMAL = 4
};

class Solution;
class SolutionCollection
{
public:
    typedef QMap<int, QSharedPointer<Solution>> SolutionMap;

public:
    ~SolutionCollection();

    static QSharedPointer<SolutionCollection> GetSolutionCollection();

    const SolutionMap& GetSolutionMap() const;

    void ClearSolutionMap();

    QSharedPointer<Solution> GetSolution(int index);

    void AppendSolution(int index);

    void RemoveSolution(int index);

    void Clear();

public:
    int GetSolutionModule();

    void SetSolutionModule(int arg);

private:
    SolutionCollection();

public:
    bool IsSolutionOptionEnabled(QString arg);

    void OutputSolutionProperty(QTextStream &out, QString arg);

    friend QTextStream &operator<<(QTextStream &out, QSharedPointer<SolutionCollection> collection);

public:
     friend class FDEMPreferenceWidget;

private:
    static QMutex mutex;
    static QSharedPointer<SolutionCollection> solutionCollection;

    int solution_module;

    int mechanicalSolution;

    int hydraulicSolution;
    int monolithicSchemes;
    int linearSparseSolver;

    int porosityAssumption;
    int porosityFlowModel;
    int porosityIntegration;
    int fractureAssumption;
    int fractureFlowModel;
    int fractureIntegration;

    int triThermStrategy;
    int triThermSolution;
    int cohThermStrategy;
    int cohThermSolution;

    int couplingStrategy;
    int maxNumIterations;
    double absMechTolerance;
    double absHydroTolerance;
    double absThermTolerance;

    SolutionMap solution_map;
};


QTextStream &operator<<(QTextStream &out, QSharedPointer<SolutionCollection> collection);

#endif // SOLUTIONCOLLECTION_H
