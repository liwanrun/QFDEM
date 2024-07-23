#ifndef FDEMMESHIMPORTERFACTORY_H
#define FDEMMESHIMPORTERFACTORY_H

#include "gmshmeshimporter.h"
#include "distmeshimporter.h"
#include "nepermesh4importer.h"
#include "abaqusmeshimporter.h"
#include "nepermeshimporter.h"
#include "hybridmeshimporter.h"

class FDEMMeshImporter;
class FDEMMeshImporterFactory
{
public:
    FDEMMeshImporterFactory();

    QSharedPointer<FDEMMeshImporter> Create(QString fname);
};

#endif // FDEMMESHIMPORTERFACTORY_H
