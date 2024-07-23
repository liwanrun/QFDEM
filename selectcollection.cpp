#include "selectcollection.h"
#include "selectionfilter.h"

#include <vtkActor.h>
#include <vtkIdTypeArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCompositeDataIterator.h>
#include <vtkCompositeDataGeometryFilter.h>
#include <vtkProperty.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkInformation.h>
#include <vtkExtractSelection.h>
#include <vtkPolyDataMapper.h>

#include <QDebug>

#include <set>
#include <cmath>
#include <algorithm>

bool compare_xy(const QPointF &p1, const QPointF &p2)
{
    double eps = 1.0e-10;
    double dx = p1.x() - p2.x();
    double dy = p1.y() - p2.y();

    if (fabs(dx) > eps)
    {
        return (dx < 0.0) ? true : false;
    }
    else
    {
        return (dy < 0.0) ? true : false;
    }
}

QSharedPointer<SelectCollection> SelectCollection::selectCollection = nullptr;

QMutex SelectCollection::mutex;

QSharedPointer<SelectCollection> SelectCollection::GetSelectCollection()
{
    if(nullptr == selectCollection)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == selectCollection)
        {
            selectCollection = QSharedPointer<SelectCollection>(new SelectCollection);
        }
    }

    return selectCollection;
}

SelectCollection::SelectCollection()
{
    this->Selection = vtkSelection::New();

    this->Input = vtkMultiBlockDataSet::New();
    this->Output = vtkMultiBlockDataSet::New();

    this->SelectActor = nullptr;
    this->SelectMapper = vtkPolyDataMapper::New();
    this->SelectMapper->ScalarVisibilityOff();

    this->selectField = Selection::CELL;
    this->selectStyle = Selection::NORMAL;
    this->selectContent = Selection::FRUSTUM;

    this->isInvertSelect = false;

    this->selectColor[0] = 1.0;
    this->selectColor[1] = 0.0;
    this->selectColor[2] = 1.0;

    this->selectFilter = QString("All");
}

void SelectCollection::UnionSelectionNodeImp(vtkSelectionNode *node)
{
    std::set<vtkIdType> uniqueIds;

    vtkAbstractArray *aa1 = this->Selection->GetNode(0)->GetSelectionList();
    for(vtkIdType id = 0; id < aa1->GetNumberOfTuples(); id++)
    {
        uniqueIds.emplace(aa1->GetVariantValue(id).ToLongLong());
    }

    vtkAbstractArray *aa2 = node->GetSelectionList();
    for(vtkIdType id = 0; id < aa2->GetNumberOfTuples(); id++)
    {
        uniqueIds.emplace(aa2->GetVariantValue(id).ToLongLong());
    }

    if(vtkIdTypeArray* aa3 = vtkIdTypeArray::SafeDownCast(aa1))
    {
        aa3->Initialize();
        for(vtkIdType id : uniqueIds)
        {
            aa3->InsertNextValue(id);
        }
    }
}

void SelectCollection::GetExtractCellGlobalIds(vtkDataSet *ds, vtkIdTypeArray *da, const QString &crit)
{
    QSharedPointer<SelectionFilterFactory> factory =
            QSharedPointer<SelectionFilterFactory>(new SelectionFilterFactory());
    QSharedPointer<SelectionFilter> cf = factory->Create(crit);
    cf->filter(ds, da);
}

void SelectCollection::GetExtractPointGlobalIds(vtkDataSet *ds, vtkIdTypeArray *da)
{
    if(vtkDataArray *globalIds = ds->GetPointData()->GetGlobalIds())
    {
        for (vtkIdType id = 0; id < globalIds->GetNumberOfValues(); id++)
        {
            da->InsertNextValue(globalIds->GetVariantValue(id).ToLongLong());
        }
    }
    else
    {
        throw std::invalid_argument("Attribute GlobalIDs is missing. Please check out!");
    }
}

vtkSelection *SelectCollection::GetVTKSelection() const
{
    return this->Selection;
}

void SelectCollection::SetInputDataSet(vtkMultiBlockDataSet *mbds)
{
    this->Input->ShallowCopy(mbds);
}

vtkMultiBlockDataSet *SelectCollection::GetSelectOutput() const
{
    return this->Output;
}

vtkActor *SelectCollection::GetSelectActor() const
{
    return this->SelectActor;
}

int SelectCollection::GetSelectField() const
{
    return this->selectField;
}

void SelectCollection::SetSelectField(int field)
{
    this->selectField = field;
}

int SelectCollection::GetSelectStyle() const
{
    return this->selectStyle;
}

void SelectCollection::SetSelectStyle(int style)
{
    this->selectStyle = style;
}

bool SelectCollection::IsInvertSelect() const
{
    return this->isInvertSelect;
}

void SelectCollection::SetInvertSelect(bool flag)
{
    this->isInvertSelect = flag;
}

