#include "gmshmeshimporter.h"
#include "blockcollection.h"
#include "groupcollection.h"
#include "constrainedtriangulation.h"

#include <QFile>
#include <QMessageBox>

GmshMeshImporter::GmshMeshImporter()
{
    this->blockCollection = BlockCollection::GetBlockCollection();
    this->grainCollection = ElemSetCollection::GetElemSetCollection();
}

GmshMeshImporter::~GmshMeshImporter()
{

}

void GmshMeshImporter::ParseMeshFile()
{
    if(this->filename.isNull() || this->filename.isEmpty())
        return;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(nullptr,QString("File Opening Error"),
                             filename + QString("\ncannot be opened. \nPlease check it out!"));
        return;
    }

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

    file.close();
}

void GmshMeshImporter::ParseGmshMeshFormat(QTextStream &in)
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

void GmshMeshImporter::ParseGmshPhysicalNames(QTextStream &in)
{
    int numPhysicals = in.readLine().toInt();
    for(int idx = 0; idx < numPhysicals; idx++)
    {
        QStringList info = in.readLine().split(" ",QString::SkipEmptyParts);
        int dim = info.at(0).toInt();
        int tag = info.at(1).toInt();
        QString name = info.at(2);
        name.remove(0,1).chop(1);

        switch (dim)
        {
        case 0:
            continue;
        case 1:
            this->gmshPhysicalCurves.insert(tag, name);
        case 2:
            this->gmshPhysicalSurfaces.insert(tag, name);
        case 3:
            continue;
        }
    }
}

void GmshMeshImporter::ParseGmshEntities(QTextStream &in)
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
    for(int idx = 0; idx < numCurves; idx++)
    {
        QStringList info = in.readLine().split(" ", QString::SkipEmptyParts);
        int dim = 1;
        int tag = info.at(0).toInt();
        QVector<int> physical_tags;
        int numPhysicalTags = info.at(7).toInt();
        for(int i = 8; i < 8 + numPhysicalTags; i++)
        {
            int physical_tag = info.at(i).toInt();
            physical_tags.push_back(physical_tag);
        }

        GmshEntity entity = {dim, physical_tags};
        this->gmshCurves.insert(tag, entity);
    }

    /* Surface info */
    for(int idx = 0; idx < numSurfaces; idx++)
    {
        QStringList info = in.readLine().split(" ", QString::SkipEmptyParts);
        int dim = 2;
        int tag = info.at(0).toInt();
        int numPhysicalTags = info.at(7).toInt();
        QVector<int> physical_tags;
        for(int i = 8; i < 8 + numPhysicalTags; i++)
        {
            int physical_tag = info.at(i).toInt();
            physical_tags.push_back(physical_tag);
        }

        GmshEntity entity = {dim, physical_tags};
        this->gmshSurfaces.insert(tag, entity);
    }

    /* Volume info */
    for(int i = 0; i < numVolumes; i++)
        in.readLine();
}

void GmshMeshImporter::ParseGmshNodes(QTextStream &in)
{
    QStringList info = in.readLine().split(" ", QString::SkipEmptyParts);
    size_t numNodes = info.at(1).toULongLong();
    this->gmshNodes.reserve(numNodes);

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
            GmshNode node = {x, y, 0.0};
            this->gmshNodes.push_back(node);
        }
        numOffset += num;
    }

    this->gmshNodes.shrink_to_fit();
}

