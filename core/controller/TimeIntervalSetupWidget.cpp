#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

#include "TimeIntervalSetupWidget.h"

TimeIntervalSetupWidget::TimeIntervalSetupWidget(QWidget *parent)
    : QWidget(parent)
    , m_valueSpinBox(new QDoubleSpinBox(this))
    , m_unitsComboBox(new QComboBox(this))
{
    m_valueSpinBox->setMinimum(1);
    m_valueSpinBox->setMaximum(100'000);

    m_unitsComboBox->addItems({
        tr("миллисекунд"),
        tr("секунд"),
        tr("минут"),
        tr("часов"),
        tr("суток"),
        tr("недель"),
        tr("месяцев"),
        tr("лет")
    });

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(new QLabel(tr("Отображаемый период:"), this));
    mainLayout->addWidget(m_valueSpinBox);
    mainLayout->addWidget(m_unitsComboBox);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    connect(m_valueSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double) { emit msecsValueChanged(msecsValue()); });
    connect(m_unitsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { emit msecsValueChanged(msecsValue()); });
}

double TimeIntervalSetupWidget::value() const
{
    return m_valueSpinBox->value();
}

double TimeIntervalSetupWidget::msecsValue() const
{
    return TimeInterval::toMsecs(m_valueSpinBox->value(), units());
}

TimeInterval::Units TimeIntervalSetupWidget::units() const
{
    return static_cast<TimeInterval::Units>(m_unitsComboBox->currentIndex());
}

void TimeIntervalSetupWidget::setValueFromMsecs(double value)
{
    m_valueSpinBox->setValue(TimeInterval::fromMsecs(value, units()));
}

void TimeIntervalSetupWidget::setValueAndUnits(double value, TimeInterval::Units units)
{
    m_valueSpinBox->setValue(value);
    m_unitsComboBox->setCurrentIndex(units);
}
