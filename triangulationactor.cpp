#include "triangulationactor.h"
#include "triangulation.h"
#include "fdemmeshsourcefactory.h"
#include "fdemmeshshrinkfactory.h"
#include "selectcollection.h"
#include "groupcollection.h"
#include "group.h"
#include "material.h"
#include "boundary.h"
#include "temporal.h"

#include "constitutivemodelfactory.h"
#include "mechanicalconstitutivemodel.h"

#include <QDebug>
#include <QVector2D>

#include <vtkStringArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointSetToLabelHierarchy.h>
#include <vtkLabelPlacementMapper.h>
#include <vtkFreeTypeLabelRenderStrategy.h>
#include <vtkTextProperty.h>
#include <vtkCellCenters.h>
#include <vtkInformation.h>
#include <vtkInformationStringKey.h>
#include <vtkCompositeDataSet.h>
#include <vtkTupleInterpolator.h>
#include <vtkFeatureEdges.h>
#include <vtkMath.h>

#include <random>

#define VTK_FEATURES 4

bool TriangulationActor::is_node_id_visible = false;

bool TriangulationActor::is_cell_id_visible = false;

TriangulationActor::TriangulationActor()
{
    qDebug() << "TriangulationActor()";

    this->mapper = vtkPolyDataMapper::New();
    this->mapper->ScalarVisibilityOff();
    this->mapper->SetColorModeToMapScalars();
    this->mapper->UseLookupTableScalarRangeOn();

    this->actor = vtkActor::New();
    this->actor->SetMapper(this->mapper);

    this->nodeLabelActor = vtkActor2D::New();
    this->nodeLabelActor->PickableOff();
    this->nodeLabelActor->VisibilityOff();

    this->cellLabelActor = vtkActor2D::New();
    this->cellLabelActor->PickableOff();
    this->cellLabelActor->VisibilityOff();

    this->featureEdgeActor = vtkActor::New();
    this->featureEdgeActor->PickableOff();
    this->featureEdgeActor->VisibilityOff();
}

TriangulationActor::~TriangulationActor()
{
    qDebug() << "~TriangulationActor()";

    this->mapper->Delete();
    this->actor->Delete();

    this->nodeLabelActor->Delete();
    this->cellLabelActor->Delete();
    this->featureEdgeActor->Delete();
}

vtkActor *TriangulationActor::GetVTKRenderActor()
{
    return this->actor;
}

vtkPolyDataMapper *TriangulationActor::GetVTKActorMapper()
{
    return this->mapper;
}

void TriangulationActor::Update(const QSharedPointer<Triangulation> &t)
{
    int type = t->meshSourceFlag();
    QSharedPointer<FDEMMeshSourceFactory> factory = FDEMMeshSourceFactory::CreateFactory(type);

    if(Triangulation::PRIMITIVE == t->meshFilterFlag())
    {
        vtkSmartPointer<FDEMMeshSource> source = factory->CreatePrimitiveMeshSource();
        source->SetTriangulation(t);
        this->SetInputConnection(source->GetOutputPort());
    }
    else if(Triangulation::GQHANDLED == t->meshFilterFlag())
    {
        vtkSmartPointer<FDEMMeshSource> source = factory->CreateInsertGlobalQuadFilter();
        source->SetTriangulation(t);
        this->SetInputConnection(source->GetOutputPort());
    }
    else if(Triangulation::LQHANDLED == t->meshFilterFlag())
    {
        vtkSmartPointer<FDEMMeshSource> source = factory->CreateInsertLocalQuadFilter();
        source->SetTriangulation(t);
        this->SetInputConnection(source->GetOutputPort());
    }
    else if(Triangulation::STHANDLED == t->meshFilterFlag())
    {
        vtkSmartPointer<FDEMMeshSource> source = factory->CreateSplitTriangulationFilter();
        source->SetTriangulation(t);
        this->SetInputConnection(source->GetOutputPort());
    }
    else if(Triangulation::THICKQUAD == t->meshFilterFlag())
    {
        vtkSmartPointer<FDEMMeshSource> source = factory->CreateThickQuadMeshSource();
        source->SetTriangulation(t);
        this->SetInputConnection(source->GetOutputPort());
    }
    else
    {
//      throw std::invalid_argument("Invalid product argument. Please check out!");
        return;
    }

    this->SetFaceColor(t->getFaceColor());
    this->SetEdgeColor(t->getEdgeColor());
    this->EdgeVisibilityOn();
}

