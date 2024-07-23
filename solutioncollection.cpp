#include "solutioncollection.h"
#include "globalmacros.h"
#include "solution.h"

#include <QDebug>

QMutex SolutionCollection::mutex;

QSharedPointer<SolutionCollection> SolutionCollection::solutionCollection = nullptr;

SolutionCollection::~SolutionCollection()
{
    qDebug() << "~SolutionCollection()";
}

QSharedPointer<SolutionCollection> SolutionCollection::GetSolutionCollection()
{
    if(nullptr == solutionCollection)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == solutionCollection)
        {
            solutionCollection = QSharedPointer<SolutionCollection>(new SolutionCollection);
        }
    }

    return solutionCollection;
}

const SolutionCollection::SolutionMap &SolutionCollection::GetSolutionMap() const
{
    return this->solution_map;
}

void SolutionCollection::ClearSolutionMap()
{
    this->solution_map.clear();
}

QSharedPointer<Solution> SolutionCollection::GetSolution(int index)
{
    if(!this->solution_map.contains(index))
    {
        this->AppendSolution(index);
    }
    return this->solution_map[index];
}

void SolutionCollection::AppendSolution(int index)
{
    QString title = QString("solution-%1").arg(index);
    QSharedPointer<Solution> solution = QSharedPointer<Solution>(new Solution());
    this->solution_map.insert(index, solution);
}

void SolutionCollection::RemoveSolution(int index)
{
    this->solution_map.remove(index);
}

void SolutionCollection::Clear()
{
    this->solution_map.clear();
}

SolutionCollection::SolutionCollection()
    : solution_module(MECHANICAL)
{
    qDebug() << "SolutionCollection()";

    /* Mechanical */
    this->mechanicalSolution = 0;

    /* Hydraulic */
    this->hydraulicSolution = 0;
    this->monolithicSchemes = 1;
    this->linearSparseSolver = 1;
    this->porosityAssumption = 0;
    this->porosityFlowModel = 0;
    this->porosityIntegration = 0;
    this->fractureAssumption = 0;
    this->fractureFlowModel = 0;
    this->fractureIntegration = 0;

    /* Thermal */
    this->triThermStrategy = 0;
    this->triThermSolution = 1;
    this->cohThermStrategy = 0;
    this->cohThermSolution = 0;

    /* Coupling */
    this->couplingStrategy = 0;
    this->maxNumIterations = 1000;
    this->absMechTolerance = 1.0e-03;
    this->absHydroTolerance = 100.0;
    this->absThermTolerance = 1.0e-03;
}

int SolutionCollection::GetSolutionModule()
{
    return this->solution_module;
}

void SolutionCollection::SetSolutionModule(int arg)
{
    this->solution_module = arg;
}

bool SolutionCollection::IsSolutionOptionEnabled(QString arg)
{
    for(auto iter = this->solution_map.begin(); iter != this->solution_map.end(); ++iter)
    {
        if(iter.value()->propertyMap[arg].toBool())
            return true;
    }
    return false;
}

void SolutionCollection::OutputSolutionProperty(QTextStream &out, QString arg)
{
    int ncols = 5;
    int index = 0;
    int width = 15;

    PRINT_INT_SCALAR(out, QString("/YD/YDC/") + arg, this->solution_map.size());
    for(auto iter = this->solution_map.begin(); iter != this->solution_map.end(); ++iter)
    {
        if((index != 0) && (index % ncols == 0))
            out << reset << endl;

        QVariant val = iter.value()->propertyMap[arg];

        if(QString("int") == val.typeName())
        {
            out << qSetFieldWidth(width) << left << val.toInt() << reset;
        }
        else if(QString("bool") == val.typeName())
        {
            out << qSetFieldWidth(width) << left << val.toBool() << reset;
        }
        else if(QString("string") == val.typeName())
        {
            out << qSetFieldWidth(width) << left << val.toString() << reset;
        }
        else if(QString("double") == val.typeName())
        {
            out << qSetFieldWidth(width) << left << forcepoint << val.toDouble() << reset;
        }

        index += 1;
    }

    out << reset << endl;
}

