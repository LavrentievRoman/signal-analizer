#pragma once

#include <memory>

#include <QWidget>

#include "player/controller/PlayerChooseDialog.h"

#include "ui_PlayerControllerWidget.h"

class QFileDialog;

class opcPlayer;
class csvPlayer;
class Player;

class PlayerCore;

class PlayerControllerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerControllerWidget(QWidget *parent = nullptr);
    ~PlayerControllerWidget() override = default;

    double speedPerSec() const;
    QString sourceName() const;

    void setCore(Player *core);
    void setSpeedPerSec(double eventsPerSec);

    void play();
    void pause();

private slots:
    void onPlayAndPauseButtonClicked();
    void onStopButtonClicked();
    void onSelectFileButtonClicked();
    void onSpeedChanged(double eventsPerMin);

private:
    std::unique_ptr<Ui::PlayerControllerWidget> m_ui;

    std::unique_ptr<PlayerChooseDialog> m_playerChooseDialog;

    Player *m_core = nullptr;
    QFileDialog *m_fileDialog = nullptr;
};