void TriangulationActor::Shrink(const QSharedPointer<Triangulation> &t)
{
    int type = t->meshSourceFlag();
    QSharedPointer<FDEMMeshShrinkFactory> factory = FDEMMeshShrinkFactory::CreateFactory(type);

    if(Triangulation::GQHANDLED == t->meshFilterFlag())
    {
        vtkSmartPointer<FDEMMeshShrink> shrink = factory->CreateInsertGlobalQuadShrink();
        shrink->SetTriangulation(t);
        shrink->SetInputConnection(this->GetInputConnection());
        this->SetInputConnection(shrink->GetOutputPort());
    }
    else if(Triangulation::LQHANDLED == t->meshFilterFlag())
    {
        vtkSmartPointer<FDEMMeshShrink> shrink = factory->CreateInsertLocalQuadShrink();
        shrink->SetTriangulation(t);
        shrink->SetInputConnection(this->GetInputConnection());
        this->SetInputConnection(shrink->GetOutputPort());
    }
    else if(Triangulation::STHANDLED == t->meshFilterFlag())
    {
        vtkSmartPointer<FDEMMeshShrink> shrink = factory->CreateSplitTriangulationShrink();
        shrink->SetTriangulation(t);
        shrink->SetInputConnection(this->GetInputConnection());
        this->SetInputConnection(shrink->GetOutputPort());
    }
    else
    {
        return;
    }

    this->SetFaceColor(t->getFaceColor());
    this->SetEdgeColor(t->getEdgeColor());
    this->EdgeVisibilityOn();
}

void TriangulationActor::UpdateNodeIdLabelActor()
{
    vtkNew<vtkPointSetToLabelHierarchy> hie;
    hie->SetInputConnection(this->mapper->GetInputConnection(0, 0));
    hie->SetLabelArrayName("NodeIdLabels");
    hie->SetMaximumDepth(15);
    hie->SetTargetLabelCount(32);
    hie->GetTextProperty()->SetFontSize(16);
    hie->GetTextProperty()->SetFontFamilyToCourier();
    hie->GetTextProperty()->SetColor(1.0,1.0,1.0);

    vtkNew<vtkLabelPlacementMapper> labelMapper;
    labelMapper->SetInputConnection(hie->GetOutputPort());
    labelMapper->SetRenderStrategy(vtkFreeTypeLabelRenderStrategy::New());
    this->nodeLabelActor->SetMapper(labelMapper);
}

void TriangulationActor::UpdateCellIdLabelActor()
{
    vtkNew<vtkCellCenters> cf;
    cf->SetInputConnection(this->mapper->GetInputConnection(0, 0));
    cf->VertexCellsOn();

    vtkNew<vtkPointSetToLabelHierarchy> hie;
    hie->SetInputConnection(cf->GetOutputPort());
    hie->SetLabelArrayName("CellIdLabels");
    hie->SetMaximumDepth(15);
    hie->SetTargetLabelCount(100);
    hie->GetTextProperty()->SetFontSize(16);
    hie->GetTextProperty()->SetFontFamilyToCourier();
    hie->GetTextProperty()->SetColor(0.0,0.0,1.0);

    vtkNew<vtkLabelPlacementMapper> labelMapper;
    labelMapper->SetInputConnection(hie->GetOutputPort());
    this->cellLabelActor->SetMapper(labelMapper);
}

vtkActor2D *TriangulationActor::GetNodeIdLabelActor()
{
    return this->nodeLabelActor;
}

vtkActor2D *TriangulationActor::GetCellIdLabelActor()
{
    return this->cellLabelActor;
}

vtkActor *TriangulationActor::GetFeatureEdgeActor()
{
    return this->featureEdgeActor;
}

void TriangulationActor::SetInputConnection(vtkAlgorithmOutput *input)
{
    this->mapper->SetInputConnection(input);
}

vtkAlgorithmOutput *TriangulationActor::GetInputConnection()
{
    return this->mapper->GetInputConnection(0, 0);
}

void TriangulationActor::SetFaceColor(QColor color)
{
    double rgb[3];
    color.getRgbF(&rgb[0], &rgb[1], &rgb[2]);
    this->actor->GetProperty()->SetColor(rgb);
}

void TriangulationActor::SetEdgeColor(QColor color)
{
    double rgb[3];
    color.getRgbF(&rgb[0], &rgb[1], &rgb[2]);
    this->actor->GetProperty()->SetEdgeColor(rgb);
}

void TriangulationActor::EdgeVisibilityOn()
{
    this->actor->GetProperty()->SetLineWidth(1.0);
    this->actor->GetProperty()->EdgeVisibilityOn();
}

