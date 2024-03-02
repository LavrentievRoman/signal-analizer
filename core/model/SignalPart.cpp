#include "SignalPart.h"

void SignalPart::append(double x, double y)
{
    m_xValues.append(x);
    m_yValues.append(y);
}

const QVector<double> &SignalPart::xValues() const
{
    return m_xValues;
}

const QVector<double> &SignalPart::yValues() const
{
    return m_yValues;
}

QVector<double> &SignalPart::xValues()
{
    return m_xValues;
}

QVector<double> &SignalPart::yValues()
{
    return m_yValues;
}

int SignalPart::size() const
{
    return m_xValues.size();
}
