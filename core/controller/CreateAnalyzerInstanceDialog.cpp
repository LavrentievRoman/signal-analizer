#include <QMessageBox>

#include "core/model/analyzer-facade.h"

#include "CreateAnalyzerInstanceDialog.h"
#include "ui_CreateAnalyzerInstanceDialog.h"

CreateAnalyzerInstanceDialog::CreateAnalyzerInstanceDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CreateAnalyzerInstanceDialog)
{
    m_ui->setupUi(this);
    m_ui->paramsTableWidget->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);

    connect(m_ui->analyzerClassComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CreateAnalyzerInstanceDialog::onClassChanged);
}

CreateAnalyzerInstanceDialog::~CreateAnalyzerInstanceDialog()
{
    delete m_ui;
}

void CreateAnalyzerInstanceDialog::accept()
{
    if (m_ui->nameLineEdit->text().isEmpty()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Название анализатора не может быть пустым!"));
        return;
    } else {
        // TODO: [tvz] check if the name already exists in instance list
    }

    QStringList invalidParams = _emptyParams();

    if (!invalidParams.isEmpty()) {
        QString message = tr("Не заданы значения параметров");
        for (const QString &paramName : invalidParams) {
            message += " \"" + paramName + "\",";
        }
        message.replace(message.length() - 1, 1, "!");
        QMessageBox::critical(this, tr("Ошибка"), message);
    }

    auto instanceConfig = std::make_shared<AnalyzerInstanceConfig>(m_classConfig);
    instanceConfig->setName(m_ui->nameLineEdit->text());

    QString paramName;
    QJsonValue paramValue;
    bool isValueValid;

    paramValue = _paramTableRowValue(0, AnalyzerClassConfig::defaultStepParam(), &isValueValid);
    if (isValueValid) {
        instanceConfig->setStep(
            paramValue.isUndefined() ?
            AnalyzerClassConfig::defaultStepParam().defaultValue().toInt() :
            paramValue.toInt()
        );
    } else {
        invalidParams.append(AnalyzerClassConfig::defaultStepParam().variableName());
    }

    paramValue = _paramTableRowValue(1, AnalyzerClassConfig::defaultThresholdParam(), &isValueValid);
    if (isValueValid) {
        instanceConfig->setThreshold(
            paramValue.isUndefined() ?
            AnalyzerClassConfig::defaultThresholdParam().defaultValue().toDouble() :
            paramValue.toDouble()
        );
    } else {
        invalidParams.append(AnalyzerClassConfig::defaultThresholdParam().variableName());
    }

    for (int i = 0; i < m_classConfig->initParams().size(); ++i) {
        paramValue = _paramTableRowValue(i + 2, m_classConfig->initParam(i), &isValueValid);
        paramName = m_classConfig->initParam(i).variableName();
        if (isValueValid) {
            instanceConfig->changedInitParamsRef().insert(paramName, paramValue);
        } else if (!paramValue.isUndefined()) {
            invalidParams.append(paramName);
        }
    }

    if (!invalidParams.isEmpty()) {
        QString message = tr("Неверные заданы значения параметров");
        for (const QString &paramName : invalidParams) {
            message += " \"" + paramName + "\",";
        }
        message.replace(message.length() - 1, 1, "!");
        QMessageBox::critical(this, tr("Ошибка"), message);
        return;
    }

    FACADE.addAnalyzerInstance(instanceConfig);
    QDialog::accept();

    FACADE.selectAnalyzer(FACADE.analyzerInstancesCount() - 1);
}

void CreateAnalyzerInstanceDialog::open()
{
    m_classesIds = FACADE.analyzerClassIds();
    int currentIndex = m_ui->analyzerClassComboBox->currentIndex();
    m_ui->analyzerClassComboBox->clear();
    m_ui->analyzerClassComboBox->addItems(FACADE.analyzerClassNames());
    if (currentIndex >= 0 && currentIndex < m_ui->analyzerClassComboBox->count()) {
        m_ui->analyzerClassComboBox->setCurrentIndex(currentIndex);
    }
    QDialog::open();
}

