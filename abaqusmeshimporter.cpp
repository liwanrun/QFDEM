#include "abaqusmeshimporter.h"
#include "blockcollection.h"
#include "constrainedtriangulation.h"

#include <QFile>
#include <QMessageBox>

AbaqusMeshImporter::AbaqusMeshImporter()
{
    this->blockCollection = BlockCollection::GetBlockCollection();

    this->translation[0] = 0.0;
    this->translation[1] = 0.0;
    this->translation[2] = 0.0;
}

void AbaqusMeshImporter::ParseMeshFile()
{
    if((!this->filename.isNull()) && (!this->filename.isEmpty()))
    {
        QFile file(this->filename);
        if(! file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(nullptr, QString("File Opening Error"),
                                 filename + QString("\ncannot be opened. \nPlease check it out!"));
            return;
        }

        QTextStream in(&file);
        do
        {
            QString lineInfo = in.readLine();

            if(lineInfo.isNull() || lineInfo.isEmpty())
                continue;

            if(lineInfo.startsWith(QLatin1String("**")))
                continue;

            if(lineInfo.startsWith(QLatin1String("*Part")))
            {
                this->parserStack.push(lineInfo);
                continue;
            }

            if(lineInfo.startsWith(QLatin1String("*Assembly")))
            {
                this->parserStack.push(lineInfo);
                continue;
            }

            if(lineInfo.startsWith(QLatin1String("*Instance")))
            {
                this->parserStack.push(lineInfo);
                this->translation[0] = 0.0;
                this->translation[1] = 0.0;
                this->translation[2] = 0.0;

                lineInfo = in.readLine();
                while(!lineInfo.startsWith(QLatin1String("*")))
                {
                    QStringList matrix = lineInfo.split(QString(","), QString::SkipEmptyParts);
                    if(3 == matrix.size())
                    {
                        this->translation[0] = matrix[0].toDouble();
                        this->translation[1] = matrix[1].toDouble();
                        this->translation[2] = 0.0;
                    }

                    lineInfo = in.readLine();
                }
            }

            if(lineInfo == QLatin1String("*Node"))
            {
                QString text = this->parserStack.top();
                QString name = text.split(QString(","), QString::SkipEmptyParts).at(1);

                vector<array<double, 3>> points;
                if(text.startsWith(QLatin1String("*Part")))
                {
                    lineInfo = this->ParseAsteriskNode(in, points);
                    this->partGeometryMap.insert(name, points);
                }
                else if(text.startsWith(QLatin1String("*Instance")))
                {
                    lineInfo = this->ParseAsteriskNode(in, points);
                    for(size_t i = 0; i < points.size(); i++)
                    {
                        points[i][0] += this->translation[0];
                        points[i][1] += this->translation[1];
                        points[i][2] += this->translation[2];
                    }

                    this->instanceGeometryMap.insert(name, points);
                }
            }

            if(lineInfo.startsWith(QLatin1String("*Element")))
            {
                QString text = this->parserStack.top();
                QString name = text.split(QString(","), QString::SkipEmptyParts).at(1);

                vector<array<size_t, 3>> triangles;
                lineInfo = this->ParseAsteriskElement(in, triangles);

                if(text.startsWith(QLatin1String("*Part")))
                {
                    this->partTriangleMap.insert(name, triangles);
                }
                else if(text.startsWith(QLatin1String("*Instance")))
                {
                    this->instanceTriangleMap.insert(name, triangles);
                }
            }

            if(lineInfo == QLatin1String("*End Part"))
            {
                this->parserStack.pop();
                continue;
            }

            if(lineInfo == QLatin1String("*End Instance"))
            {
                QString text = this->parserStack.top();
                QString name = text.split(QString(","), QString::SkipEmptyParts).at(1);
                QString part = text.split(QString(","), QString::SkipEmptyParts).at(2);
                part = part.replace(QLatin1String("part"), QLatin1String("name"));

                if(!this->instanceGeometryMap.contains(name))
                {
                    vector<array<double, 3>> points = this->partGeometryMap[part];
                    for(size_t i = 0; i < points.size(); i++)
                    {
                        points[i][0] += this->translation[0];
                        points[i][1] += this->translation[1];
                        points[i][2] += this->translation[2];
                    }

                    this->instanceGeometryMap.insert(name, points);
                }

                if(!this->instanceTriangleMap.contains(name))
                {
                    vector<array<size_t, 3>> triangles = this->partTriangleMap[part];
                    this->instanceTriangleMap.insert(name, triangles);
                }

                this->parserStack.pop();
                continue;
            }

            if(lineInfo == QLatin1String("*End Assembly"))
            {
                this->parserStack.pop();
                break;
            }
        } while(!in.atEnd());

        file.close();
    }
}