void GmshMeshImporter::ParseGmshElements(QTextStream &in)
{
    QStringList info = in.readLine().split(" ", QString::SkipEmptyParts);
    size_t numElements = info.at(1).toULongLong();

    size_t numOffset = 0;
    while(numOffset < numElements)
    {
        info = in.readLine().split(" ",QString::SkipEmptyParts);
        int entityDim = info.at(0).toInt();
        int entityTag = info.at(1).toInt();
        int elem_type = info.at(2).toInt();
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
                info = in.readLine().split(" ", QString::SkipEmptyParts);
                size_t in = info.at(1).toULongLong() - 1;
                size_t jn = info.at(2).toULongLong() - 1;
                QVector<size_t> nodeList({in, jn});
                GmshElement element = {elem_type, entityTag, nodeList};

                if(this->gmshCurves.contains(entityTag))
                {
                    for(int tag : this->gmshCurves.value(entityTag).physicalTags)
                    {
                        this->physicalCurveToElements[tag].push_back(element);
                    }
                }
            }
            numOffset += numElementsInBlock;
        }
        /* Surface */
        else if(2 == entityDim)
        {
            for(size_t i = 0; i < numElementsInBlock; i++)
            {
                info = in.readLine().split(" ", QString::SkipEmptyParts);
                size_t in = info.at(1).toULongLong() - 1;
                size_t jn = info.at(2).toULongLong() - 1;
                size_t kn = info.at(3).toULongLong() - 1;
                QVector<size_t> nodeList({in, jn, kn});
                GmshElement element = {elem_type, entityTag, nodeList};
                if(this->gmshSurfaces.contains(entityTag))
                {
                    for(int tag : this->gmshSurfaces.value(entityTag).physicalTags)
                    {
                        this->physicalSurfaceToElements[tag].push_back(element);
                    }
                }
            }
            numOffset += numElementsInBlock;
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

void GmshMeshImporter::CreateConstrainedTriangulation()
{
    this->CreateBlockTriangles();
    this->UpdateBlockConstraints();
    this->InitBlockGrainInformation();
    this->UpdateBlockGrains();
}

void GmshMeshImporter::CreateBlockTriangles()
{
    for(auto iter = physicalSurfaceToElements.begin(); iter != physicalSurfaceToElements.end(); ++iter)
    {
        if(!this->gmshPhysicalSurfaces.contains(iter.key())) continue;
        QString physicalName = gmshPhysicalSurfaces.value(iter.key());

        if(physicalName.startsWith(QString("block")))
        {
            QSharedPointer<ConstrainedTriangulation> triangulation =
                    QSharedPointer<ConstrainedTriangulation>(new ConstrainedTriangulation);
            triangulation->setMeshSourceFlag(Triangulation::IMPORTED);
            triangulation->setMeshFilterFlag(Triangulation::PRIMITIVE);
            CT& mesh = triangulation->GetCGALTriangulation();
            this->blockCollection->GetBlockTriangulation(physicalName) = triangulation;

            QVector<GmshElement> triangles = this->physicalSurfaceToElements[iter.key()];
            /* 1.Insert triangle as constraints */
            for(GmshElement triangle : triangles)
            {
                QVector<size_t> elemNodes = triangle.nodeTags;
                CT::Point p0 = CT::Point(this->gmshNodes[elemNodes[0]].x, this->gmshNodes[elemNodes[0]].y);
                CT::Point p1 = CT::Point(this->gmshNodes[elemNodes[1]].x, this->gmshNodes[elemNodes[1]].y);
                CT::Point p2 = CT::Point(this->gmshNodes[elemNodes[2]].x, this->gmshNodes[elemNodes[2]].y);
                mesh.insert_constraint(p0, p1);
                mesh.insert_constraint(p1, p2);
                mesh.insert_constraint(p2, p0);
            }

            /* 2.Update triangle information */
            for(GmshElement triangle : triangles)
            {
                QVector<size_t> elemNodes = triangle.nodeTags;
                CT::Point p0 = CT::Point(this->gmshNodes[elemNodes[0]].x, this->gmshNodes[elemNodes[0]].y);
                CT::Point p1 = CT::Point(this->gmshNodes[elemNodes[1]].x, this->gmshNodes[elemNodes[1]].y);
                CT::Point p2 = CT::Point(this->gmshNodes[elemNodes[2]].x, this->gmshNodes[elemNodes[2]].y);
                CT::Point pt = CGAL::centroid(CT::Triangle(p0, p1, p2));
                CT::Face_handle fh = mesh.locate(pt);
                fh->info().in_domain = true;
                fh->info().fid = physicalName.split(QChar('-')).last().toInt();
            }

            /* 3.Update triangulation info */
            for(auto fiter = mesh.all_faces_begin(); fiter != mesh.all_faces_end(); ++fiter)
            {
                fiter->set_constraints(false, false, false);
            }
            triangulation->InitTriangulationNodeIds();
            triangulation->InitTriangulationFaceIds();
        }
    }
}

void GmshMeshImporter::UpdateBlockGrains()
{
    for(auto iter = physicalSurfaceToElements.begin(); iter != physicalSurfaceToElements.end(); ++iter)
    {
        if(!this->gmshPhysicalSurfaces.contains(iter.key())) continue;
        QString physicalName = gmshPhysicalSurfaces.value(iter.key());

        if(physicalName.startsWith(QString("grain")))
        {
            QStringList names = physicalName.split(QChar('-'), QString::SkipEmptyParts);
            QString blockId = QString("block-") + names.last();
            QSharedPointer<Triangulation> t = this->blockCollection->GetBlockTriangulation(blockId);
            ConstrainedTriangulation* ct = static_cast<ConstrainedTriangulation*>(t.get());
            CT& mesh = ct->GetCGALTriangulation();

            QVector<GmshElement> triangles = this->physicalSurfaceToElements[iter.key()];
            for(GmshElement triangle : triangles)
            {
                QVector<size_t> elemNodes = triangle.nodeTags;
                CT::Point p0 = CT::Point(this->gmshNodes[elemNodes[0]].x, this->gmshNodes[elemNodes[0]].y);
                CT::Point p1 = CT::Point(this->gmshNodes[elemNodes[1]].x, this->gmshNodes[elemNodes[1]].y);
                CT::Point p2 = CT::Point(this->gmshNodes[elemNodes[2]].x, this->gmshNodes[elemNodes[2]].y);
                CT::Point pt = CGAL::centroid(CT::Triangle(p0, p1, p2));
                CT::Face_handle fh = mesh.locate(pt);
                fh->info().gid = names[1].toInt() + t->grainOffs;
            }
        }
    }
}

void GmshMeshImporter::InitBlockGrainInformation()
{
    QStringList physicalNames = this->gmshPhysicalSurfaces.values();
    for(QString physicalName : physicalNames)
    {
        QStringList names = physicalName.split(QChar('-'), QString::SkipEmptyParts);
        if(names[0] == QString("grain"))
        {
            QString blockId = QString("block-") + names[2];
            blockCollection->GetBlockTriangulation(blockId)->numGrains += 1;
        }
    }

    size_t offset = 0;
    BlockCollection::BlockMap block_map = blockCollection->GetBlockMap();
    for(auto iter = block_map.begin(); iter != block_map.end(); ++iter)
    {
        blockCollection->GetBlockTriangulation(iter.key())->grainOffs = offset;
        offset += blockCollection->GetBlockTriangulation(iter.key())->numGrains;
    }
    blockCollection->setBlockGrainSize(offset);
}

void GmshMeshImporter::UpdateBlockConstraints()
{
    for(auto iter = physicalCurveToElements.begin(); iter != physicalCurveToElements.end(); ++iter)
    {
        if(!this->gmshPhysicalCurves.contains(iter.key())) continue;
        QString physicalName = gmshPhysicalCurves.value(iter.key());

        if(physicalName.startsWith(QString("constraint")))
        {
            QString blockId = physicalName.replace(QString("constraint"), QString("block"));
            QSharedPointer<Triangulation> t = this->blockCollection->GetBlockTriangulation(blockId);
            ConstrainedTriangulation* ct = static_cast<ConstrainedTriangulation*>(t.get());
            std::set<CT::Edge> constrainedEdges = ct->GetIntrinsicConstrainedEdges();
            CT& mesh = ct->GetCGALTriangulation();

            QVector<GmshElement> segments = this->physicalCurveToElements[iter.key()];
            for(GmshElement segment : segments)
            {
                QVector<size_t> elemNodes = segment.nodeTags;
                CT::Point p0 = CT::Point(this->gmshNodes[elemNodes[0]].x, this->gmshNodes[elemNodes[0]].y);
                CT::Point p1 = CT::Point(this->gmshNodes[elemNodes[1]].x, this->gmshNodes[elemNodes[1]].y);
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
                        constrainedEdges.insert(CT::Edge(fh, index));
                        constrainedEdges.insert(CT::Edge(fn, fn->index(fh)));
                    }
                }
            }
        }
    }
}
