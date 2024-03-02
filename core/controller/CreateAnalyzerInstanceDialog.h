#pragma once

#include <QDialog>

#include "config/AnalyzerClassConfig.h"

namespace Ui {
class CreateAnalyzerInstanceDialog;
}

class CreateAnalyzerInstanceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateAnalyzerInstanceDialog(QWidget *parent = nullptr);
    ~CreateAnalyzerInstanceDialog() override;

public slots:
    void accept() override;
    void open() override;
    void onClassChanged(int index);

protected:
    void _configureParamTableRow(int row, const InitParam &param);

protected:
    Ui::CreateAnalyzerInstanceDialog *m_ui;

private:

    QJsonValue _paramTableRowValue(int row, const InitParam &defaultParam, bool *isValid = nullptr);
    QStringList _emptyParams();
    void _storeParamNameIfEmpty(int row, const InitParam &param, QStringList &list);

    std::shared_ptr<AnalyzerClassConfig> m_classConfig;
    QStringList m_classesIds;
};

