#include <numeric>

#include <QtCore/QtMath>
#include <QtCore/QTimer>

#include "core/model/signal-fragment.h"

#include "player-core.h"

const int PlayerCore::MinReadInterval = 20;

PlayerCore::PlayerCore(QObject *parent)
    : QObject(parent)
    , m_csvReader(new CsvSignalFragmentReader())
    , m_metronome(new QTimer(this))
{
    m_metronome->setSingleShot(true);
    m_metronome->setInterval(1000);
    connect(m_metronome, &QTimer::timeout, this, &PlayerCore::readNext);
}

bool PlayerCore::isPaused() const
{
    return m_isPaused;
}

QString PlayerCore::fileName() const
{
    return m_csvReader->fileName();
}

void PlayerCore::setFile(const QString &fileName)
{
    m_metronome->stop();
    m_isPaused = true;
    emit reset();
    if (!m_csvReader->setFile(fileName)) {
        onFileOpenError();
    }
}

bool PlayerCore::setSpeed(const double eventsPerSecond, const int precision)
{
    if (eventsPerSecond <= 0) {
        emit error(tr("Ошибка: неверное значение скорости чтения событий."));
        return false;
    } else if (qFabs(m_eventsPerSecond - eventsPerSecond) < qPow(0.1, precision)) {
        return true;
    }

    int readInterval = 1000;
    m_readCount = qFloor(eventsPerSecond);
    double remainder = (eventsPerSecond - m_readCount);

    for (int i = 0; i < precision && remainder > 0; ++i) {
        readInterval *= 10;
        m_readCount *= 10;

        remainder *= 10;
        int remainderDigit = remainder;
        remainder -= remainderDigit;
        m_readCount += remainderDigit;
    }

    if (m_readCount > 1 && readInterval > 1) {
        int gcd = std::gcd(m_readCount, readInterval);
        readInterval /= gcd;
        m_readCount /= gcd;
    }

    double reduceDegree = double(readInterval) / MinReadInterval;

    if (reduceDegree < 1) {
        m_readCount = qMax(int(m_readCount / reduceDegree), 1);
        readInterval = MinReadInterval;
    } else if (reduceDegree > 1 && m_readCount > 1) {
        reduceDegree = qMin(reduceDegree, double(m_readCount)); //can't reduce more than readCount
        int maxReducedInterval = qMax(int(readInterval / reduceDegree), MinReadInterval);
        double maxReduceDegree = double(readInterval) / maxReducedInterval;
        reduceDegree = qMin(reduceDegree, maxReduceDegree);

        m_readCount = qMax(int(m_readCount / reduceDegree), 1);
        readInterval = maxReducedInterval;
    }

    m_eventsPerSecond = eventsPerSecond;
    m_metronome->setInterval(readInterval);

    return true;
}

void PlayerCore::readNext()
{
    SignalFragment fragment({});

    int readCount = m_csvReader->read(fragment, m_readCount);

    switch (readCount) {
        case -2:
            onFileParseError();
            break;
        case -1:
            onFileOpenError();
            break;
        case 0:
            break;
        default:
            emit fragmentRead(fragment);
            break;
    }

    if (!m_isPaused) {
        m_metronome->start();
    }
}

void PlayerCore::start()
{
    restart();
}

void PlayerCore::restart()
{
    stop();
    resume();
}

void PlayerCore::stop()
{
    setFile(m_csvReader->fileName());
}

void PlayerCore::pause()
{
    if (!m_isPaused) {
        m_isPaused = true;
        m_metronome->stop();
    }
}

void PlayerCore::resume()
{
    if (m_isPaused) {
        m_isPaused = false;
        m_metronome->start();
    }
}

void PlayerCore::setPaused(bool paused)
{
    paused ? pause() : resume();
}

void PlayerCore::onFileOpenError()
{
    emit error(tr("Ошибка: файл CSV отсутствует или недоступен для чтения."));
}

void PlayerCore::onFileParseError()
{
    emit error(tr("Ошибка: неверный формат файла CSV."));
}
