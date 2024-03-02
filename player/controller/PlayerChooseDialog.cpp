#include <QMessageBox>

#include "PlayerChooseDialog.h"
#include "player/controller/ui_PlayerChooseDialog.h"

PlayerChooseDialog::PlayerChooseDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::PlayerChooseDialog)
{
    m_ui->setupUi(this);

    this->isCSV = false;
    this->isOPC = false;
}

PlayerChooseDialog::~PlayerChooseDialog()
{
    delete m_ui;
}

void PlayerChooseDialog::accept()
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
