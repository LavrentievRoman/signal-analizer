#pragma once

#include <QVector>

class SignalPart
{
public:
    SignalPart() = default;
    ~SignalPart() = default;

    void append(double x, double y);
    const QVector<double> &xValues() const;
    const QVector<double> &yValues() const;
    QVector<double> &xValues();
    QVector<double> &yValues();
    int size() const;

private:
    QVector<double> m_xValues;
    QVector<double> m_yValues;
};

