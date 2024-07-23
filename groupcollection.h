#ifndef GROUPCOLLECTION_H
#define GROUPCOLLECTION_H

#include <QMap>
#include <QMutex>
#include <QSharedPointer>
#include <QTextStream>

class Group;
class GroupCollection
{
public:
    typedef QMap<int, QSharedPointer<Group>> GroupMap;

public:
    GroupCollection();
    ~GroupCollection();

    const GroupMap& GetGroupMap() const;

    void ClearGroupMap();

    QSharedPointer<Group> GetGroup(int id);

    void RemoveGroup(int id);

    void UpdateGroup(int id, QSharedPointer<Group> group);

    size_t GetNumberOfIds();

    size_t GetGroupSize();

public:
    virtual void AppendGroup(int id) = 0;

    void OutputGroupPedigreeIds(QTextStream &out, QString arg);

    virtual void OutputGroupProperty(QTextStream &out, QString id) = 0;

protected:
    GroupMap group_map;
};


class NodeSetCollection : public GroupCollection
{
public:
    ~NodeSetCollection();

    static QSharedPointer<NodeSetCollection> GetNodeSetCollection();

public:
    void AppendGroup(int index) override;

    void OutputGroupProperty(QTextStream &out, QString arg) override;

    friend QTextStream &operator<<(QTextStream &out, QSharedPointer<NodeSetCollection> collection);

private:
    NodeSetCollection();

    static QMutex mutex;
    static QSharedPointer<NodeSetCollection> nodeSetCollection;
};

QTextStream &operator<<(QTextStream &out, QSharedPointer<NodeSetCollection> collection);


class ElemSetCollection : public GroupCollection
{
public:
    ~ElemSetCollection();

    static QSharedPointer<ElemSetCollection> GetElemSetCollection();

public:
    void AppendGroup(int index) override;

    void OutputGroupProperty(QTextStream &out, QString arg) override;

    friend QTextStream &operator<<(QTextStream &out, QSharedPointer<ElemSetCollection> collection);

private:
    ElemSetCollection();

    static QMutex mutex;
    static QSharedPointer<ElemSetCollection> elemSetCollection;
};

QTextStream &operator<<(QTextStream &out, QSharedPointer<ElemSetCollection> collection);

#endif // GROUPCOLLECTION_H