void CreateAnalyzerInstanceDialog::onClassChanged(int index)
{
    m_ui->paramsTableWidget->clearContents();

    if (index < 0 || index >= m_classesIds.size()) {
        return;
    }

    m_classConfig = FACADE.analyzerClassConfig(m_classesIds[index]);
    m_ui->descriptionLabel->setText(m_classConfig->description());
    m_ui->paramsTableWidget->setRowCount(m_classConfig->initParams().size() + 2);
    _configureParamTableRow(0, AnalyzerClassConfig::defaultStepParam());
    _configureParamTableRow(1, AnalyzerClassConfig::defaultThresholdParam());
    for (int i = 0; i < m_classConfig->initParams().size(); ++i) {
        _configureParamTableRow(i + 2, m_classConfig->initParam(i));
    }
}

void CreateAnalyzerInstanceDialog::_configureParamTableRow(int row, const InitParam &param)
{
    QTableWidgetItem *paramName = new QTableWidgetItem();
    QTableWidgetItem *paramVariableName = new QTableWidgetItem();
    QTableWidgetItem *paramDefaultValue = new QTableWidgetItem();
    QTableWidgetItem *paramDescription = new QTableWidgetItem();

    paramName->setText(param.name());
    paramVariableName->setText(param.variableName());
    paramDescription->setText(param.description());

    paramName->setFlags(Qt::ItemIsEnabled);
    paramVariableName->setFlags(Qt::ItemIsEnabled);
    paramDefaultValue->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
    paramDescription->setFlags(Qt::ItemIsEnabled);

    m_ui->paramsTableWidget->setItem(row, 0, paramName);
    m_ui->paramsTableWidget->setItem(row, 1, paramVariableName);
    m_ui->paramsTableWidget->setItem(row, 2, paramDefaultValue);
    m_ui->paramsTableWidget->setItem(row, 3, paramDescription);

    m_ui->paramsTableWidget->setCellWidget(row, 2, new QLineEdit());
    qobject_cast<QLineEdit *>(m_ui->paramsTableWidget->cellWidget(row, 2))->setPlaceholderText(
        param.defaultValue().toString()
    );
}

QJsonValue CreateAnalyzerInstanceDialog::_paramTableRowValue(int row, const InitParam &defaultParam,
                                                             bool *isValid)
{
    if (isValid) {
        *isValid = true;
    }

    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(
        m_ui->paramsTableWidget->cellWidget(row, 2)
    );
    QString valueString = lineEdit ? lineEdit->text() : "";

    if (valueString.isEmpty()) {
        return QJsonValue(QJsonValue::Undefined);
    }

    QJsonValue defaultValue = defaultParam.defaultValueJson();
    QString lowerValueString = valueString.toLower();
    QJsonValue value(QJsonValue::Null);
    switch (defaultValue.type()) {
        case QJsonValue::Bool:
            if (lowerValueString == "true") {
                value = true;
            } else if (lowerValueString == "false") {
                value = false;
            } else {
                value = valueString.toInt(isValid) != 0;
            }
            break;
        case QJsonValue::Double:
            value = valueString.toDouble(isValid);
            break;
        case QJsonValue::String:
            value = valueString;
            break;
        case QJsonValue::Null:
            if (isValid) {
                *isValid = lowerValueString == "null" || valueString.isEmpty();
            }
            break;
        default:
            if (isValid) {
                *isValid = false;
            }
            break;
    }

    return value;
}

QStringList CreateAnalyzerInstanceDialog::_emptyParams()
{
    QStringList invalidParams;

    _storeParamNameIfEmpty(0, AnalyzerClassConfig::defaultStepParam(), invalidParams);
    _storeParamNameIfEmpty(1, AnalyzerClassConfig::defaultThresholdParam(), invalidParams);
    for (int i = 0; i < m_classConfig->initParams().size(); ++i) {
        _storeParamNameIfEmpty(i + 2, m_classConfig->initParam(i), invalidParams);
    }

    return invalidParams;
}

void CreateAnalyzerInstanceDialog::_storeParamNameIfEmpty(int row, const InitParam &param,
                                                        QStringList &list)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(m_ui->paramsTableWidget->cellWidget(row, 2));
    QVariant defaultValue = param.defaultValue();
    if ((!lineEdit || lineEdit->text().isEmpty()) && defaultValue.toString().isEmpty()) {
        list.append(param.variableName());
    }
}
