#ifndef COLORCOLLECTION_H
#define COLORCOLLECTION_H

#include <QMutex>
#include <QSharedPointer>

#include <map>
#include <string>

class ColorScheme;
class vtkLookupTable;
class ColorCollection
{
public:
    typedef std::map<std::string, ColorScheme*> ColorSchemeMap;

public:
    static QSharedPointer<ColorCollection> GetColorCollection();

    ~ColorCollection() {}

    ColorSchemeMap& GetSchemeMap();

    void RemoveScheme(const std::string& name);

    vtkLookupTable *CreateLookupTable(const QString &name);

private:
    ColorCollection();

    vtkLookupTable* CreateLUTFromColorSeries(const QString &name);

    vtkLookupTable* CreateLUTbyColorFunction(const QString &name);

private:
    static QSharedPointer<ColorCollection> colorCollection;

    static QMutex mutex;

    ColorSchemeMap scheme_map;
};

#endif // COLORCOLLECTION_H
