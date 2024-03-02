#include <QMessageBox>

#include "HistoryDataSourceChooseDialog.h"
#include "core/controller/ui_HistoryDataSourceChooseDialog.h"

HistoryDataSourceChooseDialog::HistoryDataSourceChooseDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::HistoryDataSourceChooseDialog)
{
    m_ui->setupUi(this);

    this->isCSV = false;
    this->isOPC = false;
}

HistoryDataSourceChooseDialog::~HistoryDataSourceChooseDialog()
{
    delete m_ui;
}

void HistoryDataSourceChooseDialog::accept()
{
    if (m_ui->sourceButtonGroup->checkedButton() == m_ui->csvRadioButton)
    {
        this->isCSV = true;
        this->isOPC = false;
    }
    if (m_ui->sourceButtonGroup->checkedButton() == m_ui->opcRadioButton)
    {
        this->isOPC = true;
        this->isCSV = false;
    }
    if (m_ui->sourceButtonGroup->checkedButton() == nullptr)
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Должен быть выбран источник данных!"));
        return;
    }

    QDialog::accept();
}
