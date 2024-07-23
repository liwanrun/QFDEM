#include "constitutivemodelfactory.h"

#include "mechanicalconstitutivemodel.h"
#include "trianglerigidbodymotionwidget.h"
#include "trianglehookeanelasticitywidget.h"
#include "triangleanisotropicelasticitywidget.h"
#include "triangleelastoplasticitywidget.h"
#include "cohesiveexponentwidget.h"
#include "cohesivebilinearwidget.h"
#include "cohesivefracturewidget.h"

#include "contactconstitutivemodel.h"
#include "contactelasticitywidget.h"
#include "contactplasticitywidget.h"

#include "hydraulicconstitutivemodel.h"
#include "triangleisotropicseepagewidget.h"
#include "triangleanisotropicseepagewidget.h"
#include "cohesiveincompressibleflowwidget.h"
#include "cohesivecompressibleflowwidget.h"

#include "thermalconstitutivemodel.h"
#include "triangleisotropicheatconductwidget.h"
#include "triangleanisotropicheatconductwidget.h"
#include "cohesiveheatexchangewidget.h"


ConstitutiveModelFactory::ConstitutiveModelFactory()
{

}

QSharedPointer<ConstitutiveModel> ConstitutiveModelFactory::CreateMaterialModel(int type)
{
    QSharedPointer<ConstitutiveModel> model = nullptr;

    if(0 == type)
    {
        model = QSharedPointer<RigidBodyMotionModel>(new RigidBodyMotionModel);
    }
    else if(1 == type)
    {
        model = QSharedPointer<HookeanElasticityModel>(new HookeanElasticityModel);
    }
    else if(2 == type)
    {
        model = QSharedPointer<AnisotropicElasticityModel>(new AnisotropicElasticityModel);
    }
    else if(3 == type)
    {
        model = QSharedPointer<ElastoPlasticityModel>(new ElastoPlasticityModel);
    }
    else if(4 == type)  //Plane stress
    {
        model = QSharedPointer<HookeanElasticityModel>(new HookeanElasticityModel);
    }
    else if(5 == type)  //Plane strain
    {
        model = QSharedPointer<HookeanElasticityModel>(new HookeanElasticityModel);
    }

    return model;
}

QSharedPointer<ConstitutiveModel> ConstitutiveModelFactory::CreateContactModel(int type)
{
    QSharedPointer<ConstitutiveModel> model = nullptr;

    if(0 == type)
    {
        model = QSharedPointer<ContactElasticityModel>(new ContactElasticityModel);
    }
    else if(1 == type)
    {
        model = QSharedPointer<ContactPlasticityModel>(new ContactPlasticityModel);
    }

    return model;
}

QSharedPointer<ConstitutiveModel> ConstitutiveModelFactory::CreatePorositySeepageModel(int type)
{
    QSharedPointer<ConstitutiveModel> model = nullptr;

    if(0 == type)
    {
        model = QSharedPointer<IsotropicSeepageModel>(new IsotropicSeepageModel);
    }
    else if(1 == type)
    {
        model = QSharedPointer<AnisotropicSeepageModel>(new AnisotropicSeepageModel);
    }

    return model;
}

QSharedPointer<ConstitutiveModel> ConstitutiveModelFactory::CreateThermalConductModel(int type)
{
    QSharedPointer<ConstitutiveModel> model = nullptr;

    if(0 == type)
    {
        model = QSharedPointer<IsotropicConductModel>(new IsotropicConductModel);
    }
    else if(1 == type)
    {
        model = QSharedPointer<AnisotropicConductModel>(new AnisotropicConductModel);
    }

    return model;
}

QSharedPointer<ConstitutiveModel> ConstitutiveModelFactory::CreateCohesiveModel(int type)
{
    QSharedPointer<ConstitutiveModel> model = nullptr;

    if(0 == type)
    {
        model = QSharedPointer<CohesiveBilinearModel>(new CohesiveBilinearModel);
    }
    else if(1 == type)
    {
        model = QSharedPointer<CohesiveExponentModel>(new CohesiveExponentModel);
    }

    return model;
}

QSharedPointer<ConstitutiveModel> ConstitutiveModelFactory::CreateFractureSeepageModel(int type)
{
    QSharedPointer<ConstitutiveModel> model = nullptr;

    if(0 == type)
    {
        model = QSharedPointer<IncompressibleFlowModel>(new IncompressibleFlowModel);
    }
    else if(1 == type)
    {
        model = QSharedPointer<CompressibleFlowModel>(new CompressibleFlowModel);
    }

    return model;
}

QSharedPointer<ConstitutiveModel> ConstitutiveModelFactory::CreateThermalTransferModel(int type)
{
    QSharedPointer<ConstitutiveModel> model = nullptr;

    if(0 == type)
    {
        model = QSharedPointer<CohesiveThermalModel>(new CohesiveThermalModel);
    }

    return model;
}

