#pragma once

#include <QtWidgets/QWidget>

#include "core/model/TimeInterval.h"

class QComboBox;
class QDoubleSpinBox;

class TimeIntervalSetupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimeIntervalSetupWidget(QWidget *parent = nullptr);
    ~TimeIntervalSetupWidget() override = default;

    double value() const;
    double msecsValue() const;
    TimeInterval::Units units() const;
    void setValueFromMsecs(double value); // Automatically converts to current units
    void setValueAndUnits(double value, TimeInterval::Units units = TimeInterval::Milliseconds);

signals:
    void msecsValueChanged(double value);

private:
    QDoubleSpinBox *m_valueSpinBox = nullptr;
    QComboBox *m_unitsComboBox = nullptr;
};
