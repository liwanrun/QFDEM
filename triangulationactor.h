#ifndef TRIANGULATIONACTOR_H
#define TRIANGULATIONACTOR_H

#include <QSharedPointer>

#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkStringArray.h>
#include <vtkIdTypeArray.h>

class vtkUnstructuredGrid;

class Material;
class Boundary;
class Temporal;
class Triangulation;
class TriangulationActor
{
public:
    TriangulationActor();
    ~TriangulationActor();

    vtkActor *GetVTKRenderActor();

    vtkPolyDataMapper *GetVTKActorMapper();

    void Update(const QSharedPointer<Triangulation> &t);

    void Shrink(const QSharedPointer<Triangulation> &t);

public:
    void UpdateNodeIdLabelActor();

    void UpdateCellIdLabelActor();

    vtkActor2D *GetNodeIdLabelActor();

    vtkActor2D *GetCellIdLabelActor();

    vtkActor *GetFeatureEdgeActor();

public:
    void SetInputConnection(vtkAlgorithmOutput* input);

    vtkAlgorithmOutput* GetInputConnection();

    void SetFaceColor(QColor color);

    void SetEdgeColor(QColor color);

    void EdgeVisibilityOn();

    void SetPointSize(float arg);

    void UpdateInformation(const QString &id);

public:
    void SetVisibility(bool arg);

    void SetNodeIdsVisibility(bool arg);

    void SetCellIdsVisibility(bool arg);

    void SetRepresentation(int index);

    void SelectColorArray(const QString &text);

    void SetLookupTable(vtkScalarsToColors *lut);

    double *GetScalarRange(const QString &text, int comp);

public:
    void AppendNodeGlobalIds(const char* name, vtkIdType& offset);

    void AppendCellGlobalIds(const char* name, vtkIdType& offset);

    void AppendCellTypeArray(const char* name);

    void AppendNodePedigreeIds(const char* name, vtkIdType& offset);

    void AppendCellPedigreeIds(const char* name, vtkIdType& offset);

    //universal
    void AppendPointDataArray(const char *name, int comp, double value = 0.0);

    void RemovePointDataArray(const char *name);

    void AppendCellDataArray(const char *name, int comp, double value = 0.0);

    void RemoveCellDataArray(const char *name);

    //material
    void ApplyCellMaterialIndexArray(const QString &id, vtkUnstructuredGrid *ug, const char *name, Material *material);

    void ApplyCellMaterialFieldArray(const QString &id, vtkUnstructuredGrid *ug, const char *name, Material *material);

    void CancelCellMaterialIds(const QString &id, const char *name);

    //boundary
    void ApplyPointBoundaryIndexArray(const QString &id, vtkUnstructuredGrid *ug, const char *name);

    void ApplyPointBoundaryFieldArray(const char *name, vtkUnstructuredGrid *ug, Boundary* boundary);

    void CancelPointBoundaryFieldArray(const QString &id, const char *name, Boundary* boundary);

    //temporal
    void ApplyPointTemporalIndexArray(const QString &id, const char *name, vtkUnstructuredGrid *ug);

    void ApplyPointTemporalFieldArray(const char *name, vtkUnstructuredGrid *ug, Temporal *temporal);

    void CancelPointTemporalFieldArray(const QString &id, const char *name, Temporal *temporal);

    //Transform
    void TranslateTriangulationGeometry(vtkUnstructuredGrid *ug, double *comp);

    void RotateTriangulationGeometry(vtkUnstructuredGrid *ug, double *comp);

    void ScaleTriangulationGeometry(vtkUnstructuredGrid *ug, double *comp);

private:
    double GetLinearParametricCoord(const QPointF& p1, const QPointF& p2, const QPointF& p);

private:
    vtkActor *actor;
    vtkPolyDataMapper *mapper;

    vtkActor2D *nodeLabelActor;
    vtkActor2D *cellLabelActor;

    vtkActor *featureEdgeActor;

private:
    static bool is_node_id_visible;
    static bool is_cell_id_visible;
};

//Note: a) Keyword inline is prepended to the implementation rather than the declaration.
//      b) The implementation of inline function must be defined in the header file if it
//         could be used more than one source files.

#endif // TRIANGULATIONACTOR_H
