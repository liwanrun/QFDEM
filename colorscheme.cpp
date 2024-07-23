#include "colorscheme.h"
#include "vtkColorTransferFunction.h"

ColorScheme::ColorScheme()
{
    this->ColorSpace = VTK_CTF_DIVERGING;

    this->NanColor = new double[3];
    this->NanColor[0] = 1.0;
    this->NanColor[1] = 1.0;
    this->NanColor[2] = 0.0;
}

ColorScheme::~ColorScheme()
{
    delete this->NanColor;
}

int ColorScheme::getColorSpace() const
{
    return this->ColorSpace;
}

void ColorScheme::setColorSpace(int value)
{
    this->ColorSpace = value;
}

ColorScheme::RGBPoints ColorScheme::getColorSequence() const
{
    return this->ColorSequence;
}

void ColorScheme::setColorSequence(const RGBPoints &value)
{
    this->ColorSequence = value;
}

double *ColorScheme::getNanColor() const
{
    return this->NanColor;
}

void ColorScheme::setNanColor(double *value)
{
    this->NanColor = value;
}

CoolToWarmScheme::CoolToWarmScheme()
{
    this->ColorSpace = VTK_CTF_DIVERGING;
    this->NanColor[0] = 1.0;
    this->NanColor[1] = 1.0;
    this->NanColor[2] = 0.0;
}

void CoolToWarmScheme::PopulateRGBPoints()
{
    this->ColorSequence.reserve(3);

    std::array<double, 4> rgb1 = {0.0,
                                  0.23137254902000001,
                                  0.298039215686,
                                  0.75294117647100001};
    std::array<double, 4> rgb2 = {0.5,
                                  0.86499999999999999,
                                  0.86499999999999999,
                                  0.86499999999999999};
    std::array<double, 4> rgb3 = {1.0,
                                  0.70588235294099999,
                                  0.015686274509800001,
                                  0.149019607843};

    this->ColorSequence.emplace_back(rgb1);
    this->ColorSequence.emplace_back(rgb2);
    this->ColorSequence.emplace_back(rgb3);
}

JetScheme::JetScheme()
{
    this->ColorSpace = VTK_CTF_RGB;
    this->NanColor[0] = 1.0;
    this->NanColor[1] = 1.0;
    this->NanColor[2] = 0.0;
}

void JetScheme::PopulateRGBPoints()
{
    this->ColorSequence.reserve(7);

    std::array<double, 4> rgb1 = {-1,
                                  0,
                                  0,
                                  0.5625};
    std::array<double, 4> rgb2 = {-0.77777799999999997,
                                  0,
                                  0,
                                  1};
    std::array<double, 4> rgb3 = {-0.269841,
                                  0,
                                  1,
                                  1};
    std::array<double, 4> rgb4 = {-0.015873000000000002,
                                  0.5,
                                  1,
                                  0.5};
    std::array<double, 4> rgb5 = {0.238095,
                                  1,
                                  1,
                                  0};
    std::array<double, 4> rgb6 = {0.74603200000000003,
                                  1,
                                  0,
                                  0};
    std::array<double, 4> rgb7 = {1,
                                  0.5,
                                  0,
                                  0};

    this->ColorSequence.emplace_back(rgb1);
    this->ColorSequence.emplace_back(rgb2);
    this->ColorSequence.emplace_back(rgb3);
    this->ColorSequence.emplace_back(rgb4);
    this->ColorSequence.emplace_back(rgb5);
    this->ColorSequence.emplace_back(rgb6);
    this->ColorSequence.emplace_back(rgb7);
}

CoolToWarmExtendedScheme::CoolToWarmExtendedScheme()
{
    this->ColorSpace = VTK_CTF_LAB;
    this->NanColor[0] = 0.25;
    this->NanColor[1] = 0.0;
    this->NanColor[2] = 0.0;
}

