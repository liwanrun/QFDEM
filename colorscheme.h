#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include <array>
#include <vector>

class ColorScheme
{
public:
    typedef std::vector<std::array<double, 4>> RGBPoints;

public:
    ColorScheme();
    virtual ~ColorScheme();

    int getColorSpace() const;
    void setColorSpace(int value);

    RGBPoints getColorSequence() const;
    void setColorSequence(const RGBPoints &value);

    double *getNanColor() const;
    void setNanColor(double *value);

    virtual void PopulateRGBPoints() = 0;

protected:
    int ColorSpace;
    double *NanColor;
    RGBPoints ColorSequence;
};

class CoolToWarmScheme : public ColorScheme
{
public:
    typedef std::vector<std::array<double, 4>> RGBPoints;

public:
    CoolToWarmScheme();
    ~CoolToWarmScheme() override {}

    void PopulateRGBPoints() override;
};

class CoolToWarmExtendedScheme : public ColorScheme
{
public:
    typedef std::vector<std::array<double, 4>> RGBPoints;

public:
    CoolToWarmExtendedScheme();
    ~CoolToWarmExtendedScheme() override {}

    void PopulateRGBPoints() override;
};

class JetScheme : public ColorScheme
{
public:
    typedef std::vector<std::array<double, 4>> RGBPoints;

public:
    JetScheme();
    ~JetScheme() override {}

    void PopulateRGBPoints() override;
};

class RainbowUniformScheme : public ColorScheme
{
public:
    typedef std::vector<std::array<double, 4>> RGBPoints;

public:
    RainbowUniformScheme();
    ~RainbowUniformScheme() override {}

    void PopulateRGBPoints() override;
};

class KAAMStepScheme : public ColorScheme
{
public:
    typedef std::vector<std::array<double, 4>> RGBPoints;

public:
    KAAMStepScheme();
    ~KAAMStepScheme() override {}

    void PopulateRGBPoints() override;
};

class RainbowTecplotScheme : public ColorScheme
{
public:
    typedef std::vector<std::array<double, 4>> RGBPoints;

public:
    RainbowTecplotScheme();
    ~RainbowTecplotScheme() override {}

    void PopulateRGBPoints() override;
};

#endif // COLORSCHEME_H
