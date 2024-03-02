#pragma once

#include "core/model/TimeInterval.h"

#include "AbstractConfig.h"

class ApplicationConfig : public AbstractConfig
{
    Q_OBJECT
public:
    explicit ApplicationConfig(QObject *parent = nullptr);
    ~ApplicationConfig() override = default;

    int currentAnalyzerInstance() const;
    QString nextAnalyzerInstanceId() const;
    const QStringList &analyzerClasses() const;
    const QStringList &analyzerInstances() const;
    double timeIntervalValue() const;
    TimeInterval::Units timeIntervalUnits() const;
    double playerSpeed() const;
    const QString &lastPlayerFilePath() const;
    const QString &lastTrainFilePath() const;

public slots:
    void setCurrentAnalyzerInstance(int index);
    void removeAnalyzerClass(int index);
    void removeAnalyzerInstance(int index);
    void insertAnalyzerClass(const QString &classFolder, int index = -1);
    void insertAnalyzerInstance(const QString &instanceFolderPath, int index = -1);
    void setTimeIntervalValue(double value);
    void setTimeIntervalUnits(TimeInterval::Units units);
    void setPlayerSpeed(double speed);
    void setLastPlayerFilePath(const QString &path);
    void setLastTrainFilePath(const QString &path);

    bool load() override;

protected:
    bool fromJsonObject(const QJsonObject &object) override;
    bool toJsonObject(QJsonObject &object) const override;

private:
    void _scanAnalyzerClasses();
    void _scanAnalyzerInstances();

    bool _timeIntervalFromJson(const QJsonObject &object);
    bool _operativeServerFromJson(const QJsonObject &object);
    bool _historyServerFromJson(const QJsonObject &object);

    QJsonObject _timeIntervalToJson() const;
    QJsonObject _operativeServerToJson() const;
    QJsonObject _historyServerToJson() const;

private:
    int m_currentAnalyzerInstance = 0;
    QStringList m_analyzerClasses;
    QStringList m_analyzerInstances;
    double m_timeIntervalValue = 1.0;
    TimeInterval::Units m_timeIntervalUnits = TimeInterval::Hours;
    double m_playerSpeed = 1.0;
    QString m_lastPlayerFilePath;
    QString m_lastTrainFilePath;
};

