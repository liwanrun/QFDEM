#include "groupcollection.h"
#include "group.h"
#include "globalmacros.h"

#include <QDebug>

GroupCollection::GroupCollection()
{

}

GroupCollection::~GroupCollection()
{

}

const GroupCollection::GroupMap &GroupCollection::GetGroupMap() const
{
    return this->group_map;
}

void GroupCollection::ClearGroupMap()
{
    this->group_map.clear();
}

QSharedPointer<Group> GroupCollection::GetGroup(int id)
{
    if(!this->group_map.contains(id))
    {
        this->AppendGroup(id);
    }
    return this->group_map[id];
}

void GroupCollection::RemoveGroup(int id)
{
    this->group_map.remove(id);
}

void GroupCollection::UpdateGroup(int id, QSharedPointer<Group> group)
{
    if(this->group_map.contains(id))
    {
        this->group_map[id] = group;
    }
}

size_t GroupCollection::GetNumberOfIds()
{
    size_t size = 0;
    for(QSharedPointer<Group> group : this->group_map)
    {
        size += group->getSize();
    }

    return size;
}

size_t GroupCollection::GetGroupSize()
{
    return this->group_map.size();
}

void GroupCollection::OutputGroupPedigreeIds(QTextStream &out, QString arg)
{
    int field = 10;
    int ncols = 10;
    int index = 0;

    for(auto iter = this->group_map.begin(); iter != this->group_map.end(); ++iter)
    {
        for (int id = 0; id < iter.value()->pedigreeIds.size(); id++)
        {
            if((index != 0) && (index % ncols == 0))
                out << reset << endl;

            out << qSetFieldWidth(field) << left << iter.value()->pedigreeIds[id];
            index += 1;
        }
    }

    out << reset << endl;
}


QMutex NodeSetCollection::mutex;

QSharedPointer<NodeSetCollection> NodeSetCollection::nodeSetCollection = nullptr;

NodeSetCollection::~NodeSetCollection()
{
    qDebug() << "~NodeSetCollection()";
}

QSharedPointer<NodeSetCollection> NodeSetCollection::GetNodeSetCollection()
{
    if(nullptr == nodeSetCollection)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == nodeSetCollection)
        {
            nodeSetCollection = QSharedPointer<NodeSetCollection>(new NodeSetCollection);
        }
    }

    return nodeSetCollection;
}

void NodeSetCollection::AppendGroup(int index)
{
    QString id = QString("nodeSet-%1").arg(index);
    QSharedPointer<Group> nodeSet = QSharedPointer<Group>(new Group(Group::POINT, id));
    this->group_map.insert(index, nodeSet);
}

void NodeSetCollection::OutputGroupProperty(QTextStream &out, QString arg)
{
    int field = 10;
    int ncols = 10;
    int index = 0;

    size_t offset = 0;
    for(auto iter = this->group_map.begin(); iter != this->group_map.end(); ++iter)
    {
        if((index != 0) && (index % ncols == 0))
            out << reset << endl;

        if(QString("I1SNTY") == arg)
        {
            out << qSetFieldWidth(field) << left << forcepoint << iter.value()->getType() << reset;
        }
        else if(QString("I1SNHP") == arg)
        {
            out << qSetFieldWidth(field) << left << forcepoint << offset << reset;
            offset += iter.value()->getSize();
        }
        else if(QString("I1SNSZ") == arg)
        {
            out << qSetFieldWidth(field) << left << forcepoint << iter.value()->getSize() << reset;
        }

        index += 1;
    }

    out << reset << endl;
}

NodeSetCollection::NodeSetCollection()
{
    qDebug() << "NodeSetCollection()";
}

QMutex ElemSetCollection::mutex;

QSharedPointer<ElemSetCollection> ElemSetCollection::elemSetCollection = nullptr;

ElemSetCollection::~ElemSetCollection()
{
    qDebug() << "~NodeSetCollection()";
}

QSharedPointer<ElemSetCollection> ElemSetCollection::GetElemSetCollection()
{
    if(nullptr == elemSetCollection)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == elemSetCollection)
        {
            elemSetCollection = QSharedPointer<ElemSetCollection>(new ElemSetCollection);
        }
    }

    return elemSetCollection;
}