void CoolToWarmExtendedScheme::PopulateRGBPoints()
{
    this->ColorSequence.reserve(35);

    std::array<double, 4> rgb1 = {0.0,
                                  0,
                                  0,
                                  0.34902};
    std::array<double, 4> rgb2 = {0.03125,
                                  0.039216000000000001,
                                  0.062744999999999995,
                                  0.38039200000000001};
    std::array<double, 4> rgb3 = {0.0625,
                                  0.062744999999999995,
                                  0.117647,
                                  0.41176499999999999};
    std::array<double, 4> rgb4 = {0.09375,
                                  0.090195999999999998,
                                  0.18431400000000001,
                                  0.45097999999999999};
    std::array<double, 4> rgb5 = {0.125,
                                  0.12548999999999999,
                                  0.26274500000000001,
                                  0.50196099999999999};
    std::array<double, 4> rgb6 = {0.15625,
                                  0.16078400000000001,
                                  0.33725500000000003,
                                  0.54117599999999999};
    std::array<double, 4> rgb7 = {0.1875,
                                  0.20000000000000001,
                                  0.39607799999999999,
                                  0.56862699999999999};
    std::array<double, 4> rgb8 = {0.21875,
                                  0.23921600000000001,
                                  0.45490199999999997,
                                  0.59999999999999998};
    std::array<double, 4> rgb9 = {0.25,
                                  0.286275,
                                  0.52156899999999995,
                                  0.65098};
    std::array<double, 4> rgb10 = {0.28125,
                                   0.33725500000000003,
                                   0.59215700000000004,
                                   0.70196099999999995};
    std::array<double, 4> rgb11 = {0.3125,
                                   0.388235,
                                   0.65490199999999998,
                                   0.74902000000000002};
    std::array<double, 4> rgb12 = {0.34375,
                                   0.466667,
                                   0.73725499999999999,
                                   0.819608};
    std::array<double, 4> rgb13 = {0.375,
                                   0.57254899999999997,
                                   0.819608,
                                   0.87843099999999996};
    std::array<double, 4> rgb14 = {0.40625,
                                   0.65490199999999998,
                                   0.86666699999999997,
                                   0.90980399999999995};
    std::array<double, 4> rgb15 = {0.4375,
                                   0.75294099999999997,
                                   0.91764699999999999,
                                   0.94117600000000001};
    std::array<double, 4> rgb16 = {0.46875,
                                   0.82352899999999996,
                                   0.95686300000000002,
                                   0.96862700000000002};
    std::array<double, 4> rgb17 = {0.5,
                                   0.98823499999999997,
                                   0.96078399999999997,
                                   0.90196100000000001};
    std::array<double, 4> rgb18 = {0.5,
                                   0.94117600000000001,
                                   0.98431400000000002,
                                   0.98823499999999997};
    std::array<double, 4> rgb19 = {0.52000000000000002,
                                   0.98823499999999997,
                                   0.94509799999999999,
                                   0.85097999999999996};
    std::array<double, 4> rgb20 = {0.54000000000000004,
                                   0.98039200000000004,
                                   0.89803900000000003,
                                   0.78431399999999996};
    std::array<double, 4> rgb21 = {0.5625,
                                   0.96862700000000002,
                                   0.83529399999999998,
                                   0.69803899999999997};
    std::array<double, 4> rgb22 = {0.59375,
                                   0.94901999999999997,
                                   0.73333300000000001,
                                   0.58823499999999995};
    std::array<double, 4> rgb23 = {0.625,
                                   0.92941200000000002,
                                   0.65098,
                                   0.50980400000000003};
    std::array<double, 4> rgb24 = {0.65625,
                                   0.90980399999999995,
                                   0.56470600000000004,
                                   0.43529400000000001};
    std::array<double, 4> rgb25 = {0.6875,
                                   0.87843099999999996,
                                   0.45882400000000001,
                                   0.352941};
    std::array<double, 4> rgb26 = {0.71875,
                                   0.83921599999999996,
                                   0.388235,
                                   0.286275};
    std::array<double, 4> rgb27 = {0.75,
                                   0.76078400000000002,
                                   0.29411799999999999,
                                   0.21176500000000001};
    std::array<double, 4> rgb28 = {0.78125,
                                   0.70196099999999995,
                                   0.21176500000000001,
                                   0.168627};
    std::array<double, 4> rgb29 = {0.8125,
                                   0.65098,
                                   0.156863,
                                   0.129412};
    std::array<double, 4> rgb30 = {0.84375,
                                   0.59999999999999998,
                                   0.094117999999999993,
                                   0.094117999999999993};
    std::array<double, 4> rgb31 = {0.875,
                                   0.54901999999999995,
                                   0.066667000000000004,
                                   0.098039000000000001};
    std::array<double, 4> rgb32 = {0.90625,
                                   0.50196099999999999,
                                   0.050979999999999998,
                                   0.12548999999999999};
    std::array<double, 4> rgb33 = {0.9375,
                                   0.45097999999999999,
                                   0.054901999999999999,
                                   0.17254900000000001};
    std::array<double, 4> rgb34 = {0.96875,
                                   0.40000000000000002,
                                   0.054901999999999999,
                                   0.19215699999999999};
    std::array<double, 4> rgb35 = {1.0,
                                   0.34902,
                                   0.070587999999999998,
                                   0.21176500000000001};

    this->ColorSequence.emplace_back(rgb1);
    this->ColorSequence.emplace_back(rgb2);
    this->ColorSequence.emplace_back(rgb3);
    this->ColorSequence.emplace_back(rgb4);
    this->ColorSequence.emplace_back(rgb5);
    this->ColorSequence.emplace_back(rgb6);
    this->ColorSequence.emplace_back(rgb7);
    this->ColorSequence.emplace_back(rgb8);
    this->ColorSequence.emplace_back(rgb9);
    this->ColorSequence.emplace_back(rgb10);
    this->ColorSequence.emplace_back(rgb11);
    this->ColorSequence.emplace_back(rgb12);
    this->ColorSequence.emplace_back(rgb13);
    this->ColorSequence.emplace_back(rgb14);
    this->ColorSequence.emplace_back(rgb15);
    this->ColorSequence.emplace_back(rgb16);
    this->ColorSequence.emplace_back(rgb17);
    this->ColorSequence.emplace_back(rgb18);
    this->ColorSequence.emplace_back(rgb19);
    this->ColorSequence.emplace_back(rgb20);
    this->ColorSequence.emplace_back(rgb21);
    this->ColorSequence.emplace_back(rgb22);
    this->ColorSequence.emplace_back(rgb23);
    this->ColorSequence.emplace_back(rgb24);
    this->ColorSequence.emplace_back(rgb25);
    this->ColorSequence.emplace_back(rgb26);
    this->ColorSequence.emplace_back(rgb27);
    this->ColorSequence.emplace_back(rgb28);
    this->ColorSequence.emplace_back(rgb29);
    this->ColorSequence.emplace_back(rgb30);
    this->ColorSequence.emplace_back(rgb31);
    this->ColorSequence.emplace_back(rgb32);
    this->ColorSequence.emplace_back(rgb33);
    this->ColorSequence.emplace_back(rgb34);
    this->ColorSequence.emplace_back(rgb35);
}

