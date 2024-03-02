#pragma once

#include "CreateAnalyzerInstanceDialog.h"

class AnalyzerInstanceConfig;

class AnalyzerInstancePropertiesDialog : public CreateAnalyzerInstanceDialog
{
    Q_OBJECT

public:
    explicit AnalyzerInstancePropertiesDialog(QWidget *parent = nullptr);
    ~AnalyzerInstancePropertiesDialog() override = default;

public slots:
    void accept() override;
    void open() override;

private:
    void _readInstanceConfig(const AnalyzerInstanceConfig &config);
};

