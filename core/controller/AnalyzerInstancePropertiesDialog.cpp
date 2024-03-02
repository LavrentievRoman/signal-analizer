#include "core/model/analyzer-facade.h"

#include "ui_CreateAnalyzerInstanceDialog.h"

#include "AnalyzerInstancePropertiesDialog.h"

AnalyzerInstancePropertiesDialog::AnalyzerInstancePropertiesDialog(QWidget *parent)
    : CreateAnalyzerInstanceDialog(parent)
{
    m_ui->nameAndTypeWidget->hide();
    m_ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok);
}

void AnalyzerInstancePropertiesDialog::accept()
{
    QDialog::accept();
}

void AnalyzerInstancePropertiesDialog::open()
{
    auto instanceConfig = FACADE.currentAnalyzerInstanceConfig();
    if (!instanceConfig) {
        return;
    }
    _readInstanceConfig(*instanceConfig);

    QDialog::open();
}

void AnalyzerInstancePropertiesDialog::_readInstanceConfig(const AnalyzerInstanceConfig &config)
{
    setWindowTitle(tr("Свойства анализатора \"%1\", тип - \"%2\"").
                   arg(config.name()).arg(config.classConfig().name()));

    QJsonObject changedParams = config.changedInitParams();
    QList<InitParam> params = config.classConfig().initParams();
    params.prepend(AnalyzerClassConfig::defaultStepParam());
    params.prepend(AnalyzerClassConfig::defaultThresholdParam());

    m_ui->descriptionLabel->setText(config.classConfig().description());
    m_ui->paramsTableWidget->clearContents();
    m_ui->paramsTableWidget->setRowCount(params.size());
    for (int i = 0; i < params.size(); ++i) {
        _configureParamTableRow(i, params[i]);

        QString paramValue = params[i].defaultValue().toString();
        if (changedParams.contains(params[i].variableName())) {
            paramValue = changedParams[params[i].variableName()].toVariant().toString();
        }
        m_ui->paramsTableWidget->setCellWidget(i, 2, nullptr);
        m_ui->paramsTableWidget->item(i, 2)->setText(paramValue);
        m_ui->paramsTableWidget->item(i, 2)->setFlags(Qt::ItemFlag::ItemIsEnabled);
    }
}
