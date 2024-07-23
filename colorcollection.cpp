#include "colorcollection.h"
#include "colorscheme.h"

#include <array>
#include <vector>

#include <vtkNew.h>
#include <vtkIntArray.h>
#include <vtkStringArray.h>
#include <vtkLookupTable.h>
#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>


QMutex ColorCollection::mutex;

QSharedPointer<ColorCollection> ColorCollection::colorCollection = nullptr;

QSharedPointer<ColorCollection> ColorCollection::GetColorCollection()
{
    if(nullptr == colorCollection)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == colorCollection)
        {
            colorCollection = QSharedPointer<ColorCollection>(new ColorCollection);
        }
    }

    return colorCollection;
}

ColorCollection::ColorSchemeMap &ColorCollection::GetSchemeMap()
{
    return this->scheme_map;
}

void ColorCollection::RemoveScheme(const std::string &name)
{
    this->scheme_map.erase(name);
}

ColorCollection::ColorCollection()
{
    std::string name;
    ColorScheme* scheme;
    name = "Classic Diverging Cool to Warm";
    scheme = new CoolToWarmScheme();
    scheme->PopulateRGBPoints();
    this->scheme_map.emplace(name, scheme);

    name = "Classic RGB Jet";
    scheme = new JetScheme();
    scheme->PopulateRGBPoints();
    this->scheme_map.emplace(name, scheme);

    name = "Classic LAB Cool to Warm (Extended)";
    scheme = new CoolToWarmExtendedScheme();
    scheme->PopulateRGBPoints();
    this->scheme_map.emplace(name, scheme);

    name = "Classic RGB Rainbow Uniform";
    scheme = new RainbowUniformScheme();
    scheme->PopulateRGBPoints();
    this->scheme_map.emplace(name, scheme);

    name = "Custom KAAMStep";
    scheme = new KAAMStepScheme();
    scheme->PopulateRGBPoints();
    this->scheme_map.emplace(name, scheme);

    name = "Classic HSV Rainbow Tecplot";
    scheme = new RainbowTecplotScheme();
    scheme->PopulateRGBPoints();
    this->scheme_map.emplace(name, scheme);
}


vtkLookupTable *ColorCollection::CreateLookupTable(const QString &name)
{
    QString hint = name.split(QChar(' ')).first();
    if(QString("Classic") == hint)
    {
        return this->CreateLUTbyColorFunction(name);
    }
    else
    {
        return this->CreateLUTFromColorSeries(name);
    }
}

vtkLookupTable *ColorCollection::CreateLUTFromColorSeries(const QString &name)
{
    vtkNew<vtkColorSeries> series;

    QString prefix = name.split(QChar(' ')).first();
    if(QString("Custom") == prefix)
    {
        ColorCollection::ColorSchemeMap schemes = this->colorCollection->GetSchemeMap();
        ColorScheme *scheme = schemes[name.toStdString()];
        size_t numColors = scheme->getColorSequence().size();

        series->SetNumberOfColors(static_cast<int>(numColors));
        series->SetColorSchemeName(name.toStdString());
        for(int i = 0; i < numColors; i++)
        {
            auto xrgb = scheme->getColorSequence()[i];
            series->SetColor(i, vtkColor3ub(xrgb[1]*255, xrgb[2]*255, xrgb[3]*255));
        }
    }
    series->SetColorSchemeByName(name.toStdString());

    vtkLookupTable *lut = vtkLookupTable::New();
    series->BuildLookupTable(lut, vtkColorSeries::CATEGORICAL);
    lut->SetNanColor(0.8, 0.8, 0.317647, 1.0);  //banana

    vtkNew<vtkIntArray> values;
    vtkNew<vtkStringArray> labels;
    for(int i = 0; i < lut->GetNumberOfColors(); i++)
    {
        values->InsertNextValue(i + 1);
        labels->InsertNextValue(std::to_string(i + 1).c_str());
    }
    lut->SetAnnotations(values, labels);

    return lut;
}

vtkLookupTable *ColorCollection::CreateLUTbyColorFunction(const QString &name)
{
    ColorCollection::ColorSchemeMap schemes = this->colorCollection->GetSchemeMap();
    ColorScheme *scheme = schemes[name.toStdString()];

    vtkNew<vtkColorTransferFunction> ctf;
    ctf->SetColorSpace(scheme->getColorSpace());
    for(auto xrgb : scheme->getColorSequence())
    {
        ctf->AddRGBPoint(xrgb[0], xrgb[1], xrgb[2], xrgb[3]);
    }

    int tableSize = 256;
    vtkLookupTable *lut = vtkLookupTable::New();
    lut->SetNumberOfTableValues(tableSize);
    for (int i = 0; i < lut->GetNumberOfColors(); ++i)
    {
        std::array<double, 3> rgb;
        ctf->GetColor(static_cast<double>(i) / lut->GetNumberOfColors(), rgb.data());
        std::array<double, 4> rgba = { 0.0, 0.0, 0.0, 1.0 };
        std::copy(std::begin(rgb), std::end(rgb), std::begin(rgba));
        lut->SetTableValue(static_cast<vtkIdType>(i), rgba.data());
    }
    lut->IndexedLookupOff();

    return lut;
}