RainbowUniformScheme::RainbowUniformScheme()
{
    this->ColorSpace = VTK_CTF_RGB;
    this->NanColor[0] = 1.0;
    this->NanColor[1] = 0.0;
    this->NanColor[2] = 0.0;
}

void RainbowUniformScheme::PopulateRGBPoints()
{
    this->ColorSequence.reserve(43);

    std::array<double, 4> rgb1 = {0.0,
                                   0.02,
                                   0.38129999999999997,
                                   0.99809999999999999};
    std::array<double, 4> rgb2 = {0.023809523809523808,
                                   0.02000006,
                                   0.42426776799999999,
                                   0.96906968999999998};
    std::array<double, 4> rgb3 = {0.047619047619047616,
                                   0.02,
                                   0.46723376300000002,
                                   0.94003304300000001};
    std::array<double, 4> rgb4 = {0.071428571428571425,
                                   0.02,
                                   0.51019999999999999,
                                   0.91100000000000003};
    std::array<double, 4> rgb5 = {0.095238095238095233,
                                   0.02000006,
                                   0.54640149400000004,
                                   0.87266943799999996};
    std::array<double, 4> rgb6 = {0.11904761904761904,
                                   0.02,
                                   0.58260036199999998,
                                   0.83433294999999996};
    std::array<double, 4> rgb7 = {0.14285714285714285,
                                   0.02,
                                   0.61880000000000002,
                                   0.79600000000000004};
    std::array<double, 4> rgb8 = {0.16666666666666666,
                                   0.02000006,
                                   0.65253515600000001,
                                   0.74980243400000002};
    std::array<double, 4> rgb9 = {0.19047619047619047,
                                   0.02,
                                   0.68626700399999996,
                                   0.70359953799999997};
    std::array<double, 4> rgb10 = {0.21428571428571427,
                                   0.02,
                                   0.71999999999999997,
                                   0.65739999999999998};
    std::array<double, 4> rgb11 = {0.23809523809523808,
                                   0.02000006,
                                   0.757035456,
                                   0.60373535899999997};
    std::array<double, 4> rgb12 = {0.26190476190476192,
                                   0.02,
                                   0.79406703700000003,
                                   0.55006613000000004};
    std::array<double, 4> rgb13 = {0.2857142857142857,
                                   0.02,
                                   0.83109999999999995,
                                   0.49640000000000001};
    std::array<double, 4> rgb14 = {0.30952380952380953,
                                   0.021354336738172372,
                                   0.86453685552616311,
                                   0.42855794607611591};
    std::array<double, 4> rgb15 = {0.33333333333333331,
                                   0.023312914349117714,
                                   0.89799935992448399,
                                   0.36073871343115577};
    std::array<double, 4> rgb16 = {0.35714285714285715,
                                   0.015976108242848862,
                                   0.9310479513349017,
                                   0.29256318150880922};
    std::array<double, 4> rgb17 = {0.38095238095238093,
                                   0.27421074700988196,
                                   0.95256296099508297,
                                   0.15356836602739213};
    std::array<double, 4> rgb18 = {0.40476190476190477,
                                   0.49335462816816988,
                                   0.96190386253094817,
                                   0.11119493614749336};
    std::array<double, 4> rgb19 = {0.42857142857142855,
                                   0.64390000000000003,
                                   0.97729999999999995,
                                   0.046899999999999997};
    std::array<double, 4> rgb20 = {0.45238095238095238,
                                   0.76240181299999998,
                                   0.98466959099999996,
                                   0.034600153000000002};
    std::array<double, 4> rgb21 = {0.47619047619047616,
                                   0.88090118500000003,
                                   0.99203340699999998,
                                   0.022299876999999999};
    std::array<double, 4> rgb22 = {0.5,
                                   0.99952854326271467,
                                   0.99951937067814922,
                                   0.0134884641450013};
    std::array<double, 4> rgb23 = {0.52380952380952384,
                                   0.99940299799999999,
                                   0.95503637600000002,
                                   0.079066628};
    std::array<double, 4> rgb24 = {0.54761904761904767,
                                   0.99939999999999996,
                                   0.910666223,
                                   0.148134024};
    std::array<double, 4> rgb25 = {0.5714285714285714,
                                   0.99939999999999996,
                                   0.86629999999999996,
                                   0.2172};
    std::array<double, 4> rgb26 = {0.59523809523809523,
                                   0.99926966500000003,
                                   0.81803598099999997,
                                   0.21720065199999999};
    std::array<double, 4> rgb27 = {0.61904761904761907,
                                   0.99913333199999999,
                                   0.76976618399999996,
                                   0.2172};
    std::array<double, 4> rgb28 = {0.6428571428571429,
                                   0.999,
                                   0.72150000000000003,
                                   0.2172};
    std::array<double, 4> rgb29 = {0.66666666666666663,
                                   0.99913633000000002,
                                   0.673435546,
                                   0.21720065199999999};
    std::array<double, 4> rgb30 = {0.69047619047619047,
                                   0.99926666799999997,
                                   0.62536618600000005,
                                   0.2172};
    std::array<double, 4> rgb31 = {0.7142857142857143,
                                   0.99939999999999996,
                                   0.57730000000000004,
                                   0.2172};
    std::array<double, 4> rgb32 = {0.73809523809523814,
                                   0.99940299799999999,
                                   0.52106845499999999,
                                   0.21720065199999999};
    std::array<double, 4> rgb33 = {0.76190476190476186,
                                   0.99939999999999996,
                                   0.46483277099999998,
                                   0.2172};
    std::array<double, 4> rgb34 = {0.7857142857142857,
                                   0.99939999999999996,
                                   0.40860000000000002,
                                   0.2172};
    std::array<double, 4> rgb35 = {0.80952380952380953,
                                   0.99475999176873464,
                                   0.33177297300202935,
                                   0.21123096385202059};
    std::array<double, 4> rgb36 = {0.83333333333333337,
                                   0.98671295054795893,
                                   0.25951834109149341,
                                   0.19012239549291934};
    std::array<double, 4> rgb37 = {0.8571428571428571,
                                   0.99124588756464194,
                                   0.14799417507952672,
                                   0.21078892136920357};
    std::array<double, 4> rgb38 = {0.88095238095238093,
                                   0.94990303700000001,
                                   0.11686717100000001,
                                   0.252900603};
    std::array<double, 4> rgb39 = {0.90476190476190477,
                                   0.903199533,
                                   0.078432949000000002,
                                   0.29180038899999999};
    std::array<double, 4> rgb40 = {0.9285714285714286,
                                   0.85650000000000004,
                                   0.040000000000000001,
                                   0.33069999999999999};
    std::array<double, 4> rgb41 = {0.95238095238095233,
                                   0.79890262700000003,
                                   0.043333450000000003,
                                   0.35843429799999998};
    std::array<double, 4> rgb42 = {0.97619047619047616,
                                   0.74129942400000004,
                                   0.046666699999999998,
                                   0.38616694400000001};
    std::array<double, 4> rgb43 = {1.0,
                                   0.68369999999999997,
                                   0.050000000000000003,
                                   0.4138999999999999};


    this->ColorSequence.emplace_back(rgb1);
    this->ColorSequence.emplace_back(rgb2);
    this->ColorSequence.emplace_back(rgb3);
    this->ColorSequence.emplace_back(rgb4);
    this->ColorSequence.emplace_back(rgb5);
    this->ColorSequence.emplace_back(rgb6);
    this->ColorSequence.emplace_back(rgb7);
    this->ColorSequence.emplace_back(rgb8);
    this->ColorSequence.emplace_back(rgb9);
    this->ColorSequence.emplace_back(rgb10);
    this->ColorSequence.emplace_back(rgb11);
    this->ColorSequence.emplace_back(rgb12);
    this->ColorSequence.emplace_back(rgb13);
    this->ColorSequence.emplace_back(rgb14);
    this->ColorSequence.emplace_back(rgb15);
    this->ColorSequence.emplace_back(rgb16);
    this->ColorSequence.emplace_back(rgb17);
    this->ColorSequence.emplace_back(rgb18);
    this->ColorSequence.emplace_back(rgb19);
    this->ColorSequence.emplace_back(rgb20);
    this->ColorSequence.emplace_back(rgb21);
    this->ColorSequence.emplace_back(rgb22);
    this->ColorSequence.emplace_back(rgb23);
    this->ColorSequence.emplace_back(rgb24);
    this->ColorSequence.emplace_back(rgb25);
    this->ColorSequence.emplace_back(rgb26);
    this->ColorSequence.emplace_back(rgb27);
    this->ColorSequence.emplace_back(rgb28);
    this->ColorSequence.emplace_back(rgb29);
    this->ColorSequence.emplace_back(rgb30);
    this->ColorSequence.emplace_back(rgb31);
    this->ColorSequence.emplace_back(rgb32);
    this->ColorSequence.emplace_back(rgb33);
    this->ColorSequence.emplace_back(rgb34);
    this->ColorSequence.emplace_back(rgb35);
    this->ColorSequence.emplace_back(rgb36);
    this->ColorSequence.emplace_back(rgb37);
    this->ColorSequence.emplace_back(rgb38);
    this->ColorSequence.emplace_back(rgb39);
    this->ColorSequence.emplace_back(rgb40);
    this->ColorSequence.emplace_back(rgb41);
    this->ColorSequence.emplace_back(rgb42);
    this->ColorSequence.emplace_back(rgb43);
}

