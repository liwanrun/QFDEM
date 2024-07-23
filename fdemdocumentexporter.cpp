
#include "fdemdocumentexporter.h"
#include "solutioncollection.h"
#include "solution.h"
#include "actorcollection.h"
#include "actor.h"
#include "contact.h"
#include "groupcollection.h"
#include "group.h"
#include "blockcollection.h"
#include "materialcollection.h"
#include "material.h"
#include "boundarycollection.h"
#include "boundary.h"
#include "temporalcollection.h"
#include "temporal.h"
#include "history.h"
#include "historycollection.h"
#include "globalmacros.h"

#include "mechanicalconstitutivemodel.h"
#include "contactconstitutivemodel.h"
#include "hydraulicconstitutivemodel.h"
#include "thermalconstitutivemodel.h"

#include <QFile>
#include <QDateTime>
#include <QMessageBox>

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>

FDEMDocumentExporter::FDEMDocumentExporter(const QString &text)
    : filename(text)
{

}

QString FDEMDocumentExporter::GetFileName()
{
    return this->filename;
}

void FDEMDocumentExporter::Write()
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(nullptr,QString("Export Error"),QString("Failed to open the file!"));
        return;
    }
    out.setDevice(&file);

    this->WriteHeaderInformation();

    this->WriteNodeInformation();
    this->WriteElementInformation();
    this->WriteGroupInformation();
    this->WriteSolutionInformation();
    this->WriteContactInformation();
    this->WriteMaterialInformation();
    this->WriteBoundaryInformation();
    this->WriteTemporalInformation();
    this->WriteHistoryInformation();

    this->WriteClosingInformation();
}

void FDEMDocumentExporter::WriteHeaderInformation()
{
    QLocale locale = QLocale::English;
    QString datetime = locale.toString(QDateTime::currentDateTime(),QString("yyyy/MM/dd(dddd) HH:mm:ss"));

    out << QString("/* ") << qSetFieldWidth(90) << center << qSetPadChar('*')
         << QString("ACKNOWLEDGEMENT") << reset << QString(" */") << endl;

    out << QString("/* ") << qSetFieldWidth(90) << left << qSetPadChar(' ')
         << QString("This Input File Was Automatically Generated With Program FDEM2D(Version 2.0) Designed By")
         << reset << QString(" */") << endl;

    out << QString("/* ") << qSetFieldWidth(90) << left << qSetPadChar(' ')
         <<QString("the PhD Candidate Bruce Lee(liwanrun@qq.com). Enjoy and Any Feedback Is Warmly Welcomed.")
         << reset << QString(" */") << endl;

    out << QString("/* ") << qSetFieldWidth(90) << center << qSetPadChar('*')
         << datetime << reset <<QString(" */") << endl;

    out << reset << endl;
}

void FDEMDocumentExporter::WriteSolutionInformation()
{
    QSharedPointer<SolutionCollection> solutionCollection = SolutionCollection::GetSolutionCollection();
    out << solutionCollection << reset << endl;
}

void FDEMDocumentExporter::WriteElementInformation()
{
    QSharedPointer<BlockCollection> blockCollection = BlockCollection::GetBlockCollection();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();

    PRINT_STR_COMMENT(out, "/* MESHING CELLINFO */");
    PRINT_INT_SCALAR(out, "/YD/YDE/MELEM", actorCollection->numCells);
    PRINT_INT_SCALAR(out, "/YD/YDE/NELEM", actorCollection->numCells);
    PRINT_INT_SCALAR(out, "/YD/YDE/MELNO", 5);
    PRINT_INT_SCALAR(out, "/YD/YDE/NELNO", 5);
    PRINT_INT_SCALAR(out, "/YD/YDE/NEBLKS", blockCollection->GetBlockMap().size());
    PRINT_INT_SCALAR(out, "/YD/YDE/NEGRNS", blockCollection->numBlockGrains);
    PRINT_INT_SCALAR(out, "/YD/YDE/NETRIS", actorCollection->numTriangles);
    PRINT_INT_SCALAR(out, "/YD/YDE/NEQUAD", actorCollection->numQuads);
    /* | node ids | blockId */
    PRINT_INT_VECTOR(out, "/YD/YDE/I2ELTO", 21, 5, actorCollection->numCells);
    this->OutputCellTopology(out, VTK_TRIANGLE);
    this->OutputCellTopology(out, VTK_QUAD);

    if(blockCollection->numBlockGrains)
    {
        size_t offset = 0;
        PRINT_INT_SCALAR(out, "/YD/YDE/I1ELGR", actorCollection->numCells);
        this->OutputCellGrainId(out, VTK_TRIANGLE, offset);
        this->OutputCellGrainId(out, VTK_QUAD, offset);
    }

//    offset = 0;
//    PRINT_INT_SCALAR(out, "/YD/YDE/I1ELPR", actorCollection->numCells);
//    this->OutputCellProperty(out, VTK_TRIANGLE, offset);
//    this->OutputCellProperty(out, VTK_QUAD, offset);

    out << reset << endl;
}

void FDEMDocumentExporter::WriteNodeInformation()
{
    //QSharedPointer<Solution> solution = Solution::GetSolutionProperty();
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();

    PRINT_STR_COMMENT(out, "/* MESHING NODEINFO */");
    PRINT_INT_SCALAR(out, "/YD/YDN/MNOPO", actorCollection->numNodes);
    PRINT_INT_SCALAR(out, "/YD/YDN/NNOPO", actorCollection->numNodes);
    PRINT_INT_SCALAR(out, "/YD/YDN/MNODIM", 2);
    PRINT_INT_SCALAR(out, "/YD/YDN/NNODIM", 2);

    PRINT_INT_VECTOR(out, "/YD/YDN/D2NCI", 21, 2, actorCollection->numNodes);
    this->OutputNodeCoordinate(out);
    PRINT_INT_VECTOR(out, "/YD/YDN/D2NCC", 21, 2, actorCollection->numNodes);
    this->OutputNodeCoordinate(out);

//    if(Solution::MECHANICAL & solution->physicalModule)
//    {
//        PRINT_INT_SCALAR(out, "/YD/YDN/I1NOBF", actorCollection->numNodes);
//        this->OutputNodalBoundaryFeature(out);
//        PRINT_INT_SCALAR(out, "/YD/YDN/I1NOPR", actorCollection->numNodes);
//        this->OutputMechanicalBoundaryId(out);
//        PRINT_INT_SCALAR(out, "/YD/YDN/I1NOIN", actorCollection->numNodes);
//        this->OutputMechanicalTemporalId(out);
//    }

//    if(Solution::HYDRAULIC & solution->physicalModule)
//    {
//        PRINT_INT_SCALAR(out, "/YD/YDN/I1NHPR", actorCollection->numNodes);
//        this->OutputHydraulicalBoundaryId(out);
//        PRINT_INT_SCALAR(out, "/YD/YDN/I1NHIN", actorCollection->numNodes);
//        this->OutputHydraulicalTemporalId(out);
//    }

//    if(Solution::THERMAL & solution->physicalModule)
//    {
//        PRINT_INT_SCALAR(out, "/YD/YDN/I1NTPR", actorCollection->numNodes);
//        this->OutputThermalBoundaryId(out);
//        PRINT_INT_SCALAR(out, "/YD/YDN/I1NTIN", actorCollection->numNodes);
//        this->OutputThermalTemporalId(out);
//    }

    out << reset << endl;
}

void FDEMDocumentExporter::WriteGroupInformation()
{
    QSharedPointer<NodeSetCollection> nodeSetCollection = NodeSetCollection::GetNodeSetCollection();
    out << nodeSetCollection;

    QSharedPointer<ElemSetCollection> elemSetCollection = ElemSetCollection::GetElemSetCollection();
    out << elemSetCollection;
}

void FDEMDocumentExporter::WriteNodeSetInformation()
{
    QSharedPointer<NodeSetCollection> nodeSetCollection = NodeSetCollection::GetNodeSetCollection();
    out << nodeSetCollection;
}

void FDEMDocumentExporter::WriteElemSetInformation()
{
    QSharedPointer<ElemSetCollection> elemSetCollection = ElemSetCollection::GetElemSetCollection();
    out << elemSetCollection;
}

void FDEMDocumentExporter::WriteContactInformation()
{
    QSharedPointer<Contact> contact = Contact::GetContactProperty();
    out << (*contact.data());
}

void FDEMDocumentExporter::WriteMaterialInformation()
{
    QSharedPointer<MaterialCollection> materialCollection = MaterialCollection::GetMaterialCollection();
    if(materialCollection->GetMaterialMap().empty()) return;

    PRINT_STR_COMMENT(out, "/* MATERIAL COLLECTION */");
    materialCollection->ReshuffleMaterialId();
    this->OutputTriangleMaterial(out);
    this->OutputCohesiveMaterial(out);
    this->OutputNTSFluidMaterial(out);
}

void FDEMDocumentExporter::WriteTemporalInformation()
{
    QSharedPointer<TemporalCollection> temporalCollection = TemporalCollection::GetTemporalCollection();
    if(temporalCollection->GetTemporalMap().empty()) return;

    PRINT_STR_COMMENT(out, "/* TEMPORAL COLLECTION */");
    temporalCollection->ReshuffleTemporalId();
    this->OutputInitialStressStrain(out);
    this->OutputMechanicalTemporal(out);
    this->OutputHydraulicTemporal(out);
    this->OutputThermalTemporal(out);
}

void FDEMDocumentExporter::WriteBoundaryInformation()
{
    QSharedPointer<BoundaryCollection> boundaryCollection = BoundaryCollection::GetBoundaryCollection();
    if(boundaryCollection->GetBoundaryMap().empty()) return;

    PRINT_STR_COMMENT(out, "/* BOUNDARY COLLECTION */");
    boundaryCollection->ReshuffleBoundaryId();
    this->OutputMechanicalBoundary(out);
    this->OutputHydraulicalBoundary(out);
    this->OutputThermalBoundary(out);
}

void FDEMDocumentExporter::WriteClosingInformation()
{
     out << qSetFieldWidth(20) << left << QString("$YDOIT") << reset << endl;
     out << qSetFieldWidth(20) << left << QString("$YSTOP") << reset << endl;
}

void FDEMDocumentExporter::OutputCellGrainId(QTextStream &out, int celltype, size_t &count)
{
    size_t bucket = 10;

    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        vtkPolyData *pd = (iter.value())->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        vtkAbstractArray *grainIds = pd->GetCellData()->GetArray("Cell Grain Ids");
        for(vtkIdType i = 0; i < pd->GetNumberOfCells(); i++)
        {
            if(celltype == pd->GetCellType(i))
            {
                if(count != 0 && count % bucket == 0)
                    out << reset << endl;

                vtkIdType idx = grainIds->GetVariantValue(i).ToLongLong();
                vtkIdType grainId = (-1 == idx) ? -1 : (idx - 1);
                out << qSetFieldWidth(10) << left << grainId;
                count += 1;
            }
        }
    }

    if(count == actorCollection->numCells)
        out << reset << endl;
}