QTextStream &operator<<(QTextStream &out, QSharedPointer<SolutionCollection> collection)
{
    PRINT_STR_COMMENT(out, QString("/* SOLUTION PREFERENCE */"));
    PRINT_INT_NOENDL(out, QString("/YD/YDC/ICMODE"), collection->GetSolutionModule());
    PRINT_STR_COMMENT(out, QString("/* (1)Mechanical (2)Hydraulic (4)Thermal */"));

    if(MECHANICAL & collection->GetSolutionModule())
    {
        PRINT_INT_NOENDL(out, QString("/YD/YDC/ICMJDO"), collection->mechanicalSolution);
        PRINT_STR_COMMENT(out, QString("/* (0)Intrinsic (1)Extrinsic */"));
    }

    if(HYDRAULIC & collection->GetSolutionModule())
    {
        PRINT_INT_NOENDL(out, QString("/YD/YDC/ICHWAY"), collection->hydraulicSolution);
        PRINT_STR_COMMENT(out, QString("/* (0)Partitioned (1)Monolithic */"));
        PRINT_INT_NOENDL(out, QString("/YD/YDC/ICHMOD"), collection->monolithicSchemes);
        PRINT_STR_COMMENT(out, QString("/* (0)Explicit-shared (1)Implicit-shared "
                                          "(2)Explicit-channel (3)Implicit-channel (4)Implicit-discrete */"));
        PRINT_INT_NOENDL(out, QString("/YD/YDC/ICHLSS"), collection->linearSparseSolver);
        PRINT_STR_COMMENT(out, QString("/* (0)Skyline (1)Pardiso (2)SuperLU (3)DiagPcg | (0)DSS (1)ISS */"));

        PRINT_INT_NOENDL(out, QString("/YD/YDC/ICHEIF"), collection->porosityAssumption);
        PRINT_STR_COMMENT(out, QString("/* (0)Continuous (1)Discete (2)Progressive */"));
        PRINT_INT_NOENDL(out, QString("/YD/YDC/ICHECM"), collection->porosityFlowModel);
        PRINT_STR_COMMENT(out, QString("/* (0)Pipe-based (1)Cell-based */"));
        PRINT_INT_NOENDL(out, QString("/YD/YDC/ICHEDO"), collection->porosityIntegration);
        PRINT_STR_COMMENT(out, QString("/* (0)Explicit (1)Implicit */"));

        PRINT_INT_NOENDL(out, QString("/YD/YDC/ICHJIF"), collection->fractureAssumption);
        PRINT_STR_COMMENT(out, QString("/* (0)Incompressible (1)Compressible */"));
        PRINT_INT_NOENDL(out, QString("/YD/YDC/ICHJCM"), collection->fractureFlowModel);
        PRINT_STR_COMMENT(out, QString("/* (0)Uniform (1)Network (2)Lerp (3)Power */"));
        PRINT_INT_NOENDL(out, QString("/YD/YDC/ICHJDO"), collection->fractureIntegration);
        PRINT_STR_COMMENT(out, QString("/* (0)Explicit-Euler (1)Implicit-Euler (2)Explicit-Exact (3)Explicit-Frog */"));
    }

    if(THERMAL & collection->GetSolutionModule())
    {
        PRINT_INT_NOENDL(out, QString("/YD/YDC/ICTEIF"), collection->triThermStrategy);
        PRINT_STR_COMMENT(out, QString("/* (0)Continuous (1)Discontinuous (2)Progressive */"));
        PRINT_INT_NOENDL(out, QString("/YD/YDC/ICTEDO"), collection->triThermSolution);
        PRINT_STR_COMMENT(out, QString("/* (0)UPM_Scheme (1)FEM_Scheme */"));
        PRINT_INT_SCALAR(out, QString("/YD/YDC/ICTJIF"), collection->cohThermStrategy);
        PRINT_INT_SCALAR(out, QString("/YD/YDC/ICTJDO"), collection->cohThermSolution);
    }

    int solution_type = collection->GetSolutionModule();
    if(MECHANICAL != solution_type || HYDRAULIC != solution_type || THERMAL != solution_type || 0 != solution_type)
    {
        PRINT_INT_NOENDL(out, QString("/YD/YDC/ICCOUP"), collection->couplingStrategy);
        PRINT_STR_COMMENT(out, QString("/* (0)One-way (1)Two-way */"));
        PRINT_INT_SCALAR(out, QString("/YD/YDC/NITERS"), collection->maxNumIterations);
        if(MECHANICAL & solution_type)
        {
            PRINT_DBL_SCALAR(out, QString("/YD/YDC/DCMTOL"), collection->absMechTolerance);
        }
        if(HYDRAULIC & solution_type)
        {
            PRINT_DBL_SCALAR(out, QString("/YD/YDC/DCHTOL"), collection->absHydroTolerance);
        }
        if(THERMAL & solution_type)
        {
            PRINT_DBL_SCALAR(out, QString("/YD/YDC/DCTTOL"), collection->absThermTolerance);
        }
    }

    out << reset << endl;
    PRINT_STR_COMMENT(out, QString("/* SOLUTION COLLECTION */"));
    PRINT_INT_SCALAR(out, QString("/YD/YDC/MCSTEP"), collection->GetSolutionMap().size());
    if(MECHANICAL & collection->GetSolutionModule())
    {
//        PRINT_STR_COMMENT(out, QString("/* (1)Finite deformation (2)Cohesive fracture */"));
        collection->OutputSolutionProperty(out, QString("I1MMODE"));
    }

    if(HYDRAULIC & collection->GetSolutionModule())
    {
//        PRINT_STR_COMMENT(out, QString("/* (1)Porous seepage (2)Fracture flow */"));
        collection->OutputSolutionProperty(out, QString("I1HMODE"));
    }

    if(THERMAL & collection->GetSolutionModule())
    {
//        PRINT_STR_COMMENT(out, QString("/* (1)Solid exchange (2)Fluid exchange */"));
        collection->OutputSolutionProperty(out, QString("I1TMODE"));
    }

    collection->OutputSolutionProperty(out, QString("I1TSTEP"));
    collection->OutputSolutionProperty(out, QString("I1COUTF"));

    if(MECHANICAL & collection->GetSolutionModule())
    {
        collection->OutputSolutionProperty(out, QString("D1MSTEC"));
    }

    if(HYDRAULIC & collection->GetSolutionModule())
    {
        collection->OutputSolutionProperty(out, QString("D1HSTEC"));
    }

    if(THERMAL & collection->GetSolutionModule())
    {
        collection->OutputSolutionProperty(out, QString("D1TSTEC"));
    }

    if(collection->IsSolutionOptionEnabled(QString("I1CGRAV")))
    {
        collection->OutputSolutionProperty(out, QString("I1CGRAV"));
        collection->OutputSolutionProperty(out, QString("D1CGRAX"));
        collection->OutputSolutionProperty(out, QString("D1CGRAY"));
    }

    if(collection->IsSolutionOptionEnabled(QString("I1CDAMP")))
    {
        collection->OutputSolutionProperty(out, QString("I1CDAMP"));
        collection->OutputSolutionProperty(out, QString("D1CDAMP"));
    }

    if(collection->IsSolutionOptionEnabled(QString("I1CCRIT")))
    {
        collection->OutputSolutionProperty(out, QString("I1CCRIT"));
        collection->OutputSolutionProperty(out, QString("D1CCRIT"));
    }

    collection->OutputSolutionProperty(out, QString("I1CONTA"));
    collection->OutputSolutionProperty(out, QString("I1CHEFF"));
    if(collection->IsSolutionOptionEnabled(QString("I1CSTEX")))
    {
        collection->OutputSolutionProperty(out, QString("I1CSTEX"));
    }

    return out;
}
