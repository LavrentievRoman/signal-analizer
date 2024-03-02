#pragma once

#include <memory>

#include <QtCore/QObject>

#include "player/model/csv-signal-fragment-reader.h"

class HistoryData : public QObject
{
    Q_OBJECT

public:
    explicit HistoryData(QObject* parent = nullptr);

signals:
    void error(const QString& message);

public slots:
    virtual void requestData(const QDateTime& start = QDateTime(), const QDateTime& end = QDateTime()) = 0;
    virtual void setDataSource(const QString &sourceName) = 0;
};