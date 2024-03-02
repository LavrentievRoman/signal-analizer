#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QtGlobal>
#include <QtWidgets/QListView>
#include <QtWidgets/QTableView>

#include "core/model/event.h"
#include "core/model/signal-chunk.h"
#include "core/model/SignalPart.h"
#include "core/model/TimeInterval.h"

#include "core/view/SignalPlot.h"

#include "network/new/HistoryDataCsvFile.h"
#include "network/new/HistoryDataOpcClient.h"

#include "player/model/player-core.h"
#include "player/model/new/Player.h"
#include "player/model/new/csvPlayer.h"
#include "player/model/new/opcPlayer.h"
#include "player/controller/PlayerControllerWidget.h"

#include "core/model/analyzer-facade.h"

AnalyzerFacade::~AnalyzerFacade()
{
}

AnalyzerFacade &AnalyzerFacade::instance()
{
    static AnalyzerFacade _instance;
    return _instance;
}

const GeneralAnalyzer *AnalyzerFacade::analyzer() const
{
    return _generalAnalyzer.get();
}

void AnalyzerFacade::bufferInfo() const
{
    qDebug() << "unanalyzedSize is" << _signalModel->unanalyzedSize();
    qDebug() << "size is" << _signalModel->size();
    qDebug() << "chunks count is" << _signalModel->chunksCount();
}

QStringList AnalyzerFacade::analyzerClassNames() const
{
    QStringList names;
    for (const auto &config : m_classesConfigs) {
        names.append(config->name());
    }
    return names;
}

QStringList AnalyzerFacade::analyzerClassIds() const
{
    QStringList names;
    for (const auto &config : m_classesConfigs) {
        names.append(config->folderName());
    }
    return names;
}

QStringList AnalyzerFacade::analyzerInstanceNames() const
{
    QStringList names;
    for (const auto &config : m_instancesConfigs) {
        names.append(config->name());
    }
    return names;
}

int AnalyzerFacade::analyzerInstancesCount() const
{
    return m_instancesConfigs.size();
}

int AnalyzerFacade::currentAnalyzerInstance() const
{
    return m_config->currentAnalyzerInstance();
}

std::shared_ptr<AnalyzerClassConfig> AnalyzerFacade::analyzerClassConfig(const QString &id) const
{
    return m_classesConfigs[id];
}

std::shared_ptr<AnalyzerInstanceConfig> AnalyzerFacade::currentAnalyzerInstanceConfig() const
{
    int index = currentAnalyzerInstance();
    return index > -1 ? m_instancesConfigs[index] : nullptr;
}

void AnalyzerFacade::resetCurrentAnalyzer()
{
    _generalAnalyzer->resetAnalyzer();
}

void AnalyzerFacade::trainCurrentAnalyzer(const QString &fileName, const QDateTime &start,
                                                                   const QDateTime &end)
{
    m_config->setLastTrainFilePath(fileName);
    m_historyFile->setDataSource(fileName);
    m_historyFile->requestData(start, end);

    /*
    m_config->setLastTrainSourcePath(sourceName);
    m_historyServer->setDataSource(suurceName);
    m_historyServer->requestData(start, end); 
    */
}

void AnalyzerFacade::trainCurrentAnalyzer(SignalPart &&trainData)
{
    qDebug() << "AnalyzerFacade: train data obtained, size =" << trainData.size();
    _generalAnalyzer->fit(std::move(trainData));
}

void AnalyzerFacade::setEventsTableView(QTableView *eventsTableView)
{
    eventsTableView->setModel(_eventsModel.get());
}

void AnalyzerFacade::setErrorsListView(QListView *errorsListView)
{
    errorsListView->setModel(_errorsListModel.get());
}

void AnalyzerFacade::setPlayerController(PlayerControllerWidget *playerController)
{
    playerController->setCore(m_csvPlayer.get());
    //playerController->setCore(m_opcPlayer.get());
}

void AnalyzerFacade::connectPlot(const SignalPlot &plot)
{
    connect(_signalModel.get(), &SignalModel::chunkCreated, &plot, &SignalPlot::onChunkCreated);
    connect(m_timeInterval.get(), &TimeInterval::changed,
            &plot, &SignalPlot::setDisplayedTimeInterval);
}

void AnalyzerFacade::addAnalyzerInstance(std::shared_ptr<AnalyzerInstanceConfig> config)
{
    QString path = config->classConfig().folderName() + "/" + m_config->nextAnalyzerInstanceId();

    QDir dir(AnalyzerInstanceConfig::RootFolderPath + path);
    dir.mkpath(".");
    dir.mkdir("saved_data");

    config->setFile(AnalyzerInstanceConfig::RootFolderPath + path + "/config.json");
    m_instancesConfigs.push_back(config);
    _generalAnalyzer->addAnalyzer(config);
    m_config->insertAnalyzerInstance(path);

    emit analyzerInstancesListChanged();
}

void AnalyzerFacade::removeAnalyzerInstance(int index)
{
    QString path = m_config->analyzerInstances().at(index);

    QDir dir(AnalyzerInstanceConfig::RootFolderPath + path);
    dir.removeRecursively();

    auto config = m_instancesConfigs[index];
    m_instancesConfigs.erase(m_instancesConfigs.begin() + index);
    if (index < static_cast<int>(m_instancesConfigs.size())) {
        selectAnalyzer(index);
    } else {
        selectAnalyzer(index - 1);
    }
    _generalAnalyzer->removeAnalyzer(index);
    m_config->removeAnalyzerInstance(index);

    emit analyzerInstancesListChanged();
}

const ApplicationConfig &AnalyzerFacade::applicationSettings() const
{
    return *m_config;
}

ApplicationConfig &AnalyzerFacade::applicationSettings()
{
    return *m_config;
}

