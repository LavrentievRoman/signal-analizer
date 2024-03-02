#include "core/model/analyzer-facade.h"
#include "core/model/SignalPart.h"

#include "HistoryDataOpcClient.h"

HistoryDataOpcClient::HistoryDataOpcClient(HistoryData* parent)
    : HistoryData{ parent }
    //, m_opcReader(new OPC_UA())
{}

void HistoryDataOpcClient::requestData(const QDateTime& startTime, const QDateTime& endTime)
{
    emit error(tr("���������� �� ������"));
}

void HistoryDataOpcClient::setDataSource(const QString &serverName)
{
    emit error(tr("���������� �� ������"));
}

/*void HistoryDataOpcClient::onFileOpenError()
{
    emit error(tr("������: ���� CSV ����������� ��� ���������� ��� ������."));
}

void HistoryDataOpcClient::onFileParseError()
{
    emit error(tr("������: �������� ������ ����� CSV."));
}

void HistoryDataOpcClient::onWrongTimeIntervalError()
{
    emit error(tr("������: ������������� ��������� �������� ����������� � ����� CSV."));
}*/