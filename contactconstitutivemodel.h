#ifndef CONTACTCONSTITUTIVEMODEL_H
#define CONTACTCONSTITUTIVEMODEL_H

#include "constitutivemodel.h"

class ContactConstitutiveModel : public ConstitutiveModel
{
public:
    ContactConstitutiveModel();
    ~ContactConstitutiveModel() override;

public:
    friend class TriangleMaterial;
    friend class FDEMDocumentExporter;
    friend class FDEMSolutionWidget;

protected:
    double normalStiffness;
    double tangentStiffness;
    double contactFriction;
    double yieldStrength;
    double powerExponent;
};

class ContactElasticityModel : public ContactConstitutiveModel
{
public:
    ContactElasticityModel();
    ~ContactElasticityModel() override;

public:
    friend class ContactElasticityWidget;
};

class ContactPlasticityModel : public ContactConstitutiveModel
{
public:
    ContactPlasticityModel();
    ~ContactPlasticityModel() override;

public:
    friend class ContactPlasticityWidget;
};

#endif // CONTACTCONSTITUTIVEMODEL_H