void AnalyzerFacade::loadApplicationSettings()
{
    if (m_config->isSynchedWithFile()) {
        return;
    }

    if (!m_config->load()) {
        _errorsListModel->prepend(tr("Файл настроек отсутствует или повреждён, "
                                     "будут использованы настройки по умолчанию"));
    } else {
        m_instancesConfigs.clear();
        m_classesConfigs.clear();

        int size = m_config->analyzerClasses().size();
        for (int i = 0; i < size; ++i) {
            const QString &folderName = m_config->analyzerClasses()[i];
            auto config = std::make_shared<AnalyzerClassConfig>();
            config->setFolderName(folderName);
            config->load();
            m_classesConfigs.insert(folderName, config);
        }

        size = m_config->analyzerInstances().size();
        m_instancesConfigs.reserve(size);
        for (int i = 0; i < size; ++i) {
            QString classFolder = m_config->analyzerInstances()[i].section('/', 0, 0);
            m_instancesConfigs.push_back(
                std::make_shared<AnalyzerInstanceConfig>(m_classesConfigs[classFolder])
            );
            m_instancesConfigs[i]->setFile(
                AnalyzerInstanceConfig::RootFolderPath +
                m_config->analyzerInstances()[i] +
                "/config.json"
            );
            loadAnalyzerInstanceSettings(i);
            _generalAnalyzer->addAnalyzer(m_instancesConfigs[i]);
        }

        if (!m_config->lastPlayerFilePath().isEmpty()) {
            m_csvPlayer->setDataSource(m_config->lastPlayerFilePath());
        }

        if (!m_config->lastTrainFilePath().isEmpty()) {
            m_historyFile->setDataSource(m_config->lastTrainFilePath());
        }

        /*
        if (!m_config->lastTrainSourcePath().isEmpty()) {
            m_historyServer->setDataSource(m_config->lastTrainSourcePath());
        }
        */
    }
}

void AnalyzerFacade::saveSettings()
{
    m_config->save();
    for (const auto &config : m_classesConfigs) {
        config->save();
    }
    for (const auto &config : m_instancesConfigs) {
        config->save();
    }
}

void AnalyzerFacade::loadAnalyzerInstanceSettings(int index)
{
    Q_ASSERT(index >= 0 && index < static_cast<int>(m_instancesConfigs.size()));
    m_instancesConfigs[index]->load();
}

void AnalyzerFacade::saveAnalyzerInstanceSettings(int index)
{
    Q_ASSERT(index >= 0 && index < static_cast<int>(m_instancesConfigs.size()));
    m_instancesConfigs[index]->save();
}

void AnalyzerFacade::selectAnalyzer(int index)
{
    _generalAnalyzer->selectAnalyzer(index);
}

void AnalyzerFacade::setTimeInterval(double intervalMsecs)
{
    m_timeInterval->setValue(intervalMsecs);
}

AnalyzerFacade::AnalyzerFacade()
    : QObject(nullptr)
    , m_config(std::make_unique<ApplicationConfig>())
    , m_timeInterval(std::make_unique<TimeInterval>())
    , _signalModel(std::make_unique<SignalModel>())
    , _eventsModel(std::make_unique<EventsModel>())
    , _errorsListModel(std::make_unique<StringQueueModel>())
    , m_csvPlayer(std::make_unique<csvPlayer>())
    //, m_opcPlayer(std::make_unique<opcPlayer>())
    , _generalAnalyzer(std::make_unique<GeneralAnalyzer>())
    , m_historyFile(std::make_unique<HistoryDataCsvFile>())
    , m_historyServer(std::make_unique<HistoryDataOpcClient>())
{
    m_config->setFile("settings.json");

    connect(_generalAnalyzer.get(), &GeneralAnalyzer::currentAnalyzerIndexChanged,
            m_config.get(), &ApplicationConfig::setCurrentAnalyzerInstance);

    connect(_signalModel.get(), &SignalModel::chunkFull,
            _generalAnalyzer.get(), &GeneralAnalyzer::onChunkIsFull);

    connect(_signalModel.get(), &SignalModel::chunksReset,
            _eventsModel.get(), &EventsModel::onReset);

    connect(_generalAnalyzer.get(), &GeneralAnalyzer::inputSizeChanged,
            _signalModel.get(), &SignalModel::setChunkSize);

    connect(_generalAnalyzer.get(), &GeneralAnalyzer::eventGenerated,
            this, &AnalyzerFacade::onEventGenerated);

    connect(_generalAnalyzer.get(), &GeneralAnalyzer::error,
            _errorsListModel.get(), &StringQueueModel::prepend);

    connect(m_timeInterval.get(), &TimeInterval::changed,
            _signalModel.get(), &SignalModel::setChunkLifetime);
/*
    connect(m_opcPlayer.get(), &PlayerCore::fragmentRead,
            _signalModel.get(), &SignalModel::appendFragment);
    connect(m_opcPlayer.get(), &PlayerCore::error,
            _errorsListModel.get(), &StringQueueModel::prepend);
    connect(m_opcPlayer.get(), &PlayerCore::reset,
            _signalModel.get(), &SignalModel::onReset);
*/

    connect(m_csvPlayer.get(), &csvPlayer::fragmentRead,
            _signalModel.get(), &SignalModel::appendFragment);
    connect(m_csvPlayer.get(), &csvPlayer::error,
            _errorsListModel.get(), &StringQueueModel::prepend);
    connect(m_csvPlayer.get(), &csvPlayer::reset,
            _signalModel.get(), &SignalModel::onReset);
}

void AnalyzerFacade::onEventGenerated()
{
    _eventsModel->prependRow(std::move(*_generalAnalyzer->releaseNewEvent()));
}
