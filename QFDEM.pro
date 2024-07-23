#-------------------------------------------------
#
# Project created by QtCreator 2022-06-24T09:09:05
#
#-------------------------------------------------
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QFDEM
TEMPLATE = app

# executable icon
RC_FILE = logo.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    abaqusmeshimporter.cpp \
    actor.cpp \
    actorcollection.cpp \
    backgroundgrid.cpp \
    backgroundgriddialog.cpp \
    block.cpp \
    blockcollection.cpp \
    blockgeometry.cpp \
    boundary.cpp \
    boundaryclipper.cpp \
    boundarycollection.cpp \
    cohesivebilinearwidget.cpp \
    cohesivecompressibleflowwidget.cpp \
    cohesiveexponentwidget.cpp \
    cohesivefracturewidget.cpp \
    cohesiveheatexchangewidget.cpp \
    cohesiveincompressibleflowwidget.cpp \
    colorcollection.cpp \
    colorscheme.cpp \
    constitutivemodelfactory.cpp \
    constitutivemodelwidget.cpp \
    constraineddelaunaytriangulation.cpp \
    constrainedtriangulation.cpp \
    contact.cpp \
    contactconstitutivemodel.cpp \
    contactelasticitywidget.cpp \
    contactplasticitywidget.cpp \
    criterion.cpp \
    criterioncollection.cpp \
    distmeshimporter.cpp \
    fdemGQfilteredmeshshrink.cpp \
    fdemGQfilteredmeshsource.cpp \
    fdemLQfilteredmeshshrink.cpp \
    fdemLQfilteredmeshsource.cpp \
    fdemSTfilteredmeshshrink.cpp \
    fdemSTfilteredmeshsource.cpp \
    fdemboundaryitemdelegate.cpp \
    fdemboundarywidget.cpp \
    fdemcellhistorywidget.cpp \
    fdemcohesivematerialwidget.cpp \
    fdemcontactwidget.cpp \
    fdemdocumentexporter.cpp \
    fdemfieldhistorywidget.cpp \
    fdemgeometryitemdelegate.cpp \
    fdemgeometrywidget.cpp \
    fdemgroupitemdelegate.cpp \
    fdemgroupwidget.cpp \
    fdemhistoryitemdelegate.cpp \
    fdemhistorywidget.cpp \
    fdemhydraulicboundarywidget.cpp \
    fdemhydraulictemporalwidget.cpp \
    fdeminformationwidget.cpp \
    fdemmaterialitemdelegate.cpp \
    fdemmaterialwidget.cpp \
    fdemmechanicalboundarywidget.cpp \
    fdemmechanicaltemporalwidget.cpp \
    fdemmeshimporter.cpp \
    fdemmeshimporterfactory.cpp \
    fdemmeshitemdelegate.cpp \
    fdemmeshoptionitemdelegate.cpp \
    fdemmeshshrink.cpp \
    fdemmeshshrinkfactory.cpp \
    fdemmeshsource.cpp \
    fdemmeshsourcefactory.cpp \
    fdemmeshwidget.cpp \
    fdemntsfluidmaterialwidget.cpp \
    fdemparameterwidget.cpp \
    fdempointhistorywidget.cpp \
    fdempreferencewidget.cpp \
    fdemprimitivemeshsource.cpp \
    fdemshapesource.cpp \
    fdemsolutionitemdelegate.cpp \
    fdemsolutionwidget.cpp \
    fdemtemporalitemdelegate.cpp \
    fdemtemporalstressstrainwidget.cpp \
    fdemtemporalwidget.cpp \
    fdemthermalboundarywidget.cpp \
    fdemthermaltemporalwidget.cpp \
    fdemthickquadmeshsource.cpp \
    fdemtrianglematerialwidget.cpp \
    fdemtriangulationfilter.cpp \
    fdemvtkrenderwidget.cpp \
    fdemwindow.cpp \
    filterselectdelegate.cpp \
    filterselectdialog.cpp \
    fractaldimension.cpp \
    fractaldimensiondialog.cpp \
    fracturenetwork.cpp \
    fracturenetworkdelegate.cpp \
    fracturenetworkdialog.cpp \
    globalcollection.cpp \
    gmshmeshimporter.cpp \
    group.cpp \
    groupcollection.cpp \
    history.cpp \
    historycollection.cpp \
    hybridmeshimporter.cpp \
    hybridmeshtriangulation.cpp \
    hydraulicconstitutivemodel.cpp \
    interactorstyledrawpolygonselection.cpp \
    interactorstyleinteractivequery.cpp \
    interactorstyleinteractiveselection.cpp \
    interactorstylerubberbandselection.cpp \
    main.cpp \
    material.cpp \
    materialcollection.cpp \
    mechanicalconstitutivemodel.cpp \
    multicascadetree.cpp \
    nepermesh4importer.cpp \
    nepermeshimporter.cpp \
    primitiveactor.cpp \
    samplestrategy.cpp \
    selectcollection.cpp \
    selectionfilter.cpp \
    shrinkdialog.cpp \
    solution.cpp \
    solutioncollection.cpp \
    temporal.cpp \
    temporalcollection.cpp \
    thermalconstitutivemodel.cpp \
    transformdialog.cpp \
    triangleanisotropicelasticitywidget.cpp \
    triangleanisotropicheatconductwidget.cpp \
    triangleanisotropicseepagewidget.cpp \
    triangleelastoplasticitywidget.cpp \
    trianglehookeanelasticitywidget.cpp \
    triangleisotropicheatconductwidget.cpp \
    triangleisotropicseepagewidget.cpp \
    trianglerigidbodymotionwidget.cpp \
    triangulation.cpp \
    triangulationactor.cpp \
    voronoidiagram.cpp \
    voronoidiagramactor.cpp \
    voronoidiagramsource.cpp