void TriangulationActor::SetPointSize(float arg)
{
    this->actor->GetProperty()->SetPointSize(arg);
}

void TriangulationActor::UpdateInformation(const QString &id)
{
    vtkInformation *info = this->actor->GetProperty()->GetInformation();
    info->Set(vtkCompositeDataSet::NAME(), id.toStdString());
}

void TriangulationActor::SetVisibility(bool arg)
{
    this->actor->SetVisibility(arg);
    this->nodeLabelActor->SetVisibility(arg ? is_node_id_visible : false);
    this->cellLabelActor->SetVisibility(arg ? is_cell_id_visible : false);
}

void TriangulationActor::SetNodeIdsVisibility(bool arg)
{
    is_node_id_visible = arg;
    this->nodeLabelActor->SetVisibility(arg && this->actor->GetVisibility());
}

void TriangulationActor::SetCellIdsVisibility(bool arg)
{
    is_cell_id_visible = arg;
    this->cellLabelActor->SetVisibility(arg && this->actor->GetVisibility());
}

void TriangulationActor::SetRepresentation(int index)
{
    switch (index)
    {
    case VTK_POINTS: {
        this->actor->GetProperty()->SetRepresentationToPoints();
        this->actor->GetProperty()->SetPointSize(5);
        this->actor->GetProperty()->LightingOff();
        this->featureEdgeActor->VisibilityOff();
        break;
    }
    case VTK_WIREFRAME: {
        this->actor->GetProperty()->SetRepresentationToWireframe();
        this->actor->GetProperty()->LightingOff();
        this->featureEdgeActor->VisibilityOff();
        break;
    }
    case VTK_SURFACE: {
        this->actor->GetProperty()->SetRepresentationToSurface();
        this->actor->GetProperty()->EdgeVisibilityOff();
        this->actor->GetProperty()->LightingOn();
        this->featureEdgeActor->VisibilityOff();
        break;
    }
    case VTK_FEATURES:     //Feature Edges
    {
        vtkNew<vtkPolyDataMapper> edgeMapper;
        vtkNew<vtkFeatureEdges> featureEdges;
        featureEdges->SetInputData(this->mapper->GetInput());
        featureEdges->BoundaryEdgesOn();
        featureEdges->FeatureEdgesOff();
        featureEdges->ManifoldEdgesOff();
        featureEdges->NonManifoldEdgesOff();
        featureEdges->ColoringOff();
        featureEdges->Update();
        edgeMapper->SetInputConnection(featureEdges->GetOutputPort());
        edgeMapper->SetScalarModeToUseCellData();
        this->featureEdgeActor->SetMapper(edgeMapper);
        this->featureEdgeActor->VisibilityOn();
        break;
    }
    default:
    {
        this->actor->GetProperty()->SetRepresentationToSurface();
        this->actor->GetProperty()->EdgeVisibilityOn();
        this->actor->GetProperty()->LightingOn();
        this->featureEdgeActor->VisibilityOff();
        break;
    }
    }
}

void TriangulationActor::SelectColorArray(const QString &text)
{
    vtkPolyData *pd = this->mapper->GetInput();
    if(pd->GetPointData()->HasArray(text.toStdString().c_str()))
    {
        this->mapper->SetScalarVisibility(true);
        this->mapper->SetScalarModeToUsePointFieldData();
        this->mapper->SelectColorArray(text.toStdString().c_str());
    }
    else if(pd->GetCellData()->HasArray(text.toStdString().c_str()))
    {
        this->mapper->SetScalarVisibility(true);
        this->mapper->SetScalarModeToUseCellFieldData();
        this->mapper->SelectColorArray(text.toStdString().c_str());
    }
    else
    {
        this->mapper->SetScalarVisibility(false);
    }
}

void TriangulationActor::SetLookupTable(vtkScalarsToColors *lut)
{
    this->mapper->SetLookupTable(lut);
}

double *TriangulationActor::GetScalarRange(const QString &text, int comp)
{
    vtkPolyData *pd = this->mapper->GetInput();
    if(pd->GetPointData()->HasArray(text.toStdString().c_str()))
    {
        vtkDataArray *array = pd->GetPointData()->GetArray(text.toStdString().c_str());
        return array->GetRange(comp);
    }
    else if(pd->GetCellData()->HasArray(text.toStdString().c_str()))
    {
        vtkDataArray *array = pd->GetCellData()->GetArray(text.toStdString().c_str());
        return array->GetRange(comp);
    }
    else
    {
        double *range = new double[2];
        range[0] = 0.0;
        range[1] = 0.0;
        return range;
    }
}

