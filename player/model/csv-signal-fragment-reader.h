#pragma once

#include <memory>

#include <QtCore/QFile>
#include <QtCore/QString>

#include "core/model/signal-types.h"

class SignalFragment;
class SignalPart;

class CsvSignalFragmentReader
{
public:
    explicit CsvSignalFragmentReader();
    CsvSignalFragmentReader(const QString &fileName,
                            const QString &dateTimeFormat = "yyyy-MM-dd hh:mm:ss");
    ~CsvSignalFragmentReader() = default;

    QString fileName() const;

    /**
     * @return count of read points or -1 in case of IO error, -2 in case of parse error
     */
    int read(SignalFragment &fragment, const int pointCount);

    /**
     * @return count of read points or -1 in case of IO error, -2 in case of parse error
     */
    int read(SignalPart &fragment, const QDateTime &start = QDateTime(),
                                   const QDateTime &end = QDateTime());

    bool reopenFile();
    void setDateTimeFormat(const QString &dateTimeFormat);
    bool setFile(const QString &fileName, const QString &dateTimeFormat);
    bool setFile(const QString &fileName);

private:
    enum PointValidatorResult
    {
        AbortReading = -1,
        SkipPoint = 0,
        StorePoint = 1
    };
    typedef std::function<PointValidatorResult (SignalPoint &point, int readCount)> PointValidator;

    int _read(SignalFragment &fragment, PointValidator pointValidator);
    int _read(SignalPart &fragment, PointValidator pointValidator);
    bool _readNextPoint(bool &isFileValid);

private:
    std::unique_ptr<QFile> m_file; //autoclose on destroy
    QString m_dateTimeFormat = "yyyy-MM-dd hh:mm:ss";
    std::unique_ptr<SignalPoint> m_nextPoint;
};