HEADERS += \
    abaqusmeshimporter.h \
    actor.h \
    actorcollection.h \
    backgroundgrid.h \
    backgroundgriddialog.h \
    block.h \
    blockcollection.h \
    blockgeometry.h \
    boundary.h \
    boundaryclipper.h \
    boundarycollection.h \
    cgalmeshinfo.h \
    cgaltypedef.h \
    cohesivebilinearwidget.h \
    cohesivecompressibleflowwidget.h \
    cohesiveexponentwidget.h \
    cohesivefracturewidget.h \
    cohesiveheatexchangewidget.h \
    cohesiveincompressibleflowwidget.h \
    colorcollection.h \
    colorscheme.h \
    constitutivemodel.h \
    constitutivemodelfactory.h \
    constitutivemodelwidget.h \
    constraineddelaunaytriangulation.h \
    constrainedtriangulation.h \
    contact.h \
    contactconstitutivemodel.h \
    contactelasticitywidget.h \
    contactplasticitywidget.h \
    criterion.h \
    criterioncollection.h \
    distmeshimporter.h \
    fdemGQfilteredmeshshrink.h \
    fdemGQfilteredmeshsource.h \
    fdemLQfilteredmeshshrink.h \
    fdemLQfilteredmeshsource.h \
    fdemSTfilteredmeshshrink.h \
    fdemSTfilteredmeshsource.h \
    fdemboundaryitemdelegate.h \
    fdemboundarywidget.h \
    fdemcellhistorywidget.h \
    fdemcohesivematerialwidget.h \
    fdemcontactwidget.h \
    fdemdocumentexporter.h \
    fdemfieldhistorywidget.h \
    fdemgeometryitemdelegate.h \
    fdemgeometrywidget.h \
    fdemgroupitemdelegate.h \
    fdemgroupwidget.h \
    fdemhistoryitemdelegate.h \
    fdemhistorywidget.h \
    fdemhydraulicboundarywidget.h \
    fdemhydraulictemporalwidget.h \
    fdeminformationwidget.h \
    fdemmaterialitemdelegate.h \
    fdemmaterialwidget.h \
    fdemmechanicalboundarywidget.h \
    fdemmechanicaltemporalwidget.h \
    fdemmeshimporter.h \
    fdemmeshimporterfactory.h \
    fdemmeshitemdelegate.h \
    fdemmeshoptionitemdelegate.h \
    fdemmeshshrink.h \
    fdemmeshshrinkfactory.h \
    fdemmeshsource.h \
    fdemmeshsourcefactory.h \
    fdemmeshwidget.h \
    fdemntsfluidmaterialwidget.h \
    fdemparameterwidget.h \
    fdempointhistorywidget.h \
    fdempreferencewidget.h \
    fdemprimitivemeshsource.h \
    fdemshapesource.h \
    fdemsolutionitemdelegate.h \
    fdemsolutionwidget.h \
    fdemtemporalitemdelegate.h \
    fdemtemporalstressstrainwidget.h \
    fdemtemporalwidget.h \
    fdemthermalboundarywidget.h \
    fdemthermaltemporalwidget.h \
    fdemthickquadmeshsource.h \
    fdemtrianglematerialwidget.h \
    fdemtriangulationfilter.h \
    fdemvtkrenderwidget.h \
    fdemwindow.h \
    filterselectdelegate.h \
    filterselectdialog.h \
    fractaldimension.h \
    fractaldimensiondialog.h \
    fracturenetwork.h \
    fracturenetworkdelegate.h \
    fracturenetworkdialog.h \
    globalcollection.h \
    globalmacros.h \
    gmshmeshimporter.h \
    group.h \
    groupcollection.h \
    history.h \
    historycollection.h \
    hybridmeshimporter.h \
    hybridmeshtriangulation.h \
    hydraulicconstitutivemodel.h \
    interactorstyledrawpolygonselection.h \
    interactorstyleinteractivequery.h \
    interactorstyleinteractiveselection.h \
    interactorstylerubberbandselection.h \
    material.h \
    materialcollection.h \
    mechanicalconstitutivemodel.h \
    multicascadetree.h \
    nepermesh4importer.h \
    nepermeshimporter.h \
    primitiveactor.h \
    samplestrategy.h \
    selectcollection.h \
    selectionfilter.h \
    shrinkdialog.h \
    solution.h \
    solutioncollection.h \
    temporal.h \
    temporalcollection.h \
    thermalconstitutivemodel.h \
    transformdialog.h \
    triangleanisotropicelasticitywidget.h \
    triangleanisotropicheatconductwidget.h \
    triangleanisotropicseepagewidget.h \
    triangleelastoplasticitywidget.h \
    trianglehookeanelasticitywidget.h \
    triangleisotropicheatconductwidget.h \
    triangleisotropicseepagewidget.h \
    trianglerigidbodymotionwidget.h \
    triangulation.h \
    triangulationactor.h \
    voronoidiagram.h \
    voronoidiagramactor.h \
    voronoidiagramsource.h

