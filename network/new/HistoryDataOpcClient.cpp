#include "core/model/analyzer-facade.h"
#include "core/model/SignalPart.h"

#include "HistoryDataOpcClient.h"

HistoryDataOpcClient::HistoryDataOpcClient(HistoryData* parent)
    : HistoryData{ parent }
    //, m_opcReader(new OPC_UA())
{}

void HistoryDataOpcClient::requestData(const QDateTime& startTime, const QDateTime& endTime)
{
    emit error(tr("Реализация не готова"));
}

void HistoryDataOpcClient::setDataSource(const QString &serverName)
{
    emit error(tr("Реализация не готова"));
}

/*void HistoryDataOpcClient::onFileOpenError()
{
    emit error(tr("Ошибка: файл CSV отсутствует или недоступен для чтения."));
}

void HistoryDataOpcClient::onFileParseError()
{
    emit error(tr("Ошибка: неверный формат файла CSV."));
}

void HistoryDataOpcClient::onWrongTimeIntervalError()
{
    emit error(tr("Ошибка: запрашиваемый временной интервал отсутствует в файле CSV."));
}*/