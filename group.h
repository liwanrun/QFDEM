#ifndef GROUP_H
#define GROUP_H

#include <QVector>
#include <QVariant>

class Group
{
public:
    enum GroupType
    {
        GRAIN = 0,
        POINT,
        CELL
    };

public:
    Group(int type, QString name, size_t size = 0, QString text = QString("untitled"));

    int getType() const;
    void setType(int type);

    QString getName() const;
    void setName(const QString &name);

    size_t getSize() const;
    void setSize(const size_t &getSize);

    QString getText() const;
    void setText(const QString &text);

    QVector<size_t>& getGlobalIds();
    void setGlobalIds(const QVector<size_t> &array);

    QVector<size_t>& getPedigreeIds();
    void setPedigreeIds(const QVector<size_t> &value);

public:
    friend class GroupCollection;
    friend class NodeSetCollection;
    friend class ElemSetCollection;
    friend class FDEMDocumentExporter;

protected:
    int m_type;
    size_t m_size;
    QString m_name;
    QString m_text;

    QVector<size_t> globalIds;
    QVector<size_t> pedigreeIds;
};

#endif // GROUP_H