void AbaqusMeshImporter::CreateConstrainedTriangulation()
{
    int id = 1;
    int numBlocks = this->blockCollection->GetBlockMap().size();
    for(auto iter = this->instanceTriangleMap.begin(); iter != this->instanceTriangleMap.end(); ++iter)
    {
        QString blockId = QString("block-%1").arg(numBlocks + id);
        QSharedPointer<ConstrainedTriangulation> triangulation =
                QSharedPointer<ConstrainedTriangulation>(new ConstrainedTriangulation);
        this->blockCollection->GetBlockTriangulation(blockId) = triangulation;
        CT& mesh = triangulation->GetCGALTriangulation();
        id += 1;

        //1. Insert triangle cells
        vector<array<double, 3>> points = this->instanceGeometryMap[iter.key()];
        vector<array<size_t, 3>> triangles = iter.value();
        for(size_t i = 0; i < triangles.size(); i++)
        {
            array<size_t, 3> ids = triangles[i];
            CT::Point p0 = CT::Point(points[ids.at(0)][0], points[ids.at(0)][1]);
            CT::Point p1 = CT::Point(points[ids.at(1)][0], points[ids.at(1)][1]);
            CT::Point p2 = CT::Point(points[ids.at(2)][0], points[ids.at(2)][1]);
            mesh.insert_constraint(p0, p1);
            mesh.insert_constraint(p1, p2);
            mesh.insert_constraint(p2, p0);
        }

        //2. Set face in_domain info
        for(size_t i = 0; i < triangles.size(); i++)
        {
            array<size_t, 3> ids = triangles[i];
            CT::Point p0 = CT::Point(points[ids.at(0)][0], points[ids.at(0)][1]);
            CT::Point p1 = CT::Point(points[ids.at(1)][0], points[ids.at(1)][1]);
            CT::Point p2 = CT::Point(points[ids.at(2)][0], points[ids.at(2)][1]);
            CT::Point pt = CGAL::centroid(CT::Triangle(p0, p1, p2));
            CT::Face_handle fh = mesh.locate(pt);
            fh->info().in_domain = true;
        }

        //3. Update constrained edges
        for(auto fiter = mesh.all_faces_begin(); fiter != mesh.all_faces_end(); ++fiter)
        {
            fiter->set_constraints(false, false, false);
        }

        triangulation->InitTriangulationNodeIds();
        triangulation->InitTriangulationFaceIds();
        triangulation->setMeshSourceFlag(Triangulation::IMPORTED);
        triangulation->setMeshFilterFlag(Triangulation::PRIMITIVE);
    }
}

QString AbaqusMeshImporter::ParseAsteriskNode(QTextStream &in, vector<array<double, 3>> &points)
{
    QString lineInfo;
    do
    {
      lineInfo = in.readLine();
      if(lineInfo.startsWith(QLatin1String("*")))
      {
          points.shrink_to_fit();
          break;
      }

      QStringList infoList = lineInfo.split(",",QString::SkipEmptyParts);
      double x = infoList.at(1).toDouble();
      double y = infoList.at(2).toDouble();
      double z = 0.0 + this->translation[2];
      points.emplace_back(array<double, 3>{x, y, z});

    }while(true);

    return lineInfo;
}

QString AbaqusMeshImporter::ParseAsteriskElement(QTextStream &in, vector<array<size_t, 3>> &triangles)
{
    QString lineInfo;

    do
    {
      lineInfo = in.readLine();
      if(lineInfo.startsWith(QLatin1String("*")))
      {
          triangles.shrink_to_fit();
          break;
      }

      QStringList infoList = lineInfo.split(",", QString::SkipEmptyParts);
      size_t in = infoList.at(1).toULongLong() - 1;
      size_t jn = infoList.at(2).toULongLong() - 1;
      size_t kn = infoList.at(3).toULongLong() - 1;
      triangles.emplace_back(array<size_t, 3>{in, jn, kn});

    }while(true);

    return lineInfo;
}
