#pragma once

#include <memory>

#include <QtWidgets/QGroupBox>

#include "core/model/AnalyzerObserver.h"
#include "core/controller/CreateAnalyzerInstanceDialog.h"
#include "core/controller/AnalyzerInstancePropertiesDialog.h"
#include "core/controller/HistoryDataSourceChooseDialog.h"
#include "ui_analyzer-widget.h"

class QFileDialog;

class AnalyzerWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit AnalyzerWidget(QWidget *parent = nullptr);
    ~AnalyzerWidget() override = default;

signals:
    void currentAnalyzerChanged(int index);

public slots:
    void onLoadWeightsButtonPressed();
    void onSaveWeightsButtonPressed();
    void onTrainButtonPressed();
    void onFitProgressChanged(int progress);
    void onStateChanged(AnalyzerState state);
    void onAnalyzerListChanged();
    void setCurrentAnalyzer(int index);

private slots:
    void _onCurrentAnalyzerChanged(int index);

private:

    std::unique_ptr<Ui::AnalyzerWidget> m_ui;
    std::unique_ptr<CreateAnalyzerInstanceDialog> m_createInstanceDialog;
    std::unique_ptr<AnalyzerInstancePropertiesDialog> m_instancePropertiesDialog;
    std::unique_ptr<HistoryDataSourceChooseDialog> m_historyDataSourceChooseDialog;

    QFileDialog *m_fileDialog = nullptr;
};
