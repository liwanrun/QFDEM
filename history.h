#ifndef HISTORY_H
#define HISTORY_H

#include <QString>
#include <QVector>

class History
{
public:
    enum HistoryType
    {
        GLOBAL = 0,
        POINT,
        CELL
    };

public:
    History(int type = GLOBAL);

    int getHistoryType();

public:
    friend class HistoryCollection;
    friend class FDEMHistoryWidget;
    friend class FDEMDocumentExporter;

protected:
    int historyType;
    int histVariable;
    int histOperation;
    int histFrequency;
    double timeScaleFactor;
    double histScaleFactor;

    int histEntitySize;
    QVector<size_t> globalIds;
    QVector<size_t> pedigreeIds;
};

class GlobalHistory : public History
{
public:
    GlobalHistory();

public:
    friend class FDEMFieldHistoryWidget;
    friend class FDEMDocumentExporter;

private:
};

class PointHistory : public History
{
public:
    PointHistory();

public:
    friend class FDEMPointHistoryWidget;
    friend class FDEMDocumentExporter;
};

class CellHistory : public History
{
public:
    CellHistory();

public:
    friend class FDEMCellHistoryWidget;
    friend class FDEMDocumentExporter;
};

#endif // HISTORY_H
