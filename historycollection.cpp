#include "historycollection.h"
#include "history.h"

#include <QDebug>

QMutex HistoryCollection::mutex;

QSharedPointer<HistoryCollection> HistoryCollection::historyCollection = nullptr;

HistoryCollection::~HistoryCollection()
{
    qDebug() << "~HistoryCollection()";
}

QSharedPointer<HistoryCollection> HistoryCollection::GetHistoryCollection()
{
    if(nullptr == historyCollection)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == historyCollection)
        {
            historyCollection = QSharedPointer<HistoryCollection>(new HistoryCollection);
        }
    }

    return historyCollection;
}

const HistoryCollection::HistoryMap &HistoryCollection::GetHistotyMap()
{
    return this->history_map;
}

QSharedPointer<History> HistoryCollection::GetHistory(const QString &id)
{
    if(!this->history_map.contains(id))
    {
        this->AppendHistory(id);
    }
    return this->history_map[id];
}

void HistoryCollection::AppendHistory(const QString &id)
{
    QSharedPointer<History> history = QSharedPointer<History>(new History);
    this->history_map.insert(id, history);
}

void HistoryCollection::RemoveHistory(const QString &id)
{
    this->history_map.remove(id);
}

void HistoryCollection::UpdateHistory(const QString &id, QSharedPointer<History> history)
{
    if(this->history_map.contains(id))
    {
        this->history_map[id] = history;
    }
}

void HistoryCollection::Clear()
{
    this->history_map.clear();
}

HistoryCollection::HistoryCollection()
{
    qDebug() << "HistoryCollection()";
}
