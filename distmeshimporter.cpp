#include "distmeshimporter.h"
#include "blockcollection.h"
#include "groupcollection.h"
#include "constrainedtriangulation.h"

#include <QFile>
#include <QMessageBox>

DistMeshImporter::DistMeshImporter() : numVerts(0), numFaces(0), numEdges(0)
{
    this->blockCollection = BlockCollection::GetBlockCollection();
}

DistMeshImporter::~DistMeshImporter()
{

}

void DistMeshImporter::ParseMeshFile()
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

        if(lineInfo.startsWith(QLatin1String("element vertex")))
        {
            this->numVerts = lineInfo.split(QChar(' '),QString::SkipEmptyParts).last().toInt();
            this->polyVerts.reserve(this->numVerts);
        }
        else if(lineInfo.startsWith(QLatin1String("element face")))
        {
            this->numFaces = lineInfo.split(QChar(' '),QString::SkipEmptyParts).last().toInt();
            this->polyFaces.reserve(this->numFaces);
        }
        else if(lineInfo.startsWith(QLatin1String("element edge")))
        {
            this->numEdges = lineInfo.split(QChar(' '),QString::SkipEmptyParts).last().toInt();
            this->polyEdges.reserve(this->numEdges);
        }
        else if(lineInfo.startsWith(QLatin1String("end_header")))
        {
            for(int i = 0; i < this->numVerts; i++)
            {
                lineInfo = in.readLine();
                QStringList info = lineInfo.split(QChar(' '), QString::SkipEmptyParts);
                double x = info.at(0).toDouble();
                double y = info.at(1).toDouble();
                double z = info.at(2).toDouble();
                this->polyVerts.push_back(std::array<double, 3>{x, y, z});
            }

            for(int i = 0; i < this->numFaces; i++)
            {
                lineInfo = in.readLine();
                QStringList info = lineInfo.split(QChar(' '), QString::SkipEmptyParts);
                int l = info.at(1).toInt();
                int m = info.at(2).toInt();
                int n = info.at(3).toInt();
                this->polyFaces.push_back(std::array<int, 3>{l, m, n});
            }

            for(int i = 0; i < this->numEdges; i++)
            {
                lineInfo = in.readLine();
                QStringList info = lineInfo.split(QChar(' '), QString::SkipEmptyParts);
                int j = info.at(0).toInt();
                int k = info.at(1).toInt();
                this->polyEdges.push_back(std::array<int, 2>{j, k});
            }
        }
        else if(lineInfo.isNull() || lineInfo.isEmpty())
            continue;

    } while(!in.atEnd());

    file.close();
}

void DistMeshImporter::CreateConstrainedTriangulation()
{
    this->CreateBlockTriangles();
    this->UpdateBlockConstraints();
}

void DistMeshImporter::CreateBlockTriangles()
{
    int numBlks = this->blockCollection->GetBlockMap().size();
    QString blockId = QString("block-%1").arg(numBlks + 1);
    QSharedPointer<ConstrainedTriangulation> triangulation =
            QSharedPointer<ConstrainedTriangulation>(new ConstrainedTriangulation);
    triangulation->setMeshSourceFlag(Triangulation::IMPORTED);
    triangulation->setMeshFilterFlag(Triangulation::PRIMITIVE);
    CT& mesh = triangulation->GetCGALTriangulation();
    this->blockCollection->GetBlockTriangulation(blockId) = triangulation;

    /* 1.Insert triangle as constraints */
    for(std::array<int, 3> tri : this->polyFaces)
    {
        CT::Point p0 = CT::Point(polyVerts[tri[0]][0], polyVerts[tri[0]][1]);
        CT::Point p1 = CT::Point(polyVerts[tri[1]][0], polyVerts[tri[1]][1]);
        CT::Point p2 = CT::Point(polyVerts[tri[2]][0], polyVerts[tri[2]][1]);
        mesh.insert_constraint(p0, p1);
        mesh.insert_constraint(p1, p2);
        mesh.insert_constraint(p2, p0);
    }

    /* 2.Update triangle information */
    for(std::array<int, 3> tri : this->polyFaces)
    {
        CT::Point p0 = CT::Point(polyVerts[tri[0]][0], polyVerts[tri[0]][1]);
        CT::Point p1 = CT::Point(polyVerts[tri[1]][0], polyVerts[tri[1]][1]);
        CT::Point p2 = CT::Point(polyVerts[tri[2]][0], polyVerts[tri[2]][1]);
        CT::Point pt = CGAL::centroid(CT::Triangle(p0, p1, p2));
        CT::Face_handle fh = mesh.locate(pt);
        fh->info().in_domain = true;
        fh->info().fid = (numBlks + 1);
    }

    /* 3.Update triangulation info */
    for(auto fiter = mesh.all_faces_begin(); fiter != mesh.all_faces_end(); ++fiter)
    {
        fiter->set_constraints(false, false, false);
    }
    triangulation->InitTriangulationNodeIds();
    triangulation->InitTriangulationFaceIds();
}

void DistMeshImporter::UpdateBlockConstraints()
{

}