void FDEMDocumentExporter::OutputCellProperty(QTextStream &out, int celltype, size_t &count)
{
    size_t bucket = 10;
    const char *name = "Cell Material Ids";

    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        vtkPolyData *pd = (iter.value())->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        if(!pd->GetCellData()->HasArray(name))
            return;

        vtkAbstractArray *materialIds = pd->GetCellData()->GetAbstractArray(name);
        for(vtkIdType i = 0; i < pd->GetNumberOfCells(); i++)
        {
            if(celltype != pd->GetCellType(i))
                continue;

            if(count != 0 && count % bucket == 0)
                out << reset << endl;

            if(VTK_TRIANGLE == celltype)
            {
                int id = materialIds->GetVariantValue(i).ToInt();
                int index = this->triangleMatIds.indexOf(id);
                out << qSetFieldWidth(10) << left << index;
                count += 1;
            }
            else if(VTK_QUAD == celltype)
            {
                int id = materialIds->GetVariantValue(i).ToInt();
                int index = this->cohesiveMatIds.indexOf(id);
                index = (-1 == index) ? (-1) : (index + triangleMatIds.size());
                out << qSetFieldWidth(10) << left << index;
                count += 1;
            }
        }
    }

    if(count == actorCollection->numCells)
        out << reset << endl;
}

void FDEMDocumentExporter::OutputCellTopology(QTextStream &out, int celltype)
{
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        QSharedPointer<TriangulationActor> t = (iter.value())->GetTriangulationActor();
        vtkPolyData *pd = t->GetVTKActorMapper()->GetInput();
        vtkDataArray *globalIds = pd->GetPointData()->GetGlobalIds();

        for(vtkIdType i = 0; i < pd->GetNumberOfCells(); i++)
        {
            if(celltype != pd->GetCellType(i))
                continue;

            if(VTK_TRIANGLE == celltype)
            {
                vtkIdList *ids = pd->GetCell(i)->GetPointIds();
                vtkIdType n0 = globalIds->GetVariantValue(ids->GetId(0)).ToLongLong();
                vtkIdType n1 = globalIds->GetVariantValue(ids->GetId(1)).ToLongLong();
                vtkIdType n2 = globalIds->GetVariantValue(ids->GetId(2)).ToLongLong();
                vtkIdType n3 = -1;
                vtkIdType blockId = iter.key().split(QChar('-')).last().toInt() - 1;
                out << qSetFieldWidth(10) << left << n0 << n1 << n2 << n3 << blockId;
                out << reset << endl;
            }
            else if(VTK_QUAD == celltype)
            {
                vtkIdList *ids = pd->GetCell(i)->GetPointIds();
                vtkIdType n0 = globalIds->GetVariantValue(ids->GetId(1)).ToLongLong();
                vtkIdType n1 = globalIds->GetVariantValue(ids->GetId(0)).ToLongLong();
                vtkIdType n2 = globalIds->GetVariantValue(ids->GetId(3)).ToLongLong();
                vtkIdType n3 = globalIds->GetVariantValue(ids->GetId(2)).ToLongLong();
                vtkIdType blockId = iter.key().split(QChar('-')).last().toInt() - 1;
                out << qSetFieldWidth(10) << left << n0 << n1 << n2 << n3 << blockId;
                out << reset << endl;
            }
        }
    }
}

void FDEMDocumentExporter::OutputNodeCoordinate(QTextStream &out)
{
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        vtkPolyData *pd = (iter.value())->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        for(vtkIdType i = 0; i < pd->GetNumberOfPoints(); i++)
        {
            double *coords = pd->GetPoints()->GetPoint(i);
            if(qAbs(coords[0]) < 1.0E-15) coords[0] = 0.0;
            if(qAbs(coords[1]) < 1.0E-15) coords[1] = 0.0;
            out << qSetFieldWidth(20) << qSetRealNumberPrecision(6)
                << left << forcepoint  << coords[0] << coords[1] << reset << endl;
        }
    }
}

void FDEMDocumentExporter::OutputNodalBoundaryFeature(QTextStream &out)
{
    size_t count = 0;
    size_t bucket = 10;
    const char *name = "Point Boundary Tags";

    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        vtkPolyData *pd = (iter.value())->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        if(!pd->GetPointData()->HasArray(name))
            return;

        vtkAbstractArray* boundaryIds = pd->GetPointData()->GetAbstractArray(name);
        for(vtkIdType i = 0; i < pd->GetNumberOfPoints(); i++)
        {
            if(count != 0 && count % bucket == 0)
                out << reset << endl;

            int id = boundaryIds->GetVariantValue(i).ToInt();
            out << qSetFieldWidth(10) << left << id;
            count += 1;
        }
    }

    if(count == actorCollection->numNodes)
        out << reset << endl;
}

void FDEMDocumentExporter::OutputMechanicalBoundaryId(QTextStream &out)
{
    size_t count = 0;
    size_t bucket = 10;
    const char *name = "Point Boundary Ids (M)";

    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        vtkPolyData *pd = (iter.value())->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        if(!pd->GetPointData()->HasArray(name))
            return;

        vtkAbstractArray* boundaryIds = pd->GetPointData()->GetAbstractArray(name);
        for(vtkIdType i = 0; i < pd->GetNumberOfPoints(); i++)
        {
            if(count != 0 && count % bucket == 0)
                out << reset << endl;

            int id = boundaryIds->GetVariantValue(i).ToInt();
            int index = this->mechanicalBndIds.indexOf(id);
            out << qSetFieldWidth(10) << left << index;
            count += 1;
        }
    }

    if(count == actorCollection->numNodes)
        out << reset << endl;
}

void FDEMDocumentExporter::OutputMechanicalTemporalId(QTextStream &out)
{
    size_t count = 0;
    size_t bucket = 10;
    const char *name = "Point Initial Ids (M)";

    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        vtkPolyData *pd = (iter.value())->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        if(!pd->GetPointData()->HasArray(name))
            return;

//        vtkAbstractArray* temporalIds = pd->GetPointData()->GetAbstractArray(name);
//        for(vtkIdType i = 0; i < pd->GetNumberOfPoints(); i++)
//        {
            if(count != 0 && count % bucket == 0)
                out << reset << endl;

//            int id = temporalIds->GetVariantValue(i).ToInt();
//            int index = this->mechanicalTempIds.indexOf(id);
//            out << qSetFieldWidth(10) << left << index;
//            count += 1;
//        }
    }

    if(count == actorCollection->numNodes)
        out << reset << endl;
}

void FDEMDocumentExporter::OutputTemporalVelocity(QTextStream &out)
{
    const char *name = "Point Initial Velocity (M)";
    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        vtkPolyData *pd = (iter.value())->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        if(!pd->GetPointData()->HasArray(name))
            return;

        vtkAbstractArray* pointArray = pd->GetPointData()->GetAbstractArray(name);
        vtkDoubleArray* velocities = static_cast<vtkDoubleArray*>(pointArray);
        for(vtkIdType i = 0; i < pd->GetNumberOfPoints(); i++)
        {
            double *vel = velocities->GetTuple2(i);
            out << qSetFieldWidth(20) << qSetRealNumberPrecision(6)
                << left << forcepoint <<vel[0] << vel[1] << reset << endl;
        }
    }
}

void FDEMDocumentExporter::OutputHydraulicalBoundaryId(QTextStream &out)
{
    size_t count = 0;
    size_t bucket = 10;
    const char *name = "Point Boundary Ids (H)";

    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        vtkPolyData *pd = (iter.value())->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        if(!pd->GetPointData()->HasArray(name))
            return;

        vtkAbstractArray* boundaryIds = pd->GetPointData()->GetAbstractArray(name);
        for(vtkIdType i = 0; i < pd->GetNumberOfPoints(); i++)
        {
            if(count != 0 && count % bucket == 0)
                out << reset << endl;

            int id = boundaryIds->GetVariantValue(i).ToInt();
            int index = this->hydraulicalBndIds.indexOf(id);
            out << qSetFieldWidth(10) << left << index;
            count += 1;
        }
    }

    if(count == actorCollection->numNodes)
        out << reset << endl;
}

void FDEMDocumentExporter::OutputHydraulicalTemporalId(QTextStream &out)
{
    size_t count = 0;
    size_t bucket = 10;
    const char *name = "Point Initial Ids (H)";

    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        vtkPolyData *pd = (iter.value())->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        if(!pd->GetPointData()->HasArray(name))
            return;

//        vtkAbstractArray* boundaryIds = pd->GetPointData()->GetAbstractArray(name);
//        for(vtkIdType i = 0; i < pd->GetNumberOfPoints(); i++)
//        {
            if(count != 0 && count % bucket == 0)
                out << reset << endl;

//            int id = boundaryIds->GetVariantValue(i).ToInt();
//            int index = this->hydraulicalTempIds.indexOf(id);
//            out << qSetFieldWidth(10) << left << index;
//            count += 1;
//        }
    }

    if(count == actorCollection->numNodes)
        out << reset << endl;
}

void FDEMDocumentExporter::OutputTemporalPressure(QTextStream &out)
{
    size_t count = 0;
    size_t bucket = 10;
    const char *name = "Point Initial Pressure (H)";

    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        vtkPolyData *pd = (iter.value())->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        if(!pd->GetPointData()->HasArray(name))
            return;

        vtkAbstractArray* pressures = pd->GetPointData()->GetAbstractArray(name);
        for(vtkIdType i = 0; i < pd->GetNumberOfPoints(); i++)
        {
            if(count != 0 && count % bucket == 0)
                out << reset << endl;

            double pressure = pressures->GetVariantValue(i).ToDouble();
            out << qSetFieldWidth(10) << left << pressure;
            count += 1;
        }
    }

    if(count == actorCollection->numNodes)
        out << reset << endl;
}

void FDEMDocumentExporter::OutputThermalBoundaryId(QTextStream &out)
{
    size_t count = 0;
    size_t bucket = 10;
    const char *name = "Point Boundary Ids (T)";

    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        vtkPolyData *pd = (iter.value())->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        if(!pd->GetPointData()->HasArray(name))
            return;

        vtkAbstractArray* boundaryIds = pd->GetPointData()->GetAbstractArray(name);
        for(vtkIdType i = 0; i < pd->GetNumberOfPoints(); i++)
        {
            if(count != 0 && count % bucket == 0)
                out << reset << endl;

            int id = boundaryIds->GetVariantValue(i).ToInt();
            int index = this->thermalBndIds.indexOf(id);
            out << qSetFieldWidth(10) << left << index;
            count += 1;
        }
    }

    if(count == actorCollection->numNodes)
        out << reset << endl;
}

void FDEMDocumentExporter::OutputThermalTemporalId(QTextStream &out)
{
    size_t count = 0;
    size_t bucket = 10;
    const char *name = "Point Initial Ids (T)";

    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        vtkPolyData *pd = (iter.value())->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        if(!pd->GetPointData()->HasArray(name))
            return;

//        vtkAbstractArray* boundaryIds = pd->GetPointData()->GetAbstractArray(name);
//        for(vtkIdType i = 0; i < pd->GetNumberOfPoints(); i++)
//        {
            if(count != 0 && count % bucket == 0)
                out << reset << endl;

//            int id = boundaryIds->GetVariantValue(i).ToInt();
//            int index = this->thermalTempIds.indexOf(id);
//            out << qSetFieldWidth(10) << left << index;
//            count += 1;
//        }
    }

    if(count == actorCollection->numNodes)
        out << reset << endl;
}

