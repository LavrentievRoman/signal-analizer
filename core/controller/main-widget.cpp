#include "config/ApplicationConfig.h"

#include "core/model/analyzer-facade.h"
#include "core/view/SignalPlot.h"
#include "core/controller/TimeIntervalSetupWidget.h"
#include "player/controller/PlayerControllerWidget.h"

#include "core/controller/main-widget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , _ui(std::make_unique<Ui::MainWidget>())
{
    _ui->setupUi(this);

    FACADE.setEventsTableView(_ui->eventsTableView);
    FACADE.setErrorsListView(_ui->errorsListView);
    FACADE.setPlayerController(_ui->playerController);
    FACADE.connectPlot(*_ui->plot);

    connect(_ui->timeIntervalSetupWidget, &TimeIntervalSetupWidget::msecsValueChanged,
            &FACADE, &AnalyzerFacade::setTimeInterval);

    connect(_ui->analyzerWidget, &AnalyzerWidget::currentAnalyzerChanged,
            &FACADE, &AnalyzerFacade::selectAnalyzer);
    connect(&FACADE, &AnalyzerFacade::analyzerInstancesListChanged,
            _ui->analyzerWidget, &AnalyzerWidget::onAnalyzerListChanged);
    connect(FACADE.analyzer(), &GeneralAnalyzer::currentAnalyzerIndexChanged,
            _ui->analyzerWidget, &AnalyzerWidget::setCurrentAnalyzer);
    connect(FACADE.analyzer(), &GeneralAnalyzer::fitProgressChanged,
            _ui->analyzerWidget, &AnalyzerWidget::onFitProgressChanged);
    connect(FACADE.analyzer(), &GeneralAnalyzer::stateChanged,
            _ui->analyzerWidget, &AnalyzerWidget::onStateChanged);

    connect(FACADE.analyzer(), &GeneralAnalyzer::stateChanged, [this](AnalyzerState state) {
        _ui->playerController->setEnabled(state == AnalyzerState::SuccessTrained);
        _ui->timeIntervalSetupWidget->setEnabled(state == AnalyzerState::SuccessTrained);

        if (!_ui->playerController->isEnabled()) {
            _ui->playerController->pause();
        }
    });
}

MainWidget::~MainWidget()
{
    saveSettings();
}

void MainWidget::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    load();
}

void MainWidget::saveSettings()
{
    ApplicationConfig &settings = FACADE.applicationSettings();
    settings.setTimeIntervalValue(_ui->timeIntervalSetupWidget->value());
    settings.setTimeIntervalUnits(_ui->timeIntervalSetupWidget->units());
    settings.setPlayerSpeed(_ui->playerController->speedPerSec());
    settings.setLastPlayerFilePath(_ui->playerController->sourceName());
    FACADE.saveSettings();
}

void MainWidget::load()
{
    if (_isLoaded) {
        return;
    }

    _isLoaded = true;

    FACADE.loadApplicationSettings();

    _ui->analyzerWidget->onAnalyzerListChanged();
    _ui->analyzerWidget->setCurrentAnalyzer(FACADE.currentAnalyzerInstance());

    const ApplicationConfig &settings = FACADE.applicationSettings();
    _ui->timeIntervalSetupWidget->setValueAndUnits(settings.timeIntervalValue(),
                                                   settings.timeIntervalUnits());
    _ui->playerController->setSpeedPerSec(settings.playerSpeed());
}
