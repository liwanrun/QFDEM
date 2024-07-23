#include "selectionfilter.h"

#include <vtkIdTypeArray.h>
#include <vtkDataSet.h>
#include <vtkCellData.h>
#include <vtkCellType.h>


void SelectElementFilter::filter(vtkDataSet *ds, vtkIdTypeArray *out)
{
    if(vtkDataArray *globalIds = ds->GetCellData()->GetGlobalIds())
    {
        for (vtkIdType id = 0; id < globalIds->GetNumberOfValues(); id++)
        {
            out->InsertNextValue(globalIds->GetVariantValue(id).ToLongLong());
        }
    }
    else
    {
        throw std::invalid_argument("Attribute GlobalIDs is missing. Please check out!");
    }
}

void SelectElementFilter::filter(vtkDataSet *ds, vtkIdTypeArray *in, vtkIdTypeArray *out)
{
    if(vtkDataArray *globalIds = ds->GetCellData()->GetGlobalIds())
    {
        for (vtkIdType id = 0; id < in->GetNumberOfValues(); id++)
        {
            vtkIdType index = in->GetValue(id);
            out->InsertNextValue(globalIds->GetVariantValue(index).ToLongLong());
        }
    }
    else
    {
        throw std::invalid_argument("Attribute GlobalIDs is missing. Please check out!");
    }
}

void SelectTriangleFilter::filter(vtkDataSet *ds, vtkIdTypeArray *out)
{
    if(vtkDataArray *globalIds = ds->GetCellData()->GetGlobalIds())
    {
        for (vtkIdType id = 0; id < globalIds->GetNumberOfValues(); id++)
        {
            if(VTK_TRIANGLE == ds->GetCellType(id))
            {
                out->InsertNextValue(globalIds->GetVariantValue(id).ToLongLong());
            }
        }
    }
    else
    {
        throw std::invalid_argument("Attribute GlobalIDs is missing. Please check out!");
    }
}

void SelectTriangleFilter::filter(vtkDataSet *ds, vtkIdTypeArray *in, vtkIdTypeArray *out)
{
    if(vtkDataArray *globalIds = ds->GetCellData()->GetGlobalIds())
    {
        for (vtkIdType id = 0; id < in->GetNumberOfValues(); id++)
        {
            if(VTK_TRIANGLE == ds->GetCellType(in->GetValue(id)))
            {
                vtkIdType index = in->GetValue(id);
                out->InsertNextValue(globalIds->GetVariantValue(index).ToLongLong());
            }
        }
    }
    else
    {
        throw std::invalid_argument("Attribute GlobalIDs is missing. Please check out!");
    }
}

void SelectQuadFilter::filter(vtkDataSet *ds, vtkIdTypeArray *out)
{
    if(vtkDataArray *globalIds = ds->GetCellData()->GetGlobalIds())
    {
        for (vtkIdType id = 0; id < globalIds->GetNumberOfValues(); id++)
        {
            if(VTK_QUAD == ds->GetCellType(id))
            {
                out->InsertNextValue(globalIds->GetVariantValue(id).ToLongLong());
            }
        }
    }
    else
    {
        throw std::invalid_argument("Attribute GlobalIDs is missing. Please check out!");
    }
}

void SelectQuadFilter::filter(vtkDataSet *ds, vtkIdTypeArray *in, vtkIdTypeArray *out)
{
    if(vtkDataArray *globalIds = ds->GetCellData()->GetGlobalIds())
    {
        for (vtkIdType id = 0; id < in->GetNumberOfValues(); id++)
        {
            if(VTK_QUAD == ds->GetCellType(in->GetValue(id)))
            {
                vtkIdType index = in->GetValue(id);
                out->InsertNextValue(globalIds->GetVariantValue(index).ToLongLong());
            }
        }
    }
    else
    {
        throw std::invalid_argument("Attribute GlobalIDs is missing. Please check out!");
    }
}