KAAMStepScheme::KAAMStepScheme()
{
    this->ColorSpace = VTK_CTF_LAB;
    this->NanColor[0] = 1.0;
    this->NanColor[1] = 0.0;
    this->NanColor[2] = 0.0;
}

void KAAMStepScheme::PopulateRGBPoints()
{
    this->ColorSequence.reserve(12);

    std::array<double, 4> rgb1 = {0.083000000000000004,
                                  1.0,
                                  1.0,
                                  1.0};
    std::array<double, 4> rgb2 = {0.16600000000000001,
                                  1.0,
                                  0.0,
                                  0.0};
    std::array<double, 4> rgb3 = {0.25,
                                  0.0,
                                  1.0,
                                  0.0};
    std::array<double, 4> rgb4 = {0.33300000000000002,
                                  0.0,
                                  0.0,
                                  1.0};
    std::array<double, 4> rgb5 = {0.41599999999999998,
                                  1.0,
                                  1.0,
                                  0.0};
    std::array<double, 4> rgb6 = {0.5,
                                  1.0,
                                  0.0,
                                  1.0};
    std::array<double, 4> rgb7 = {0.58299999999999996,
                                  0.0,
                                  1.0,
                                  1.0};
    std::array<double, 4> rgb8 = {0.66600000000000004,
                                  0.63,
                                  0.63,
                                  1.0};
    std::array<double, 4> rgb9 = {0.75,
                                   0.67000000000000004,
                                   0.5,
                                   0.33000000000000002,};
    std::array<double, 4> rgb10 = {0.83299999999999996,
                                   1.0,
                                   0.5,
                                   0.75};
    std::array<double, 4> rgb11 = {0.91600000000000004,
                                   0.53000000000000003,
                                   0.34999999999999998,
                                   0.69999999999999996};
    std::array<double, 4> rgb12 = {1.0,
                                   1.0,
                                   0.75,
                                   0.5};

    this->ColorSequence.emplace_back(rgb1);
    this->ColorSequence.emplace_back(rgb2);
    this->ColorSequence.emplace_back(rgb3);
    this->ColorSequence.emplace_back(rgb4);
    this->ColorSequence.emplace_back(rgb5);
    this->ColorSequence.emplace_back(rgb6);
    this->ColorSequence.emplace_back(rgb7);
    this->ColorSequence.emplace_back(rgb8);
    this->ColorSequence.emplace_back(rgb9);
    this->ColorSequence.emplace_back(rgb10);
    this->ColorSequence.emplace_back(rgb11);
    this->ColorSequence.emplace_back(rgb12);
}

RainbowTecplotScheme::RainbowTecplotScheme()
{
    this->ColorSpace = VTK_CTF_HSV;
    this->NanColor[0] = 1.0;
    this->NanColor[1] = 0.0;
    this->NanColor[2] = 0.0;
}

void RainbowTecplotScheme::PopulateRGBPoints()
{
    this->ColorSequence.reserve(3);

    std::array<double, 4> rgb1 = {0.0,
                                  0.0,
                                  0.0,
                                  1.0};
    std::array<double, 4> rgb2 = {0.5,
                                  0.0,
                                  1.0,
                                  0.0};
    std::array<double, 4> rgb3 = {1.0,
                                  1.0,
                                  0.0,
                                  0.0};

    this->ColorSequence.emplace_back(rgb1);
    this->ColorSequence.emplace_back(rgb2);
    this->ColorSequence.emplace_back(rgb3);
}
