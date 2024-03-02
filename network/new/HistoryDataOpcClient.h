#pragma once

#include <string>

#include "network/new/HistoryData.h"
//#include "OPC_UA.h"

class HistoryDataOpcClient : public HistoryData
{
    Q_OBJECT

public:
    explicit HistoryDataOpcClient(HistoryData* parent = nullptr);
    ~HistoryDataOpcClient() override = default;

signals:
    void error(const QString& message);

public slots:
    void requestData(const QDateTime& start = QDateTime(), const QDateTime& end = QDateTime());
    void setDataSource(const QString &serverName) override;

/*private:
    void onFileOpenError();
    void onFileParseError();
    void onWrongTimeIntervalError();*/
    
private:
    //std : std::unique_ptr<OPC_UA> m_opcReader;
};