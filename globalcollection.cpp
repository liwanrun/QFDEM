#include "globalcollection.h"

QSharedPointer<GlobalCollection> GlobalCollection::globalCollection = nullptr;

QMutex GlobalCollection::mutex;

QSharedPointer<GlobalCollection> GlobalCollection::GetGlobalCollection()
{
    if(nullptr == globalCollection)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == globalCollection)
        {
            globalCollection = QSharedPointer<GlobalCollection>(new GlobalCollection);
        }
    }

    return globalCollection;
}

QString GlobalCollection::getWorkspace() const
{
    return this->workspace;
}

void GlobalCollection::setWorkspace(const QString &value)
{
    this->workspace = value;
}
