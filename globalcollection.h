#ifndef GLOBALCOLLECTION_H
#define GLOBALCOLLECTION_H

#include <QString>
#include <QDir>
#include <QDebug>
#include <QMutex>

class GlobalCollection
{
public:
    static QSharedPointer<GlobalCollection> GetGlobalCollection();
    ~GlobalCollection() {qDebug() << "~GlobalCollection()";}

    QString getWorkspace() const;
    void setWorkspace(const QString &value);

private:
    GlobalCollection() : workspace(QDir::currentPath())
    { qDebug() << "GlobalCollection()"; }

private:
    static QSharedPointer<GlobalCollection> globalCollection;

    static QMutex mutex;

    QString workspace;
};

#endif // GLOBALCOLLECTION_H
