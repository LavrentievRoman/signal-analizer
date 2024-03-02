#pragma once

#include <QDialog>

namespace Ui {
class HistoryDataSourceChooseDialog;
}

class HistoryDataSourceChooseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HistoryDataSourceChooseDialog(QWidget *parent = nullptr);
    ~HistoryDataSourceChooseDialog();

    bool isCSV, isOPC;

public slots:
    void accept() override;

protected:
    Ui::HistoryDataSourceChooseDialog *m_ui;
};