void SelectCollection::SetHighlightColor(double *rgb)
{
    this->selectColor[0] = rgb[0];
    this->selectColor[1] = rgb[1];
    this->selectColor[2] = rgb[2];
}

QString SelectCollection::GetSelectFilter() const
{
    return this->selectFilter;
}

void SelectCollection::SetSelectFilter(const QString &filter)
{
    this->selectFilter = filter;
}

int SelectCollection::GetSelectContent() const
{
    return this->selectContent;
}

void SelectCollection::SetSelectContent(int content)
{
    this->selectContent = content;
}

SelectCollection::~SelectCollection()
{
    if(this->Selection)
    {
        this->Selection->Delete();
    }
    
    if(this->Input)
    {
        this->Input->Delete();
    }

    if(this->Output)
    {
        this->Output->Delete();
    }

    if(this->SelectActor)
    {
        this->SelectActor->Delete();
    }

    if(this->SelectMapper)
    {
        this->SelectMapper->Delete();
    }
}

vtkIdTypeArray *SelectCollection::GetExtractGlobalIds(vtkDataObject* db, const QString &cf)
{
    vtkIdTypeArray *globalIds = vtkIdTypeArray::New();
    if(vtkMultiBlockDataSet *mbds = vtkMultiBlockDataSet::SafeDownCast(db))
    {
        vtkCompositeDataIterator *iter = mbds->NewIterator();
        for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
        {
            qDebug() << "Flat index: " << iter->GetCurrentFlatIndex();
            vtkDataSet *ds = vtkDataSet::SafeDownCast(iter->GetCurrentDataObject());
            if(Selection::POINT == this->selectField)
            {
                this->GetExtractPointGlobalIds(ds, globalIds);
            }
            else if(Selection::CELL == this->selectField)
            {
                this->GetExtractCellGlobalIds(ds, globalIds, cf);
            }
        }
        iter->Delete();
    }

    qDebug() << "Selected Global Ids: " << globalIds->GetNumberOfValues();
    return globalIds;
}

QVector<size_t> SelectCollection::GetExtractGlobalIds()
{
    QVector<size_t> globalIds;
    vtkCompositeDataIterator *iter = this->Output->NewIterator();
    for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
        if(vtkUnstructuredGrid *ug = vtkUnstructuredGrid::SafeDownCast(iter->GetCurrentDataObject()))
        {
            if(Selection::POINT == this->selectField)
            {
                vtkAbstractArray *ids = ug->GetPointData()->GetGlobalIds();
                for (vtkIdType i = 0; i < ids->GetNumberOfValues(); i++)
                {
                    globalIds.push_back(ids->GetVariantValue(i).ToLongLong());
                }
            }
            else if(Selection::CELL == this->selectField)
            {
                vtkAbstractArray *ids = ug->GetCellData()->GetGlobalIds();
                for (vtkIdType i = 0; i < ids->GetNumberOfValues(); i++)
                {
                    globalIds.push_back(ids->GetVariantValue(i).ToLongLong());
                }
            }
        }
    }

    iter->Delete();
    globalIds.shrink_to_fit();

    qDebug() << "Selected GlobalIds: " << globalIds.size();
    return globalIds;
}

QVector<size_t> SelectCollection::GetExtractPedigreeIds()
{
    QVector<size_t> pedigreeIds;
    vtkCompositeDataIterator *iter = this->Output->NewIterator();
    for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
        if(vtkUnstructuredGrid *ug = vtkUnstructuredGrid::SafeDownCast(iter->GetCurrentDataObject()))
        {
            if(Selection::POINT == this->selectField)
            {
                vtkAbstractArray *ids = ug->GetPointData()->GetPedigreeIds();
                for (vtkIdType i = 0; i < ids->GetNumberOfValues(); i++)
                {
                    pedigreeIds.push_back(ids->GetVariantValue(i).ToLongLong());
                }
            }
            else if(Selection::CELL == this->selectField)
            {
                vtkAbstractArray *ids = ug->GetCellData()->GetPedigreeIds();
                for (vtkIdType i = 0; i < ids->GetNumberOfValues(); i++)
                {
                    pedigreeIds.push_back(ids->GetVariantValue(i).ToLongLong());
                }
            }
        }
    }

    iter->Delete();
    pedigreeIds.shrink_to_fit();

    qDebug() << "Selected PedigreeIds: " << pedigreeIds.size();
    return pedigreeIds;
}

