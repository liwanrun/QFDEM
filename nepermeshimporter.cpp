#include "nepermeshimporter.h"
#include "blockcollection.h"
#include "groupcollection.h"
#include "constrainedtriangulation.h"

#include <QFile>
#include <QMessageBox>

NeperMeshImporter::NeperMeshImporter()
{
    this->blockCollection = BlockCollection::GetBlockCollection();
}

NeperMeshImporter::~NeperMeshImporter()
{

}

void NeperMeshImporter::ParseMeshFile()
{
    // Invalid file name
    if(this->filename.isNull() || this->filename.isEmpty())
        return;

    QFile file(filename);

    // File opening error
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(nullptr,
                             QString("File Opening Error"),
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
            this->ParseNeperMeshFormat(in);
        }
        else if(lineInfo.startsWith(QLatin1String("$Nodes")))
        {
            this->ParseNeperNodes(in);
        }
        else if(lineInfo.startsWith(QLatin1String("$Elements")))
        {
            this->ParseNeperElements(in);
        }
        else if(lineInfo.startsWith(QLatin1String("$NSets")))
        {
            this->ParseNeperNodeSets(in);
        }
        else if(lineInfo.startsWith(QLatin1String("$PhysicalNames")))
        {
            this->ParseNeperPhysicalNames(in);
        }
        else if(lineInfo.startsWith(QLatin1String("$ElsetOrientations")))
        {
            this->ParseElsetOrientations(in);
        }
        else if(lineInfo.isNull() || lineInfo.isEmpty())
            continue;

    } while(!in.atEnd());
}

void NeperMeshImporter::CreateConstrainedTriangulation()
{
    int numBlocks = this->blockCollection->GetBlockMap().size();
    QString blockId = QString("block-%1").arg(numBlocks + 1);
    QSharedPointer<ConstrainedTriangulation> triangulation =
            QSharedPointer<ConstrainedTriangulation>(new ConstrainedTriangulation);
    this->blockCollection->GetBlockTriangulation(blockId) = triangulation;
    std::set<CT::Edge> constrainedEdges = triangulation->GetIntrinsicConstrainedEdges();
    CT& mesh = triangulation->GetCGALTriangulation();

    //1. Insert triangle cells
    for(auto iter = this->physicalFacesMap.begin(); iter != this->physicalFacesMap.end(); ++iter)
    {
        vector<array<size_t, 3>> triangles = iter.value();
        for(size_t i = 0; i < triangles.size(); i++)
        {
            array<size_t, 3> ids = triangles[i];
            CT::Point p0 = CT::Point(this->points[ids.at(0)].x(), this->points[ids.at(0)].y());
            CT::Point p1 = CT::Point(this->points[ids.at(1)].x(), this->points[ids.at(1)].y());
            CT::Point p2 = CT::Point(this->points[ids.at(2)].x(), this->points[ids.at(2)].y());
            mesh.insert_constraint(p0, p1);
            mesh.insert_constraint(p1, p2);
            mesh.insert_constraint(p2, p0);
        }
    }

    //2. Set face in_domain info
    for(auto iter = this->physicalFacesMap.begin(); iter != this->physicalFacesMap.end(); ++iter)
    {
        vector<array<size_t, 3>> triangles = iter.value();
        for(size_t i = 0; i < triangles.size(); i++)
        {
            array<size_t, 3> ids = triangles[i];
            CT::Point p0 = CT::Point(this->points[ids.at(0)].x(), this->points[ids.at(0)].y());
            CT::Point p1 = CT::Point(this->points[ids.at(1)].x(), this->points[ids.at(1)].y());
            CT::Point p2 = CT::Point(this->points[ids.at(2)].x(), this->points[ids.at(2)].y());
            CT::Point pt = CGAL::centroid(CT::Triangle(p0, p1, p2));
            CT::Face_handle fh = mesh.locate(pt);
            fh->info().in_domain = true;
            fh->info().fid = iter.key();
        }
    }

    //3. Update constrained edges
    for(auto fiter = mesh.all_faces_begin(); fiter != mesh.all_faces_end(); ++fiter)
    {
        fiter->set_constraints(false, false, false);
    }

    for(auto iter = this->physicalEdgesMap.begin(); iter != this->physicalEdgesMap.end(); ++iter)
    {
        vector<array<size_t, 2>> segments = iter.value();
        for(size_t j = 0; j < segments.size(); j++)
        {
            array<size_t, 2> ids = segments[j];
            CT::Point p0 = CT::Point(this->points[ids.at(0)].x(), this->points[ids.at(0)].y());
            CT::Point p1 = CT::Point(this->points[ids.at(1)].x(), this->points[ids.at(1)].y());
            CT::Vertex_handle v0 = mesh.insert(p0);
            CT::Vertex_handle v1 = mesh.insert(p1);

            int index;
            CT::Face_handle fh;
            if(mesh.is_edge(v0, v1, fh, index))
            {
                CT::Face_handle fn = fh->neighbor(index);
                if(fh->info().in_domain && fn->info().in_domain)
                {
                    fh->set_constraint(index, true);
                    fn->set_constraint(fn->index(fh), true);
                    constrainedEdges.emplace(CT::Edge(fh, index));
                    constrainedEdges.emplace(CT::Edge(fn, fn->index(fh)));
                }
            }
        }
    }

    triangulation->InitTriangulationNodeIds();
    triangulation->InitTriangulationFaceIds();
    triangulation->setMeshSourceFlag(Triangulation::IMPORTED);
    triangulation->setMeshFilterFlag(Triangulation::PRIMITIVE);

    this->blockCollection->setBlockGrainSize(this->physicalFacesMap.size());
}

