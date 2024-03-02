#pragma once

#include <memory>

#include <QtWidgets/QWidget>

#include "ui_main-widget.h"

class PlayerControllerWidget;
class SignalPlot;
class TimeIntervalSetupWidget;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget() override;

public slots:
    void saveSettings();
    void setVisible(bool visible) override;

private:
    void load();

private:
    std::unique_ptr<Ui::MainWidget> _ui;
    bool _isLoaded = false;
};
