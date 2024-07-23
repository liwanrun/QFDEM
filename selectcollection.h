#ifndef SELECTCOLLECTION_H
#define SELECTCOLLECTION_H

#include <vtkNew.h>
#include <vtkSphereSource.h>

#include <QMutex>
#include <QSharedPointer>

class vtkActor;
class vtkPolyDataMapper;
class vtkIdTypeArray;
class vtkDataObject;
class vtkSelection;
class vtkSelectionNode;
class vtkMultiBlockDataSet;
class vtkInteractorStyle;
class vtkAbstractArray;
class vtkUnstructuredGrid;
class vtkPolyData;

class InteractorStyleInteractiveQuery;
class InteractorStyleRubberBandSelection;
class InteractorStyleInteractiveSelection;
class InteractorStyleDrawPolygonSelection;

//Compare QPointF
bool compare_xy(const QPointF& p1, const QPointF& p2);

class SelectCollection
{
public:
    static QSharedPointer<SelectCollection> GetSelectCollection();

    vtkIdTypeArray *GetExtractGlobalIds(vtkDataObject* db, const QString& cf);

    QVector<size_t> GetExtractGlobalIds();

    QVector<size_t> GetExtractPedigreeIds();

    QVector<QPointF> GetSpatialExtremePoints();

    void UnionSelectionNode(vtkSelectionNode* node);

    void GlobalIDsSelect(vtkAbstractArray *ids);

    void HighlightSelection();

    void Initialize();

    void InvertSelection(int invert);

    ~SelectCollection();

private:
    SelectCollection();

    void UnionSelectionNodeImp(vtkSelectionNode* node);

    void GetExtractCellGlobalIds(vtkDataSet *ds, vtkIdTypeArray *da, const QString& crit);

    void GetExtractPointGlobalIds(vtkDataSet *ds, vtkIdTypeArray *da);

private:
    static QSharedPointer<SelectCollection> selectCollection;

    static QMutex mutex;

public:
    vtkSelection *GetVTKSelection() const;

    void SetInputDataSet(vtkMultiBlockDataSet *mbds);
    vtkMultiBlockDataSet *GetSelectOutput() const;

    vtkActor *GetSelectActor() const;

    int GetSelectField() const;
    void SetSelectField(int field);

    int GetSelectStyle() const;
    void SetSelectStyle(int style);

    int GetSelectContent() const;
    void SetSelectContent(int content);

    bool IsInvertSelect() const;
    void SetInvertSelect(bool flag);

    void SetHighlightColor(double *rgb);

    QString GetSelectFilter() const;
    void SetSelectFilter(const QString &filter);

public:
    friend class InteractorStyleInteractiveQuery;
    friend class InteractorStyleRubberBandSelection;
    friend class InteractorStyleInteractiveSelection;
    friend class InteractorStyleDrawPolygonSelection;

    friend class ActorCollection;
    friend class BlockCollection;

private:
    vtkSelection *Selection;
    vtkMultiBlockDataSet *Input;        //vtkPolyData
    vtkMultiBlockDataSet *Output;       //vtkUnstructuredGrid

    vtkActor *SelectActor;
    vtkPolyDataMapper *SelectMapper;

    //Properties
    int selectField;
    int selectStyle;
    int selectContent;

    bool isInvertSelect;
    double selectColor[3];

    QString selectFilter;
};


namespace Selection
{
    enum SelectField
    {
        NONE = 0,
        POINT,
        CELL
    };

    enum SelectStyle
    {
        NORMAL = 0,
        UNION,
        MINUS
    };

    enum SelectContent
    {
        FRUSTUM = 0,
        BLOCKS
    };
};

#endif // SELECTCOLLECTION_H
