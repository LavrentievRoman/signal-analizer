#include <QMessageBox>
#include <QtCore/QDebug>

#include <QtWidgets/QFileDialog>

#include "core/model/analyzer-facade.h"

#include "core/controller/analyzer-widget.h"

AnalyzerWidget::AnalyzerWidget(QWidget *parent)
    : QGroupBox(parent)
    , m_ui(std::make_unique<Ui::AnalyzerWidget>())
    , m_createInstanceDialog(new CreateAnalyzerInstanceDialog(this))
    , m_instancePropertiesDialog(new AnalyzerInstancePropertiesDialog(this))
    , m_historyDataSourceChooseDialog (new HistoryDataSourceChooseDialog(this))
    , m_fileDialog(new QFileDialog(this))
{
    m_ui->setupUi(this);

    m_fileDialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    m_fileDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    m_fileDialog->setNameFilter("*.csv");

    // TODO: [tvz] show after save/load implementation
    m_ui->groupBoxSaveLoad->setHidden(true);

    connect(m_ui->comboBoxAnalyzer, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AnalyzerWidget::_onCurrentAnalyzerChanged);

    connect(m_ui->pushButtonAddInstance, &QPushButton::pressed,
            m_createInstanceDialog.get(), &CreateAnalyzerInstanceDialog::open);
    connect(m_ui->pushButtonRemoveInstance, &QPushButton::pressed, this, [this]() {
        FACADE.removeAnalyzerInstance(m_ui->comboBoxAnalyzer->currentIndex());
    });
    connect(m_ui->pushButtonInstanceProperties, &QPushButton::pressed,
            m_instancePropertiesDialog.get(), &AnalyzerInstancePropertiesDialog::open);

    connect(m_ui->pushButtonTrain, &QPushButton::pressed,
            m_historyDataSourceChooseDialog.get(), &HistoryDataSourceChooseDialog::open);

    connect(m_historyDataSourceChooseDialog.get(), &HistoryDataSourceChooseDialog::accepted,
            this, &AnalyzerWidget::onTrainButtonPressed);

    connect(m_ui->pushButtonLoad, &QPushButton::pressed,
            this, &AnalyzerWidget::onLoadWeightsButtonPressed);
    connect(m_ui->pushButtonSave, &QPushButton::pressed,
            this, &AnalyzerWidget::onSaveWeightsButtonPressed);
}

void AnalyzerWidget::onLoadWeightsButtonPressed()
{
    // TODO: [tvz] implement
}

void AnalyzerWidget::onSaveWeightsButtonPressed()
{
    // TODO: [tvz] implement
}

void AnalyzerWidget::onTrainButtonPressed()
{
    if (m_historyDataSourceChooseDialog->isCSV)
    {
        if (m_fileDialog->exec() == QDialog::Accepted && !m_fileDialog->selectedFiles().isEmpty()) {
            if (m_ui->checkBoxResetWeights->isChecked()) {
                FACADE.resetCurrentAnalyzer();
            }
            QString fileName = m_fileDialog->selectedFiles().at(0);
            m_fileDialog->setDirectory(QDir(fileName));
            // TODO: [tvz] request start and end time
            FACADE.trainCurrentAnalyzer(fileName);
        }
    }

    if (m_historyDataSourceChooseDialog->isOPC)
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Реализация не готова!"));
        return;
    }
}


void AnalyzerWidget::onFitProgressChanged(int progress)
{
    m_ui->labelStateValue->setText(tr("Обучается... (%1%)").arg(progress));
}

void AnalyzerWidget::onStateChanged(AnalyzerState state)
{
    QPalette palette = m_ui->labelStateValue->palette();
    QPalette::ColorRole role = m_ui->labelStateValue->foregroundRole();
    switch (state) {
        case AnalyzerState::Loading:
            m_ui->labelStateValue->setText(tr("Загружается..."));
            palette.setColor(role, Qt::darkYellow);
            break;
        case AnalyzerState::NotTrained:
            m_ui->labelStateValue->setText(tr("Не обучен"));
            palette.setColor(role, Qt::black);
            break;
        case AnalyzerState::Training:
            m_ui->labelStateValue->setText(tr("Обучается..."));
            palette.setColor(role, Qt::darkYellow);
            break;
        case AnalyzerState::SuccessTrained:
            m_ui->labelStateValue->setText(tr("Обучен"));
            palette.setColor(role, Qt::darkGreen);
            break;
        case AnalyzerState::FailTrained:
            m_ui->labelStateValue->setText(tr("Ошибка обучения"));
            palette.setColor(role, Qt::darkRed);
            break;
    }
    m_ui->labelStateValue->setPalette(palette);
}

void AnalyzerWidget::onAnalyzerListChanged()
{
    m_ui->comboBoxAnalyzer->blockSignals(true);

    int currentIndex = m_ui->comboBoxAnalyzer->currentIndex();
    m_ui->comboBoxAnalyzer->clear();
    m_ui->comboBoxAnalyzer->addItems(FACADE.analyzerInstanceNames());

    if (currentIndex < m_ui->comboBoxAnalyzer->count()) {
        m_ui->comboBoxAnalyzer->setCurrentIndex(currentIndex);
    } else if (m_ui->comboBoxAnalyzer->count()) {
        m_ui->comboBoxAnalyzer->setCurrentIndex(m_ui->comboBoxAnalyzer->count() - 1);
    }

    m_ui->comboBoxAnalyzer->blockSignals(false);
}

void AnalyzerWidget::setCurrentAnalyzer(int index)
{
    if (index != m_ui->comboBoxAnalyzer->currentIndex()) {
        m_ui->comboBoxAnalyzer->setCurrentIndex(index);
    }
}

void AnalyzerWidget::_onCurrentAnalyzerChanged(int index)
{
    m_ui->pushButtonRemoveInstance->setEnabled(index > -1);
    m_ui->pushButtonInstanceProperties->setEnabled(index > -1);
    m_ui->pushButtonTrain->setEnabled(index > -1);
    m_ui->checkBoxResetWeights->setEnabled(index > -1);
    m_ui->groupBoxSaveLoad->setEnabled(index > -1);
    emit currentAnalyzerChanged(index);
}