void NeperMeshImporter::ParseNeperMeshFormat(QTextStream &in)
{
    QString lineInfo = in.readLine();
    QStringList fmtInfo = lineInfo.split(" ", QString::SkipEmptyParts);
    if(fmtInfo.at(0) != QLatin1String("2.2"))
    {
        QMessageBox::warning(nullptr,
                             QString("Format error"),
                             QString("The format is incompatible. \nPlease select correct version."));
        return;
    }
}

void NeperMeshImporter::ParseNeperPhysicalNames(QTextStream &in)
{
    int numPhysicals = in.readLine().toInt();
    if(!numPhysicals) return;

    QString lineInfo = in.readLine();
    while(lineInfo != QString("$EndPhysicalNames"))
    {
        QStringList info = lineInfo.split(" ", QString::SkipEmptyParts);

        lineInfo = in.readLine();
    }
}

void NeperMeshImporter::ParseElsetOrientations(QTextStream &in)
{
    QStringList info = in.readLine().split(" ", QString::SkipEmptyParts);
    for(int i = 0; i < info.at(0); i++)
    {
        in.readLine();
    }
}

void NeperMeshImporter::ParseNeperNodeSets(QTextStream &in)
{
    int numSets = in.readLine().toInt();
    if(!numSets) return;

    for(int i = 0; i < numSets; i++)
    {
        QString id = in.readLine();
        int numNodes = in.readLine().toInt();
        for(int j = 0; j < numNodes; j++)
        {
            size_t index = in.readLine().toULongLong();
            this->boundaryNodeSets[id].insert(index - 1);
        }
    }
}

void NeperMeshImporter::ParseNeperNodes(QTextStream &in)
{
    size_t numNodes = in.readLine().toULongLong();
    this->points.reserve(numNodes);

    for(size_t i = 0; i < numNodes; i++)
    {
        QStringList info = in.readLine().split(" ", QString::SkipEmptyParts);
        double x = info.at(1).toDouble();
        double y = info.at(2).toDouble();
        this->points.emplace_back(QPointF(x,y));
    }

    this->points.shrink_to_fit();
}

void NeperMeshImporter::ParseNeperElements(QTextStream &in)
{
    size_t numElements = in.readLine().toULongLong();
    for(size_t i = 0; i < numElements; i++)
    {
        QStringList info = in.readLine().split(" ", QString::SkipEmptyParts);
        int elemType = info.at(1).toInt();

        int entityTag;
        int physicalTag;

        if(15 == elemType)      /* 1-node point */
            continue;

        if(1 == elemType)       /* 2-node line */
        {
            physicalTag = info.at(3).toInt();
            entityTag = info.at(4).toInt();
            size_t in = info.at(6).toInt() - 1;
            size_t jn = info.at(7).toInt() - 1;

            this->edgeEntityPhysicalMap[entityTag] = physicalTag;
            this->physicalEdgesMap[physicalTag].emplace_back(array<size_t, 2>{in, jn});
        }
        else if(2 == elemType)   /* 3-node triangle */
        {
            physicalTag = info.at(3).toInt();
            entityTag = info.at(4).toInt();
            size_t in = info.at(6).toInt() - 1;
            size_t jn = info.at(7).toInt() - 1;
            size_t kn = info.at(8).toInt() - 1;

            this->faceEntityPhysicalMap[entityTag] = physicalTag;
            this->physicalFacesMap[physicalTag].emplace_back(array<size_t, 3>{in, jn, kn});
        }
    }
}
