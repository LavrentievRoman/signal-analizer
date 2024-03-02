#pragma once

#include "network/new/HistoryData.h"
#include "player/model/csv-signal-fragment-reader.h"

class HistoryDataCsvFile : public HistoryData
{
    Q_OBJECT

public:
    explicit HistoryDataCsvFile(HistoryData* parent = nullptr);
    ~HistoryDataCsvFile() override = default;

signals:
    void error(const QString& message);

public slots:
    void requestData(const QDateTime& start = QDateTime(), const QDateTime& end = QDateTime());
    void setDataSource(const QString& fileName) override;

private:
    void onFileOpenError();
    void onFileParseError();
    void onWrongTimeIntervalError();

private:
    std::unique_ptr<CsvSignalFragmentReader> m_csvReader;
};
