#include "materialcollection.h"
#include "material.h"
#include "solutioncollection.h"
#include "mechanicalconstitutivemodel.h"
#include "hydraulicconstitutivemodel.h"
#include "thermalconstitutivemodel.h"
#include "globalmacros.h"

#include <QDebug>


QMutex MaterialCollection::mutex;

QSharedPointer<MaterialCollection> MaterialCollection::materialCollection = nullptr;

MaterialCollection::~MaterialCollection()
{
    qDebug() << "~MaterialCollection()";
}

QSharedPointer<MaterialCollection> MaterialCollection::GetMaterialCollection()
{
    if(nullptr == materialCollection)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == materialCollection)
        {
            materialCollection = QSharedPointer<MaterialCollection>(new MaterialCollection);
        }
    }

    return materialCollection;
}

const MaterialCollection::MaterialMap &MaterialCollection::GetMaterialMap() const
{
    return this->material_map;
}

QSharedPointer<Material> MaterialCollection::GetMaterial(const QString &id)
{
    if(!this->material_map.contains(id))
    {
        this->AppendMaterial(id);
    }
    return this->material_map[id];
}

void MaterialCollection::AppendMaterial(const QString &id)
{
    QSharedPointer<Material> material = QSharedPointer<TriangleMaterial>(new TriangleMaterial);
    this->material_map.insert(id, material);
}

void MaterialCollection::RemoveMaterial(const QString &id)
{
    this->material_map.remove(id);
}

void MaterialCollection::UpdateMaterial(const QString &id, QSharedPointer<Material> material)
{
    if(this->material_map.contains(id))
    {
        this->material_map[id] = material;
    }
}

void MaterialCollection::Clear()
{
    this->material_map.clear();
}

MaterialCollection::MaterialCollection()
{
    qDebug() << "MaterialCollection()";
}

bool MaterialCollection::IsTriangleMechanicalPropertyInhomoEnabled()
{
    for(const QString id : triMaterialIds)
    {
        TriangleMaterial *material = static_cast<TriangleMaterial*>(this->material_map[id].data());
        TriangleMechanicalModel* model = static_cast<TriangleMechanicalModel*>(material->mechanicalModel.data());
        if(model->isPropertyInhomoEnabled) { return true; }
    }
    return false;
}

bool MaterialCollection::IsTriangleHydraulicPropertyInhomoEnabled()
{
    for(const QString id : triMaterialIds)
    {
        TriangleMaterial *material = static_cast<TriangleMaterial*>(this->material_map[id].data());
        TriangleHydraulicModel* model = static_cast<TriangleHydraulicModel*>(material->hydraulicModel.data());
        if(model->isPermeabilityInhomoEnabled) { return true; }
    }
    return false;
}

bool MaterialCollection::IsTriangleThermalPropertyInhomoEnabled()
{
    for(const QString id : triMaterialIds)
    {
        TriangleMaterial *material = static_cast<TriangleMaterial*>(this->material_map[id].data());
        TriangleThermalModel* model = static_cast<TriangleThermalModel*>(material->thermalModel.data());
        if(model->isConductivityInhomoEnabled) { return true; }
    }
    return false;
}

bool MaterialCollection::IsTriangleMechanicalPropertyInvolved()
{
    for(const QString id : triMaterialIds)
    {
        TriangleMaterial *material = static_cast<TriangleMaterial*>(this->material_map[id].data());
        if(material->isMechanicalAnalysisEnabled) { return true; }
    }
    return false;
}

bool MaterialCollection::IsTriangleHydraulicPropertyInvolved()
{
    for(const QString id : triMaterialIds)
    {
        TriangleMaterial *material = static_cast<TriangleMaterial*>(this->material_map[id].data());
        if(material->isHydraulicAnalysisEnabled) { return true; }
    }
    return false;
}

bool MaterialCollection::IsTriangleThermalPropertyInvolved()
{
    for(const QString id : triMaterialIds)
    {
        TriangleMaterial *material = static_cast<TriangleMaterial*>(this->material_map[id].data());
        if(material->isThermalAnalysisEnabled) { return true; }
    }
    return false;
}

bool MaterialCollection::IsTriangleElasticityEnabled()
{
    for(const QString id : triMaterialIds)
    {
        TriangleMaterial *material = static_cast<TriangleMaterial*>(this->material_map[id].data());
        if(material->mechanicalModelType) { return true; }
    }
    return false;
}

bool MaterialCollection::IsTrianglePlasticityEnabled()
{
    for(const QString id : triMaterialIds)
    {
        TriangleMaterial *material = static_cast<TriangleMaterial*>(this->material_map[id].data());
        if(3 == material->mechanicalModelType) { return true; }
    }
    return false;
}

