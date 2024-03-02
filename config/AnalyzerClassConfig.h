#pragma once

#include "AbstractConfig.h"
#include "InitParam.h"

class AnalyzerClassConfig : public AbstractConfig
{
    Q_OBJECT
public:
    static constexpr const char *RootFolderPath = "./analyzers/classes/";
    //TODO: [tvz] move to application config and made configurable?
    static constexpr const char *InputSizeParamName = "inputSize";

    explicit AnalyzerClassConfig(QObject *parent = nullptr);
    ~AnalyzerClassConfig() override = default;

    const QString &name() const;
    const QString &description() const;
    const QString &folderName() const;
    const QString &pythonFileName() const;
    const QString &pythonClassName() const;
    const QList<InitParam> &initParams() const;
    const InitParam &initParam(int index) const;
    InitParam &initParamRef(int index);
    int inputSize() const;

    void setName(const QString &name);
    void setDescription(const QString &description);
    void setFolderName(const QString &folderName);
    void setPythonFileName(const QString &pythonFileName);
    void setPythonClassName(const QString &pythonClassName);

    static InitParam defaultStepParam();
    static InitParam defaultThresholdParam();

protected:
    bool fromJsonObject(const QJsonObject &object) override;
    bool toJsonObject(QJsonObject &object) const override;

private:
    void _updateFilePath();

private:
    QString m_name;
    QString m_description;
    QString m_folderName;
    QString m_pythonFileName;
    QString m_pythonClassName;
    QList<InitParam> m_initParams;
};