void FDEMDocumentExporter::OutputTemporalTemperature(QTextStream &out)
{
    size_t count = 0;
    size_t bucket = 10;
    const char *name = "Point Initial Temperature (T)";

    QSharedPointer<ActorCollection> actorCollection = ActorCollection::GetActorCollection();
    ActorCollection::BlockActorMap block_map = actorCollection->GetBlockActorMap();

    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        vtkPolyData *pd = (iter.value())->GetTriangulationActor()->GetVTKActorMapper()->GetInput();
        if(!pd->GetPointData()->HasArray(name))
            return;

        vtkAbstractArray* temperatures = pd->GetPointData()->GetAbstractArray(name);
        for(vtkIdType i = 0; i < pd->GetNumberOfPoints(); i++)
        {
            if(count != 0 && count % bucket == 0)
                out << reset << endl;

            double temperature = temperatures->GetVariantValue(i).ToDouble();
            out << qSetFieldWidth(10) << left << temperature;
            count += 1;
        }
    }

    if(count == actorCollection->numNodes)
        out << reset << endl;
}

void FDEMDocumentExporter::OutputTriangleMaterial(QTextStream &out)
{
    QSharedPointer<MaterialCollection> collection = MaterialCollection::GetMaterialCollection();
    int numMats = collection->GetTriangleMaterialIds().size();
    if(0 == numMats) return;

    PRINT_STR_COMMENT(out, "/* Triangle Property */");
    PRINT_INT_SCALAR(out, "/YD/YDPE/MPROP", numMats);
    PRINT_INT_SCALAR(out, "/YD/YDPE/NPROP", numMats);

    PRINT_INT_SCALAR(out, "/YD/YDPE/I1PEST", numMats);
    this->OutputTriangleMechanicalProperty(out, QString("enableStep"));
    PRINT_INT_SCALAR(out, "/YD/YDPE/I1PEBD", numMats);
    this->OutputTriangleMechanicalProperty(out, QString("elemStatus"));
    PRINT_INT_SCALAR(out, "/YD/YDPE/D1PERO", numMats);
    this->OutputTriangleMechanicalProperty(out, QString("density"));

    // Mechanical Property
    if(collection->IsTriangleMechanicalPropertyInvolved())
    {
        QString modelNames = QString("/* (0)Rigid (1)Hookean (2)Anisotropy (3)Elastoplasticity (4)Plane_stress (5)Plane_strain */");
        PRINT_STR_COMMENT(out, modelNames);
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/I1PETY"), numMats);
        this->OutputTriangleMechanicalProperty(out, QString("mechanicalModelType"));
        if(collection->IsTriangleElasticityEnabled())
        {
            bool isInhomoEnabled = collection->IsTriangleMechanicalPropertyInhomoEnabled();
            if(isInhomoEnabled)
            {
                PRINT_INT_SCALAR(out, QString("/YD/YDPE/I1PEIN"), numMats);
                this->OutputTriangleMechanicalProperty(out, QString("isPropertyInhomoEnabled"));
                PRINT_INT_SCALAR(out, QString("/YD/YDPE/I1PEIO"), numMats);
                this->OutputTriangleMechanicalProperty(out, QString("inhomogeneousOption"));
            }
            PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PEYE"), numMats);
            this->OutputTriangleMechanicalProperty(out, QString("youngModulus"));
            if(isInhomoEnabled)
            {
                PRINT_INT_SCALAR(out, QString("/YD/YDPE/I1PSYE"), numMats);
                this->OutputTriangleMechanicalProperty(out, QString("youngModulusSeed"));
                PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PMYE"), numMats);
                this->OutputTriangleMechanicalProperty(out, QString("youngModulusIndex"));
            }
            PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PENU"), numMats);
            this->OutputTriangleMechanicalProperty(out, QString("poissonRatio"));
            if(isInhomoEnabled)
            {
                PRINT_INT_SCALAR(out, QString("/YD/YDPE/I1PSNU"), numMats);
                this->OutputTriangleMechanicalProperty(out, QString("poissonRatioSeed"));
                PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PMNU"), numMats);
                this->OutputTriangleMechanicalProperty(out, QString("poissonRatioIndex"));
            }
            PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PEKS"), numMats);
            this->OutputTriangleMechanicalProperty(out, QString("viscousDamping"));
        }
        if(collection->IsTrianglePlasticityEnabled())
        {
            PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PESM"), numMats);
            this->OutputTriangleMechanicalProperty(out, QString("softenStretch"));
        }

        // Contact Property
        QString cotactNames = QString("/* (0)Elasticity (1)Dissipation */");
        PRINT_STR_COMMENT(out, cotactNames);
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/I1PECT"), numMats);
        this->OutputTriangleContactProperty(out, QString("contactModelType"));
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PEPE"), numMats);
        this->OutputTriangleContactProperty(out, QString("normalStiffness"));
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PEPT"), numMats);
        this->OutputTriangleContactProperty(out, QString("tangentStiffness"));
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PEFR"), numMats);
        this->OutputTriangleContactProperty(out, QString("contactFriction"));
        if(collection->IsTriangleContactDampEnabled())
        {
            PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PEYS"), numMats);
            this->OutputTriangleContactProperty(out, QString("yieldStrength"));
            PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PEXP"), numMats);
            this->OutputTriangleContactProperty(out, QString("powerExponent"));
        }
    }

    // Hydraulic Property
    if(collection->IsTriangleHydraulicPropertyInvolved())
    {
        QString modelNames = QString("/* (0)Isotropy (1)Anisotropy */");
        PRINT_STR_COMMENT(out, modelNames);
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/I1PHTY"), numMats);
        this->OutputTriangleHydraulicProperty(out, QString("hydraulicModelType"));
        bool isInhomoEnabled = collection->IsTriangleHydraulicPropertyInhomoEnabled();
        if(isInhomoEnabled)
        {
            PRINT_INT_SCALAR(out, QString("/YD/YDPE/I1PHIN"), numMats);
            this->OutputTriangleHydraulicProperty(out, QString("isPermeabilityInhomoEnabled"));
        }
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PHXX"), numMats);
        this->OutputTriangleHydraulicProperty(out, QString("majorPermeability"));
        if(isInhomoEnabled)
        {
            PRINT_INT_SCALAR(out, QString("/YD/YDPE/I1PSPC"), numMats);
            this->OutputTriangleHydraulicProperty(out, QString("permeabilitySeed"));
            PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PMPC"), numMats);
            this->OutputTriangleHydraulicProperty(out, QString("permeabilityIndex"));
        }
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PHYY"), numMats);
        this->OutputTriangleHydraulicProperty(out, QString("minorPermeability"));
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PHDD"), numMats);
        this->OutputTriangleHydraulicProperty(out, QString("orientationAngle"));

        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PEHB"), numMats);
        this->OutputTriangleHydraulicProperty(out, QString("biotCoefficient"));
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PEHN"), numMats);
        this->OutputTriangleHydraulicProperty(out, QString("porePorosity"));
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PEHK"), numMats);
        this->OutputTriangleHydraulicProperty(out, QString("poreModulus"));
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PEHS"), numMats);
        this->OutputTriangleHydraulicProperty(out, QString("poreSource"));
    }

    // Thermal Property
    if(collection->IsTriangleThermalPropertyInvolved())
    {
        QString modelNames = QString("/* (0)Isotropy (1)Anisotropy */");
        PRINT_STR_COMMENT(out, modelNames);
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/I1PTTY"), numMats);
        this->OutputTriangleThermalProperty(out, QString("thermalModelType"));
        bool isInhomoEnabled = collection->IsTriangleThermalPropertyInhomoEnabled();
        if(isInhomoEnabled)
        {
            PRINT_INT_SCALAR(out, QString("/YD/YDPE/I1PTIN"), numMats);
            this->OutputTriangleThermalProperty(out, QString("isConductivityInhomoEnabled"));
        }
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PTXX"), numMats);
        this->OutputTriangleThermalProperty(out, QString("majorConductivity"));
        if(isInhomoEnabled)
        {
            PRINT_INT_SCALAR(out, QString("/YD/YDPE/I1PSCC"), numMats);
            this->OutputTriangleThermalProperty(out, QString("majorConductivitySeed"));
            PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PMCC"), numMats);
            this->OutputTriangleThermalProperty(out, QString("majorConductivityIndex"));
        }
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PTYY"), numMats);
        this->OutputTriangleThermalProperty(out, QString("minorConductivity"));
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PTDD"), numMats);
        this->OutputTriangleThermalProperty(out, QString("orientationAngle"));

        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PETC"), numMats);
        this->OutputTriangleThermalProperty(out, QString("heatCapacity"));
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PETH"), numMats);
        this->OutputTriangleThermalProperty(out, QString("heatExchange"));
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PETS"), numMats);
        this->OutputTriangleThermalProperty(out, QString("heatSource"));
        PRINT_INT_SCALAR(out, QString("/YD/YDPE/D1PETE"), numMats);
        this->OutputTriangleThermalProperty(out, QString("heatExpansion"));
    }

    // Element Sets
    this->OutputTriangleMaterialElemSets(out);
    out << reset << endl;
}

