#include "triangulation.h"

#include <QDebug>
#include <QtMath>

#include <vtkPolyData.h>

#define CGAL_MESH_2_OPTIMIZER_VERBOSE


bool Triangulation::is_constraint_visible = false;

Triangulation::Triangulation() :
    shrinkFactor(1.0),
    face_color(QColor(204,204,81)),
    edge_color(QColor(Qt::black))
{
    this->numGrains = 0;
    this->grainOffs = 0;
}

Triangulation::~Triangulation()
{

}

int Triangulation::meshSourceFlag()
{
    return this->mesh_source_flag;
}

void Triangulation::setMeshSourceFlag(int arg)
{
    this->mesh_source_flag = arg;
}

int Triangulation::meshFilterFlag()
{
    return this->mesh_filter_flag;
}

void Triangulation::setMeshFilterFlag(int arg)
{
    this->mesh_filter_flag = arg;
}

void Triangulation::setConstraintVisible(bool flag)
{
    is_constraint_visible = flag;
}

bool Triangulation::isConstraintVisible()
{
    return is_constraint_visible;
}

QColor Triangulation::getEdgeColor() const
{
    return this->edge_color;
}

void Triangulation::setEdgeColor(const QColor &color)
{
    this->edge_color = color;
}

double Triangulation::getShrinkFactor() const
{
    return this->shrinkFactor;
}

void Triangulation::setShrinkFactor(double newShrinkFactor)
{
    this->shrinkFactor = newShrinkFactor;
}

size_t Triangulation::GetNumberOfPoints() const
{
    return this->numPoints;
}

size_t Triangulation::GetNumberOfTriangles() const
{
    return this->numTriangles;
}

size_t Triangulation::GetNumberOfQuads() const
{
    return this->numQuads;
}

size_t Triangulation::InitTriangulationNodeIds()
{
    return 0;
}

size_t Triangulation::InitTriangulationFaceIds()
{
    return 0;
}

void Triangulation::UpdateGlobalQuadInsertionInfo()
{
    return;
}

void Triangulation::UpdateLocalQuadInsertionInfo()
{
    return;
}

void Triangulation::UpdateSplitTriangulationInfo()
{
    return;
}

void Triangulation::UpdateInsertConstraintsInfo(QMap<QString, QSharedPointer<Primitive>> &)
{
    return;
}

void Triangulation::RestoreTriangulationStatus()
{
    return;
}

void Triangulation::UpdateTriangulationConstraint(vtkUnstructuredGrid *)
{
    return;
}

double Triangulation::GetTriangulationCharacteristicLength()
{
    return 0.0;
}

double Triangulation::GetTriangulationMinimumSize()
{
    return 0.0;
}

double Triangulation::GetTriangulationMinimumArea()
{
    return 0.0;
}

QColor Triangulation::getFaceColor() const
{
    return this->face_color;
}

void Triangulation::setFaceColor(const QColor &color)
{
    this->face_color = color;
}
