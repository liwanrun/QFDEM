#include "hybridmeshimporter.h"
#include "blockcollection.h"
#include "hybridmeshtriangulation.h"

#include <QFile>
#include <QMessageBox>

HybridMeshImporter::HybridMeshImporter()
{
    this->blockCollection = BlockCollection::GetBlockCollection();
}

HybridMeshImporter::~HybridMeshImporter()
{

}

void HybridMeshImporter::ParseMeshFile()
{
    // Invalid file name
    if(this->filename.isNull() || this->filename.isEmpty())
        return;

    QFile file(filename);

    // File opening error
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(nullptr,QString("File Opening Error"),
                             filename + QString("\ncannot be opened. \nPlease check it out!"));
        return;
    }

    // Input file stream
    QTextStream in(&file);
    do
    {
        QString lineInfo = in.readLine();

        if(lineInfo.isNull() || lineInfo.isEmpty())
            continue;

        if(lineInfo.startsWith(QLatin1String("$MeshFormat")))
        {
            this->ParseGmshMeshFormat(in);
        }
        else if(lineInfo.startsWith(QLatin1String("$PhysicalNames")))
        {
            this->ParseGmshPhysicalNames(in);
        }
        else if(lineInfo.startsWith(QLatin1String("$Entities")))
        {
            this->ParseGmshEntities(in);
        }
        else if(lineInfo.startsWith(QLatin1String("$Nodes")))
        {
            this->ParseGmshNodes(in);
        }
        else if(lineInfo.startsWith(QLatin1String("$Elements")))
        {
            this->ParseGmshElements(in);
        }
        else if(lineInfo.isNull() || lineInfo.isEmpty())
            continue;

    } while(!in.atEnd());
}

void HybridMeshImporter::CreateConstrainedTriangulation()
{
    QString blockId = QString("block-1");
    QSharedPointer<HybridMeshTriangulation> triangulation =
            QSharedPointer<HybridMeshTriangulation>(new HybridMeshTriangulation);
    this->blockCollection->GetBlockTriangulation(blockId) = triangulation;

    triangulation->points = this->points;
    triangulation->triangles = this->triangles;
    triangulation->quads = this->quads;

    triangulation->setMeshSourceFlag(Triangulation::IMPORTED);
    triangulation->setMeshFilterFlag(Triangulation::THICKQUAD);
}

void HybridMeshImporter::ParseGmshMeshFormat(QTextStream &in)
{
    QString lineInfo = in.readLine();
    QStringList fmtInfo = lineInfo.split(" ",QString::SkipEmptyParts);
    if(fmtInfo.at(0) != QLatin1String("4.1"))
    {
        QMessageBox::warning(nullptr,
                             QString("Format error"),
                             QString("The format is incompatible. \nPlease select correct version."));
        return;
    }
}

void HybridMeshImporter::ParseGmshPhysicalNames(QTextStream &in)
{
    int numPhysical = in.readLine().toInt();
    for(int i = 0; i < numPhysical; i++)
    {
        QStringList info = in.readLine().split(" ",QString::SkipEmptyParts);
        int tag = info.at(1).toInt();
        QString physicalName = info.at(2);
        physicalName.remove(0,1).chop(1);               /* remove " from begin and end */
        this->physicalGroups.insert(tag, physicalName);
    }
}

void HybridMeshImporter::ParseGmshEntities(QTextStream &in)
{
    QStringList info = in.readLine().split(" ", QString::SkipEmptyParts);
    int numPoints = info.at(0).toInt();
    int numCurves = info.at(1).toInt();
    int numSurfaces = info.at(2).toInt();
    int numVolumes = info.at(3).toInt();

    /* Point info */
    for(int i = 0; i < numPoints; i++)
        in.readLine();

    /* Curve info */
    for(int i = 0; i < numCurves; i++)
        in.readLine();

    /* Surface info */
    for(int i = 0; i < numSurfaces; i++)
        in.readLine();

    /* Volume info */
    for(int i = 0; i < numVolumes; i++)
        in.readLine();
}

void HybridMeshImporter::ParseGmshNodes(QTextStream &in)
{
    QStringList info = in.readLine().split(" ", QString::SkipEmptyParts);
    size_t numNodes = info.at(1).toULongLong();
    this->points.reserve(numNodes);

    size_t numOffset = 0;
    while(numOffset < numNodes)
    {
        size_t num = in.readLine().split(" ", QString::SkipEmptyParts).at(3).toULongLong();
        for(size_t i = 0; i < num; i++)
            in.readLine();

        for(size_t i = 0; i < num; i++)
        {
            QStringList info = in.readLine().split(" ", QString::SkipEmptyParts);
            double x = info.at(0).toDouble();
            double y = info.at(1).toDouble();
            this->points.emplace_back(QPointF(x,y));
        }

        numOffset += num;
    }

    this->points.shrink_to_fit();
}

void HybridMeshImporter::ParseGmshElements(QTextStream &in)
{
    QStringList info = in.readLine().split(" ", QString::SkipEmptyParts);
    size_t numElements = info.at(1).toULongLong();
    size_t numOffset = 0;
    while(numOffset < numElements)
    {
        info = in.readLine().split(" ",QString::SkipEmptyParts);
        int entityDim = info.at(0).toInt();
        size_t numElementsInBlock = info.at(3).toULongLong();

        /* Point */
        if(0 == entityDim)
        {
            for(size_t i = 0; i < numElementsInBlock; i++)
            {
                in.readLine();
            }
            numOffset += numElementsInBlock;
        }
        /* Curve */
        else if(1 == entityDim)
        {
            for(size_t i = 0; i < numElementsInBlock; i++)
            {
                in.readLine();
            }
            numOffset += numElementsInBlock;
        }
        /* Surface */
        else if(2 == entityDim)
        {
            int elemType = info.at(2).toInt();
            if(2 == elemType)
            {
                for(size_t i = 0; i < numElementsInBlock; i++)
                {
                    std::array<size_t, 3> triangle;
                    info = in.readLine().split(" ", QString::SkipEmptyParts);
                    triangle[0] = info.at(1).toULongLong() - 1;
                    triangle[1] = info.at(2).toULongLong() - 1;
                    triangle[2] = info.at(3).toULongLong() - 1;
                    triangles.emplace_back(triangle);
                }
                numOffset += numElementsInBlock;
            }
            else if(3 == elemType)
            {
                for(size_t i = 0; i < numElementsInBlock; i++)
                {
                    std::array<size_t, 4> quad;
                    info = in.readLine().split(" ", QString::SkipEmptyParts);
                    quad[0] = info.at(1).toULongLong() - 1;
                    quad[1] = info.at(2).toULongLong() - 1;
                    quad[2] = info.at(3).toULongLong() - 1;
                    quad[3] = info.at(4).toULongLong() - 1;
                    quads.emplace_back(quad);
                }
                numOffset += numElementsInBlock;
            }
        }
        /* Volume */
        else if(3 == entityDim)
        {
            for(size_t i = 0; i < numElementsInBlock; i++)
            {
                in.readLine();
            }
            numOffset += numElementsInBlock;
        }
    }
}
