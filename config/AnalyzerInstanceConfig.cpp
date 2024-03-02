#include <QJsonObject>

#include "config/JsonKeys.h"

#include "AnalyzerInstanceConfig.h"

AnalyzerInstanceConfig::AnalyzerInstanceConfig(std::shared_ptr<AnalyzerClassConfig> classConfig,
                                               QObject *parent)
    : AbstractConfig(parent)
    , m_classConfig(classConfig)
    , m_step(DefaultStep)
    , m_threshold(DefaultThreshold)
{}

const QString &AnalyzerInstanceConfig::name() const
{
    return m_name;
}

int AnalyzerInstanceConfig::inputSize() const
{
    if (m_initParams.contains(AnalyzerClassConfig::InputSizeParamName)) {
        return m_initParams[AnalyzerClassConfig::InputSizeParamName].toInt();
    } else {
        return m_classConfig->inputSize();
    }
}

int AnalyzerInstanceConfig::step() const
{
    return m_step;
}

QJsonObject AnalyzerInstanceConfig::allInitParams() const
{
    QJsonObject params;

    for (const InitParam &param : m_classConfig->initParams()) {
        if (m_initParams.contains(param.variableName())) {
            params[param.variableName()] = m_initParams.value(param.variableName());
        } else {
            params[param.variableName()] = param.defaultValueJson();
        }
    }

    return params;
}

double AnalyzerInstanceConfig::threshold() const
{
    return m_threshold;
}

const AnalyzerClassConfig &AnalyzerInstanceConfig::classConfig() const
{
    return *m_classConfig;
}

const QJsonObject &AnalyzerInstanceConfig::changedInitParams() const
{
    return m_initParams;
}

QJsonObject &AnalyzerInstanceConfig::changedInitParamsRef()
{
    m_synchedWithFile = false;
    return m_initParams;
}

void AnalyzerInstanceConfig::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        m_synchedWithFile = false;
    }
}

void AnalyzerInstanceConfig::setStep(int step)
{
    if (m_step != step) {
        m_step = step;
        m_synchedWithFile = false;
    }
}

void AnalyzerInstanceConfig::setThreshold(double threshold)
{
    if (m_threshold != threshold) {
        m_threshold = threshold;
        m_synchedWithFile = false;
    }
}

bool AnalyzerInstanceConfig::fromJsonObject(const QJsonObject &object)
{
    using namespace JsonKeys::AnalyzerInstanceConfig;
    if (object.isEmpty()) {
        return false;
    }

    QJsonValue value = object.value(jk_name);
    if (!value.isString()) {
        return false;
    }
    m_name = value.toString();

    value = object.value(jk_step);
    if (!value.isDouble()) {
        return false;
    }
    m_step = value.toInt();

    value = object.value(jk_threshold);
    if (!value.isDouble()) {
        return false;
    }
    m_threshold = value.toDouble();

    value = object.value(jk_initParams);
    if (!value.isObject()) {
        return false;
    }
    m_initParams = value.toObject();

    return true;
}

bool AnalyzerInstanceConfig::toJsonObject(QJsonObject &object) const
{
    using namespace JsonKeys::AnalyzerInstanceConfig;
    object.insert(jk_name, m_name);
    object.insert(jk_step, m_step);
    object.insert(jk_threshold, m_threshold);
    object.insert(jk_initParams, m_initParams);
    return true;
}
