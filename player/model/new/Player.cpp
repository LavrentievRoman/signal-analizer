#include <numeric>

#include <QtCore/QtMath>
#include <QtCore/QTimer>

#include "Player.h"

const int Player::MinReadInterval = 20;

Player::Player(QObject* parent) 
    : QObject(parent)
    , m_metronome(new QTimer(this))
{
    m_metronome->setSingleShot(true);
    m_metronome->setInterval(1000);
}

bool Player::isPaused() const
{
    return m_isPaused;
}

int Player::readCount() const
{
    return m_readCount;
}

void Player::setIsPaused(bool pause)
{
    this->m_isPaused = pause;
}

bool Player::setSpeed(const double eventsPerSecond, const int precision)
{
    if (eventsPerSecond <= 0) {
        emit error(tr("Ошибка: неверное значение скорости чтения событий."));
        return false;
    }
    else if (qFabs(m_eventsPerSecond - eventsPerSecond) < qPow(0.1, precision)) {
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
    }
    else if (reduceDegree > 1 && m_readCount > 1) {
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

void Player::start()
{
    restart();
}

void Player::restart()
{
    stop();
    resume();
}

void Player::pause()
{
    if (!m_isPaused) {
        m_isPaused = true;
        m_metronome->stop();
    }
}

void Player::resume()
{
    if (m_isPaused) {
        m_isPaused = false;
        m_metronome->start();
    }
}

void Player::setPaused(bool paused)
{
    paused ? pause() : resume();
}
