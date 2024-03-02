#include <QtCore/QTimer>

//#include "core/model/signal-fragment.h"

#include "opcPlayer.h"


opcPlayer::opcPlayer(Player* parent)
    : Player(parent)
    //, m_opcReader(new OPC_UA())
{
    m_metronome->setSingleShot(true);
    m_metronome->setInterval(1000);
    connect(m_metronome, &QTimer::timeout, this, &opcPlayer::readNext);
}

QString opcPlayer::sourceName() const
{
    //return m_opcReader->serverName();
}

void opcPlayer::setDataSource(const QString &serverName)
{
    m_metronome->stop();
    Player::setIsPaused(true);
    emit reset();
    emit error(tr("Реализация не готова"));
    /*if (!m_csvReader->setFile(fileName)) {
        onFileOpenError();
    }*/
}

void opcPlayer::readNext()
{
    emit error(tr("Реализация не готова"));
}

void opcPlayer::stop()
{
    emit error(tr("Реализация не готова"));
}

/*void opcPlayer::onFileOpenError()
{
    emit error(tr("Ошибка: файл CSV отсутствует или недоступен для чтения."));
}

void opcPlayer::onFileParseError()
{
    emit error(tr("Ошибка: неверный формат файла CSV."));
}*/
