#pragma once

#include <QDialog>

namespace Ui {
class PlayerChooseDialog;
}

class PlayerChooseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerChooseDialog(QWidget *parent = nullptr);
    ~PlayerChooseDialog();

    bool isCSV, isOPC;

public slots:
    void accept() override;

protected:
    Ui::PlayerChooseDialog *m_ui;
};