void SelectConstrainedQuadFilter::filter(vtkDataSet *ds, vtkIdTypeArray *out)
{
    vtkDataArray *globalIds = ds->GetCellData()->GetGlobalIds();
    vtkAbstractArray* constraints = ds->GetCellData()->GetAbstractArray("Cell Constraint Tags");

    if(globalIds && constraints)
    {
        for (vtkIdType id = 0; id < globalIds->GetNumberOfValues(); id++)
        {
            if((VTK_QUAD == ds->GetCellType(id)) && (1 == constraints->GetVariantValue(id).ToInt()))
            {
                out->InsertNextValue(globalIds->GetVariantValue(id).ToLongLong());
            }
        }
    }
    else
    {
        throw std::invalid_argument("Attribute GlobalIDs is missing. Please check out!");
    }
}

void SelectConstrainedQuadFilter::filter(vtkDataSet *ds, vtkIdTypeArray *in, vtkIdTypeArray *out)
{
    if(vtkDataArray *globalIds = ds->GetCellData()->GetGlobalIds())
    {
        for (vtkIdType id = 0; id < in->GetNumberOfValues(); id++)
        {
            if(VTK_QUAD == ds->GetCellType(in->GetValue(id)))
            {
                vtkIdType index = in->GetValue(id);
                out->InsertNextValue(globalIds->GetVariantValue(index).ToLongLong());
            }
        }
    }
    else
    {
        throw std::invalid_argument("Attribute GlobalIDs is missing. Please check out!");
    }
}

void SelectUnconstrainedQuadFilter::filter(vtkDataSet *ds, vtkIdTypeArray *out)
{
    vtkDataArray *globalIds = ds->GetCellData()->GetGlobalIds();
    vtkAbstractArray* constraints = ds->GetCellData()->GetAbstractArray("Cell Constraint Tags");

    if(globalIds && constraints)
    {
        for (vtkIdType id = 0; id < globalIds->GetNumberOfValues(); id++)
        {
            if((VTK_QUAD == ds->GetCellType(id)) && (0 == constraints->GetVariantValue(id).ToInt()))
            {
                out->InsertNextValue(globalIds->GetVariantValue(id).ToLongLong());
            }
        }
    }
    else
    {
        throw std::invalid_argument("Attribute GlobalIDs is missing. Please check out!");
    }
}

void SelectUnconstrainedQuadFilter::filter(vtkDataSet *ds, vtkIdTypeArray *in, vtkIdTypeArray *out)
{
    if(vtkDataArray *globalIds = ds->GetCellData()->GetGlobalIds())
    {
        for (vtkIdType id = 0; id < in->GetNumberOfValues(); id++)
        {
            if(VTK_QUAD == ds->GetCellType(in->GetValue(id)))
            {
                vtkIdType index = in->GetValue(id);
                out->InsertNextValue(globalIds->GetVariantValue(index).ToLongLong());
            }
        }
    }
    else
    {
        throw std::invalid_argument("Attribute GlobalIDs is missing. Please check out!");
    }
}

QSharedPointer<SelectionFilter> SelectionFilterFactory::Create(QString criterion)
{
    QSharedPointer<SelectionFilter> filter = nullptr;

    if(QString("All") == criterion)
    {
        filter = QSharedPointer<SelectElementFilter>(new SelectElementFilter);
    }
    else if(QString("Triangles") == criterion)
    {
        filter = QSharedPointer<SelectTriangleFilter>(new SelectTriangleFilter);
    }
    else if(QString("Quads") == criterion)
    {
        filter = QSharedPointer<SelectQuadFilter>(new SelectQuadFilter);
    }
    else if(QString("Constrained Quads") == criterion)
    {
        filter = QSharedPointer<SelectConstrainedQuadFilter>(new SelectConstrainedQuadFilter);
    }
    else if(QString("Unconstrained Quads") == criterion)
    {
        filter = QSharedPointer<SelectUnconstrainedQuadFilter>(new SelectUnconstrainedQuadFilter);
    }

    return filter;
}

