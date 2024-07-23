#include "contact.h"
#include "globalmacros.h"

QMutex Contact::mutex;

QSharedPointer<Contact> Contact::contact = nullptr;

QSharedPointer<Contact> Contact::GetContactProperty()
{
    if(nullptr == contact)
    {
        QMutexLocker locker(&mutex);

        if(nullptr == contact)
        {
            contact = QSharedPointer<Contact>(new Contact);
        }
    }

    return contact;
}

Contact::~Contact()
{
    qDebug() << "~Contact()";
}

Contact::Contact()
{
    qDebug() << "Contact()";

    this->coupleCapacity  = 100000;
    this->coupleSize      = 0;
    this->bufferZoneSize  = 0.0002;
    this->contactSelection = Contact::UBIQUITOUS;
    this->contactAlgorithm = Contact::MUNJIZANBS;
    this->contactPotential = Contact::DISTANCE;
}

QTextStream &operator<<(QTextStream &out, const Contact &contact)
{
    PRINT_STR_COMMENT(out, "/* CONTACT PROPERTY */");

    PRINT_INT_SCALAR(out, "/YD/YDI/MICOUP", contact.coupleCapacity);
    PRINT_INT_SCALAR(out, "/YD/YDI/NICOUP", contact.coupleSize);
    PRINT_INT_SCALAR(out, "/YD/YDI/MISTATE", 6);
    PRINT_DBL_SCALAR(out, "/YD/YDI/DIEZON", contact.bufferZoneSize);
    PRINT_DBL_SCALAR(out, "/YD/YDI/DIEDIS", 200.0);
    PRINT_INT_SCALAR(out, "/YD/YDI/IIECFF", -2);
    PRINT_INT_SCALAR(out, "/YD/YDI/IIFAST", contact.contactSelection);
    PRINT_INT_SCALAR(out, "/YD/YDI/IIALGO", contact.contactAlgorithm);
    PRINT_INT_SCALAR(out, "/YD/YDI/IIPOFN", contact.contactPotential);

    return out << reset << endl;
}