void ElemSetCollection::AppendGroup(int index)
{
    QString id = QString("elemSet-%1").arg(index);
    QSharedPointer<Group> elemSet = QSharedPointer<Group>(new Group(Group::CELL, id));
    this->group_map.insert(index, elemSet);
}

void ElemSetCollection::OutputGroupProperty(QTextStream &out, QString arg)
{
    int field = 10;
    int ncols = 10;
    int index = 0;

    size_t offset = 0;
    for(auto iter = this->group_map.begin(); iter != this->group_map.end(); ++iter)
    {
        if((index != 0) && (index % ncols == 0))
            out << reset << endl;

        if(QString("I1SETY") == arg)
        {
            out << qSetFieldWidth(field) << left << forcepoint << iter.value()->getType() << reset;
        }
        else if(QString("I1SEHP") == arg)
        {
            out << qSetFieldWidth(field) << left << forcepoint << offset << reset;
            offset += iter.value()->getSize();
        }
        else if(QString("I1SESZ") == arg)
        {
            out << qSetFieldWidth(field) << left << forcepoint << iter.value()->getSize() << reset;
        }

        index += 1;
    }

    out << reset << endl;
}

ElemSetCollection::ElemSetCollection()
{
    qDebug() << "NodeSetCollection()";
}

QTextStream &operator<<(QTextStream &out, QSharedPointer<NodeSetCollection> collection)
{
    if(collection->group_map.empty())
        return out;

    PRINT_STR_COMMENT(out, "/* NODESET COLLECTION */");
    PRINT_INT_SCALAR(out, QString("/YD/YDSN/MNSET"), collection->group_map.size());
    PRINT_INT_SCALAR(out, QString("/YD/YDSN/NNSET"), collection->group_map.size());
    PRINT_INT_SCALAR(out, QString("/YD/YDSN/MNODE"), collection->GetNumberOfIds());

    PRINT_INT_SCALAR(out, "/YD/YDSN/I1SNTY", collection->group_map.size());
    collection->OutputGroupProperty(out, QString("I1SNTY"));
    PRINT_INT_SCALAR(out, "/YD/YDSN/I1SNHP", collection->group_map.size());
    collection->OutputGroupProperty(out, QString("I1SNHP"));
    PRINT_INT_SCALAR(out, "/YD/YDSN/I1SNSZ", collection->group_map.size());
    collection->OutputGroupProperty(out, QString("I1SNSZ"));
    PRINT_INT_SCALAR(out, "/YD/YDSN/I1SNID", collection->GetNumberOfIds());
    collection->OutputGroupPedigreeIds(out, QString("I1SNID"));

    return out << reset << endl;
}

QTextStream &operator<<(QTextStream &out, QSharedPointer<ElemSetCollection> collection)
{
    if(collection->group_map.empty())
        return out;

    PRINT_STR_COMMENT(out, "/* ELEMSET COLLECTION */");
    PRINT_INT_SCALAR(out, QString("/YD/YDSE/MESET"), collection->group_map.size());
    PRINT_INT_SCALAR(out, QString("/YD/YDSE/NESET"), collection->group_map.size());
    PRINT_INT_SCALAR(out, QString("/YD/YDSE/MELEM"), collection->GetNumberOfIds());

    PRINT_INT_SCALAR(out, "/YD/YDSE/I1SETY", collection->group_map.size());
    collection->OutputGroupProperty(out, QString("I1SETY"));
    PRINT_INT_SCALAR(out, "/YD/YDSE/I1SEHP", collection->group_map.size());
    collection->OutputGroupProperty(out, QString("I1SEHP"));
    PRINT_INT_SCALAR(out, "/YD/YDSE/I1SESZ", collection->group_map.size());
    collection->OutputGroupProperty(out, QString("I1SESZ"));
    PRINT_INT_SCALAR(out, "/YD/YDSE/I1SEID", collection->GetNumberOfIds());
    collection->OutputGroupPedigreeIds(out, QString("I1SEID"));

    return out << reset << endl;
}
