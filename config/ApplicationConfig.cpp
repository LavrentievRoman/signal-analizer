#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include "config/JsonKeys.h"
#include "config/AnalyzerClassConfig.h"
#include "config/AnalyzerInstanceConfig.h"

#include "ApplicationConfig.h"

ApplicationConfig::ApplicationConfig(QObject *parent)
    : AbstractConfig(parent)
{}

int ApplicationConfig::currentAnalyzerInstance() const
{
    return m_currentAnalyzerInstance;
}

QString ApplicationConfig::nextAnalyzerInstanceId() const
{
    if (m_analyzerInstances.isEmpty()) {
        return "0";
    } else {
        return QString::number(m_analyzerInstances.last().section('/', 1, 1).toInt() + 1);
    }
}

const QStringList &ApplicationConfig::analyzerClasses() const
{
    return m_analyzerClasses;
}

const QStringList &ApplicationConfig::analyzerInstances() const
{
    return m_analyzerInstances;
}

double ApplicationConfig::timeIntervalValue() const
{
    return m_timeIntervalValue;
}

TimeInterval::Units ApplicationConfig::timeIntervalUnits() const
{
    return m_timeIntervalUnits;
}

double ApplicationConfig::playerSpeed() const
{
    return m_playerSpeed;
}

const QString &ApplicationConfig::lastPlayerFilePath() const
{
    return m_lastPlayerFilePath;
}

const QString &ApplicationConfig::lastTrainFilePath() const
{
    return m_lastTrainFilePath;
}

void ApplicationConfig::setCurrentAnalyzerInstance(int index)
{
    if (m_currentAnalyzerInstance != index) {
        m_synchedWithFile = false;
        m_currentAnalyzerInstance = index;
    }
}

void ApplicationConfig::removeAnalyzerClass(int index)
{
    if (index < 0) {
        index = m_analyzerClasses.size() + index;
    }
    Q_ASSERT(index >= 0 && index < m_analyzerClasses.size());

    m_analyzerClasses.removeAt(index);
    m_synchedWithFile = false;
}

void ApplicationConfig::removeAnalyzerInstance(int index)
{
    if (index < 0) {
        index = m_analyzerInstances.size() + index;
    }
    Q_ASSERT(index >= 0 && index < m_analyzerInstances.size());

    m_analyzerInstances.removeAt(index);
    m_synchedWithFile = false;
}

void ApplicationConfig::insertAnalyzerClass(const QString &classFolder, int index)
{
    if (index < 0) {
        index = m_analyzerClasses.size() + index + 1;
    }
    Q_ASSERT(index >= 0 && index <= m_analyzerClasses.size());

    m_analyzerClasses.insert(index, classFolder);
    m_synchedWithFile = false;
}

void ApplicationConfig::insertAnalyzerInstance(const QString &instanceFolderPath, int index)
{
    if (index < 0) {
        index = m_analyzerInstances.size() + index + 1;
    }
    Q_ASSERT(index >= 0 && index <= m_analyzerInstances.size());

    m_analyzerInstances.insert(index, instanceFolderPath);
    m_synchedWithFile = false;
}

void ApplicationConfig::setTimeIntervalValue(double value)
{
    if (m_timeIntervalValue != value) {
        m_synchedWithFile = false;
        m_timeIntervalValue = value;
    }
}

void ApplicationConfig::setTimeIntervalUnits(TimeInterval::Units units)
{
    if (m_timeIntervalUnits != units) {
        m_synchedWithFile = false;
        m_timeIntervalUnits = units;
    }
}

void ApplicationConfig::setPlayerSpeed(double speed)
{
    if (m_playerSpeed != speed) {
        m_synchedWithFile = false;
        m_playerSpeed = speed;
    }
}

void ApplicationConfig::setLastPlayerFilePath(const QString &path)
{
    if (m_lastPlayerFilePath != path) {
        m_synchedWithFile = false;
        m_lastPlayerFilePath = path;
    }
}

void ApplicationConfig::setLastTrainFilePath(const QString &path)
{
    if (m_lastTrainFilePath != path) {
        m_synchedWithFile = false;
        m_lastTrainFilePath = path;
    }
}

bool ApplicationConfig::load()
{
    bool loaded = AbstractConfig::load();
    _scanAnalyzerClasses();
    _scanAnalyzerInstances();
    return loaded;
}

