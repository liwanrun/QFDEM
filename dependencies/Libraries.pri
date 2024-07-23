# Configure VTK library
INCLUDEPATH += D:/TPL/VTK-8.1.1/debug_x64/include/vtk-8.1
DEPENDPATH += D:/TPL/VTK-8.1.1/debug_x64/include/vtk-8.1

win32: LIBS += -LD:/TPL/VTK-8.1.1/debug_x64/lib/ \
-lvtkalglib-8.1 \
-lvtkalglib-8.1 \
-lvtkChartsCore-8.1 \
-lvtkCommonColor-8.1 \
-lvtkCommonComputationalGeometry-8.1 \
-lvtkCommonCore-8.1 \
-lvtkCommonDataModel-8.1 \
-lvtkCommonExecutionModel-8.1 \
-lvtkCommonMath-8.1 \
-lvtkCommonMisc-8.1 \
-lvtkCommonSystem-8.1 \
-lvtkCommonTransforms-8.1 \
-lvtkDICOMParser-8.1 \
-lvtkDomainsChemistry-8.1 \
-lvtkDomainsChemistryOpenGL2-8.1 \
-lvtkexoIIc-8.1 \
-lvtkexpat-8.1 \
-lvtkFiltersAMR-8.1 \
-lvtkFiltersCore-8.1 \
-lvtkFiltersExtraction-8.1 \
-lvtkFiltersFlowPaths-8.1 \
-lvtkFiltersGeneral-8.1 \
-lvtkFiltersGeneric-8.1 \
-lvtkFiltersGeometry-8.1 \
-lvtkFiltersHybrid-8.1 \
-lvtkFiltersHyperTree-8.1 \
-lvtkFiltersImaging-8.1 \
-lvtkFiltersModeling-8.1 \
-lvtkFiltersParallel-8.1 \
-lvtkFiltersParallelImaging-8.1 \
-lvtkFiltersPoints-8.1 \
-lvtkFiltersProgrammable-8.1 \
-lvtkFiltersSelection-8.1 \
-lvtkFiltersSMP-8.1 \
-lvtkFiltersSources-8.1 \
-lvtkFiltersStatistics-8.1 \
-lvtkFiltersTexture-8.1 \
-lvtkFiltersTopology-8.1 \
-lvtkFiltersVerdict-8.1 \
-lvtkfreetype-8.1 \
-lvtkGeovisCore-8.1 \
-lvtkgl2ps-8.1 \
-lvtkglew-8.1 \
-lvtkGUISupportMFC-8.1 \
-lvtkGUISupportQt-8.1 \
-lvtkGUISupportQtOpenGL-8.1 \
-lvtkGUISupportQtSQL-8.1 \
-lvtkhdf5-8.1 \
-lvtkhdf5_hl-8.1 \
-lvtkImagingColor-8.1 \
-lvtkImagingCore-8.1 \
-lvtkImagingFourier-8.1 \
-lvtkImagingGeneral-8.1 \
-lvtkImagingHybrid-8.1 \
-lvtkImagingMath-8.1 \
-lvtkImagingMorphological-8.1 \
-lvtkImagingSources-8.1 \
-lvtkImagingStatistics-8.1 \
-lvtkImagingStencil-8.1 \
-lvtkInfovisCore-8.1 \
-lvtkInfovisLayout-8.1 \
-lvtkInteractionImage-8.1 \
-lvtkInteractionStyle-8.1 \
-lvtkInteractionWidgets-8.1 \
-lvtkIOAMR-8.1 \
-lvtkIOCore-8.1 \
-lvtkIOEnSight-8.1 \
-lvtkIOExodus-8.1 \
-lvtkIOExport-8.1 \
-lvtkIOExportOpenGL2-8.1 \
-lvtkIOGeometry-8.1 \
-lvtkIOImage-8.1 \
-lvtkIOImport-8.1 \
-lvtkIOInfovis-8.1 \
-lvtkIOLegacy-8.1 \
-lvtkIOLSDyna-8.1 \
-lvtkIOMINC-8.1 \
-lvtkIOMovie-8.1 \
-lvtkIONetCDF-8.1 \
-lvtkIOParallel-8.1 \
-lvtkIOParallelXML-8.1 \
-lvtkIOPLY-8.1 \
-lvtkIOSQL-8.1 \
-lvtkIOTecplotTable-8.1 \
-lvtkIOVideo-8.1 \
-lvtkIOXML-8.1 \
-lvtkIOXMLParser-8.1 \
-lvtkjpeg-8.1 \
-lvtkjsoncpp-8.1 \
-lvtklibharu-8.1 \
-lvtklibxml2-8.1 \
-lvtkLocalExample-8.1 \
-lvtklz4-8.1 \
-lvtkmetaio-8.1 \
-lvtkNetCDF-8.1 \
-lvtknetcdfcpp-8.1 \
-lvtkoggtheora-8.1 \
-lvtkParallelCore-8.1 \
-lvtkpng-8.1 \
-lvtkproj4-8.1 \
-lvtkRenderingAnnotation-8.1 \
-lvtkRenderingContext2D-8.1 \
-lvtkRenderingContextOpenGL2-8.1 \
-lvtkRenderingCore-8.1 \
-lvtkRenderingFreeType-8.1 \
-lvtkRenderingGL2PSOpenGL2-8.1 \
-lvtkRenderingImage-8.1 \
-lvtkRenderingLabel-8.1 \
-lvtkRenderingLOD-8.1 \
-lvtkRenderingOpenGL2-8.1 \
-lvtkRenderingQt-8.1 \
-lvtkRenderingVolume-8.1 \
-lvtkRenderingVolumeOpenGL2-8.1 \
-lvtksqlite-8.1 \
-lvtksys-8.1 \
-lvtktiff-8.1 \
-lvtkverdict-8.1 \
-lvtkViewsContext2D-8.1 \
-lvtkViewsCore-8.1 \
-lvtkViewsInfovis-8.1 \
-lvtkViewsQt-8.1 \
-lvtkzlib-8.1


#Configure CGAL Library
INCLUDEPATH += D:/TPL/CGAL-4.13.1/debug_x64/include
DEPENDPATH += D:/TPL/CGAL-4.13.1/debug_x64/include

win32: LIBS += -LD:/TPL/CGAL-4.13.1/debug_x64/lib/ \
-llibgmp-10 \
-llibmpfr-4


#Configure Boost Library
INCLUDEPATH += D:/TPL/boost_1_68_0
DEPENDPATH += D:/TPL/boost_1_68_0

win32: LIBS += -LD:/TPL/boost_1_68_0/lib64-msvc-14.1/ \
-lboost_thread-vc141-mt-gd-x64-1_68 \
-lboost_system-vc141-mt-gd-x64-1_68


#Configure QtPropertyBrowser
INCLUDEPATH += D:/TPL/QtPropertyBrowser/debug_x64/include/QtPropertyBrowser
DEPENDPATH += D:/TPL/QtPropertyBrowser/debug_x64/include/QtPropertyBrowser

win32: LIBS += -LD:/TPL/QtPropertyBrowser/debug_x64/lib/ \
-lQtPropertyBrowser


# ExodusII
# INCLUDEPATH += D:/TPL/VTK-8.1.1/exodusII
# DEPENDPATH += D:/TPL/VTK-8.1.1/exodusII
