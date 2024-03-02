#pragma once

#include <memory>

#include <QtCore/QObject>

#include "player/model/csv-signal-fragment-reader.h"

class QString;
class QTimer;

class SignalFragment;

class PlayerCore : public QObject
{
    Q_OBJECT

public:
    explicit PlayerCore(QObject *parent = nullptr);
    ~PlayerCore() override = default;

    bool isPaused() const;

    QString fileName() const;

    /**
     * @brief stops reading the current file and opens a new file named 'fileName'
     * @param fileName - the name of new file
     */
    void setFile(const QString &fileName);

    /**
     * @brief sets the player speed.
     * Player will try to keep least possible timeout (20ms) to decrease size of read fragment.
     * @param eventsPerSecond - desired count of events per second.
     */
    bool setSpeed(const double eventsPerSecond, const int precision = 3);

signals:
    void fragmentRead(SignalFragment &fragment);
    void reset();
    void error(const QString &message);

public slots:
    void readNext();

    void start();
    void restart();

    /**
     * @brief stops reading and reopen current file.
     */
    void stop();
    void pause();
    void resume();
    void setPaused(bool paused);

    void onFileOpenError();
    void onFileParseError();

private:
    static const int MinReadInterval;

    std::unique_ptr<CsvSignalFragmentReader> m_csvReader;
    QTimer *m_metronome = nullptr;
    bool m_isPaused = true;
    int m_readCount = 1;
    double m_eventsPerSecond = 1.0;
};
