#ifndef MATERIALCOLLECTION_H
#define MATERIALCOLLECTION_H

#include <QMap>
#include <QMutex>
#include <QVector>
#include <QSharedPointer>
#include <QTextStream>
#include <QStringList>

class Material;
class MaterialCollection
{
public:
    typedef QMap<QString, QSharedPointer<Material>> MaterialMap;

public:
    ~MaterialCollection();

    static QSharedPointer<MaterialCollection> GetMaterialCollection();

    const MaterialMap& GetMaterialMap() const;

    QSharedPointer<Material> GetMaterial(const QString& id);

    void AppendMaterial(const QString& id);

    void RemoveMaterial(const QString& id);

    void UpdateMaterial(const QString& id, QSharedPointer<Material> material);

    void Clear();

private:
    void ReshuffleMaterialId();

    QStringList GetTriangleMaterialIds() const;

    QStringList GetCohesiveMaterialIds() const;

    QStringList GetNTSFluidMaterialIds() const;


private:
    MaterialCollection();

private:
    bool IsTriangleElasticityEnabled();

    bool IsTrianglePlasticityEnabled();

    bool IsTriangleContactDampEnabled();

    bool IsTriangleMechanicalPropertyInvolved();

    bool IsTriangleHydraulicPropertyInvolved();

    bool IsTriangleThermalPropertyInvolved();

    bool IsTriangleMechanicalPropertyInhomoEnabled();

    bool IsTriangleHydraulicPropertyInhomoEnabled();

    bool IsTriangleThermalPropertyInhomoEnabled();

private:
    bool IsCohesiveMechanicalPropertyInvolved();

    bool IsCohesiveHydraulicPropertyInvolved();

    bool IsCohesiveThermalPropertyInvolved();

    bool IsCohesiveMechanicalPropertyInhomoEnabled();

    bool IsCohesiveHydraulicPropertyInhomoEnabled();

    bool IsCohesiveThermalPropertyInhomoEnabled();

public:
    friend class FDEMDocumentExporter;

    size_t GetNumberOfTriangleElemSets();

    size_t GetNumberOfCohesiveElemSets();

private:
    static QMutex mutex;
    static QSharedPointer<MaterialCollection> materialCollection;

    MaterialMap material_map;

    QStringList triMaterialIds;
    QStringList cohMaterialIds;
    QStringList ntfMaterialIds;
};

#endif // MATERIALCOLLECTION_H
