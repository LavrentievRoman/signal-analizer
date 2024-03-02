#pragma once

#include <QwtPlot>

class QwtPlotCurve;

class SignalChunk;

class SignalPlot : public QwtPlot
{
    Q_OBJECT

public:
    explicit SignalPlot(QWidget *parent = nullptr);
    ~SignalPlot() override = default;

    int maxCapacity() const;
    double displayedTimeInterval() const;

    void setAxisTitle(QwtAxisId axisId, const QString &title);
    void setDataRange(const double minY, const double maxY);

public slots:
    void onChunkCreated(const SignalChunk *chunk);
    void setDisplayedTimeInterval(const double intervalMsecs);

private:
    void addLegendItem(const QString &text, const QColor color);
    bool isCurveVisible(const QwtPlotCurve *curve) const;
    void recalculateTimeScale();

private:
    double m_displayedTimeInterval = 1.0;
    QwtPlotCurve *m_lastCurve = nullptr;
};
