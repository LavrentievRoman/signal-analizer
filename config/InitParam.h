#pragma once

#include <QVariant>

class InitParam
{
public:
    InitParam() = default;
    ~InitParam() = default;

    const QString &name() const;
    const QString &variableName() const;
    const QVariant &defaultValue() const;
    QJsonValue defaultValueJson() const;
    const QString &description() const;

    void setName(const QString &name);
    void setVariableName(const QString &variableName);
    void setDefaultValue(const QVariant &defaultValue);
    void setDescription(const QString &description);

    bool fromJsonObject(const QJsonObject &object);
    void toJsonObject(QJsonObject &object) const;
    QJsonObject toNewJsonObject() const;

private:
    QString m_name;
    QString m_variableName;
    QVariant m_defaultValue;
    QString m_description;
};

