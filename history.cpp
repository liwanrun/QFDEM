#include "history.h"

History::History(int type)
{
    this->historyType = type;

    this->histVariable = 0;
    this->histOperation = 0;
    this->histFrequency = 10;
    this->timeScaleFactor = 1.0;
    this->histScaleFactor = 1.0;

    this->histEntitySize = 0;
    this->pedigreeIds.clear();
}

int History::getHistoryType()
{
    return this->historyType;
}

GlobalHistory::GlobalHistory()
    : History (GLOBAL)
{

}

PointHistory::PointHistory()
    : History (POINT)
{

}

CellHistory::CellHistory()
    : History (CELL)
{

}
