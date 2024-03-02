#pragma once

#include <memory>

#include <QtCore/QObject>

#include "player/model/csv-signal-fragment-reader.h"

class HistoryServerStub : public QObject
{
    Q_OBJECT

public:
    explicit HistoryServerStub(QObject *parent = nullptr);
    ~HistoryServerStub() override = default;

signals:
    void error(const QString &message);

public slots:
    void requestData(const QDateTime &start = QDateTime(), const QDateTime &end = QDateTime());
    void setFile(const QString &fileName);

private:
    void onFileOpenError();
    void onFileParseError();
    void onWrongTimeIntervalError();

private:
    std::unique_ptr<CsvSignalFragmentReader> m_csvReader;
};

