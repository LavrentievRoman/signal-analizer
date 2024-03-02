#pragma once

#include <QJsonObject>

#include "AnalyzerClassConfig.h"

class AnalyzerInstanceConfig : public AbstractConfig
{
    Q_OBJECT
public:
    static constexpr const char *RootFolderPath = "./analyzers/instances/";
    static constexpr const int DefaultStep = 1;
    static constexpr const double DefaultThreshold = 0.1;

    explicit AnalyzerInstanceConfig(std::shared_ptr<AnalyzerClassConfig> classConfig,
                                    QObject *parent = nullptr);
    ~AnalyzerInstanceConfig() override = default;

    const QString &name() const;
    int inputSize() const;
    int step() const;
    double threshold() const;
    const AnalyzerClassConfig &classConfig() const;

    /**
     * @brief initParams - returns a dictionary that contains all (including default) params
     */
    QJsonObject allInitParams() const;

    /**
     * @brief initParams - returns a dictionary that contains only non-default params
     */
    const QJsonObject &changedInitParams() const;


    QJsonObject &changedInitParamsRef();
    void setName(const QString &name);
    void setStep(int step);
    void setThreshold(double threshold);

protected:
    bool fromJsonObject(const QJsonObject &object) override;
    bool toJsonObject(QJsonObject &object) const override;

private:
    std::shared_ptr<AnalyzerClassConfig> m_classConfig;
    QString m_name;
    int m_step;
    double m_threshold;
    QJsonObject m_initParams;
};