ConstitutiveModelWidgetFactory::ConstitutiveModelWidgetFactory()
{

}

QSharedPointer<ConstitutiveModelWidget> ConstitutiveModelWidgetFactory::CreateMaterialModelWidget(int type)
{
    QSharedPointer<ConstitutiveModelWidget> modelWidget = nullptr;

    if(0 == type)
    {
        modelWidget = QSharedPointer<TriangleRigidBodyMotionWidget>(new TriangleRigidBodyMotionWidget);
    }
    else if(1 == type)
    {
        modelWidget = QSharedPointer<TriangleHookeanElasticityWidget>(new TriangleHookeanElasticityWidget);
    }
    else if(2 == type)
    {
        modelWidget = QSharedPointer<TriangleAnisotropicElasticityWidget>(new TriangleAnisotropicElasticityWidget);
    }
    else if(3 == type)
    {
        modelWidget = QSharedPointer<TriangleElastoPlasticityWidget>(new TriangleElastoPlasticityWidget);
    }
    else if(4 == type)  //Plane stress
    {
        modelWidget = QSharedPointer<TriangleHookeanElasticityWidget>(new TriangleHookeanElasticityWidget);
    }
    else if(5 == type)  //Plane strain
    {
        modelWidget = QSharedPointer<TriangleHookeanElasticityWidget>(new TriangleHookeanElasticityWidget);
    }

    return modelWidget;
}

QSharedPointer<ConstitutiveModelWidget> ConstitutiveModelWidgetFactory::CreateContactModelWidget(int type)
{
    QSharedPointer<ConstitutiveModelWidget> modelWidget = nullptr;

    if(0 == type)
    {
        modelWidget = QSharedPointer<ContactElasticityWidget>(new ContactElasticityWidget);
    }
    else if(1 == type)
    {
        modelWidget = QSharedPointer<ContactPlasticityWidget>(new ContactPlasticityWidget);
    }

    return modelWidget;
}

QSharedPointer<ConstitutiveModelWidget> ConstitutiveModelWidgetFactory::CreatePorositySeepageModelWidget(int type)
{
    QSharedPointer<ConstitutiveModelWidget> modelWidget = nullptr;

    if(0 == type)
    {
        modelWidget = QSharedPointer<TriangleIsotropicSeepageWidget>(new TriangleIsotropicSeepageWidget);
    }
    else if(1 == type)
    {
        modelWidget = QSharedPointer<TriangleAnisotropicSeepageWidget>(new TriangleAnisotropicSeepageWidget);
    }

    return modelWidget;
}

QSharedPointer<ConstitutiveModelWidget> ConstitutiveModelWidgetFactory::CreateThermalConductModelWidget(int type)
{
    QSharedPointer<ConstitutiveModelWidget> modelWidget = nullptr;

    if(0 == type)
    {
        modelWidget = QSharedPointer<TriangleIsotropicHeatConductWidget>(new TriangleIsotropicHeatConductWidget);
    }
    else if(1 == type)
    {
        modelWidget = QSharedPointer<TriangleAnisotropicHeatConductWidget>(new TriangleAnisotropicHeatConductWidget);
    }

    return modelWidget;
}

QSharedPointer<ConstitutiveModelWidget> ConstitutiveModelWidgetFactory::CreateCohesiveModelWidget(int type)
{
    QSharedPointer<ConstitutiveModelWidget> modelWidget = nullptr;

    if(0 == type)
    {
        modelWidget = QSharedPointer<CohesiveBilinearWidget>(new CohesiveBilinearWidget);
    }
    else if(1 == type)
    {
        modelWidget = QSharedPointer<CohesiveExponentWidget>(new CohesiveExponentWidget);
    }

    return modelWidget;
}

QSharedPointer<ConstitutiveModelWidget> ConstitutiveModelWidgetFactory::CreateFractureSeepageModelWidget(int type)
{
    QSharedPointer<ConstitutiveModelWidget> modelWidget = nullptr;

    if(0 == type)
    {
        modelWidget = QSharedPointer<CohesiveIncompressibleFlowWidget>(new CohesiveIncompressibleFlowWidget);
    }
    else if(1 == type)
    {
        modelWidget = QSharedPointer<CohesiveCompressibleFlowWidget>(new CohesiveCompressibleFlowWidget);
    }

    return modelWidget;
}

QSharedPointer<ConstitutiveModelWidget> ConstitutiveModelWidgetFactory::CreateThermalTransferModelWidget(int type)
{
    QSharedPointer<ConstitutiveModelWidget> modelWidget = nullptr;

    if(0 == type)
    {
        modelWidget = QSharedPointer<CohesiveHeatExchangeWidget>(new CohesiveHeatExchangeWidget);
    }

    return modelWidget;
}
