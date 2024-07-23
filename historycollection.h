#ifndef HISTORYCOLLECTION_H
#define HISTORYCOLLECTION_H

#include <QMap>
#include <QMutex>
#include <QSharedPointer>

class History;
class HistoryCollection
{
public:
    typedef QMap<QString, QSharedPointer<History>> HistoryMap;

public:
    ~HistoryCollection();

    static QSharedPointer<HistoryCollection> GetHistoryCollection();

    const HistoryMap& GetHistotyMap();

    QSharedPointer<History> GetHistory(const QString& id);

    void AppendHistory(const QString& id);

    void RemoveHistory(const QString& id);

    void UpdateHistory(const QString& id, QSharedPointer<History> history);

    void Clear();

private:
    HistoryCollection();

private:
    static QMutex mutex;
    static QSharedPointer<HistoryCollection> historyCollection;

    QMap<QString, QSharedPointer<History>> history_map;
};

#endif // HISTORYCOLLECTION_H
