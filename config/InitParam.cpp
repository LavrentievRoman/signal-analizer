#include <QJsonObject>

#include "config/JsonKeys.h"

#include "InitParam.h"

const QString &InitParam::name() const
{
    return m_name;
}

const QString &InitParam::variableName() const
{
    return m_variableName;
}

const QVariant &InitParam::defaultValue() const
{
    return m_defaultValue;
}

QJsonValue InitParam::defaultValueJson() const
{
    return QJsonValue::fromVariant(m_defaultValue);
}

const QString &InitParam::description() const
{
    return m_description;
}


void InitParam::setName(const QString &name)
{
    m_name = name;
}

void InitParam::setVariableName(const QString &variableName)
{
    m_variableName = variableName;
}

void InitParam::setDefaultValue(const QVariant &defaultValue)
{
    m_defaultValue = defaultValue;
}

void InitParam::setDescription(const QString &description)
{
    m_description = description;
}

bool InitParam::fromJsonObject(const QJsonObject &object)
{
    using namespace JsonKeys::InitParam;

    if (object.isEmpty()) {
        return false;
    }

    QJsonValue value = object.value(jk_name);
    if (!value.isString()) {
        return false;
    }
    m_name = value.toString();

    value = object.value(jk_variableName);
    if (!value.isString()) {
        return false;
    }
    m_variableName = value.toString();

    value = object.value(jk_defaultValue);
    m_defaultValue = value.toVariant();

    value = object.value(jk_description);
    if (!value.isString()) {
        return false;
    }
    m_description = value.toString();

    return true;
}

void InitParam::toJsonObject(QJsonObject &object) const
{
    using namespace JsonKeys::InitParam;
    object.insert(jk_name,          name());
    object.insert(jk_variableName,  variableName());
    object.insert(jk_defaultValue,  defaultValueJson());
    object.insert(jk_description,   description());
}

QJsonObject InitParam::toNewJsonObject() const
{
    QJsonObject object;
    toJsonObject(object);
    return object;
}