void TriangulationActor::AppendNodeGlobalIds(const char *name, vtkIdType &offset)
{
    vtkPolyData *pd = this->mapper->GetInput();

    vtkIdTypeArray *nodeGlobalIds = vtkIdTypeArray::New();
    nodeGlobalIds->SetNumberOfValues(pd->GetNumberOfPoints());
    nodeGlobalIds->SetName(name);

    vtkAbstractArray *nodeLabels = pd->GetPointData()->GetAbstractArray("NodeIdLabels");
    for(vtkIdType id = 0; id < pd->GetNumberOfPoints(); id++)
    {
        nodeGlobalIds->SetValue(id, id + offset);
        nodeLabels->SetVariantValue(id, std::to_string(id + offset).c_str());
    }

    pd->GetPointData()->SetGlobalIds(nodeGlobalIds);
    nodeGlobalIds->Delete();

    offset += pd->GetNumberOfPoints();
}

void TriangulationActor::AppendCellGlobalIds(const char *name, vtkIdType &offset)
{
    vtkPolyData *pd = this->mapper->GetInput();

    vtkIdTypeArray *cellGlobalIds = vtkIdTypeArray::New();
    cellGlobalIds->SetNumberOfValues(pd->GetNumberOfCells());
    cellGlobalIds->SetName(name);

    vtkAbstractArray *cellLabels = pd->GetCellData()->GetAbstractArray("CellIdLabels");
    for(vtkIdType id = 0; id < pd->GetNumberOfCells(); id++)
    {
        cellGlobalIds->SetValue(id, id + offset);
        cellLabels->SetVariantValue(id, std::to_string(id + offset).c_str());
    }
    pd->GetCellData()->SetGlobalIds(cellGlobalIds);
    cellGlobalIds->Delete();
    offset += pd->GetNumberOfCells();
}

void TriangulationActor::AppendCellTypeArray(const char *name)
{
    vtkPolyData *pd = this->mapper->GetInput();

    vtkIntArray *cellTypes = vtkIntArray::New();
    cellTypes->SetNumberOfValues(pd->GetNumberOfCells());
    cellTypes->SetName(name);

    for(vtkIdType id = 0; id < pd->GetNumberOfCells(); id++)
    {
        cellTypes->SetValue(id, pd->GetCellType(id));
    }
    pd->GetCellData()->AddArray(cellTypes);
    cellTypes->Delete();
}

void TriangulationActor::AppendNodePedigreeIds(const char *name, vtkIdType &offset)
{
    vtkPolyData *pd = this->mapper->GetInput();

    vtkIdTypeArray *nodePedigreeIds = vtkIdTypeArray::New();
    nodePedigreeIds->SetNumberOfValues(pd->GetNumberOfPoints());
    nodePedigreeIds->SetName(name);

    for(vtkIdType id = 0; id < pd->GetNumberOfPoints(); id++)
    {
        nodePedigreeIds->SetValue(id, id + offset);
    }

    pd->GetPointData()->SetPedigreeIds(nodePedigreeIds);
    nodePedigreeIds->Delete();

    offset += pd->GetNumberOfPoints();
}

void TriangulationActor::AppendCellPedigreeIds(const char *name, vtkIdType &offset)
{
    vtkPolyData *pd = this->mapper->GetInput();

    vtkIdTypeArray *cellPedigreeIds = vtkIdTypeArray::New();
    cellPedigreeIds->SetNumberOfValues(pd->GetNumberOfCells());
    cellPedigreeIds->SetName(name);
    cellPedigreeIds->FillValue(-1);
    pd->GetCellData()->SetPedigreeIds(cellPedigreeIds);
    cellPedigreeIds->Delete();

    offset += pd->GetNumberOfCells();
}

void TriangulationActor::AppendPointDataArray(const char *name, int comp, double value)
{
    vtkPolyData *pd = this->mapper->GetInput();
    if(!pd->GetPointData()->HasArray(name))
    {
        vtkNew<vtkDoubleArray> array;
        array->SetNumberOfComponents(comp);
        array->SetNumberOfTuples(pd->GetNumberOfPoints());
        array->FillValue(value);
        array->SetName(name);
        pd->GetPointData()->AddArray(array);
    }
}

void TriangulationActor::RemovePointDataArray(const char *name)
{
    vtkPolyData *pd = this->mapper->GetInput();
    if(pd->GetPointData()->HasArray(name))
    {
        pd->GetPointData()->RemoveArray(name);
    }
}

