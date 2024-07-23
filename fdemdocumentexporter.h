#ifndef FDEMDOCUMENTEXPORTER_H
#define FDEMDOCUMENTEXPORTER_H

#include <QVector>
#include <QTextStream>

class FDEMDocumentExporter
{
public:
    FDEMDocumentExporter(const QString& text);

    QString GetFileName();

    void Write();

protected:
    void WriteHeaderInformation();

    void WriteSolutionInformation();

    void WriteElementInformation();

    void WriteNodeInformation();

    void WriteGroupInformation();

    void WriteNodeSetInformation();

    void WriteElemSetInformation();

    void WriteContactInformation();

    void WriteMaterialInformation();

    void WriteBoundaryInformation();

    void WriteTemporalInformation();

    void WriteHistoryInformation();

    void WriteClosingInformation();

private:
    void OutputCellGrainId(QTextStream& out, int celltype, size_t &count);

    void OutputCellProperty(QTextStream& out, int celltype, size_t &count);

    void OutputCellTopology(QTextStream& out, int celltype);

private:
    void OutputNodeCoordinate(QTextStream& out);

    void OutputNodalBoundaryFeature(QTextStream& out);

    void OutputMechanicalBoundaryId(QTextStream& out);

    void OutputMechanicalTemporalId(QTextStream& out);

    void OutputTemporalVelocity(QTextStream& out);

    void OutputHydraulicalBoundaryId(QTextStream& out);

    void OutputHydraulicalTemporalId(QTextStream& out);

    void OutputTemporalPressure(QTextStream& out);

    void OutputThermalBoundaryId(QTextStream& out);

    void OutputThermalTemporalId(QTextStream& out);

    void OutputTemporalTemperature(QTextStream& out);

private:
    // Solid Triangle
    void OutputTriangleMaterial(QTextStream& out);

    void OutputTriangleMaterialElemSets(QTextStream& out);

    void OutputTriangleMechanicalProperty(QTextStream& out, QString arg);

    void OutputTriangleContactProperty(QTextStream& out, QString arg);

    void OutputTriangleHydraulicProperty(QTextStream& out, QString arg);

    void OutputTriangleThermalProperty(QTextStream& out, QString arg);

    // Cohesive Quad
    void OutputCohesiveMaterial(QTextStream& out);

    void OutputCohesiveMaterialElemSets(QTextStream& out);

    void OutputCohesiveMechanicalProperty(QTextStream& out, QString arg);

    void OutputCohesiveHydraulicProperty(QTextStream& out, QString arg);

    void OutputCohesiveThermalProperty(QTextStream& out, QString arg);

    // Newton Fluid
    void OutputNTSFluidMaterial(QTextStream& out);

private:
    //Mechanical
    void OutputMechanicalBoundary(QTextStream& out);

    void OutputMechanicalBoundaryProperty(QTextStream& out, QString arg);

    void OutputMechanicalBoundarySpatialFactor(QTextStream& out);

    void OutputMechanicalBoundaryTemporalFactor(QTextStream& out);

    //Hydraulic
    void OutputHydraulicalBoundary(QTextStream& out);

    void OutputHydraulicBoundaryProperty(QTextStream& out, QString arg);

    void OutputHydraulicBoundarySpatialFactor(QTextStream& out);

    void OutputHydraulicBoundaryTemporalFactor(QTextStream& out);

    //Thermal
    void OutputThermalBoundary(QTextStream& out);

    void OutputThermalBoundaryProperty(QTextStream& out, QString arg);

    void OutputThermalBoundarySpatialFactor(QTextStream& out);

    void OutputThermalBoundaryTemporalFactor(QTextStream& out);

private:
    //Mechanical
    void OutputMechanicalTemporal(QTextStream& out);

    void OutputMechanicalTemporalProperty(QTextStream& out, QString arg);

    void OutputMechanicalTemporalSpatialFactor(QTextStream& out);

    //Hydrauic
    void OutputHydraulicTemporal(QTextStream& out);

    void OutputHydraulicTemporalProperty(QTextStream& out, QString arg);

    void OutputHydraulicTemporalSpatialFactor(QTextStream& out);

    //Thermal
    void OutputThermalTemporal(QTextStream& out);

    void OutputThermalTemporalProperty(QTextStream& out, QString arg);

    void OutputThermalTemporalSpatialFactor(QTextStream& out);

    //Insitu Stress
    void OutputInitialStressStrain(QTextStream& out);

private:
    void OutputHistoryProperty(QTextStream& out, QString arg);

    void OutputHistoryPedigreeIds(QTextStream& out);

private:
    QString filename;
    QTextStream out;

    QVector<int> triangleMatIds;
    QVector<int> cohesiveMatIds;
    QVector<int> ntsfluidMatIds;

    QVector<int> mechanicalBndIds;
    QVector<int> hydraulicalBndIds;
    QVector<int> thermalBndIds;

//    QVector<int> mechanicalTempIds;
//    QVector<int> hydraulicalTempIds;
//    QVector<int> thermalTempIds;
//    QVector<int> stressStrainIds;
};

#endif // FDEMDOCUMENTEXPORTER_H