bool ApplicationConfig::fromJsonObject(const QJsonObject &object)
{
    using namespace JsonKeys::ApplicationConfig;
    m_currentAnalyzerInstance = object.value(jk_currentAnalyzerInstance).toInt(-1);
    return _timeIntervalFromJson(object.value(jk_timeInterval).toObject())
        && _operativeServerFromJson(object.value(jk_operativeServer).toObject())
        && _historyServerFromJson(object.value(jk_historyServer).toObject());
}

bool ApplicationConfig::toJsonObject(QJsonObject &object) const
{
    using namespace JsonKeys::ApplicationConfig;
    object.insert(jk_currentAnalyzerInstance, m_currentAnalyzerInstance);
    object.insert(jk_timeInterval, _timeIntervalToJson());
    object.insert(jk_operativeServer, _operativeServerToJson());
    object.insert(jk_historyServer, _historyServerToJson());
    return true;
}

void ApplicationConfig::_scanAnalyzerClasses()
{
    QFileInfoList dirsInfo = QDir(AnalyzerClassConfig::RootFolderPath).entryInfoList(
                                  QDir::Filter::Dirs | QDir::NoDotAndDotDot
                             );

    std::sort(dirsInfo.begin(), dirsInfo.end(), [](const QFileInfo &a, const QFileInfo &b) {
        return a.birthTime() < b.birthTime();
    });

    m_analyzerClasses.clear();
    for (const QFileInfo &dirInfo : dirsInfo) {
        m_analyzerClasses.append(dirInfo.baseName());
    }
}

void ApplicationConfig::_scanAnalyzerInstances()
{
    QFileInfoList dirsInfo;

    for (QString &className : m_analyzerClasses) {
        dirsInfo += QDir(AnalyzerInstanceConfig::RootFolderPath + className).entryInfoList(
                         QDir::Filter::Dirs | QDir::NoDotAndDotDot
                    );
    }

    std::sort(dirsInfo.begin(), dirsInfo.end(), [](const QFileInfo &a, const QFileInfo &b) {
        return a.birthTime() < b.birthTime();
    });

    m_analyzerInstances.clear();
    for (const QFileInfo &dirInfo : dirsInfo) {
        m_analyzerInstances.append(dirInfo.dir().dirName() + "/" + dirInfo.baseName());
    }

    if (m_currentAnalyzerInstance >= m_analyzerInstances.size()) {
        m_currentAnalyzerInstance = -1;
    }
}

bool ApplicationConfig::_timeIntervalFromJson(const QJsonObject &object)
{
    using namespace JsonKeys::ApplicationConfig;
    if (object.isEmpty()) {
        return false;
    }

    QJsonValue value = object.value(jk_timeIntervalValue);
    if (!value.isDouble()) {
        return false;
    }
    m_timeIntervalValue = value.toDouble();

    value = object.value(jk_timeIntervalUnits);
    if (!value.isString()) {
        return false;
    }
    m_timeIntervalUnits = TimeInterval::unitsFromString(value.toString());

    return true;
}

bool ApplicationConfig::_operativeServerFromJson(const QJsonObject &object)
{
    using namespace JsonKeys::ApplicationConfig;
    if (object.isEmpty()) {
        return false;
    }

    QJsonValue value = object.value(jk_valuesPerSec);
    if (!value.isDouble()) {
        return false;
    }
    m_playerSpeed = value.toDouble();

    value = object.value(jk_serverSource);
    if (!value.isString()) {
        return false;
    }
    m_lastPlayerFilePath = value.toString();

    return true;
}

bool ApplicationConfig::_historyServerFromJson(const QJsonObject &object)
{
    using namespace JsonKeys::ApplicationConfig;
    if (object.isEmpty()) {
        return false;
    }

    QJsonValue value = object.value(jk_serverSource);
    if (!value.isString()) {
        return false;
    }
    m_lastTrainFilePath = value.toString();

    return true;
}

QJsonObject ApplicationConfig::_timeIntervalToJson() const
{
    using namespace JsonKeys::ApplicationConfig;
    QJsonObject object;
    object.insert(jk_timeIntervalValue, m_timeIntervalValue);
    object.insert(jk_timeIntervalUnits, TimeInterval::unitsToString(m_timeIntervalUnits));
    return object;
}

QJsonObject ApplicationConfig::_operativeServerToJson() const
{
    using namespace JsonKeys::ApplicationConfig;
    QJsonObject object;
    object.insert(jk_valuesPerSec, m_playerSpeed);
    object.insert(jk_serverSource, m_lastPlayerFilePath);
    return object;
}

QJsonObject ApplicationConfig::_historyServerToJson() const
{
    using namespace JsonKeys::ApplicationConfig;
    QJsonObject object;
    object.insert(jk_serverSource, m_lastTrainFilePath);
    return object;
}