QVector<QPointF> SelectCollection::GetSpatialExtremePoints()
{
    QVector<QPointF> selectPoints;
    vtkCompositeDataIterator *iter = this->Output->NewIterator();
    for(iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
        if(vtkUnstructuredGrid *ug = vtkUnstructuredGrid::SafeDownCast(iter->GetCurrentDataObject()))
        {
            if(Selection::POINT == this->selectField)
            {
                vtkPoints *points = ug->GetPoints();
                for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
                {
                    double x[3];
                    points->GetPoint(i, x);
                    selectPoints.push_back(QPointF(x[0], x[1]));
                }
            }
        }
    }

    std::sort(selectPoints.begin(), selectPoints.end(), compare_xy);
    qDebug() << "The number of selected points: " << selectPoints.size();

    QVector<QPointF> extremePoints;
    extremePoints.push_back(selectPoints.first());
    extremePoints.push_back(selectPoints.last());

    return extremePoints;
}

void SelectCollection::UnionSelectionNode(vtkSelectionNode *node)
{
    bool merged = false;
    for (unsigned int tn = 0; tn < this->Selection->GetNumberOfNodes(); ++tn)
    {
        vtkSelectionNode* tnode = this->Selection->GetNode(tn);
        if (tnode->EqualProperties(node))
        {
            this->UnionSelectionNodeImp(node);
            merged = true;
            break;
        }
    }
    if (!merged)
    {
        vtkSmartPointer<vtkSelectionNode> clone =
                vtkSmartPointer<vtkSelectionNode>::New();
        clone->DeepCopy(node);
        this->Selection->AddNode(clone);
    }
}

void SelectCollection::GlobalIDsSelect(vtkAbstractArray *ids)
{
    int style = this->selectStyle;
    int field = (Selection::CELL == this->selectField) ? vtkSelectionNode::CELL : vtkSelectionNode::POINT;

    vtkNew<vtkSelectionNode> node;
    node->SetContentType(vtkSelectionNode::GLOBALIDS);
    node->SetSelectionList(ids);
    node->SetFieldType(field);
    node->GetProperties()->Set(vtkSelectionNode::INVERSE(), this->isInvertSelect);

    switch (style)
    {
    case Selection::UNION:
    {
        //this->SelectionManager->Selection->Union(node);     /* !!! Something wrong more than two unions */
        this->UnionSelectionNode(node);
        //qDebug() << "UNION: " <<this->Selection->GetNode(0)->GetSelectionList()->GetNumberOfValues();
        break;
    }
    case Selection::MINUS:
    {
        this->Selection->Subtract(node);
        //qDebug() << "Minus: " <<this->Selection->GetNode(0)->GetSelectionList()->GetNumberOfValues();
        break;
    }
    case Selection::NORMAL:
    default:
    {
        this->Selection->Initialize();
        this->Selection->AddNode(node);
        //qDebug() << "NORMAL: " <<this->Selection->GetNode(0)->GetSelectionList()->GetNumberOfValues();
        break;
    }
    }

    vtkNew<vtkExtractSelection> extractor;
    extractor->SetInputData(0, this->Input);
    extractor->SetInputData(1, this->Selection);
    extractor->PreserveTopologyOff();
    extractor->Update();

    this->Output->Initialize();
    this->Output->ShallowCopy(extractor->GetOutput());
}

void SelectCollection::HighlightSelection()
{
    vtkNew<vtkCompositeDataGeometryFilter> filter;
    filter->SetInputData(this->Output);
    this->SelectMapper->SetInputConnection(filter->GetOutputPort());

    if(nullptr == this->SelectActor)
    {
        this->SelectActor = vtkActor::New();
        this->SelectActor->PickableOff();
        this->SelectActor->DragableOff();
        this->SelectActor->SetMapper(this->SelectMapper);
        this->SelectActor->GetProperty()->SetColor(this->selectColor);
        this->SelectActor->GetProperty()->SetAmbient(1.0);
        this->SelectActor->GetProperty()->SetDiffuse(0.0);
        this->SelectActor->GetProperty()->SetLighting(false);
    }

    if(Selection::CELL == this->selectField)
    {
        this->SelectActor->GetProperty()->SetRepresentationToWireframe();
        this->SelectActor->GetProperty()->SetLineWidth(2.0);
    }
    else if(Selection::POINT == selectField)
    {
        this->SelectActor->GetProperty()->SetRepresentationToPoints();
        this->SelectActor->GetProperty()->SetPointSize(10.0);
    }
}

void SelectCollection::Initialize()
{
    this->Output->Initialize();
    this->Selection->Initialize();
    this->isInvertSelect = false;
}

void SelectCollection::InvertSelection(int invert)
{
    if(nullptr == this->Selection->GetNode(0))
        return;

    vtkSelectionNode *node = this->Selection->GetNode(0);
    node->GetProperties()->Set(vtkSelectionNode::INVERSE(), invert);

    vtkNew<vtkExtractSelection> extractor;
    extractor->SetInputData(0, this->Input);
    extractor->SetInputData(1, this->Selection);
    extractor->PreserveTopologyOff();
    extractor->Update();

    this->Output->Initialize();
    this->Output->ShallowCopy(extractor->GetOutput());

    this->HighlightSelection();
}
