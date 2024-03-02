#include "core/model/analyzer-facade.h"
#include "core/model/SignalPart.h"

#include "HistoryDataCsvFile.h"

HistoryDataCsvFile::HistoryDataCsvFile(HistoryData* parent)
    : HistoryData{ parent }
    , m_csvReader(new CsvSignalFragmentReader())
{}

void HistoryDataCsvFile::requestData(const QDateTime& startTime, const QDateTime& endTime)
{
    m_csvReader->reopenFile();
    SignalPart fragment;

    int readCount = m_csvReader->read(fragment, startTime, endTime);

    switch (readCount) {
    case -2:
        onFileParseError();
        break;
    case -1:
        onFileOpenError();
        break;
    case 0:
        onWrongTimeIntervalError();
        break;
    default:
        FACADE.trainCurrentAnalyzer(std::move(fragment));
        break;
    }
}

void HistoryDataCsvFile::setDataSource(const QString& fileName)
{
    if (!m_csvReader->setFile(fileName)) {
        onFileOpenError();
    }
}

void HistoryDataCsvFile::onFileOpenError()
{
    emit error(tr("������: ���� CSV ����������� ��� ���������� ��� ������."));
}

void HistoryDataCsvFile::onFileParseError()
{
    emit error(tr("������: �������� ������ ����� CSV."));
}

void HistoryDataCsvFile::onWrongTimeIntervalError()
{
    emit error(tr("������: ������������� ��������� �������� ����������� � ����� CSV."));
}