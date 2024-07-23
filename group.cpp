#include "group.h"

Group::Group(int type, QString name, size_t size, QString text)
    : m_type(type),
      m_size(size),
      m_name(name),
      m_text(text)
{

}

int Group::getType() const
{
    return this->m_type;
}

void Group::setType(int type)
{
    this->m_type = type;
}

size_t Group::getSize() const
{
    return this->m_size;
}

void Group::setSize(const size_t &size)
{
    this->m_size = size;
}

QString Group::getText() const
{
    return this->m_text;
}

void Group::setText(const QString &text)
{
    this->m_text = text;
}

QVector<size_t> &Group::getGlobalIds()
{
    return this->globalIds;
}

void Group::setGlobalIds(const QVector<size_t> &array)
{
    this->globalIds = array;
}

QVector<size_t> &Group::getPedigreeIds()
{
    return this->pedigreeIds;
}

void Group::setPedigreeIds(const QVector<size_t> &value)
{
    this->pedigreeIds = value;
}

QString Group::getName() const
{
    return this->m_name;
}

void Group::setName(const QString &name)
{
    this->m_name = name;
}