void TriangulationActor::AppendCellDataArray(const char *name, int comp, double value)
{
    vtkPolyData *pd = this->mapper->GetInput();
    if(!pd->GetCellData()->HasArray(name))
    {
        vtkNew<vtkDoubleArray> array;
        array->SetNumberOfComponents(comp);
        array->SetNumberOfTuples(pd->GetNumberOfCells());
        array->FillValue(value);
        array->SetName(name);
        pd->GetCellData()->AddArray(array);
    }
}

void TriangulationActor::RemoveCellDataArray(const char *name)
{
    vtkPolyData *pd = this->mapper->GetInput();
    if(pd->GetCellData()->HasArray(name))
    {
        pd->GetCellData()->RemoveArray(name);
    }
}

void TriangulationActor::ApplyCellMaterialIndexArray(const QString& id, vtkUnstructuredGrid *ug, const char *name, Material *material)
{
    vtkPolyData *pd = this->mapper->GetInput();
    vtkAbstractArray *globalIds = pd->GetCellData()->GetGlobalIds();
    vtkAbstractArray *materialIds = pd->GetCellData()->GetAbstractArray(name);

    int matId = id.split(QChar('-')).last().toInt();
    int cellType = (Material::TRIANGLE == material->getMaterialType()) ? VTK_TRIANGLE : VTK_QUAD;

    vtkAbstractArray *extractIds = ug->GetCellData()->GetGlobalIds();
    for(vtkIdType i = 0; i < extractIds->GetNumberOfValues(); i++)
    {
        if(cellType != ug->GetCellType(i))
            continue;

        if(VTK_TRIANGLE == ug->GetCellType(i))
        {
            vtkIdType index = globalIds->LookupValue(extractIds->GetVariantValue(i));
            materialIds->SetVariantValue(index, matId);
        }
        else if(VTK_QUAD == ug->GetCellType(i))
        {
            vtkIdType index = globalIds->LookupValue(extractIds->GetVariantValue(i));
            materialIds->SetVariantValue(index, matId);
        }
    }

    materialIds->Modified();
}

void TriangulationActor::ApplyCellMaterialFieldArray(const QString &id, vtkUnstructuredGrid *ug, const char *name, Material *material)
{
    vtkPolyData *pd = this->mapper->GetInput();
    vtkAbstractArray *globalIds = pd->GetCellData()->GetGlobalIds();
    vtkAbstractArray *fieldData = pd->GetCellData()->GetAbstractArray(name);

    if(Material::TRIANGLE == material->getMaterialType())
    {
        TriangleMaterial* solidMaterial = static_cast<TriangleMaterial*>(material);
        TriangleMechanicalModel *model = static_cast<TriangleMechanicalModel*>(solidMaterial->mechanicalModel.get());

        if(model->isPropertyInhomoEnabled)
        {
            std::random_device device;
            int seed = model->youngModulusSeed;
            if(!seed) { seed = device(); }

            std::default_random_engine engine(seed);
            std::weibull_distribution<double> distribution(model->youngModulusIndex, model->youngModulus);

            if(0 == model->inhomogeneousOption)    //Element-wise
            {
                vtkAbstractArray *extractIds = ug->GetCellData()->GetGlobalIds();
                for(vtkIdType i = 0; i < extractIds->GetNumberOfValues(); i++)
                {
                    if(VTK_TRIANGLE == ug->GetCellType(i))
                    {
                        vtkIdType index = globalIds->LookupValue(extractIds->GetVariantValue(i));
                        fieldData->SetVariantValue(index, distribution(engine));
                    }
                }
            }
            else if(1 == model->inhomogeneousOption)   //Grain-wise
            {
                if(!pd->GetCellData()->HasArray("Cell Grain Ids"))
                    return;

                int matId = id.split(QChar('-')).last().toInt();
                vtkAbstractArray *materialIds = pd->GetCellData()->GetAbstractArray("Cell Material Ids");

                QSharedPointer<ElemSetCollection> elemSetCollection = ElemSetCollection::GetElemSetCollection();
                GroupCollection::GroupMap elemSets = elemSetCollection->GetGroupMap();
                for(auto iter = elemSets.begin(); iter != elemSets.end(); ++iter)
                {
                    QVector<size_t> extractIds = iter.value()->getGlobalIds();
                    vtkIdType index = globalIds->LookupValue(extractIds[0]);

                    if(Group::GRAIN == iter.value()->getType() &&
                            materialIds->GetVariantValue(index).ToInt() == matId)
                    {
                        double value = distribution(engine);
                        for(int i = 0; i < extractIds.size(); i++)
                        {
                            index = globalIds->LookupValue(extractIds[i]);
                            if(VTK_TRIANGLE == pd->GetCellType(index))
                            {
                                fieldData->SetVariantValue(index, value);
                            }
                        }
                    }
                }
            }

            fieldData->Modified();
        }
        else
        {
            vtkAbstractArray *extractIds = ug->GetCellData()->GetGlobalIds();
            for(vtkIdType i = 0; i < extractIds->GetNumberOfValues(); i++)
            {
                if(VTK_TRIANGLE == ug->GetCellType(i))
                {
                    vtkIdType index = globalIds->LookupValue(extractIds->GetVariantValue(i));
                    fieldData->SetVariantValue(index, model->youngModulus);
                }
            }
            fieldData->Modified();
        }
    }
}

