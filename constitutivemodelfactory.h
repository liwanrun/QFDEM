#ifndef CONSTITUTIVEMODELFACTORY_H
#define CONSTITUTIVEMODELFACTORY_H

#include <QSharedPointer>

class ConstitutiveModel;
class ConstitutiveModelFactory
{
public:
    ConstitutiveModelFactory();

    static QSharedPointer<ConstitutiveModel> CreateMaterialModel(int type);

    static QSharedPointer<ConstitutiveModel> CreateContactModel(int type);

    static QSharedPointer<ConstitutiveModel> CreatePorositySeepageModel(int type);

    static QSharedPointer<ConstitutiveModel> CreateThermalConductModel(int type);

    static QSharedPointer<ConstitutiveModel> CreateCohesiveModel(int type);

    static QSharedPointer<ConstitutiveModel> CreateFractureSeepageModel(int type);

    static QSharedPointer<ConstitutiveModel> CreateThermalTransferModel(int type);
};

class ConstitutiveModelWidget;
class ConstitutiveModelWidgetFactory
{
public:
    ConstitutiveModelWidgetFactory();

    static QSharedPointer<ConstitutiveModelWidget> CreateMaterialModelWidget(int type);

    static QSharedPointer<ConstitutiveModelWidget> CreateContactModelWidget(int type);

    static QSharedPointer<ConstitutiveModelWidget> CreatePorositySeepageModelWidget(int type);

    static QSharedPointer<ConstitutiveModelWidget> CreateThermalConductModelWidget(int type);

    static QSharedPointer<ConstitutiveModelWidget> CreateCohesiveModelWidget(int type);

    static QSharedPointer<ConstitutiveModelWidget> CreateFractureSeepageModelWidget(int type);

    static QSharedPointer<ConstitutiveModelWidget> CreateThermalTransferModelWidget(int type);
};

#endif // CONSTITUTIVEMODELFACTORY_H
