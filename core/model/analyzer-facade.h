#pragma once

#include <memory>

#include <QtCore/QObject>

#include "config/AnalyzerClassConfig.h"
#include "config/AnalyzerInstanceConfig.h"
#include "config/ApplicationConfig.h"

#include "core/model/events-model.h"
#include "core/model/general-analyzer.h"
#include "core/model/signal-model.h"
#include "core/model/string-queue-model.h"

class QListView;
class QTableView;

class Analyzer;
class HistoryDataCsvFile;
class HistoryDataOpcClient;
class csvPlayer;
class opcPlayer;
class Player;
class PlayerControllerWidget;
class SignalPart;
class SignalPlot;
class TimeInterval;

class PlayerCore;

#define FACADE AnalyzerFacade::instance()


class AnalyzerFacade : public QObject
{
    Q_OBJECT

public:
    virtual ~AnalyzerFacade();

    static AnalyzerFacade &instance();

    const GeneralAnalyzer *analyzer() const;

    void bufferInfo() const;  // TODO: remove this method

    QStringList analyzerClassNames() const;
    QStringList analyzerClassIds() const;
    QStringList analyzerInstanceNames() const;
    int analyzerInstancesCount() const;
    int currentAnalyzerInstance() const;

    std::shared_ptr<AnalyzerClassConfig> analyzerClassConfig(const QString &id) const;
    std::shared_ptr<AnalyzerInstanceConfig> currentAnalyzerInstanceConfig() const;

    void resetCurrentAnalyzer();
    void trainCurrentAnalyzer(const QString &fileName, const QDateTime &start = QDateTime(),
                                                       const QDateTime &end = QDateTime());
    void trainCurrentAnalyzer(SignalPart &&trainData);

    void setEventsTableView(QTableView *eventsTableView);
    void setErrorsListView(QListView *errorsListView);
    void setPlayerController(PlayerControllerWidget *playerController);

    void connectPlot(const SignalPlot &plot);

    void addAnalyzerInstance(std::shared_ptr<AnalyzerInstanceConfig> config);
    void removeAnalyzerInstance(int index);

    const ApplicationConfig &applicationSettings() const;
    ApplicationConfig &applicationSettings();
    void loadApplicationSettings();
    void saveSettings();
    void loadAnalyzerInstanceSettings(int index);
    void saveAnalyzerInstanceSettings(int index);

signals:
    void analyzerInstancesListChanged();

public slots:
    void selectAnalyzer(int index);
    void setTimeInterval(double intervalMsecs);

private:
    explicit AnalyzerFacade();

private slots:
    void onEventGenerated();

private:
    std::unique_ptr<ApplicationConfig> m_config;
    QMap<QString, std::shared_ptr<AnalyzerClassConfig>> m_classesConfigs;
    std::vector<std::shared_ptr<AnalyzerInstanceConfig>> m_instancesConfigs;
    std::unique_ptr<TimeInterval> m_timeInterval;
    std::unique_ptr<SignalModel> _signalModel;
    std::unique_ptr<EventsModel> _eventsModel;
    std::unique_ptr<StringQueueModel> _errorsListModel;
    std::unique_ptr<csvPlayer> m_csvPlayer;
    std::unique_ptr<opcPlayer> m_opcPlayer;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<PlayerCore> m_playerCore;
    std::unique_ptr<GeneralAnalyzer> _generalAnalyzer;
    std::unique_ptr<HistoryDataCsvFile> m_historyFile;
    std::unique_ptr<HistoryDataOpcClient> m_historyServer;
};
