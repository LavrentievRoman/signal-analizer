#pragma once

#include <QtCore/QObject>

class QString;
class QTimer;

class SignalFragment;

class Player : public QObject
{
    Q_OBJECT

public:
    explicit Player(QObject* parent = nullptr);
    //~Player() override = default;

    bool isPaused() const;

    int readCount() const;

    void setIsPaused(bool pause);

    virtual QString sourceName() const = 0;

    virtual void setDataSource(const QString& sourceName) = 0;

    bool setSpeed(const double eventsPerSecond, const int precision = 3);

signals:
    void fragmentRead(SignalFragment& fragment);
    void reset();
    void error(const QString& message);

public slots:
    virtual void readNext() = 0;

    void start();
    void restart();

    virtual void stop() = 0;
    void pause();
    void resume();
    void setPaused(bool paused);

protected:
    QTimer* m_metronome = nullptr;

private:
    static const int MinReadInterval;

    bool m_isPaused = true;
    int m_readCount = 1;
    double m_eventsPerSecond = 1.0;
};
