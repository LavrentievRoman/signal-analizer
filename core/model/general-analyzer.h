#pragma once

#include <memory>

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>

#include "core/model/analyzer.h"
#include "core/model/event.h"

class AnalyzerInstanceConfig;
class SignalChunk;
class SignalPart;


class GeneralAnalyzer : public QObject
{
    Q_OBJECT

public:
    explicit GeneralAnalyzer(QObject *parent = nullptr);
    ~GeneralAnalyzer() override;

    void addAnalyzer(std::shared_ptr<AnalyzerInstanceConfig> settings);
    void selectAnalyzer(int index);
    void removeAnalyzer(int index);
    void resetAnalyzer();
    void fit(SignalPart &&trainData);

    Event *releaseNewEvent();

signals:
    void currentAnalyzerIndexChanged(int index);
    void stateChanged(AnalyzerState state);
    void fitProgressChanged(int progress);

    // Domain model manipulation
    void inputSizeChanged(quint32 newSize);
    void classifyAsGreen(SignalChunk &chunk);
    void classifyAsRed(SignalChunk &chunk);
    void eventGenerated();  // call releaseNewEvent to get event

    void error(const QString &);

public slots:
    void onChunkIsFull(SignalChunk *chunk);

private:
    void _generateRedEvent(const SignalChunk &redChunk);

private:
    std::vector<std::shared_ptr<Analyzer>> m_analyzers;
    std::shared_ptr<Analyzer> m_currentAnalyzer;  // points to m_analyzers or nullptr
    std::unique_ptr<Event> m_newEvent;

};
