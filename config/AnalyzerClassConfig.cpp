#include <QJsonArray>
#include <QJsonObject>

#include "config/JsonKeys.h"

#include "AnalyzerClassConfig.h"

AnalyzerClassConfig::AnalyzerClassConfig(QObject *parent)
    : AbstractConfig(parent)
{}

const QString &AnalyzerClassConfig::name() const
{
    return m_name;
}

const QString &AnalyzerClassConfig::description() const
{
    return m_description;
}

const QString &AnalyzerClassConfig::folderName() const
{
    return m_folderName;
}

const QString &AnalyzerClassConfig::pythonFileName() const
{
    return m_pythonFileName;
}

const QString &AnalyzerClassConfig::pythonClassName() const
{
    return m_pythonClassName;
}

const QList<InitParam> &AnalyzerClassConfig::initParams() const
{
    return m_initParams;
}

const InitParam &AnalyzerClassConfig::initParam(int index) const
{
    return m_initParams[index];
}

InitParam &AnalyzerClassConfig::initParamRef(int index)
{
    m_synchedWithFile = false;
    return m_initParams[index];
}

int AnalyzerClassConfig::inputSize() const
{
    for (const InitParam &param : m_initParams) {
        if (param.variableName() == InputSizeParamName) {
            return param.defaultValue().toInt();
        }
    }
    return -1;
}

void AnalyzerClassConfig::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        m_synchedWithFile = false;
    }
}

void AnalyzerClassConfig::setDescription(const QString &description)
{
    if (m_description != description) {
        m_description = description;
        m_synchedWithFile = false;
    }
}

void AnalyzerClassConfig::setFolderName(const QString &folderName)
{
    if (m_folderName != folderName) {
        m_folderName = folderName;
        _updateFilePath();
    }
}

void AnalyzerClassConfig::setPythonFileName(const QString &pythonFileName)
{
    if (m_pythonFileName != pythonFileName) {
        m_pythonFileName = pythonFileName;
        m_synchedWithFile = false;
    }
}

void AnalyzerClassConfig::setPythonClassName(const QString &pythonClassName)
{
    if (m_pythonClassName != pythonClassName) {
        m_pythonClassName = pythonClassName;
        m_synchedWithFile = false;
    }
}

InitParam AnalyzerClassConfig::defaultStepParam()
{
    InitParam param;
    param.setName(tr("Сдвиг окна"));
    param.setVariableName("step");
    param.setDefaultValue(1);
    param.setDescription(tr("Определяет сдвиг следующего окна для анализа относительно текущего."));
    return param;
}

InitParam AnalyzerClassConfig::defaultThresholdParam()
{
    InitParam param;
    param.setName(tr("Порог аномальности"));
    param.setVariableName("threshold");
    param.setDefaultValue(0.1);
    param.setDescription(tr("Минимальное отношение результатов анализа отрезка "
                            "(не аномальный/аномальный), при превышении которого "
                            "отрезок будет считаться аномальным."));
    return param;
}

bool AnalyzerClassConfig::fromJsonObject(const QJsonObject &object)
{
    using namespace JsonKeys::AnalyzerClassConfig;
    if (object.isEmpty()) {
        return false;
    }

    QJsonValue value = object.value(jk_name);
    if (!value.isString()) {
        return false;
    }
    m_name = value.toString();

    value = object.value(jk_description);
    if (!value.isString()) {
        return false;
    }
    m_description = value.toString();

    value = object.value(jk_folderName);
    if (!value.isString()) {
        return false;
    }
    m_folderName = value.toString();

    value = object.value(jk_pythonFileName);
    if (!value.isString()) {
        return false;
    }
    m_pythonFileName = value.toString();

    value = object.value(jk_pythonClassName);
    if (!value.isString()) {
        return false;
    }
    m_pythonClassName = value.toString();

    value = object.value(jk_initParams);
    if (!value.isArray()) {
        return false;
    }
    m_initParams.clear();
    for (const QJsonValue &paramJson : value.toArray()) {
        InitParam param;
        if (!paramJson.isObject() || !param.fromJsonObject(paramJson.toObject())) {
            return false;
        }
        m_initParams.append(param);
    }

    _updateFilePath();
    return true;
}

bool AnalyzerClassConfig::toJsonObject(QJsonObject &object) const
{
    using namespace JsonKeys::AnalyzerClassConfig;
    object.insert(jk_name, m_name);
    object.insert(jk_description, m_description);
    object.insert(jk_folderName, m_folderName);
    object.insert(jk_pythonFileName, m_pythonFileName);
    object.insert(jk_pythonClassName, m_pythonClassName);
    QJsonArray initParamsJson;
    for (const InitParam &initParam : m_initParams) {
        initParamsJson.append(initParam.toNewJsonObject());
    }
    object.insert(jk_initParams, initParamsJson);
    return true;
}

void AnalyzerClassConfig::_updateFilePath()
{
    setFile(RootFolderPath + m_folderName + "/config.json");
}
