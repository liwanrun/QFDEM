#include "criterion.h"
#include "actorcollection.h"
#include "actor.h"

#include <QRectF>

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkPointLocator.h>
#include <vtkStaticPointLocator.h>
#include <vtkCellLocator.h>
#include <vtkStaticCellLocator.h>
#include <vtkLine.h>

Criterion::Criterion()
    : numberRows(1),
      logicsType(QString("Normal"))
{
    this->filterData = vtkDoubleArray::New();
}

Criterion::~Criterion()
{
    if(this->filterData)
    {
        this->filterData->Delete();
    }
}

PointCriterion::PointCriterion()
{
    this->numberCols = 4;
    this->filterType = QString("Point(s)");
    this->selectType = QString("Nearest to (~)");

    this->headLabels = QStringList({ "X", "Y", "Z", "eps" });
    this->filterData->SetNumberOfComponents(this->numberCols);
    this->filterData->SetNumberOfTuples(this->numberRows);
    double x[4] = {0.0, 0.0, 0.0, 0.001};
    this->filterData->SetTuple(0, x);
}

vtkIdTypeArray* PointCriterion::filterNode()
{
    vtkIdTypeArray *filterIds = vtkIdTypeArray::New();

    vtkSmartPointer<vtkStaticPointLocator> locator = vtkSmartPointer<vtkStaticPointLocator>::New();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();
    for(auto iter = block_map.begin(); iter != block_map.end(); iter++)
    {
        vtkSmartPointer<vtkPolyData> pd = iter.value()->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        vtkSmartPointer<vtkDataArray> globalIds = pd->GetPointData()->GetGlobalIds();
        locator->SetDataSet(pd);
        locator->SetTolerance(0.001);
        locator->BuildLocator();

        for(int i = 0; i < this->numberRows; i++)
        {
            double tuple[4];
            this->filterData->GetTuple(i, tuple);

            vtkIdType id = locator->FindClosestPoint(tuple);
            filterIds->InsertNextTuple1(globalIds->GetTuple1(id));
        }
    }

    return filterIds;
}

vtkIdTypeArray* PointCriterion::filterCell(int cellType)
{
    vtkIdTypeArray *filterIds = vtkIdTypeArray::New();

    vtkSmartPointer<vtkStaticCellLocator> locator = vtkSmartPointer<vtkStaticCellLocator>::New();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();
    for(auto iter = block_map.begin(); iter != block_map.end(); iter++)
    {
        vtkSmartPointer<vtkPolyData> pd = iter.value()->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        vtkSmartPointer<vtkDataArray> globalIds = pd->GetCellData()->GetGlobalIds();
        locator->SetDataSet(pd);
        locator->SetTolerance(0.001);
        locator->BuildLocator();

        for(int i = 0; i < this->numberRows; i++)
        {
            double tuple[4];
            this->filterData->GetTuple(i, tuple);

            vtkIdType id = locator->FindCell(tuple);
            vtkCell *cell = pd->GetCell(id);
            if(cell->GetCellType() != cellType)
                continue;

            filterIds->InsertNextTuple1(globalIds->GetTuple1(id));
        }
    }

    return filterIds;
}

SegmentCriterion::SegmentCriterion()
{
    this->numberCols = 5;
    this->filterType = QString("Segment(s)");
    this->selectType = QString("Interact with (=)");

    this->headLabels = QStringList({ "X0", "Y0", "X1", "Y1", "eps"});
    this->filterData->SetNumberOfComponents(this->numberCols);
    this->filterData->SetNumberOfTuples(this->numberRows);
    double x[5] = { 0.0, 0.0, 1.0, 1.0, 1.0 };
    this->filterData->SetTuple(0, x);
}

vtkIdTypeArray* SegmentCriterion::filterNode()
{
    vtkIdTypeArray *filterIds = vtkIdTypeArray::New();

    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();
    for(auto iter = block_map.begin(); iter != block_map.end(); iter++)
    {
        vtkSmartPointer<vtkPolyData> pd = iter.value()->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        vtkSmartPointer<vtkDataArray> globalIds = pd->GetPointData()->GetGlobalIds();

        for(int i = 0; i < this->numberRows; i++)
        {
            double tuple[5];
            this->filterData->GetTuple(i, tuple);
            double dist2 = tuple[4] * tuple[4];

            double xMin = (tuple[0] < tuple[2]) ? tuple[0] : tuple[2];
            double xMax = (tuple[0] > tuple[2]) ? tuple[0] : tuple[2];
            double yMin = (tuple[1] > tuple[3]) ? tuple[1] : tuple[3];
            double yMax = (tuple[1] > tuple[3]) ? tuple[3] : tuple[1];
            double width = xMax - xMin;
            double height = yMax - yMin;
            QRectF boundRect = QRect(xMin, yMin, width, height);

            double p[3] = { tuple[0], tuple[1], 0.0 };
            double q[3] = { tuple[2], tuple[3], 0.0 };

            vtkSmartPointer<vtkPoints> points = pd->GetPoints();
            for(vtkIdType id = 0; id < points->GetNumberOfPoints(); id++)
            {
                double x[3];
                points->GetPoint(id, x);
                if(boundRect.contains(x[0], x[1]) && vtkLine::DistanceToLine(x, p, q) < dist2)
                {
                    filterIds->InsertNextTuple1(globalIds->GetTuple1(id));
                }
            }
        }
    }

    return filterIds;
}

