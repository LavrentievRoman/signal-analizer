#include <QMessageBox>

#include <QtWidgets/QAction>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QToolButton>

#include "player/model/new/csvPlayer.h"
#include "player/model/new/opcPlayer.h"

#include "player/model/player-core.h"

#include "PlayerControllerWidget.h"

PlayerControllerWidget::PlayerControllerWidget(QWidget *parent)
    : QWidget{parent}
    , m_ui(std::make_unique<Ui::PlayerControllerWidget>())
    , m_fileDialog(new QFileDialog(this))
    , m_playerChooseDialog(new PlayerChooseDialog(this))
{
    m_ui->setupUi(this);

    m_ui->stopButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaStop));
    connect(m_ui->stopButton, &QToolButton::clicked,
            this, &PlayerControllerWidget::onStopButtonClicked);

    m_ui->playAndPauseButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
    connect(m_ui->playAndPauseButton, &QToolButton::clicked,
            this, &PlayerControllerWidget::onPlayAndPauseButtonClicked);

    m_ui->selectFileButton->setIcon(this->style()->standardIcon(QStyle::SP_FileDialogContentsView));

    connect(m_ui->selectFileButton, &QToolButton::clicked,
            m_playerChooseDialog.get(), &PlayerChooseDialog::open);

    connect(m_playerChooseDialog.get(), &PlayerChooseDialog::accepted,
            this, &PlayerControllerWidget::onSelectFileButtonClicked);

    connect(m_ui->doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PlayerControllerWidget::onSpeedChanged);


    m_fileDialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    m_fileDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    m_fileDialog->setNameFilter("*.csv");
}

double PlayerControllerWidget::speedPerSec() const
{
    return m_ui->doubleSpinBox->value() / 60;
}

QString PlayerControllerWidget::sourceName() const
{
    return m_core->sourceName();
}

void PlayerControllerWidget::setCore(Player *core)
{
    m_core = core;
}

void PlayerControllerWidget::setSpeedPerSec(double eventsPerSec)
{
    m_ui->doubleSpinBox->setValue(eventsPerSec * 60);
}

void PlayerControllerWidget::play()
{
    m_ui->playAndPauseButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPause));
    m_core->resume();
}

void PlayerControllerWidget::pause()
{
    m_core->pause();
    m_ui->playAndPauseButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
}

void PlayerControllerWidget::onPlayAndPauseButtonClicked()
{
    Q_ASSERT(m_core);
    m_core->isPaused() ? play() : pause();
}

void PlayerControllerWidget::onStopButtonClicked()
{
    Q_ASSERT(m_core);
    m_core->stop();
    m_ui->playAndPauseButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
}

void PlayerControllerWidget::onSelectFileButtonClicked()
{
    Q_ASSERT(m_core);
    if (m_playerChooseDialog->isCSV)
    {
        if (m_fileDialog->exec() == QDialog::Accepted && !m_fileDialog->selectedFiles().isEmpty()) {
            QString fileName = m_fileDialog->selectedFiles().at(0);
            m_fileDialog->setDirectory(QDir(fileName));
            m_core->setDataSource(fileName);
            m_ui->playAndPauseButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
        }
    }
    if (m_playerChooseDialog->isOPC)
    {
        //setCore(m_opcPlayer.get());
        QMessageBox::critical(this, tr("Ошибка"), tr("Реализация не готова!"));
        return;
    }

}

void PlayerControllerWidget::onSpeedChanged(double eventsPerMin)
{
    Q_ASSERT(m_core);
    m_core->setSpeed(eventsPerMin / 60);
}