void TriangulationActor::CancelCellMaterialIds(const QString &id, const char *name)
{
    vtkPolyData *pd = this->mapper->GetInput();
    if(pd->GetCellData()->HasArray(name))
    {
        vtkAbstractArray *materialIds = pd->GetCellData()->GetAbstractArray(name);
        int matId = id.split(QChar('-')).last().toInt();
        for(vtkIdType i = 0; i < materialIds->GetNumberOfValues(); i++)
        {
            if(matId == materialIds->GetVariantValue(i).ToInt())
            {
                materialIds->SetVariantValue(i, -1);
            }
        }
        materialIds->Modified();
    }
}

void TriangulationActor::ApplyPointBoundaryIndexArray(const QString &id, vtkUnstructuredGrid *ug, const char *name)
{
    vtkPolyData *pd = this->mapper->GetInput();
    vtkAbstractArray *globalIds = pd->GetPointData()->GetGlobalIds();
    vtkAbstractArray *boundaryIds = pd->GetPointData()->GetAbstractArray(name);

    int bndId = id.split(QChar('-')).last().toInt();
    vtkAbstractArray *extractIds = ug->GetPointData()->GetGlobalIds();
    for(vtkIdType i = 0; i < extractIds->GetNumberOfValues(); i++)
    {
        vtkIdType index = globalIds->LookupValue(extractIds->GetVariantValue(i));
        boundaryIds->SetVariantValue(index, bndId);
    }
    boundaryIds->Modified();
}

void TriangulationActor::ApplyPointBoundaryFieldArray(const char *name, vtkUnstructuredGrid *ug, Boundary *boundary)
{
    vtkPolyData *pd = this->mapper->GetInput();
    vtkAbstractArray *globalIds = pd->GetPointData()->GetGlobalIds();
    vtkAbstractArray *baseArray = pd->GetPointData()->GetAbstractArray(name);
    vtkDoubleArray *dataArray = static_cast<vtkDoubleArray*>(baseArray);
    int numComps = dataArray->GetNumberOfComponents();

    if(boundary->isAmplitudeSiteDependent)
    {
        QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
        QVector<QPointF> extremePoints = selectCollection->GetSpatialExtremePoints();

        vtkSmartPointer<vtkDoubleArray> ampArr = vtkSmartPointer<vtkDoubleArray>::New();
        ampArr.TakeReference(boundary->GetSpatialFieldAmplitudes());

        vtkNew<vtkTupleInterpolator> interpolator;
        interpolator->SetInterpolationTypeToLinear();
        interpolator->SetNumberOfComponents(numComps);
        interpolator->AddTuple(0.0, ampArr->GetTuple(0));
        interpolator->AddTuple(1.0, ampArr->GetTuple(1));

        double *values = new double[numComps];
        vtkAbstractArray *extractIds = ug->GetPointData()->GetGlobalIds();
        for(vtkIdType i = 0; i < extractIds->GetNumberOfValues(); i++)
        {
            double coord[3];
            ug->GetPoints()->GetPoint(i, coord);
            QPointF movingPt(coord[0], coord[1]);
            QPointF sourcePt = extremePoints[0];
            QPointF targetPt = extremePoints[1];

            double t = this->GetLinearParametricCoord(sourcePt, targetPt, movingPt);
            interpolator->InterpolateTuple(t, values);

            vtkIdType index = globalIds->LookupValue(extractIds->GetVariantValue(i));
            dataArray->SetTuple(index, values);
        }
        delete []values;
        dataArray->Modified();
    }
    else
    {
        vtkSmartPointer<vtkDoubleArray> ampArr = vtkSmartPointer<vtkDoubleArray>::New();
        ampArr.TakeReference(boundary->GetSpatialFieldAmplitudes());

        vtkAbstractArray *extractIds = ug->GetPointData()->GetGlobalIds();
        for(vtkIdType i = 0; i < extractIds->GetNumberOfValues(); i++)
        {
            vtkIdType index = globalIds->LookupValue(extractIds->GetVariantValue(i));
            dataArray->SetTuple(index, 0, ampArr);
        }
        dataArray->Modified();
    }
}