vtkIdTypeArray* SegmentCriterion::filterCell(int cellType)
{
    vtkIdTypeArray *filterIds = vtkIdTypeArray::New();

    vtkSmartPointer<vtkCellLocator> locator = vtkSmartPointer<vtkCellLocator>::New();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();
    for(auto iter = block_map.begin(); iter != block_map.end(); iter++)
    {
        vtkSmartPointer<vtkPolyData> pd = iter.value()->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        vtkSmartPointer<vtkDataArray> globalIds = pd->GetCellData()->GetGlobalIds();
        locator->SetDataSet(pd);
        locator->BuildLocator();

        for(int i = 0; i < this->numberRows; i++)
        {
            double tuple[5];
            this->filterData->GetTuple(i, tuple);

            vtkNew<vtkIdList> idList;
            double p1[3] = { tuple[0], tuple[1], 0.0 };
            double p2[3] = { tuple[2], tuple[3], 0.0 };
            locator->FindCellsAlongLine(p1, p2, tuple[4], idList);

            for(vtkIdType i = 0; i < idList->GetNumberOfIds(); i++)
            {
                vtkCell *cell = pd->GetCell(idList->GetId(i));
                if(cell->GetCellType() != cellType)
                    continue;

                filterIds->InsertNextTuple1(globalIds->GetTuple1(idList->GetId(i)));
            }
        }
    }

    return filterIds;
}

RectangleCriterion::RectangleCriterion()
{
    this->numberCols = 4;
    this->filterType = QString("Rectangle(s)");
    this->selectType = QString("Within (<)");

    this->headLabels = QStringList({ "X0", "Y0", "X1", "Y1" });
    this->filterData->SetNumberOfComponents(this->numberCols);
    this->filterData->SetNumberOfTuples(this->numberRows);
    double x[5] = {0.0, 0.0, 1.0, 1.0};
    this->filterData->SetTuple(0, x);
}

vtkIdTypeArray* RectangleCriterion::filterNode()
{
    vtkIdTypeArray *filterIds = vtkIdTypeArray::New();

    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();
    for(auto iter = block_map.begin(); iter != block_map.end(); iter++)
    {
        vtkSmartPointer<vtkPolyData> pd = iter.value()->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        vtkSmartPointer<vtkDataArray> globalIds = pd->GetPointData()->GetGlobalIds();

        for(int i = 0; i < this->numberRows; i++)
        {
            double tuple[4];
            this->filterData->GetTuple(i, tuple);

            double width = tuple[2] - tuple[0];
            double height = tuple[3] - tuple[1];
            QRectF boundRect = QRectF(tuple[0], tuple[1], width, height);

            double x[3];
            vtkSmartPointer<vtkPoints> points = pd->GetPoints();
            for(vtkIdType id = 0; id < points->GetNumberOfPoints(); id++)
            {
                points->GetPoint(id, x);
                if(boundRect.contains(x[0], x[1]))
                {
                    filterIds->InsertNextTuple1(globalIds->GetTuple1(id));
                }
            }
        }
    }

    return filterIds;
}

vtkIdTypeArray* RectangleCriterion::filterCell(int cellType)
{
    vtkIdTypeArray *filterIds = vtkIdTypeArray::New();

    vtkSmartPointer<vtkStaticCellLocator> locator = vtkSmartPointer<vtkStaticCellLocator>::New();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();
    for(auto iter = block_map.begin(); iter != block_map.end(); iter++)
    {
        vtkSmartPointer<vtkPolyData> pd = iter.value()->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        vtkSmartPointer<vtkDataArray> globalIds = pd->GetCellData()->GetGlobalIds();
        locator->SetDataSet(pd);
        locator->BuildLocator();

        for(int i = 0; i < this->numberRows; i++)
        {
            double tuple[4];
            this->filterData->GetTuple(i, tuple);

            vtkNew<vtkIdList> idList;
            double bbox[6] = { tuple[0], tuple[2], tuple[1], tuple[3], -0.1, 0.1 };
            locator->FindCellsWithinBounds(bbox, idList);
            for(vtkIdType i = 0; i < idList->GetNumberOfIds(); i++)
            {
                vtkCell *cell = pd->GetCell(idList->GetId(i));
                if(cell->GetCellType() != cellType)
                    continue;

                if(this->IsCellOverlappedWithRectangle(cell, bbox))
                {
                    filterIds->InsertNextTuple1(globalIds->GetTuple1(idList->GetId(i)));
                }
            }
        }
    }

    return filterIds;
}

