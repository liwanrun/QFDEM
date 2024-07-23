#include "fdemtriangulationfilter.h"
#include "triangulation.h"

FDEMTriangulationFilter::FDEMTriangulationFilter()
{
    this->mesh = nullptr;
}

FDEMTriangulationFilter::~FDEMTriangulationFilter()
{

}

void FDEMTriangulationFilter::SetTriangulation(QSharedPointer<Triangulation> &t)
{
    this->mesh = t;
}

void FDEMTriangulationFilter::SetConstraintInfo(QMap<QString, QSharedPointer<Primitive>>& ci)
{
    this->constraints = ci;
}

void FDEMTriangulationFilter::filter()
{
    return;
}

InsertGlobalQuadFilter::InsertGlobalQuadFilter()
    : FDEMTriangulationFilter()
{

}

InsertGlobalQuadFilter::~InsertGlobalQuadFilter()
{

}

void InsertGlobalQuadFilter::filter()
{
    mesh->UpdateGlobalQuadInsertionInfo();
}


InsertLocalQuadFilter::InsertLocalQuadFilter()
    : FDEMTriangulationFilter()
{

}

InsertLocalQuadFilter::~InsertLocalQuadFilter()
{

}

void InsertLocalQuadFilter::filter()
{
    mesh->UpdateLocalQuadInsertionInfo();
}


SplitTriangulationFilter::SplitTriangulationFilter()
    : FDEMTriangulationFilter()
{

}

SplitTriangulationFilter::~SplitTriangulationFilter()
{

}

void SplitTriangulationFilter::filter()
{
    mesh->UpdateSplitTriangulationInfo();
}

InsertConstraintFilter::InsertConstraintFilter()
    : FDEMTriangulationFilter()
{

}

InsertConstraintFilter::~InsertConstraintFilter()
{

}

void InsertConstraintFilter::filter()
{
    mesh->UpdateInsertConstraintsInfo(this->constraints);
}