void TriangulationActor::CancelPointBoundaryFieldArray(const QString &id, const char *name, Boundary *boundary)
{
    vtkPolyData *pd = this->mapper->GetInput();
    if(pd->GetPointData()->HasArray(name))
    {
        vtkAbstractArray *indexArray = pd->GetPointData()->GetAbstractArray(boundary->GetIndexArrayName());
        vtkDoubleArray *fieldArray = static_cast<vtkDoubleArray*>(pd->GetPointData()->GetAbstractArray(name));
        int numComps = fieldArray->GetNumberOfComponents();
        double *values = new double[numComps];
        for(int i = 0; i < numComps; i++)
        {
            values[i] = 0.0;
        }

        int bndId = id.split(QChar('-')).last().toInt();
        for(vtkIdType i = 0; i < indexArray->GetNumberOfValues(); i++)
        {
            if(bndId == indexArray->GetVariantValue(i).ToInt())
            {
                fieldArray->SetTuple(i, values);
                indexArray->SetVariantValue(i, -1);
            }
        }

        delete[] values;
        fieldArray->Modified();
        indexArray->Modified();
    }
}

void TriangulationActor::ApplyPointTemporalIndexArray(const QString &id, const char *name, vtkUnstructuredGrid *ug)
{
    vtkPolyData *pd = this->mapper->GetInput();
    vtkAbstractArray *globalIds = pd->GetPointData()->GetGlobalIds();
    vtkAbstractArray *temporalIds = pd->GetPointData()->GetAbstractArray(name);

    int tempId = id.split(QChar('-')).last().toInt();
    vtkAbstractArray *extractIds = ug->GetPointData()->GetGlobalIds();
    for(vtkIdType i = 0; i < extractIds->GetNumberOfValues(); i++)
    {
        vtkIdType index = globalIds->LookupValue(extractIds->GetVariantValue(i));
        temporalIds->SetVariantValue(index, tempId);
    }
    temporalIds->Modified();
}

void TriangulationActor::ApplyPointTemporalFieldArray(const char *name, vtkUnstructuredGrid *ug, Temporal *temporal)
{
    vtkPolyData *pd = this->mapper->GetInput();
    vtkAbstractArray *globalIds = pd->GetPointData()->GetGlobalIds();
    vtkAbstractArray *baseArray = pd->GetPointData()->GetAbstractArray(name);
    vtkDoubleArray *dataArray = static_cast<vtkDoubleArray*>(baseArray);
    int numComps = dataArray->GetNumberOfComponents();

    if(temporal->isAmplitudeSiteDependent)
    {
        QSharedPointer<SelectCollection> selectCollection = SelectCollection::GetSelectCollection();
        QVector<QPointF> extremePoints = selectCollection->GetSpatialExtremePoints();

        vtkSmartPointer<vtkDoubleArray> ampArr = vtkSmartPointer<vtkDoubleArray>::New();
        ampArr.TakeReference(temporal->GetSpatialFieldAmplitudes());

        vtkNew<vtkTupleInterpolator> interpolator;
        interpolator->SetInterpolationTypeToLinear();
        interpolator->SetNumberOfComponents(numComps);
        interpolator->AddTuple(0.0, ampArr->GetTuple(0));
        interpolator->AddTuple(1.0, ampArr->GetTuple(1));

        double *values = new double[numComps];
        vtkAbstractArray *extractIds = ug->GetPointData()->GetGlobalIds();
        for(vtkIdType i = 0; i < extractIds->GetNumberOfValues(); i++)
        {
            double coord[3];
            ug->GetPoints()->GetPoint(i, coord);
            QPointF movingPt(coord[0], coord[1]);
            QPointF sourcePt = extremePoints[0];
            QPointF targetPt = extremePoints[1];

            double t = this->GetLinearParametricCoord(sourcePt, targetPt, movingPt);
            interpolator->InterpolateTuple(t, values);

            vtkIdType index = globalIds->LookupValue(extractIds->GetVariantValue(i));
            dataArray->SetTuple(index, values);
        }
        delete []values;
        dataArray->Modified();
    }
    else
    {
        vtkSmartPointer<vtkDoubleArray> ampArr = vtkSmartPointer<vtkDoubleArray>::New();
        ampArr.TakeReference(temporal->GetSpatialFieldAmplitudes());

        vtkAbstractArray *extractIds = ug->GetPointData()->GetGlobalIds();
        for(vtkIdType i = 0; i < extractIds->GetNumberOfValues(); i++)
        {
            vtkIdType index = globalIds->LookupValue(extractIds->GetVariantValue(i));
            dataArray->SetTuple(index, 0, ampArr);
        }

        dataArray->Modified();
    }
}