bool MaterialCollection::IsTriangleContactDampEnabled()
{
    for(const QString id : triMaterialIds)
    {
        TriangleMaterial *material = static_cast<TriangleMaterial*>(this->material_map[id].data());
        if(material->contactModelType) { return true; }
    }
    return false;
}

bool MaterialCollection::IsCohesiveMechanicalPropertyInvolved()
{
    for(const QString id : cohMaterialIds)
    {
        CohesiveMaterial *material = static_cast<CohesiveMaterial*>(this->material_map[id].data());
        if(material->isMechanicalAnalysisEnabled) { return true; }
    }
    return false;
}

bool MaterialCollection::IsCohesiveHydraulicPropertyInvolved()
{
    for(const QString id : cohMaterialIds)
    {
        CohesiveMaterial *material = static_cast<CohesiveMaterial*>(this->material_map[id].data());
        if(material->isHydraulicAnalysisEnabled) { return true; }
    }
    return false;
}

bool MaterialCollection::IsCohesiveThermalPropertyInvolved()
{
    for(const QString id : cohMaterialIds)
    {
        CohesiveMaterial *material = static_cast<CohesiveMaterial*>(this->material_map[id].data());
        if(material->isThermalAnalysisEnabled) { return true; }
    }
    return false;
}

bool MaterialCollection::IsCohesiveMechanicalPropertyInhomoEnabled()
{
    for(const QString id : cohMaterialIds)
    {
        CohesiveMaterial *material = static_cast<CohesiveMaterial*>(this->material_map[id].data());
        CohesiveMechanicalModel* model = static_cast<CohesiveMechanicalModel*>(material->mechanicalModel.data());
        if(model->isPropertyInhomoEnabled) { return true; }
    }
    return false;
}

bool MaterialCollection::IsCohesiveHydraulicPropertyInhomoEnabled()
{
    for(const QString id : cohMaterialIds)
    {
        CohesiveMaterial *material = static_cast<CohesiveMaterial*>(this->material_map[id].data());
        CohesiveMechanicalModel* model = static_cast<CohesiveMechanicalModel*>(material->hydraulicModel.data());
        if(model->isPropertyInhomoEnabled) { return true; }
    }
    return false;
}

bool MaterialCollection::IsCohesiveThermalPropertyInhomoEnabled()
{
    for(const QString id : cohMaterialIds)
    {
        CohesiveMaterial *material = static_cast<CohesiveMaterial*>(this->material_map[id].data());
        CohesiveMechanicalModel* model = static_cast<CohesiveMechanicalModel*>(material->thermalModel.data());
        if(model->isPropertyInhomoEnabled) { return true; }
    }
    return false;
}

size_t MaterialCollection::GetNumberOfTriangleElemSets()
{
    size_t size = 0;
    for(QSharedPointer<Material> material : this->material_map)
    {
        if(Material::TRIANGLE == material->getMaterialType())
        {
            size += material->elementSet.split(QChar(','), QString::SkipEmptyParts).size();
        }
    }

    return size;
}

size_t MaterialCollection::GetNumberOfCohesiveElemSets()
{
    size_t size = 0;
    for(QSharedPointer<Material> material : this->material_map)
    {
        if(Material::COHESIVE == material->getMaterialType())
        {
            size += material->elementSet.split(QChar(','), QString::SkipEmptyParts).size();
        }
    }

    return size;
}

void MaterialCollection::ReshuffleMaterialId()
{
    if(!triMaterialIds.isEmpty()) triMaterialIds.clear();
    if(!cohMaterialIds.isEmpty()) cohMaterialIds.clear();
    if(!ntfMaterialIds.isEmpty()) ntfMaterialIds.clear();

    QSharedPointer<MaterialCollection> materials = MaterialCollection::GetMaterialCollection();
    MaterialCollection::MaterialMap material_map = materials->GetMaterialMap();

    for(auto iter = material_map.begin(); iter != material_map.end(); ++iter)
    {
        int etype = (iter.value())->getMaterialType();
        if(Material::TRIANGLE == etype)
        {
            this->triMaterialIds.push_back(iter.key());
        }
        else if(Material::COHESIVE == etype)
        {
            this->cohMaterialIds.push_back(iter.key());
        }
        else if(Material::NTSFLUID == etype)
        {
            this->ntfMaterialIds.push_back(iter.key());
        }
    }
}

QStringList MaterialCollection::GetTriangleMaterialIds() const
{
    return this->triMaterialIds;
}

QStringList MaterialCollection::GetCohesiveMaterialIds() const
{
    return this->cohMaterialIds;
}

QStringList MaterialCollection::GetNTSFluidMaterialIds() const
{
    return this->ntfMaterialIds;
}
