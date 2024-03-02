#pragma once

#include <QObject>

enum class AnalyzerState
{
    Loading,
    NotTrained,
    Training,
    SuccessTrained,
    FailTrained
};

class AnalyzerObserver : public QObject
{
    Q_OBJECT
public:
    explicit AnalyzerObserver(QObject *parent = nullptr) : QObject(parent) {}
    ~AnalyzerObserver() override = default;

signals:
    void error(const QString &message);
    void stateChanged(AnalyzerState state);
    void fitProgressChanged(int progress);
};