void TriangulationActor::CancelPointTemporalFieldArray(const QString &id, const char *name, Temporal *temporal)
{
    vtkPolyData *pd = this->mapper->GetInput();
    if(pd->GetPointData()->HasArray(name))
    {
        vtkAbstractArray *indexArray = pd->GetPointData()->GetAbstractArray(temporal->GetIndexArrayName());
        vtkDoubleArray *fieldArray = static_cast<vtkDoubleArray*>(pd->GetPointData()->GetAbstractArray(name));
        int numComps = fieldArray->GetNumberOfComponents();
        double *values = new double[numComps];
        for(int i = 0; i < numComps; i++)
        {
            values[i] = 0.0;
        }

        int tempId = id.split(QChar('-')).last().toInt();
        for(vtkIdType i = 0; i < indexArray->GetNumberOfValues(); i++)
        {
            if(tempId == indexArray->GetVariantValue(i).ToInt())
            {
                fieldArray->SetTuple(i, values);
                indexArray->SetVariantValue(i, -1);
            }
        }

        delete[] values;
        fieldArray->Modified();
        indexArray->Modified();
    }
}

void TriangulationActor::TranslateTriangulationGeometry(vtkUnstructuredGrid *ug, double *comp)
{
    vtkPolyData *pd = this->mapper->GetInput();
    vtkAbstractArray *globalIds = pd->GetPointData()->GetGlobalIds();
    vtkAbstractArray *extractIds = ug->GetPointData()->GetGlobalIds();

    vtkPoints *points = pd->GetPoints();
    for(vtkIdType i = 0; i < extractIds->GetNumberOfValues(); i++)
    {
        double point[3];
        vtkIdType index = globalIds->LookupValue(extractIds->GetVariantValue(i));
        points->GetPoint(index, point);

        for(int i = 0; i < 3; i++)
        {
            point[i] += comp[i];
        }

        points->SetPoint(index, point);
    }

    points->Modified();
}

void TriangulationActor::RotateTriangulationGeometry(vtkUnstructuredGrid *ug, double *comp)
{
    vtkPolyData *pd = this->mapper->GetInput();
    vtkAbstractArray *globalIds = pd->GetPointData()->GetGlobalIds();
    vtkAbstractArray *extractIds = ug->GetPointData()->GetGlobalIds();

    vtkPoints *points = pd->GetPoints();
    double angleRad = vtkMath::RadiansFromDegrees(comp[3]);
    for(vtkIdType i = 0; i < extractIds->GetNumberOfValues(); i++)
    {
        double point[3], newPoint[3];
        vtkIdType index = globalIds->LookupValue(extractIds->GetVariantValue(i));
        points->GetPoint(index, point);

        newPoint[0] = (point[0] - comp[0])*cos(angleRad) - (point[1] - comp[1])*sin(angleRad) + comp[0];
        newPoint[1] = (point[0] - comp[0])*sin(angleRad) + (point[1] - comp[1])*cos(angleRad) + comp[1];
        newPoint[2] = 0.0;

        points->SetPoint(index, newPoint);
    }

    points->Modified();
}

void TriangulationActor::ScaleTriangulationGeometry(vtkUnstructuredGrid *ug, double *comp)
{
    vtkPolyData *pd = this->mapper->GetInput();
    vtkAbstractArray *globalIds = pd->GetPointData()->GetGlobalIds();
    vtkAbstractArray *extractIds = ug->GetPointData()->GetGlobalIds();

    vtkPoints *points = pd->GetPoints();
    for(vtkIdType i = 0; i < extractIds->GetNumberOfValues(); i++)
    {
        double point[3];
        vtkIdType index = globalIds->LookupValue(extractIds->GetVariantValue(i));
        points->GetPoint(index, point);

        for(int i = 0; i < 3; i++)
        {
            point[i] *= comp[i];
        }

        points->SetPoint(index, point);
    }

    points->Modified();
}

double TriangulationActor::GetLinearParametricCoord(const QPointF& p1, const QPointF& p2, const QPointF& pt)
{
    QVector2D u = QVector2D(p2 - p1);
    QVector2D v = QVector2D(pt - p1);
    return (v.length() / u.length());
}

