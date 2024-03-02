#include <QtCore/QTimer>
#include <QtCore/QtMath>

#include "core/model/signal-fragment.h"

#include "csvPlayer.h"

csvPlayer::csvPlayer(Player* parent)
    : Player(parent)
    , m_csvReader(new CsvSignalFragmentReader())
{
    m_metronome->setSingleShot(true);
    m_metronome->setInterval(1000);
    connect(m_metronome, &QTimer::timeout, this, &csvPlayer::readNext);
}

QString csvPlayer::sourceName() const
{
    return m_csvReader->fileName();
}

void csvPlayer::setDataSource(const QString& fileName)
{
    m_metronome->stop();
    Player::setIsPaused(true);
    emit reset();
    if (!m_csvReader->setFile(fileName)) {
        onFileOpenError();
    }
}

void csvPlayer::readNext()
{
    SignalFragment fragment({});

    int readCount = m_csvReader->read(fragment, Player::readCount());

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

    if (!Player::isPaused()) {
        m_metronome->start();
    }
}

void csvPlayer::stop()
{
    setDataSource(m_csvReader->fileName());
}

void csvPlayer::onFileOpenError()
{
    emit error(tr("Ошибка: файл CSV отсутствует или недоступен для чтения."));
}

void csvPlayer::onFileParseError()
{
    emit error(tr("Ошибка: неверный формат файла CSV."));
}
