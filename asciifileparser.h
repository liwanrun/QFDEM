#ifndef ASCIIFILEPARSER_H
#define ASCIIFILEPARSER_H

#include<QString>

class vtkDoubleArray;
class ASCIIFileParser
{
public:
    ASCIIFileParser();

    void ParseVtkFile(QString fname);

    void ParsePolyFile(QString fname);

    vtkDoubleArray *GetPointArray();

private:
    vtkDoubleArray *array;
};

#endif // ASCIIFILEPARSER_H
