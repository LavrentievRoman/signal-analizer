#include "core/model/signal-fragment.h"
#include "core/model/SignalPart.h"

#include "csv-signal-fragment-reader.h"

CsvSignalFragmentReader::CsvSignalFragmentReader()
    : m_file(new QFile())
{}

CsvSignalFragmentReader::CsvSignalFragmentReader(const QString &fileName,
                                                 const QString &dateTimeFormat)
    : m_file(new QFile())
    , m_dateTimeFormat(dateTimeFormat)
    , m_nextPoint(nullptr)
{
    setFile(fileName);
}

QString CsvSignalFragmentReader::fileName() const
{
    return m_file->fileName();
}

int CsvSignalFragmentReader::read(SignalFragment &fragment, const int pointCount)
{
    return _read(fragment, [pointCount](SignalPoint &point, int readCount) -> PointValidatorResult {
        if (readCount >= pointCount) {
            return PointValidatorResult::AbortReading;
        }

        return PointValidatorResult::StorePoint;
    });
}

int CsvSignalFragmentReader::read(SignalPart &fragment, const QDateTime &start,
                                                        const QDateTime &end)
{
    return _read(fragment, [&start, &end](SignalPoint &point, int) -> PointValidatorResult {
        if (start.isValid() && point.first < start) {
            return PointValidatorResult::SkipPoint;
        } else if (end.isValid() && point.first > end) {
            return PointValidatorResult::AbortReading;
        }

        return PointValidatorResult::StorePoint;
    });
}

bool CsvSignalFragmentReader::reopenFile()
{
    return setFile(m_file->fileName());
}

void CsvSignalFragmentReader::setDateTimeFormat(const QString &dateTimeFormat)
{
    m_dateTimeFormat = dateTimeFormat;
}

bool CsvSignalFragmentReader::setFile(const QString &fileName, const QString &dateTimeFormat)
{
    setDateTimeFormat(dateTimeFormat);
    return setFile(fileName);
}

bool CsvSignalFragmentReader::setFile(const QString &fileName)
{
    if (m_file->isOpen()) {
        m_file->close();
    }

    m_file->setFileName(fileName);
    if (!m_file->open(QFile::ReadOnly)) {
        return false;
    }

    m_nextPoint.reset(nullptr);
    QString format = m_file->readLine();
    Q_UNUSED(format);   //TODO: [tvz] parse and check CSV format

    return true;
}

int CsvSignalFragmentReader::_read(SignalFragment &fragment, PointValidator pointValidator)
{
    if (!m_file->isOpen()) {
        return -1;
    }

    int readPointCount = 0;
    bool isFileValid = true;
    bool hasReadPoint = (m_nextPoint != nullptr);
    if (!hasReadPoint) {
        m_nextPoint = std::make_unique<SignalPoint>();
    }

    if (hasReadPoint || (_readNextPoint(isFileValid) && isFileValid)) {
        do {
            switch (pointValidator(*m_nextPoint, readPointCount)) {
                case AbortReading:
                    return readPointCount;
                case SkipPoint:
                    break;
                case StorePoint:
                    fragment.append(*m_nextPoint);
                    ++readPointCount;
                    break;
            }
        } while (_readNextPoint(isFileValid) && isFileValid);
    }

    m_nextPoint.reset(nullptr);
    return isFileValid ? readPointCount : -2;
}

int CsvSignalFragmentReader::_read(SignalPart &fragment, PointValidator pointValidator)
{
    if (!m_file->isOpen()) {
        return -1;
    }

    int readPointCount = 0;
    bool isFileValid = true;
    bool hasReadPoint = (m_nextPoint != nullptr);
    if (!hasReadPoint) {
        m_nextPoint = std::make_unique<SignalPoint>();
    }

    if (hasReadPoint || (_readNextPoint(isFileValid) && isFileValid)) {
        do {
            switch (pointValidator(*m_nextPoint, readPointCount)) {
                case AbortReading:
                    return readPointCount;
                case SkipPoint:
                    break;
                case StorePoint:
                    fragment.append(static_cast<double>(m_nextPoint->first.toMSecsSinceEpoch()),
                                    m_nextPoint->second);
                    ++readPointCount;
                    break;
            }
        } while (_readNextPoint(isFileValid) && isFileValid);
    }

    m_nextPoint.reset(nullptr);
    return isFileValid ? readPointCount : -2;
}

bool CsvSignalFragmentReader::_readNextPoint(bool &isFileValid)
{
    if (m_file->atEnd()) { // Already read (at least 1 line in "setFile") before this check
        return false;
    }

    QStringList fileEntry = QString(m_file->readLine()).split(',');

    QDateTime dateTime = QDateTime::fromString(fileEntry[0], m_dateTimeFormat);
    bool isValueValid = true;
    double value = fileEntry[1].toDouble(&isValueValid);

    if (!dateTime.isValid() || !isValueValid) {
        isFileValid = false;
        return false;
    }

    m_nextPoint->first = dateTime;
    m_nextPoint->second = value;

    return true;
}