FORMS += \
    backgroundgriddialog.ui \
    cohesivebilinearwidget.ui \
    cohesivecompressibleflowwidget.ui \
    cohesiveexponentwidget.ui \
    cohesivefracturewidget.ui \
    cohesiveheatexchangewidget.ui \
    cohesiveincompressibleflowwidget.ui \
    contactelasticitywidget.ui \
    contactplasticitywidget.ui \
    fdemboundarywidget.ui \
    fdemcellhistorywidget.ui \
    fdemcohesivematerialwidget.ui \
    fdemcontactwidget.ui \
    fdemfieldhistorywidget.ui \
    fdemgeometrywidget.ui \
    fdemgroupwidget.ui \
    fdemhistorywidget.ui \
    fdemhydraulicboundarywidget.ui \
    fdemhydraulictemporalwidget.ui \
    fdeminformationwidget.ui \
    fdemmaterialwidget.ui \
    fdemmechanicalboundarywidget.ui \
    fdemmechanicaltemporalwidget.ui \
    fdemmeshwidget.ui \
    fdemntsfluidmaterialwidget.ui \
    fdemparameterwidget.ui \
    fdempointhistorywidget.ui \
    fdempreferencewidget.ui \
    fdemsolutionwidget.ui \
    fdemtemporalstressstrainwidget.ui \
    fdemtemporalwidget.ui \
    fdemthermalboundarywidget.ui \
    fdemthermaltemporalwidget.ui \
    fdemtrianglematerialwidget.ui \
    fdemvtkrenderwidget.ui \
    fdemwindow.ui \
    filterselectdialog.ui \
    fractaldimensiondialog.ui \
    fracturenetworkdialog.ui \
    shrinkdialog.ui \
    transformdialog.ui \
    triangleanisotropicelasticitywidget.ui \
    triangleanisotropicheatconductwidget.ui \
    triangleanisotropicseepagewidget.ui \
    triangleelastoplasticitywidget.ui \
    trianglehookeanelasticitywidget.ui \
    triangleisotropicheatconductwidget.ui \
    triangleisotropicseepagewidget.ui \
    trianglerigidbodymotionwidget.ui

# PRI file for subdirectories
include($${PWD}/gui/gui.pri)
include($${PWD}/doc/doc.pri)

# PRI file for third party libraries
include($${PWD}/dependencies/Libraries.pri)

RESOURCES += \
    qFDEM.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
