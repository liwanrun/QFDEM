#ifndef CONTACT_H
#define CONTACT_H

#include <QDebug>
#include <QMutex>
#include <QSharedPointer>

class Contact
{
public:
    enum ContactStrategy
    {
        ADAPTIVE = 0, UBIQUITOUS = 1
    };

    enum ContactAlgorithm
    {
        MUNJIZANBS = 0, MIGRATEALG = 1
    };

    enum ContactPotential
    {
        AREA = 0, DISTANCE = 1
    };

public:
    static QSharedPointer<Contact> GetContactProperty();

    ~Contact();

    friend class FDEMContactWidget;

    friend QTextStream &operator<<(QTextStream &out, const Contact& contact);

private:
    Contact();

private:
    int    coupleCapacity;
    int    coupleSize;
    double bufferZoneSize;
    int    contactSelection;
    int    contactAlgorithm;

    int    contactPotential;

private:
    static QSharedPointer<Contact> contact;
    static QMutex mutex;
};

QTextStream &operator<<(QTextStream &out, const Contact& contact);

#endif // CONTACT_H
