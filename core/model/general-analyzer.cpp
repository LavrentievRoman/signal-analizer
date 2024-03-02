#include <cassert>

#include <QtCore/QDateTime>

#include "config/AnalyzerInstanceConfig.h"

#include "core/model/analyzer.h"
#include "core/model/event.h"
#include "core/model/signal-chunk.h"
#include "core/model/SignalPart.h"

#include "core/model/general-analyzer.h"

GeneralAnalyzer::GeneralAnalyzer(QObject *parent)
    : QObject(parent)
    , m_analyzers()
    , m_currentAnalyzer()
    , m_newEvent()
{
    connect(&Analyzer::Observer, &AnalyzerObserver::error,
            this, &GeneralAnalyzer::error);
    connect(&Analyzer::Observer, &AnalyzerObserver::fitProgressChanged,
            this, &GeneralAnalyzer::fitProgressChanged);
    connect(&Analyzer::Observer, &AnalyzerObserver::stateChanged,
            this, &GeneralAnalyzer::stateChanged);
}

GeneralAnalyzer::~GeneralAnalyzer()
{
}

void GeneralAnalyzer::addAnalyzer(std::shared_ptr<AnalyzerInstanceConfig> settings)
{
    // TODO: [Evgeny] may be it's good idea to emit that error from AnalyzerConfig.
    if (!settings || settings->name().isEmpty()) {
        emit error(tr("Настройки анализатора не найдены."));
        return;
    }

    m_analyzers.push_back(std::make_shared<Analyzer>(settings));
}

void GeneralAnalyzer::selectAnalyzer(int index)
{
    emit currentAnalyzerIndexChanged(index);

    if (index < 0 || index >= static_cast<int>(m_analyzers.size())) {
        return;
    }

    const quint32 oldSize = m_currentAnalyzer ? m_currentAnalyzer->inputSize() : 0;
    m_currentAnalyzer = m_analyzers[index];

    if (!m_currentAnalyzer->isLoaded()) {
        //TODO: [tvz] send signal to show message box
        if (!m_currentAnalyzer->load()) {
            emit error(tr("Во время загрузки выбранного анализатора произошла ошибка."));
            return;
        }
    }

    const quint32 newSize = m_currentAnalyzer->inputSize();

    if (oldSize != newSize) {
        emit inputSizeChanged(newSize);
    }

    emit stateChanged(m_currentAnalyzer->state());
}

void GeneralAnalyzer::removeAnalyzer(int index)
{
    if (m_currentAnalyzer == m_analyzers[index]) {
        m_currentAnalyzer->unload();
        m_currentAnalyzer = nullptr;
    }
    m_analyzers.erase(m_analyzers.begin() + index);
}

void GeneralAnalyzer::resetAnalyzer()
{
    // TODO: [tvz] try to reset analyzer in a different way (without full unload)
    m_currentAnalyzer->unload();
    m_currentAnalyzer->load();
}

void GeneralAnalyzer::fit(SignalPart &&trainData)
{
    m_currentAnalyzer->fit(trainData.yValues());
}

Event *GeneralAnalyzer::releaseNewEvent()
{
    return m_newEvent.release();
}

void GeneralAnalyzer::onChunkIsFull(SignalChunk *chunk)
{
    assert(chunk->isFull());

    if (!m_currentAnalyzer) {
        emit error(tr("Накоплен блок данных для анализа, но анализатор не задан."));
        return;
    }

    // TODO: [Evgeny] Get from config
    static const double threshold = 0.5;

    // TODO: [Evgeny, tvz] Split into signal and slot so as not to freeze ui by slow analyzer.
    if (m_currentAnalyzer->estimate(chunk->constVectorY()) < threshold) {
        chunk->labelRegular();
    } else {
        chunk->labelAnomalous();
        _generateRedEvent(*chunk);
        emit eventGenerated();
    }
}

void GeneralAnalyzer::_generateRedEvent(const SignalChunk &redChunk)
{
    // Chunk may be colored as red a bit later

    QDateTime beg;
    beg.setMSecsSinceEpoch(static_cast<qint64>(redChunk.firstX()));

    QDateTime end;
    end.setMSecsSinceEpoch(static_cast<qint64>(redChunk.lastX()));

    m_newEvent = std::make_unique<Event>(Event::generate(beg, end));
}
