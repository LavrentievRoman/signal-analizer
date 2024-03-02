#include "core/model/analyzer-facade.h"
#include "core/model/SignalPart.h"

#include "HistoryServerStub.h"

HistoryServerStub::HistoryServerStub(QObject *parent)
    : QObject{parent}
    , m_csvReader(new CsvSignalFragmentReader())
{}

void HistoryServerStub::requestData(const QDateTime &startTime, const QDateTime &endTime)
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

void HistoryServerStub::setFile(const QString &fileName)
{
    if (!m_csvReader->setFile(fileName)) {
        onFileOpenError();
    }
}

void HistoryServerStub::onFileOpenError()
{
    emit error(tr("Ошибка: файл CSV отсутствует или недоступен для чтения."));
}

void HistoryServerStub::onFileParseError()
{
    emit error(tr("Ошибка: неверный формат файла CSV."));
}

void HistoryServerStub::onWrongTimeIntervalError()
{
    emit error(tr("Ошибка: запрашиваемый временной интервал отсутствует в файле CSV."));
}