void FDEMDocumentExporter::OutputTriangleMaterialElemSets(QTextStream &out)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<MaterialCollection> collection = MaterialCollection::GetMaterialCollection();
    QStringList materialIds = collection->GetTriangleMaterialIds();

    PRINT_INT_SCALAR(out, "/YD/YDPE/I1SEHP", materialIds.size());
    int offset = 0;
    for(int i = 0; i < materialIds.size(); i++)
    {
        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        QSharedPointer<Material> superMat = collection->GetMaterial(materialIds[i]);
        TriangleMaterial *material = static_cast<TriangleMaterial*>(superMat.data());
        out << qSetFieldWidth(field) << left << offset << reset;

        QStringList elemSets = material->elementSet.split(QChar(','), QString::SkipEmptyParts);
        for (auto elemSet : elemSets)
        {
            if (elemSet.contains(QChar(':')))
            {
                QStringList ids = elemSet.split(QChar(':'), QString::SkipEmptyParts);
                offset += (ids[1].toInt() - ids[0].toInt()) / ids[2].toInt();
            }
            else
            {
                offset += 1;
            }
        }

//        offset += elemSets.size();
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPE/I1SESZ", materialIds.size());
    for(int i = 0; i < materialIds.size(); i++)
    {
        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        QSharedPointer<Material> superMat = collection->GetMaterial(materialIds[i]);
        TriangleMaterial *material = static_cast<TriangleMaterial*>(superMat.data());
        QStringList elemSets = material->elementSet.split(QChar(','), QString::SkipEmptyParts);

        int count = 0;
        for (auto elemSet : elemSets)
        {
            if (elemSet.contains(QChar(':')))
            {
                QStringList ids = elemSet.split(QChar(':'), QString::SkipEmptyParts);
                count += (ids[1].toInt() - ids[0].toInt()) / ids[2].toInt();
            }
            else
            {
                count += 1;
            }
        }

        out << qSetFieldWidth(field) << left << count << reset;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPE/MELSET", offset);
    PRINT_INT_SCALAR(out, "/YD/YDPE/I1SEID", offset);
    int index = 0;
    for(const QString id : collection->GetTriangleMaterialIds())
    {
        if((index != 0) && (index % ncols == 0))
            out << reset << endl;

        QSharedPointer<Material> superMat = collection->GetMaterial(id);
        TriangleMaterial *material = static_cast<TriangleMaterial*>(superMat.data());

        QStringList elemSets = material->elementSet.split(QChar(','), QString::SkipEmptyParts);
        for (auto elemSet : elemSets)
        {
            if (elemSet.contains(QChar(':')))
            {
                QStringList ids = elemSet.split(QChar(':'), QString::SkipEmptyParts);
                for (int i = ids[0].toInt(); i < ids[1].toInt(); i += ids[2].toInt())
                {
                    out << qSetFieldWidth(10) << left << i - 1 << reset;
                    index += 1;
                }
            }
            else
            {
                out << qSetFieldWidth(10) << left << elemSet.toInt() - 1 << reset;
                index += 1;
            }
        }

        //for(auto iter = elemSets.begin(); iter != elemSets.end(); ++iter)
        //{
        //    out << qSetFieldWidth(10) << left << (*iter).toInt() - 1 << reset;
        //    index += 1;
        //}
    }
    out << reset << endl;
}

void FDEMDocumentExporter::OutputTriangleMechanicalProperty(QTextStream &out, QString arg)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<MaterialCollection> collection = MaterialCollection::GetMaterialCollection();
    QStringList materialIds = collection->GetTriangleMaterialIds();
    for(int i = 0; i < materialIds.size(); i++)
    {
        QSharedPointer<Material> superMat = collection->GetMaterial(materialIds[i]);
        TriangleMaterial *material = static_cast<TriangleMaterial*>(superMat.data());
        QSharedPointer<ConstitutiveModel> superModel = material->mechanicalModel;
        TriangleMechanicalModel *model = static_cast<TriangleMechanicalModel*>(superModel.data());

        if(i != 0 && i % ncols == 0)
            out << reset << endl;

        if(arg == QString("enableStep"))
        {
            out << qSetFieldWidth(field) << left << material->enableStep - 1 << reset;
        }
        else if(arg == QString("elemStatus"))
        {
            out << qSetFieldWidth(field) << left << material->elemStatus << reset;
        }
        else if(arg == QString("density"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << material->density << reset;
        }
        else if(arg == QString("mechanicalModelType"))
        {
            out << qSetFieldWidth(field) << left << material->mechanicalModelType << reset;
        }
        else if(arg == QString("isPropertyInhomoEnabled"))
        {
            out << qSetFieldWidth(field) << left << model->isPropertyInhomoEnabled << reset;
        }
        else if(arg == QString("inhomogeneousOption"))
        {
            out << qSetFieldWidth(field) << left << model->inhomogeneousOption << reset;
        }
        else if(arg == QString("youngModulus"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->youngModulus << reset;
        }
        else if(arg == QString("youngModulusSeed"))
        {
            out << qSetFieldWidth(field) << left << model->youngModulusSeed << reset;
        }
        else if(arg == QString("youngModulusIndex"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->youngModulusIndex << reset;
        }
        else if(arg == QString("poissonRatio"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->poissonRatio << reset;
        }
        else if(arg == QString("poissonRatioSeed"))
        {
            out << qSetFieldWidth(field) << left << model->poissonRatioSeed << reset;
        }
        else if(arg == QString("poissonRatioIndex"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->poissonRatioIndex << reset;
        }
        else if(arg == QString("viscousDamping"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->viscousDamping << reset;
        }
        else if(arg == QString("softenStretch"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->softenStretch << reset;
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputTriangleContactProperty(QTextStream &out, QString arg)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<MaterialCollection> collection = MaterialCollection::GetMaterialCollection();
    QStringList materialIds = collection->GetTriangleMaterialIds();
    for(int i = 0; i < materialIds.size(); i++)
    {
        QSharedPointer<Material> superMat = collection->GetMaterial(materialIds[i]);
        TriangleMaterial *material = static_cast<TriangleMaterial*>(superMat.data());
        QSharedPointer<ConstitutiveModel> superModel = material->contactModel;
        ContactConstitutiveModel *model = static_cast<ContactConstitutiveModel*>(superModel.data());

        if(i != 0 && i % ncols == 0)
            out << reset << endl;

        if(arg == QString("contactModelType"))
        {
            out << qSetFieldWidth(field) << left << material->contactModelType << reset;
        }
        else if(arg == QString("normalStiffness"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->normalStiffness << reset;
        }
        else if(arg == QString("tangentStiffness"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->tangentStiffness << reset;
        }
        else if(arg == QString("contactFriction"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->contactFriction << reset;
        }
        else if(arg == QString("yieldStrength"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->yieldStrength << reset;
        }
        else if(arg == QString("powerExponent"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->powerExponent << reset;
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputTriangleHydraulicProperty(QTextStream &out, QString arg)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<MaterialCollection> collection = MaterialCollection::GetMaterialCollection();
    QStringList materialIds = collection->GetTriangleMaterialIds();
    for(int i = 0; i < materialIds.size(); i++)
    {
        QSharedPointer<Material> superMat = collection->GetMaterial(materialIds[i]);
        TriangleMaterial *material = static_cast<TriangleMaterial*>(superMat.data());
        QSharedPointer<ConstitutiveModel> superModel = material->hydraulicModel;
        TriangleHydraulicModel *model = static_cast<TriangleHydraulicModel*>(superModel.data());

        if(i != 0 && i % ncols == 0)
            out << reset << endl;

        if(arg == QString("hydraulicModelType"))
        {
            out << qSetFieldWidth(field) << left << material->hydraulicModelType << reset;
        }
        else if(arg == QString("isPermeabilityInhomoEnabled"))
        {
            out << qSetFieldWidth(field) << left << model->isPermeabilityInhomoEnabled << reset;
        }
        else if(arg == QString("majorPermeability"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->majorPermeability << reset;
        }
        else if(arg == QString("permeabilitySeed"))
        {
            out << qSetFieldWidth(field) << left << model->permeabilitySeed << reset;
        }
        else if(arg == QString("permeabilityIndex"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->permeabilityIndex << reset;
        }
        else if(arg == QString("minorPermeability"))
        {
            out << qSetFieldWidth(field) << left << model->minorPermeability << reset;
        }
        else if(arg == QString("orientationAngle"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->orientationAngle << reset;
        }
        else if(arg == QString("biotCoefficient"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->biotCoefficient << reset;
        }
        else if(arg == QString("porePorosity"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->porePorosity << reset;
        }
        else if(arg == QString("poreModulus"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->poreModulus << reset;
        }
        else if(arg == QString("poreSource"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->poreSource << reset;
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputTriangleThermalProperty(QTextStream &out, QString arg)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<MaterialCollection> collection = MaterialCollection::GetMaterialCollection();
    QStringList materialIds = collection->GetTriangleMaterialIds();
    for(int i = 0; i < materialIds.size(); i++)
    {
        QSharedPointer<Material> superMat = collection->GetMaterial(materialIds[i]);
        TriangleMaterial *material = static_cast<TriangleMaterial*>(superMat.data());
        QSharedPointer<ConstitutiveModel> superModel = material->thermalModel;
        TriangleThermalModel *model = static_cast<TriangleThermalModel*>(superModel.data());

        if(i != 0 && i % ncols == 0)
            out << reset << endl;

        if(arg == QString("thermalModelType"))
        {
            out << qSetFieldWidth(field) << left << material->thermalModelType << reset;
        }
        else if(arg == QString("isConductivityInhomoEnabled"))
        {
            out << qSetFieldWidth(field) << left << model->isConductivityInhomoEnabled << reset;
        }
        else if(arg == QString("majorConductivity"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->majorConductivity << reset;
        }
        else if(arg == QString("conductivitySeed"))
        {
            out << qSetFieldWidth(field) << left << model->conductivitySeed << reset;
        }
        else if(arg == QString("conductivityIndex"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->conductivityIndex << reset;
        }
        else if(arg == QString("minorConductivity"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->minorConductivity << reset;
        }
        else if(arg == QString("orientationAngle"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->orientationAngle << reset;
        }
        else if(arg == QString("heatCapacity"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->heatCapacity << reset;
        }
        else if(arg == QString("heatExchange"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->heatExchange << reset;
        }
        else if(arg == QString("heatSource"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->heatSource << reset;
        }
        else if(arg == QString("heatExpansion"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->heatExpansion << reset;
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputCohesiveMaterial(QTextStream &out)
{
    QSharedPointer<MaterialCollection> collection = MaterialCollection::GetMaterialCollection();
    int numMats = collection->GetCohesiveMaterialIds().size();
    if(0 == numMats) return;

    PRINT_STR_COMMENT(out, "/* Cohesive Property */");
    PRINT_INT_SCALAR(out, "/YD/YDPJ/MPROP", numMats);
    PRINT_INT_SCALAR(out, "/YD/YDPJ/NPROP", numMats);
    PRINT_INT_SCALAR(out, "/YD/YDPJ/NINTPT", 3);

    PRINT_INT_SCALAR(out, "/YD/YDPJ/I1PJST", numMats);
    this->OutputCohesiveMechanicalProperty(out, QString("enableStep"));
    PRINT_INT_SCALAR(out, "/YD/YDPJ/I1PJBD", numMats);
    this->OutputCohesiveMechanicalProperty(out, QString("elemStatus"));
    PRINT_INT_SCALAR(out, "/YD/YDPJ/I1PJBK", numMats);
    this->OutputCohesiveMechanicalProperty(out, QString("isCohesiveBroken"));

    // Mechanical  Property
    if(collection->IsCohesiveMechanicalPropertyInvolved())
    {
        QString modelNames = QString("/* (0)Bilinearity (1)Exponential */");
        PRINT_STR_COMMENT(out, modelNames);
        PRINT_INT_SCALAR(out, "/YD/YDPJ/I1PJTY", numMats);
        this->OutputCohesiveMechanicalProperty(out, QString("mechanicalModelType"));
        bool isInhomoEnabled = collection->IsCohesiveMechanicalPropertyInhomoEnabled();
        if(isInhomoEnabled)
        {
            PRINT_INT_SCALAR(out, "/YD/YDPJ/I1PJIN", numMats);
            this->OutputCohesiveMechanicalProperty(out, QString("isPropertyInhomoEnabled"));
            PRINT_INT_SCALAR(out, "/YD/YDPJ/I1PJIO", numMats);
            this->OutputCohesiveMechanicalProperty(out, QString("inhomogeneousOption"));
        }
        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PJPE", numMats);
        this->OutputCohesiveMechanicalProperty(out, QString("normalPenalty"));
        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PJPS", numMats);
        this->OutputCohesiveMechanicalProperty(out, QString("tangentPenalty"));

        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PJFT", numMats);
        this->OutputCohesiveMechanicalProperty(out, QString("tension"));
        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PJCO", numMats);
        this->OutputCohesiveMechanicalProperty(out, QString("cohesion"));
        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PJFR", numMats);
        this->OutputCohesiveMechanicalProperty(out, QString("friction"));
        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PJGT", numMats);
        this->OutputCohesiveMechanicalProperty(out, QString("tensionFERR"));
        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PJGS", numMats);
        this->OutputCohesiveMechanicalProperty(out, QString("shearFERR"));
    }

    // Hydraulic Property
    if(collection->IsCohesiveHydraulicPropertyInvolved())
    {
        QString modelNames = QString("/* (0)Incompressible (1)Compressible */");
        PRINT_STR_COMMENT(out, modelNames);
        PRINT_INT_SCALAR(out, "/YD/YDPJ/I1PHTY", numMats);
        this->OutputCohesiveHydraulicProperty(out, QString("hydraulicModelType"));
        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PHHJ", numMats);
        this->OutputCohesiveHydraulicProperty(out, QString("cohesiveExchange"));
        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PHHF", numMats);
        this->OutputCohesiveHydraulicProperty(out, QString("fractureExchange"));

        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PJAI", numMats);
        this->OutputCohesiveHydraulicProperty(out, QString("iniAperture"));
        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PJAL", numMats);
        this->OutputCohesiveHydraulicProperty(out, QString("minAperture"));
        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PJAU", numMats);
        this->OutputCohesiveHydraulicProperty(out, QString("maxAperture"));
    }

    // Thermal Property
    if(collection->IsCohesiveThermalPropertyInvolved())
    {
        QString modelNames = QString("/* (0)Heat_convectivon */");
        PRINT_STR_COMMENT(out, modelNames);
        PRINT_INT_SCALAR(out, "/YD/YDPJ/I1PTTY", numMats);
        this->OutputCohesiveThermalProperty(out, QString("thermalModelType"));
        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PTHJ", numMats);
        this->OutputCohesiveThermalProperty(out, QString("cohesiveExchange"));
        PRINT_INT_SCALAR(out, "/YD/YDPJ/D1PTHF", numMats);
        this->OutputCohesiveThermalProperty(out, QString("fractureExchange"));
    }

    // Element Sets
    this->OutputCohesiveMaterialElemSets(out);
    out << reset << endl;
}

void FDEMDocumentExporter::OutputCohesiveMaterialElemSets(QTextStream &out)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<MaterialCollection> collection = MaterialCollection::GetMaterialCollection();
    QStringList materialIds = collection->GetCohesiveMaterialIds();

    PRINT_INT_SCALAR(out, "/YD/YDPJ/I1SEHP", materialIds.size());
    int offset = 0;
    for(int i = 0; i < materialIds.size(); i++)
    {
        QSharedPointer<Material> superMat = collection->GetMaterial(materialIds[i]);
        CohesiveMaterial *material = static_cast<CohesiveMaterial*>(superMat.data());
        QStringList elemSets = material->elementSet.split(QChar(','), QString::SkipEmptyParts);

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        out << qSetFieldWidth(field) << left << offset << reset;
        offset += elemSets.size();
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPJ/I1SESZ", materialIds.size());
    for(int i = 0; i < materialIds.size(); i++)
    {
        QSharedPointer<Material> superMat = collection->GetMaterial(materialIds[i]);
        CohesiveMaterial *material = static_cast<CohesiveMaterial*>(superMat.data());
        QStringList elemSets = material->elementSet.split(QChar(','), QString::SkipEmptyParts);

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        out << qSetFieldWidth(field) << left << elemSets.size() << reset;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPJ/MELSET", offset);
    PRINT_INT_SCALAR(out, "/YD/YDPJ/I1SEID", offset);
    int index = 0;
    for(const QString id : collection->GetCohesiveMaterialIds())
    {
        QSharedPointer<Material> superMat = collection->GetMaterial(id);
        CohesiveMaterial *material = static_cast<CohesiveMaterial*>(superMat.data());
        QStringList elemSets = material->elementSet.split(QChar(','), QString::SkipEmptyParts);

        if((index != 0) && (index % ncols == 0))
            out << reset << endl;

        for(auto iter = elemSets.begin(); iter != elemSets.end(); ++iter)
        {
            out << qSetFieldWidth(10) << left << (*iter).toInt() - 1 << reset;
            index += 1;
        }
    }
    out << reset << endl;
}

void FDEMDocumentExporter::OutputCohesiveMechanicalProperty(QTextStream &out, QString arg)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<MaterialCollection> collection = MaterialCollection::GetMaterialCollection();
    QStringList materialIds = collection->GetCohesiveMaterialIds();
    for(int i = 0; i < materialIds.size(); i++)
    {
        QSharedPointer<Material> superMat = collection->GetMaterial(materialIds[i]);
        CohesiveMaterial *material = static_cast<CohesiveMaterial*>(superMat.data());
        QSharedPointer<ConstitutiveModel> superModel = material->mechanicalModel;
        CohesiveMechanicalModel *model = static_cast<CohesiveMechanicalModel*>(superModel.data());

        if(i != 0 && i % ncols == 0)
            out << reset << endl;

        if(arg == QString("enableStep"))
        {
            out << qSetFieldWidth(field) << left << material->enableStep - 1 << reset;
        }
        else if(arg == QString("elemStatus"))
        {
            out << qSetFieldWidth(field) << left << material->elemStatus << reset;
        }
        else if(arg == QString("isCohesiveBroken"))
        {
            out << qSetFieldWidth(field) << left << material->isCohesiveBroken << reset;
        }
        else if(arg == QString("mechanicalModelType"))
        {
            out << qSetFieldWidth(field) << left << material->mechanicalModelType << reset;
        }
        else if(arg == QString("isPropertyInhomoEnabled"))
        {
            out << qSetFieldWidth(field) << left << model->isPropertyInhomoEnabled << reset;
        }
        else if(arg == QString("inhomogeneousOption"))
        {
            out << qSetFieldWidth(field) << left << model->inhomogeneousOption << reset;
        }
        else if(arg == QString("normalPenalty"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->normalPenalty << reset;
        }
        else if(arg == QString("tangentPenalty"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->tangentPenalty << reset;
        }
        else if(arg == QString("tension"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->tension << reset;
        }
        else if(arg == QString("cohesion"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->cohesion << reset;
        }
        else if(arg == QString("friction"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->friction << reset;
        }
        else if(arg == QString("tensionFERR"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->tensionFERR << reset;
        }
        else if(arg == QString("shearFERR"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->shearFERR << reset;
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputCohesiveHydraulicProperty(QTextStream &out, QString arg)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<MaterialCollection> collection = MaterialCollection::GetMaterialCollection();
    QStringList materialIds = collection->GetCohesiveMaterialIds();
    for(int i = 0; i < materialIds.size(); i++)
    {
        QSharedPointer<Material> superMat = collection->GetMaterial(materialIds[i]);
        CohesiveMaterial *material = static_cast<CohesiveMaterial*>(superMat.data());
        QSharedPointer<ConstitutiveModel> superModel = material->hydraulicModel;
        CohesiveHydraulicModel *model = static_cast<CohesiveHydraulicModel*>(superModel.data());

        if(i != 0 && i % ncols == 0)
            out << reset << endl;

        if(arg == QString("hydraulicModelType"))
        {
            out << qSetFieldWidth(field) << left << material->hydraulicModelType << reset;
        }
        else if(arg == QString("cohesiveExchange"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->cohesiveExchange << reset;
        }
        else if(arg == QString("fractureExchange"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->fractureExchange << reset;
        }
        else if(arg == QString("iniAperture"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->iniAperture << reset;
        }
        else if(arg == QString("minAperture"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->minAperture << reset;
        }
        else if(arg == QString("maxAperture"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->maxAperture << reset;
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputCohesiveThermalProperty(QTextStream &out, QString arg)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<MaterialCollection> collection = MaterialCollection::GetMaterialCollection();
    QStringList materialIds = collection->GetCohesiveMaterialIds();
    for(int i = 0; i < materialIds.size(); i++)
    {
        QSharedPointer<Material> superMat = collection->GetMaterial(materialIds[i]);
        CohesiveMaterial *material = static_cast<CohesiveMaterial*>(superMat.data());
        QSharedPointer<ConstitutiveModel> superModel = material->thermalModel;
        CohesiveThermalModel *model = static_cast<CohesiveThermalModel*>(superModel.data());

        if(i != 0 && i % ncols == 0)
            out << reset << endl;

        if(arg == QString("thermalModelType"))
        {
            out << qSetFieldWidth(field) << left << material->thermalModelType << reset;
        }
        else if(arg == QString("cohesiveExchange"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->cohesiveExchange << reset;
        }
        else if(arg == QString("fractureExchange"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << model->fractureExchange << reset;
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputNTSFluidMaterial(QTextStream &out)
{
    QSharedPointer<SolutionCollection> solutionCollection = SolutionCollection::GetSolutionCollection();
    QSharedPointer<MaterialCollection> collection = MaterialCollection::GetMaterialCollection();
    int numMats = collection->GetNTSFluidMaterialIds().size();
    if(0 == numMats) return;

    QString materialId = collection->GetNTSFluidMaterialIds().front();
    QSharedPointer<Material> superMat = collection->GetMaterial(materialId);
    NTFluidMaterial *material = static_cast<NTFluidMaterial*>(superMat.data());

    PRINT_STR_COMMENT(out, "/* NtsFluid Property */");
    PRINT_DBL_SCALAR(out, QString("/YD/YDPF/DPFRHO"), material->density);
    if(HYDRAULIC & solutionCollection->GetSolutionModule())
    {
        PRINT_DBL_SCALAR(out, QString("/YD/YDPF/DPFMOD"), material->bulkModulus);
        PRINT_DBL_SCALAR(out, QString("/YD/YDPF/DPFEPS"), material->bulkViscosity);
        PRINT_DBL_SCALAR(out, QString("/YD/YDPF/DPFVSC"), material->dynaViscosity);
    }

    if(THERMAL & solutionCollection->GetSolutionModule())
    {
        PRINT_DBL_SCALAR(out, QString("/YD/YDPF/DPFTTK"), material->conductivity);
        PRINT_DBL_SCALAR(out, QString("/YD/YDPF/DPFTTC"), material->heatCapacity);
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputMechanicalBoundary(QTextStream &out)
{
    QString classicBnds = QString("/* (0)Force (1)Acceleration (2)Velocity (3)Traction */");
    QString viscousBnds = QString("/* (-1)None (0)VBC (1)VSBC (2)Free-field (3)Static-dynamic */");

    QSharedPointer<BoundaryCollection> collection = BoundaryCollection::GetBoundaryCollection();
    int numBnds = collection->GetMechanicalBoundaryList().size();
    if(0 == numBnds) return;

    PRINT_STR_COMMENT(out, "/* Mechanical Boundary */");
    PRINT_INT_SCALAR(out, "/YD/YDPN/MPROP", numBnds);
    PRINT_INT_SCALAR(out, "/YD/YDPN/NPROP", numBnds);

    PRINT_INT_SCALAR(out, "/YD/YDPN/I1PNST", numBnds);
    this->OutputMechanicalBoundaryProperty(out, QString("loadStep"));
    PRINT_INT_SCALAR(out, "/YD/YDPN/I1PNSN", numBnds);
    this->OutputMechanicalBoundaryProperty(out, QString("nodeSetId"));

    PRINT_STR_COMMENT(out, classicBnds);
    PRINT_INT_SCALAR(out, "/YD/YDPN/I1PNTX", numBnds);
    this->OutputMechanicalBoundaryProperty(out, QString("loadingTypeX"));
    PRINT_INT_SCALAR(out, "/YD/YDPN/I1PNTY", numBnds);
    this->OutputMechanicalBoundaryProperty(out, QString("loadingTypeY"));
    PRINT_INT_SCALAR(out, "/YD/YDPN/D1PNAX", numBnds);
    this->OutputMechanicalBoundaryProperty(out, QString("amplitudeX"));
    PRINT_INT_SCALAR(out, "/YD/YDPN/D1PNAY", numBnds);
    this->OutputMechanicalBoundaryProperty(out, QString("amplitudeY"));

    if(collection->IsMechanicalBoundarySiteDependent())
    {
        this->OutputMechanicalBoundarySpatialFactor(out);
    }

    if(collection->IsMechanicalBoundaryTimeDependent())
    {
        this->OutputMechanicalBoundaryTemporalFactor(out);
    }

    PRINT_INT_SCALAR(out, "/YD/YDPN/I1PNCB", numBnds);
    this->OutputMechanicalBoundaryProperty(out, QString("contactBnd"));
    if(collection->IsArtificialBoundaryEnabled())
    {
        PRINT_STR_COMMENT(out, viscousBnds);
        PRINT_INT_SCALAR(out, "/YD/YDPN/I1PNVB", numBnds);
        this->OutputMechanicalBoundaryProperty(out, QString("viscousBnd"));
    }

    PRINT_INT_SCALAR(out, "/YD/YDPN/D1PNXX", numBnds);
    this->OutputMechanicalBoundaryProperty(out, QString("localAxisXX"));
    PRINT_INT_SCALAR(out, "/YD/YDPN/D1PNXY", numBnds);
    this->OutputMechanicalBoundaryProperty(out, QString("localAxisXY"));
    PRINT_INT_SCALAR(out, "/YD/YDPN/D1PNYX", numBnds);
    this->OutputMechanicalBoundaryProperty(out, QString("localAxisYX"));
    PRINT_INT_SCALAR(out, "/YD/YDPN/D1PNYY", numBnds);
    this->OutputMechanicalBoundaryProperty(out, QString("localAxisYY"));

    out << reset <<endl;
}

void FDEMDocumentExporter::OutputMechanicalBoundaryProperty(QTextStream &out, QString arg)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<BoundaryCollection> collection = BoundaryCollection::GetBoundaryCollection();
    QStringList boundaryIds = collection->GetMechanicalBoundaryList();
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        MechanicalBoundary *boundary = static_cast<MechanicalBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        if(arg == QString("loadStep"))
        {
            out << qSetFieldWidth(field) << left << boundary->loadStep - 1 << reset;
        }
        else if(arg == QString("nodeSetId"))
        {
            out << qSetFieldWidth(field) << left << boundary->nodeSetId - 1 << reset;
        }
        else if(arg == QString("loadingTypeX"))
        {
            out << qSetFieldWidth(field) << left << boundary->loadingTypeX << reset;
        }
        else if(arg == QString("loadingTypeY"))
        {
            out << qSetFieldWidth(field) << left << boundary->loadingTypeY << reset;
        }
        else if(arg == QString("amplitudeX"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << boundary->amplitudeX << reset;
        }
        else if(arg == QString("amplitudeY"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << boundary->amplitudeY << reset;
        }
        else if(arg == QString("contactBnd"))
        {
            out << qSetFieldWidth(field) << left << boundary->isContactBoundaryEnabled << reset;
        }
        else if(arg == QString("viscousBnd"))
        {
            out << qSetFieldWidth(field) << left << boundary->artificialBoundary << reset;
        }
        else if(arg == QString("localAxisXX"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << boundary->localAxisXX << reset;
        }
        else if(arg == QString("localAxisXY"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << boundary->localAxisXY << reset;
        }
        else if(arg == QString("localAxisYX"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << boundary->localAxisYX << reset;
        }
        else if(arg == QString("localAxisYY"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << boundary->localAxisYY << reset;
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputMechanicalBoundarySpatialFactor(QTextStream &out)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<BoundaryCollection> collection = BoundaryCollection::GetBoundaryCollection();
    QStringList boundaryIds = collection->GetMechanicalBoundaryList();

    PRINT_INT_SCALAR(out, "/YD/YDPN/I1SFHP", boundaryIds.size());
    int offset = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        MechanicalBoundary *boundary = static_cast<MechanicalBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        out << qSetFieldWidth(field) << left << offset << reset;
        int size = boundary->isAmplitudeSiteDependent ? boundary->spatialSample : 0;
        offset += size;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPN/I1SFSZ", boundaryIds.size());
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        MechanicalBoundary *boundary = static_cast<MechanicalBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        int size = boundary->isAmplitudeSiteDependent ? boundary->spatialSample : 0;
        out << qSetFieldWidth(field) << left << size << reset;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPN/MSFACT", offset);
    PRINT_INT_SCALAR(out, "/YD/YDPN/D1PNSX", offset);
    int index = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        MechanicalBoundary *boundary = static_cast<MechanicalBoundary*>(collection->GetBoundary(id).data());

        if(boundary->isAmplitudeSiteDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < boundary->spatialSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << boundary->spatialFactor[i][0] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPN/D1PNSF", offset);
    index = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        MechanicalBoundary *boundary = static_cast<MechanicalBoundary*>(collection->GetBoundary(id).data());

        if(boundary->isAmplitudeSiteDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < boundary->spatialSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << boundary->spatialFactor[i][1] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;
}

void FDEMDocumentExporter::OutputMechanicalBoundaryTemporalFactor(QTextStream &out)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<BoundaryCollection> collection = BoundaryCollection::GetBoundaryCollection();
    QStringList boundaryIds = collection->GetMechanicalBoundaryList();

    PRINT_INT_SCALAR(out, "/YD/YDPN/I1TFHP", boundaryIds.size());
    int offset = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        MechanicalBoundary *boundary = static_cast<MechanicalBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        out << qSetFieldWidth(field) << left << offset << reset;
        offset += (boundary->isAmplitudeTimeDependent ? boundary->temporalSample : 0);
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPN/I1TFSZ", boundaryIds.size());
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        MechanicalBoundary *boundary = static_cast<MechanicalBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        int size = boundary->isAmplitudeTimeDependent ? boundary->temporalSample : 0;
        out << qSetFieldWidth(field) << left << size << reset;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPN/MTFACT", offset);
    PRINT_INT_SCALAR(out, "/YD/YDPN/D1PNTT", offset);
    int index = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        MechanicalBoundary *boundary = static_cast<MechanicalBoundary*>(collection->GetBoundary(id).data());

        if(boundary->isAmplitudeTimeDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < boundary->temporalSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << boundary->temporalFactor[i][0] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPN/D1PNTF", offset);
    index = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        MechanicalBoundary *boundary = static_cast<MechanicalBoundary*>(collection->GetBoundary(id).data());

        if(boundary->isAmplitudeTimeDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < boundary->temporalSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << boundary->temporalFactor[i][1] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;
}

void FDEMDocumentExporter::OutputHydraulicalBoundary(QTextStream &out)
{
    QString bndNames = QString("/* (0)Pressure (1)Flow_rate */");
    QSharedPointer<BoundaryCollection> collection = BoundaryCollection::GetBoundaryCollection();
    int numBnds = collection->GetHydraulicBoundaryList().size();
    if(0 == numBnds) return;

    PRINT_STR_COMMENT(out, "/* Hydraulic Boundary */");
    PRINT_INT_SCALAR(out, "/YD/YDPH/MPROP", numBnds);
    PRINT_INT_SCALAR(out, "/YD/YDPH/NPROP", numBnds);

    PRINT_INT_SCALAR(out, "/YD/YDPH/I1PHST", numBnds);
    this->OutputHydraulicBoundaryProperty(out, QString("loadStep"));
    PRINT_INT_SCALAR(out, "/YD/YDPH/I1PHSN", numBnds);
    this->OutputHydraulicBoundaryProperty(out, QString("nodeSetId"));
    PRINT_STR_COMMENT(out, bndNames);
    PRINT_INT_SCALAR(out, "/YD/YDPH/I1PHTY", numBnds);
    this->OutputHydraulicBoundaryProperty(out, QString("loadingTypeX"));
    PRINT_INT_SCALAR(out, "/YD/YDPH/D1PHAX", numBnds);
    this->OutputHydraulicBoundaryProperty(out, QString("amplitudeX"));

    if(collection->IsHydraulicBoundarySiteDependent())
    {
        this->OutputHydraulicBoundarySpatialFactor(out);
    }

    if(collection->IsHydraulicBoundaryTimeDependent())
    {
        this->OutputHydraulicBoundaryTemporalFactor(out);
    }

    out << reset <<endl;
}

void FDEMDocumentExporter::OutputHydraulicBoundaryProperty(QTextStream &out, QString arg)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<BoundaryCollection> collection = BoundaryCollection::GetBoundaryCollection();
    QStringList boundaryIds = collection->GetHydraulicBoundaryList();
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        HydraulicBoundary *boundary = static_cast<HydraulicBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        if(arg == QString("loadStep"))
        {
            out << qSetFieldWidth(field) << left << boundary->loadStep - 1 << reset;
        }
        else if(arg == QString("nodeSetId"))
        {
            out << qSetFieldWidth(field) << left << boundary->nodeSetId - 1 << reset;
        }
        else if(arg == QString("loadingTypeX"))
        {
            out << qSetFieldWidth(field) << left << boundary->loadingTypeX << reset;
        }
        else if(arg == QString("amplitudeX"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << boundary->amplitudeX << reset;
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputHydraulicBoundarySpatialFactor(QTextStream &out)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<BoundaryCollection> collection = BoundaryCollection::GetBoundaryCollection();
    QStringList boundaryIds = collection->GetHydraulicBoundaryList();

    PRINT_INT_SCALAR(out, "/YD/YDPH/I1SFHP", boundaryIds.size());
    int offset = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        HydraulicBoundary *boundary = static_cast<HydraulicBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        out << qSetFieldWidth(field) << left << offset << reset;
        int size = boundary->isAmplitudeSiteDependent ? boundary->spatialSample : 0;
        offset += size;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPH/I1SFSZ", boundaryIds.size());
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        HydraulicBoundary *boundary = static_cast<HydraulicBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        int size = boundary->isAmplitudeSiteDependent ? boundary->spatialSample : 0;
        out << qSetFieldWidth(field) << left << size << reset;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPH/MSFACT", offset);
    PRINT_INT_SCALAR(out, "/YD/YDPH/D1PHSX", offset);
    int index = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        HydraulicBoundary *boundary = static_cast<HydraulicBoundary*>(collection->GetBoundary(id).data());

        if(boundary->isAmplitudeSiteDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < boundary->spatialSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << boundary->spatialFactor[i][0] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPH/D1PHSF", offset);
    index = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        HydraulicBoundary *boundary = static_cast<HydraulicBoundary*>(collection->GetBoundary(id).data());

        if(boundary->isAmplitudeSiteDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < boundary->spatialSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << boundary->spatialFactor[i][1] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;
}

void FDEMDocumentExporter::OutputHydraulicBoundaryTemporalFactor(QTextStream &out)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<BoundaryCollection> collection = BoundaryCollection::GetBoundaryCollection();
    QStringList boundaryIds = collection->GetHydraulicBoundaryList();

    PRINT_INT_SCALAR(out, "/YD/YDPH/I1TFHP", boundaryIds.size());
    int offset = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        HydraulicBoundary *boundary = static_cast<HydraulicBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        out << qSetFieldWidth(field) << left << offset << reset;
        offset += (boundary->isAmplitudeTimeDependent ? boundary->temporalSample : 0);
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPH/I1TFSZ", boundaryIds.size());
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        HydraulicBoundary *boundary = static_cast<HydraulicBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        int size = boundary->isAmplitudeTimeDependent ? boundary->temporalSample : 0;
        out << qSetFieldWidth(field) << left << size << reset;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPH/MTFACT", offset);
    PRINT_INT_SCALAR(out, "/YD/YDPH/D1PHTT", offset);
    int index = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        HydraulicBoundary *boundary = static_cast<HydraulicBoundary*>(collection->GetBoundary(id).data());

        if(boundary->isAmplitudeTimeDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < boundary->temporalSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << boundary->temporalFactor[i][0] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPH/D1PHTF", offset);
    index = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        HydraulicBoundary *boundary = static_cast<HydraulicBoundary*>(collection->GetBoundary(id).data());

        if(boundary->isAmplitudeTimeDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < boundary->temporalSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << boundary->temporalFactor[i][1] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;
}

void FDEMDocumentExporter::OutputThermalBoundary(QTextStream &out)
{
    QString bndNames = QString("/* (0)Temperature (1)Heat_flux (2)Heat_convection */");
    QSharedPointer<BoundaryCollection> collection = BoundaryCollection::GetBoundaryCollection();
    int numBnds = collection->GetThermalBoundaryList().size();
    if(0 == numBnds) return;

    PRINT_STR_COMMENT(out, "/* Thermal Boundary */");
    PRINT_INT_SCALAR(out, "/YD/YDPT/MPROP", numBnds);
    PRINT_INT_SCALAR(out, "/YD/YDPT/NPROP", numBnds);

    PRINT_INT_SCALAR(out, "/YD/YDPT/I1PTST", numBnds);
    this->OutputThermalBoundaryProperty(out, QString("loadStep"));
    PRINT_INT_SCALAR(out, "/YD/YDPT/I1PTSN", numBnds);
    this->OutputThermalBoundaryProperty(out, QString("nodeSetId"));
    PRINT_STR_COMMENT(out, bndNames);
    PRINT_INT_SCALAR(out, "/YD/YDPT/I1PTTY", numBnds);
    this->OutputThermalBoundaryProperty(out, QString("loadingTypeX"));
    PRINT_INT_SCALAR(out, "/YD/YDPT/D1PTAX", numBnds);
    this->OutputThermalBoundaryProperty(out, QString("amplitudeX"));

    if(collection->IsThermalBoundarySiteDependent())
    {
        this->OutputThermalBoundarySpatialFactor(out);
    }

    if(collection->IsThermalBoundaryTimeDependent())
    {
        this->OutputThermalBoundaryTemporalFactor(out);
    }

    out << reset <<endl;
}

void FDEMDocumentExporter::OutputThermalBoundaryProperty(QTextStream &out, QString arg)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<BoundaryCollection> collection = BoundaryCollection::GetBoundaryCollection();
    QStringList boundaryIds = collection->GetThermalBoundaryList();
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        HydraulicBoundary *boundary = static_cast<HydraulicBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        if(arg == QString("loadStep"))
        {
            out << qSetFieldWidth(field) << left << boundary->loadStep - 1 << reset;
        }
        else if(arg == QString("nodeSetId"))
        {
            out << qSetFieldWidth(field) << left << boundary->nodeSetId - 1 << reset;
        }
        else if(arg == QString("loadingTypeX"))
        {
            out << qSetFieldWidth(field) << left << boundary->loadingTypeX << reset;
        }
        else if(arg == QString("amplitudeX"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << boundary->amplitudeX << reset;
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputThermalBoundarySpatialFactor(QTextStream &out)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<BoundaryCollection> collection = BoundaryCollection::GetBoundaryCollection();
    QStringList boundaryIds = collection->GetThermalBoundaryList();

    PRINT_INT_SCALAR(out, "/YD/YDPT/I1SFHP", boundaryIds.size());
    int offset = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        ThermalBoundary *boundary = static_cast<ThermalBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        out << qSetFieldWidth(field) << left << offset << reset;
        int size = boundary->isAmplitudeSiteDependent ? boundary->spatialSample : 0;
        offset += size;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPT/I1SFSZ", boundaryIds.size());
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        ThermalBoundary *boundary = static_cast<ThermalBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        int size = boundary->isAmplitudeSiteDependent ? boundary->spatialSample : 0;
        out << qSetFieldWidth(field) << left << size << reset;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPT/MSFACT", offset);
    PRINT_INT_SCALAR(out, "/YD/YDPT/D1PTSX", offset);
    int index = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        ThermalBoundary *boundary = static_cast<ThermalBoundary*>(collection->GetBoundary(id).data());

        if(boundary->isAmplitudeSiteDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < boundary->spatialSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << boundary->spatialFactor[i][0] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPT/D1PTSF", offset);
    index = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        ThermalBoundary *boundary = static_cast<ThermalBoundary*>(collection->GetBoundary(id).data());

        if(boundary->isAmplitudeSiteDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < boundary->spatialSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << boundary->spatialFactor[i][1] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;
}

void FDEMDocumentExporter::OutputThermalBoundaryTemporalFactor(QTextStream &out)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<BoundaryCollection> collection = BoundaryCollection::GetBoundaryCollection();
    QStringList boundaryIds = collection->GetThermalBoundaryList();

    PRINT_INT_SCALAR(out, "/YD/YDPT/I1TFHP", boundaryIds.size());
    int offset = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        ThermalBoundary *boundary = static_cast<ThermalBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        out << qSetFieldWidth(field) << left << offset << reset;
        offset += (boundary->isAmplitudeTimeDependent ? boundary->temporalSample : 0);
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPT/I1TFSZ", boundaryIds.size());
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        ThermalBoundary *boundary = static_cast<ThermalBoundary*>(collection->GetBoundary(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        int size = boundary->isAmplitudeTimeDependent ? boundary->temporalSample : 0;
        out << qSetFieldWidth(field) << left << size << reset;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPT/MTFACT", offset);
    PRINT_INT_SCALAR(out, "/YD/YDPT/D1PTTT", offset);
    int index = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        ThermalBoundary *boundary = static_cast<ThermalBoundary*>(collection->GetBoundary(id).data());

        if(boundary->isAmplitudeTimeDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < boundary->temporalSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << boundary->temporalFactor[i][0] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDPT/D1PTTF", offset);
    index = 0;
    for(int i = 0; i < boundaryIds.size(); i++)
    {
        QString id = boundaryIds[i];
        ThermalBoundary *boundary = static_cast<ThermalBoundary*>(collection->GetBoundary(id).data());

        if(boundary->isAmplitudeTimeDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < boundary->temporalSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << boundary->temporalFactor[i][1] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;
}

void FDEMDocumentExporter::OutputInitialStressStrain(QTextStream &out)
{
    QSharedPointer<TemporalCollection> collection = TemporalCollection::GetTemporalCollection();
    int numTemps = collection->GetIniStressTemporalList().size();
    if(!numTemps) return;

    int field = 15;

    PRINT_STR_COMMENT(out, "/* Initial Insitu-Stress */");
    QString id = collection->GetIniStressTemporalList().front();
    TemporalStressStrain *temporal = static_cast<TemporalStressStrain*>(collection->GetTemporal(id).data());

    PRINT_INT_SCALAR(out, "/YD/YDIS/IISTY", temporal->initialField);
    PRINT_DBL_SCALAR(out, "/YD/YDIS/DISXX", temporal->amplitudeXX);
    PRINT_DBL_SCALAR(out, "/YD/YDIS/DISYY", temporal->amplitudeYY);
    PRINT_DBL_SCALAR(out, "/YD/YDIS/DISXY", temporal->amplitudeXY);

    if(temporal->isAmplitudeSiteDependent)
    {
        PRINT_INT_SCALAR(out, "/YD/YDIS/D1REFP", 3);
        out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[0][0];
        out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[0][1];
        out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[0][2];
        out << reset << endl;

        PRINT_INT_VECTOR(out, "/YD/YDIS/D2GRAD", 12, 2, 3);
        out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[1][0];
        out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[1][1];
        out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[1][2];
        out << reset << endl;

        out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[2][0];
        out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[2][1];
        out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[2][2];
        out << reset << endl;
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputMechanicalTemporal(QTextStream &out)
{
    QSharedPointer<TemporalCollection> collection = TemporalCollection::GetTemporalCollection();
    int numTemps = collection->GetMechanicalTemporalList().size();
    if(!numTemps) return;

    PRINT_STR_COMMENT(out, "/* Mechanical Temporal */");
    PRINT_INT_SCALAR(out, "/YD/YDIN/MPROP", numTemps);
    PRINT_INT_SCALAR(out, "/YD/YDIN/NPROP", numTemps);

    PRINT_INT_SCALAR(out, "/YD/YDIN/I1INSN", numTemps);
    this->OutputMechanicalTemporalProperty(out, QString("nodeSetId"));
    PRINT_INT_SCALAR(out, "/YD/YDIN/I1INTY", numTemps);
    this->OutputMechanicalTemporalProperty(out, QString("fieldType"));
    PRINT_INT_SCALAR(out, "/YD/YDIN/D1INAX", numTemps);
    this->OutputMechanicalTemporalProperty(out, QString("amplitudeX"));
    PRINT_INT_SCALAR(out, "/YD/YDIN/D1INAY", numTemps);
    this->OutputMechanicalTemporalProperty(out, QString("amplitudeY"));

    if(collection->IsMechanicalTemporalSiteDependent())
    {
        this->OutputMechanicalTemporalSpatialFactor(out);
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputMechanicalTemporalProperty(QTextStream &out, QString arg)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<TemporalCollection> collection = TemporalCollection::GetTemporalCollection();
    QStringList temporalIds = collection->GetMechanicalTemporalList();
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        MechanicalTemporal *temporal = static_cast<MechanicalTemporal*>(collection->GetTemporal(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        if(arg == QString("nodeSetId"))
        {
            out << qSetFieldWidth(field) << left << temporal->nodeSetId - 1 << reset;
        }
        else if(arg == QString("fieldType"))
        {
            out << qSetFieldWidth(field) << left << temporal->initialField << reset;
        }
        else if(arg == QString("amplitudeX"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << temporal->amplitudeX << reset;
        }
        else if(arg == QString("amplitudeY"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << temporal->amplitudeY << reset;
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputMechanicalTemporalSpatialFactor(QTextStream &out)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<TemporalCollection> collection = TemporalCollection::GetTemporalCollection();
    QStringList temporalIds = collection->GetMechanicalTemporalList();

    PRINT_INT_SCALAR(out, "/YD/YDIN/I1SFHP", temporalIds.size());
    int offset = 0;
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        MechanicalTemporal *temporal = static_cast<MechanicalTemporal*>(collection->GetTemporal(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        out << qSetFieldWidth(field) << left << offset << reset;
        int size = temporal->isAmplitudeSiteDependent ? temporal->spatialSample : 0;
        offset += size;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDIN/I1SFSZ", temporalIds.size());
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        MechanicalTemporal *temporal = static_cast<MechanicalTemporal*>(collection->GetTemporal(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        int size = temporal->isAmplitudeSiteDependent ? temporal->spatialSample : 0;
        out << qSetFieldWidth(field) << left << size << reset;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDIN/MSFACT", offset);
    PRINT_INT_SCALAR(out, "/YD/YDIN/D1INSX", offset);
    int index = 0;
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        MechanicalTemporal *temporal = static_cast<MechanicalTemporal*>(collection->GetTemporal(id).data());

        if(temporal->isAmplitudeSiteDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < temporal->spatialSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[i][0] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDIN/D1INSF", offset);
    index = 0;
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        MechanicalTemporal *temporal = static_cast<MechanicalTemporal*>(collection->GetTemporal(id).data());

        if(temporal->isAmplitudeSiteDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < temporal->spatialSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[i][1] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;
}

void FDEMDocumentExporter::OutputHydraulicTemporal(QTextStream &out)
{
    QSharedPointer<TemporalCollection> collection = TemporalCollection::GetTemporalCollection();
    int numTemps = collection->GetHydraulicTemporalList().size();
    if(!numTemps) return;

    PRINT_STR_COMMENT(out, "/* Hydraulic Temporal */");
    PRINT_INT_SCALAR(out, "/YD/YDIH/MPROP", numTemps);
    PRINT_INT_SCALAR(out, "/YD/YDIH/NPROP", numTemps);

    PRINT_INT_SCALAR(out, "/YD/YDIH/I1IHSN", numTemps);
    this->OutputHydraulicTemporalProperty(out, QString("nodeSetId"));
    PRINT_INT_SCALAR(out, "/YD/YDIH/I1IHTY", numTemps);
    this->OutputHydraulicTemporalProperty(out, QString("fieldType"));
    PRINT_INT_SCALAR(out, "/YD/YDIH/D1IHAX", numTemps);
    this->OutputHydraulicTemporalProperty(out, QString("amplitudeX"));

    if(collection->IsHydraulicTemporalSiteDependent())
    {
        this->OutputHydraulicTemporalSpatialFactor(out);
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputHydraulicTemporalProperty(QTextStream &out, QString arg)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<TemporalCollection> collection = TemporalCollection::GetTemporalCollection();
    QStringList temporalIds = collection->GetHydraulicTemporalList();
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        HydraulicTemporal *temporal = static_cast<HydraulicTemporal*>(collection->GetTemporal(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        if(arg == QString("nodeSetId"))
        {
            out << qSetFieldWidth(field) << left << temporal->nodeSetId - 1 << reset;
        }
        else if(arg == QString("fieldType"))
        {
            out << qSetFieldWidth(field) << left << temporal->initialField << reset;
        }
        else if(arg == QString("amplitudeX"))
        {
            out << qSetFieldWidth(15) << left << forcepoint << temporal->amplitudeX << reset;
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputHydraulicTemporalSpatialFactor(QTextStream &out)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<TemporalCollection> collection = TemporalCollection::GetTemporalCollection();
    QStringList temporalIds = collection->GetHydraulicTemporalList();

    PRINT_INT_SCALAR(out, "/YD/YDIH/I1SFHP", temporalIds.size());
    int offset = 0;
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        HydraulicTemporal *temporal = static_cast<HydraulicTemporal*>(collection->GetTemporal(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        out << qSetFieldWidth(field) << left << offset << reset;
        int size = temporal->isAmplitudeSiteDependent ? temporal->spatialSample : 0;
        offset += size;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDIH/I1SFSZ", temporalIds.size());
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        HydraulicTemporal *temporal = static_cast<HydraulicTemporal*>(collection->GetTemporal(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        int size = temporal->isAmplitudeSiteDependent ? temporal->spatialSample : 0;
        out << qSetFieldWidth(field) << left << size << reset;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDIH/MSFACT", offset);
    PRINT_INT_SCALAR(out, "/YD/YDIH/D1IHSX", offset);
    int index = 0;
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        HydraulicTemporal *temporal = static_cast<HydraulicTemporal*>(collection->GetTemporal(id).data());

        if(temporal->isAmplitudeSiteDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < temporal->spatialSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[i][0] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDIH/D1IHSF", offset);
    index = 0;
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        HydraulicTemporal *temporal = static_cast<HydraulicTemporal*>(collection->GetTemporal(id).data());

        if(temporal->isAmplitudeSiteDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < temporal->spatialSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[i][1] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;
}

void FDEMDocumentExporter::OutputThermalTemporal(QTextStream &out)
{
    QSharedPointer<TemporalCollection> collection = TemporalCollection::GetTemporalCollection();
    int numTemps = collection->GetThermalTemporalList().size();
    if(!numTemps) return;

    PRINT_STR_COMMENT(out, "/* Thermal Temporal */");
    PRINT_INT_SCALAR(out, "/YD/YDIT/MPROP", numTemps);
    PRINT_INT_SCALAR(out, "/YD/YDIT/NPROP", numTemps);

    PRINT_INT_SCALAR(out, "/YD/YDIT/I1ITSN", numTemps);
    this->OutputThermalTemporalProperty(out, QString("nodeSetId"));
    PRINT_INT_SCALAR(out, "/YD/YDIT/I1ITTY", numTemps);
    this->OutputThermalTemporalProperty(out, QString("fieldType"));
    PRINT_INT_SCALAR(out, "/YD/YDIT/D1ITAX", numTemps);
    this->OutputThermalTemporalProperty(out, QString("amplitudeX"));

    if(collection->IsThermalTemporalSiteDependent())
    {
        this->OutputThermalTemporalSpatialFactor(out);
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputThermalTemporalProperty(QTextStream &out, QString arg)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<TemporalCollection> collection = TemporalCollection::GetTemporalCollection();
    QStringList temporalIds = collection->GetThermalTemporalList();
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        ThermalTemporal *temporal = static_cast<ThermalTemporal*>(collection->GetTemporal(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        if(arg == QString("nodeSetId"))
        {
            out << qSetFieldWidth(field) << left << temporal->nodeSetId - 1 << reset;
        }
        else if(arg == QString("fieldType"))
        {
            out << qSetFieldWidth(field) << left << temporal->initialField << reset;
        }
        else if(arg == QString("amplitudeX"))
        {
            out << qSetFieldWidth(15) << left << forcepoint << temporal->amplitudeX << reset;
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputThermalTemporalSpatialFactor(QTextStream &out)
{
    int ncols = 10;
    int field = 15;

    QSharedPointer<TemporalCollection> collection = TemporalCollection::GetTemporalCollection();
    QStringList temporalIds = collection->GetThermalTemporalList();

    PRINT_INT_SCALAR(out, "/YD/YDIT/I1SFHP", temporalIds.size());
    int offset = 0;
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        ThermalTemporal *temporal = static_cast<ThermalTemporal*>(collection->GetTemporal(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        out << qSetFieldWidth(field) << left << offset << reset;
        int size = temporal->isAmplitudeSiteDependent ? temporal->spatialSample : 0;
        offset += size;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDIT/I1SFSZ", temporalIds.size());
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        ThermalTemporal *temporal = static_cast<ThermalTemporal*>(collection->GetTemporal(id).data());

        if((i != 0) && (i % ncols == 0))
            out << reset << endl;

        int size = temporal->isAmplitudeSiteDependent ? temporal->spatialSample : 0;
        out << qSetFieldWidth(field) << left << size << reset;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDIT/MSFACT", offset);
    PRINT_INT_SCALAR(out, "/YD/YDIT/D1ITSX", offset);
    int index = 0;
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        ThermalTemporal *temporal = static_cast<ThermalTemporal*>(collection->GetTemporal(id).data());

        if(temporal->isAmplitudeSiteDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < temporal->spatialSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[i][0] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDIT/D1ITSF", offset);
    index = 0;
    for(int i = 0; i < temporalIds.size(); i++)
    {
        QString id = temporalIds[i];
        ThermalTemporal *temporal = static_cast<ThermalTemporal*>(collection->GetTemporal(id).data());

        if(temporal->isAmplitudeSiteDependent)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            for(int i = 0; i < temporal->spatialSample; i++)
            {
                out << qSetFieldWidth(field) << left << forcepoint << temporal->spatialFactor[i][1] << reset;
                index += 1;
            }
        }
    }
    out << reset << endl;
}

void FDEMDocumentExporter::WriteHistoryInformation()
{
    QSharedPointer<HistoryCollection> collection = HistoryCollection::GetHistoryCollection();
    int numHists = collection->GetHistotyMap().size();
    if(0 == numHists) return;

    PRINT_STR_COMMENT(out, "/* HISTORY PROPERTY */");
    PRINT_INT_SCALAR(out, "/YD/YDO/MOHYS", numHists);
    PRINT_INT_SCALAR(out, "/YD/YDO/NOHYS", numHists);
    PRINT_INT_SCALAR(out, "/YD/YDO/I1OHTY", numHists);
    this->OutputHistoryProperty(out, QString("historyType"));
    PRINT_INT_SCALAR(out, "/YD/YDO/I1OHXY", numHists);
    this->OutputHistoryProperty(out, QString("histVariable"));
    PRINT_INT_SCALAR(out, "/YD/YDO/I1OHOP", numHists);
    this->OutputHistoryProperty(out, QString("histOperation"));
    PRINT_INT_SCALAR(out, "/YD/YDO/I1OHFQ", numHists);
    this->OutputHistoryProperty(out, QString("histFrequency"));
    PRINT_INT_SCALAR(out, "/YD/YDO/D1OHTF", numHists);
    this->OutputHistoryProperty(out, QString("timeScaleFactor"));
    PRINT_INT_SCALAR(out, "/YD/YDO/D1OHSF", numHists);
    this->OutputHistoryProperty(out, QString("histScaleFactor"));
    PRINT_INT_SCALAR(out, "/YD/YDO/I1OHSZ", numHists);
    this->OutputHistoryProperty(out, QString("histEntitySize"));
    this->OutputHistoryPedigreeIds(out);

    out << reset << endl;
}

void FDEMDocumentExporter::OutputHistoryProperty(QTextStream &out, QString arg)
{
    int index = 0;
    int ncols = 10;
    int field = 15;

    QSharedPointer<HistoryCollection> collection = HistoryCollection::GetHistoryCollection();
    HistoryCollection::HistoryMap history_map = collection->GetHistotyMap();
    for(auto iter = history_map.begin(); iter != history_map.end(); ++iter)
    {
        if((index != 0) && (index % ncols == 0))
            out << reset << endl;

        if(arg == QString("historyType"))
        {
            out << qSetFieldWidth(field) << left << iter.value()->historyType;
        }
        else if(arg == QString("histVariable"))
        {
            out << qSetFieldWidth(field) << left << iter.value()->histVariable;
        }
        else if(arg == QString("histOperation"))
        {
            out << qSetFieldWidth(field) << left << iter.value()->histOperation;
        }
        else if(arg == QString("histFrequency"))
        {
            out << qSetFieldWidth(field) << left << iter.value()->histFrequency;
        }
        else if(arg == QString("timeScaleFactor"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << iter.value()->timeScaleFactor;
        }
        else if(arg == QString("histScaleFactor"))
        {
            out << qSetFieldWidth(field) << left << forcepoint << iter.value()->histScaleFactor;
        }
        else if(arg == QString("histEntitySize"))
        {
            out << qSetFieldWidth(field) << left << iter.value()->pedigreeIds.size();
        }
    }

    out << reset << endl;
}

void FDEMDocumentExporter::OutputHistoryPedigreeIds(QTextStream &out)
{
    int index;
    int ncols = 10;
    int field = 15;

    QSharedPointer<HistoryCollection> collection = HistoryCollection::GetHistoryCollection();
    HistoryCollection::HistoryMap history_map = collection->GetHistotyMap();

    PRINT_INT_SCALAR(out, "/YD/YDO/I1OHOF", history_map.size());
    index = 0;
    int offset = 0;
    for(auto iter = history_map.begin(); iter != history_map.end(); ++iter)
    {
        if((index != 0) && (index % ncols == 0))
            out << reset << endl;

        out << qSetFieldWidth(field) << left << offset;
        offset += iter.value()->pedigreeIds.size();
        index += 1;
    }
    out << reset << endl;

    PRINT_INT_SCALAR(out, "/YD/YDO/I1OHID", offset);
    index = 0;
    for(auto iter = history_map.begin(); iter != history_map.end(); ++iter)
    {
        for (int i = 0; i < iter.value()->pedigreeIds.size(); i++)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            out << qSetFieldWidth(field) << left << iter.value()->pedigreeIds[i];
            index += 1;
        }
    }
    out << reset << endl;
}
