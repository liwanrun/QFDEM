#ifndef FDEMGEOMETRYSOURCE_H
#define FDEMGEOMETRYSOURCE_H

#include <vtkPolyDataAlgorithm.h>
#include <vtkDoubleArray.h>

#include <QString>

/* Super class */
class FDEMShapeSource : public vtkPolyDataAlgorithm
{
public:
    vtkTypeMacro(FDEMShapeSource,vtkPolyDataAlgorithm);

    void PrintSelf(ostream& os, vtkIndent indent) override;

    static FDEMShapeSource* New();

    void SetRawData(vtkDoubleArray* data);

//    vtkSetMacro(RawData, vtkDoubleArray*);
//    vtkGetMacro(RawData, vtkDoubleArray*);

protected:
    FDEMShapeSource();

    ~FDEMShapeSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    vtkDoubleArray *RawData;

private:
    FDEMShapeSource(const FDEMShapeSource&) = delete;

    void operator=(const FDEMShapeSource&) = delete;
};

/* Polygon Subclass */
class FDEMPolygonSource : public FDEMShapeSource
{
public:
    vtkTypeMacro(FDEMPolygonSource, FDEMShapeSource);

    static FDEMPolygonSource* New();

protected:
    FDEMPolygonSource();

    ~FDEMPolygonSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMPolygonSource(const FDEMPolygonSource&) = delete;

    void operator=(const FDEMPolygonSource&) = delete;
};

/* Ellipse Subclass */
class FDEMEllipseSource : public FDEMShapeSource
{
public:
    vtkTypeMacro(FDEMEllipseSource,FDEMShapeSource);

    static FDEMEllipseSource* New();

protected:
    FDEMEllipseSource();

    ~FDEMEllipseSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMEllipseSource(const FDEMEllipseSource&) = delete;

    void operator=(const FDEMEllipseSource&) = delete;
};

/* Segment Subclass */
class FDEMSegmentSource : public FDEMShapeSource
{
public:
    vtkTypeMacro(FDEMSegmentSource,FDEMShapeSource);

    static FDEMSegmentSource* New();

protected:
    FDEMSegmentSource();

    ~FDEMSegmentSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMSegmentSource(const FDEMSegmentSource&) = delete;

    void operator=(const FDEMSegmentSource&) = delete;
};


/* Point Subclass */
class FDEMPointSource : public FDEMShapeSource
{
public:
    vtkTypeMacro(FDEMPointSource,FDEMShapeSource);

    static FDEMPointSource* New();

protected:
    FDEMPointSource();

    ~FDEMPointSource() override;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
    FDEMPointSource(const FDEMPointSource&) = delete;

    void operator=(const FDEMPointSource&) = delete;
};


class ShapeFactory
{
public:
    vtkSmartPointer<FDEMShapeSource> create(QString type)
    {
        vtkSmartPointer<FDEMShapeSource> shape = nullptr;

        if(QString("Polygon") == type)
        {
            shape = vtkSmartPointer<FDEMPolygonSource>::New();
        }
        else if(QString("Ellipse") == type)
        {
            shape = vtkSmartPointer<FDEMEllipseSource>::New();
        }
        else if(QString("Segment") == type)
        {
            shape = vtkSmartPointer<FDEMSegmentSource>::New();
        }
        else if(QString("Point") == type)
        {
            shape = vtkSmartPointer<FDEMPointSource>::New();
        }

        return shape;
    }
};

#endif // FDEMGEOMETRYSOURCE_H