bool RectangleCriterion::IsCellOverlappedWithRectangle(vtkCell *cell, double *bbox)
{
    double x[3] = {0.0};
    for(vtkIdType i = 0; i < cell->GetNumberOfPoints(); i++)
    {
        double point[3];
        cell->GetPoints()->GetPoint(i, point);

        x[0] += point[0];
        x[1] += point[1];
        x[2] += point[2];
    }
    x[0] = x[0] / cell->GetNumberOfPoints();
    x[1] = x[1] / cell->GetNumberOfPoints();
    x[2] = x[2] / cell->GetNumberOfPoints();

    QRectF boundingRect = QRectF(bbox[0], bbox[2], bbox[1] - bbox[0], bbox[3] - bbox[2]);
    return boundingRect.contains(x[0], x[1]);
}

EllipseCriterion::EllipseCriterion()
{
    this->numberCols = 4;
    this->filterType = QString("Circle(s)");
    this->selectType = QString("Within (<)");

    this->headLabels = QStringList({ "X", "Y", "Z", "R"});
    this->filterData->SetNumberOfComponents(this->numberCols);
    this->filterData->SetNumberOfTuples(this->numberRows);
    double x[4] = {0.0, 0.0, 0.0, 1.0};
    this->filterData->SetTuple(0, x);
}

vtkIdTypeArray* EllipseCriterion::filterNode()
{
    vtkIdTypeArray *filterIds = vtkIdTypeArray::New();

    vtkSmartPointer<vtkPointLocator> locator = vtkSmartPointer<vtkPointLocator>::New();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();
    for(auto iter = block_map.begin(); iter != block_map.end(); iter++)
    {
        vtkSmartPointer<vtkPolyData> pd = iter.value()->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        vtkSmartPointer<vtkDataArray> globalIds = pd->GetPointData()->GetGlobalIds();
        locator->SetDataSet(pd);
        locator->BuildLocator();

        for(int i = 0; i < this->numberRows; i++)
        {
            double tuple[4];
            this->filterData->GetTuple(i, tuple);

            vtkNew<vtkIdList> idList;
            double x[3] = { tuple[0], tuple[1], tuple[2] };
            locator->FindPointsWithinRadius(tuple[3], x, idList);
            for(vtkIdType i = 0; i < idList->GetNumberOfIds(); i++)
            {
                filterIds->InsertNextTuple1(globalIds->GetTuple1(idList->GetId(i)));
            }
        }
    }

    return filterIds;
}

vtkIdTypeArray* EllipseCriterion::filterCell(int cellType)
{
    vtkIdTypeArray *filterIds = vtkIdTypeArray::New();

    vtkSmartPointer<vtkStaticCellLocator> locator = vtkSmartPointer<vtkStaticCellLocator>::New();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();
    for(auto iter = block_map.begin(); iter != block_map.end(); iter++)
    {
        vtkSmartPointer<vtkPolyData> pd = iter.value()->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        vtkSmartPointer<vtkDataArray> globalIds = pd->GetCellData()->GetGlobalIds();
        locator->SetDataSet(pd);
        locator->BuildLocator();

        for(int i = 0; i < this->numberRows; i++)
        {
            double tuple[4];
            this->filterData->GetTuple(i, tuple);

            double xMin = tuple[0] - tuple[3];
            double xMax = tuple[0] + tuple[3];
            double yMin = tuple[1] - tuple[3];
            double yMax = tuple[1] + tuple[3];
            double zMin = -0.1, zMax =  0.1;

            vtkNew<vtkIdList> idList;
            double bbox[6] = { xMin, xMax, yMin, yMax, zMin, zMax };
            locator->FindCellsWithinBounds(bbox, idList);

            for(vtkIdType i = 0; i < idList->GetNumberOfIds(); i++)
            {
                vtkCell *cell = pd->GetCell(idList->GetId(i));
                if(cell->GetCellType() != cellType)
                    continue;

                if(this->IsCellOverlappedWithCircle(cell, tuple, tuple[3]))
                {
                    filterIds->InsertNextTuple1(globalIds->GetTuple1(idList->GetId(i)));
                }
            }
        }
    }

    return filterIds;
}

bool EllipseCriterion::IsCellOverlappedWithCircle(vtkCell *cell, double *x0, double R)
{
    double x[3] = {0.0};
    for(vtkIdType i = 0; i < cell->GetNumberOfPoints(); i++)
    {
        double point[3];
        cell->GetPoints()->GetPoint(i, point);

        x[0] += point[0];
        x[1] += point[1];
        x[2] += point[2];
    }
    x[0] = x[0] / cell->GetNumberOfPoints();
    x[1] = x[1] / cell->GetNumberOfPoints();
    x[2] = x[2] / cell->GetNumberOfPoints();

    double dx = x[0] - x0[0];
    double dy = x[1] - x0[1];
    double dz = x[2] - x0[2];

    return (dx*dx + dy*dy + dz*dz <= R*R);
}

