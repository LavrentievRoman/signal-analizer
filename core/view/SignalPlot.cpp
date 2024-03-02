#include <QtCore/QDateTime>

#include <QtGui/QPen>

#include <QtWidgets/QApplication>

#include <QwtDateScaleDraw>
#include <QwtLegend>
#include <QwtLinearScaleEngine>
#include <QwtPlotCurve>
#include <QwtPlotGrid>
#include <QwtPlotLayout>
#include <QwtScaleWidget>

#include "core/model/signal-chunk.h"

#include "SignalPlot.h"

namespace
{
    class FixedTickCountScaleEngine : public QwtLinearScaleEngine
    {
    public:
        FixedTickCountScaleEngine() = default;
        ~FixedTickCountScaleEngine() override = default;

        int tickCount(QwtScaleDiv::TickType tickType) const
        {
            return m_ticksCount[tickType];
        }

        void setTickCount(QwtScaleDiv::TickType tickType, const int count)
        {
            m_ticksCount[tickType] = count;
        }

        static QList<double> calculateTickPositions(const QwtInterval &interval,
                                                    int totalTicksCount)
        {
            QList<double> ticks;
            if (totalTicksCount < 1) {
                return ticks;
            }

            ticks.reserve(totalTicksCount);
            int insertedTicksCount = 0;
            int tickOffset = 0;
            double ticksDistance = interval.width() / (totalTicksCount - 1);

            if (totalTicksCount % 2) {
                ticks.append(interval.minValue() + interval.width() / 2);
                ++insertedTicksCount;
            }

            while (insertedTicksCount < totalTicksCount) {
                ticks.insert(insertedTicksCount - tickOffset,
                             interval.maxValue() - ticksDistance * tickOffset);
                ticks.insert(tickOffset, interval.minValue() + ticksDistance * tickOffset);
                insertedTicksCount += 2;
                ++tickOffset;
            }


            return ticks;
        }

        QwtScaleDiv divideScale(double x1, double x2, int /*maxMajorSteps*/, int /*maxMinorSteps*/,
                                double /*stepSize*/) const override
        {
            QList<double> ticks[QwtScaleDiv::NTickTypes];
            QwtInterval interval(x1, x2);
            interval.setBorderFlags(QwtInterval::BorderFlag::IncludeBorders);

            for (int tickType = 0; tickType < QwtScaleDiv::NTickTypes; ++tickType) {
                ticks[tickType] = calculateTickPositions(
                                      interval,
                                      tickCount(QwtScaleDiv::TickType(tickType))
                );
            }

            return QwtScaleDiv(interval, ticks);
        }

    private:
        int m_ticksCount[QwtScaleDiv::NTickTypes] = {0};
    };
}

SignalPlot::SignalPlot(QWidget *parent)
    : QwtPlot{parent}
{
    setAutoReplot(false);
    setAutoDelete(true);

    setFont(QApplication::font());

    setContentsMargins(10, 10, 10, 10);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::white);
    setAutoFillBackground(true);
    setPalette(palette);

    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->setMajorPen(QPen(Qt::lightGray, 0, Qt::SolidLine));
    grid->attach(this);
    grid->setAxes(QwtAxis::XBottom, QwtAxis::YRight);

    QwtDateScaleDraw *dateScaleDraw = new QwtDateScaleDraw;
    dateScaleDraw->setDateFormat(QwtDate::IntervalType::Millisecond, "dd.MM.yyyy\nhh:mm:ss");
    setAxisScaleDraw(QwtAxis::XBottom, dateScaleDraw);

    FixedTickCountScaleEngine *scaleEngine = new FixedTickCountScaleEngine;
    scaleEngine->setTickCount(QwtScaleDiv::MajorTick, 5);
    setAxisScaleEngine(QwtAxis::XBottom, scaleEngine);
    setAxisLabelAlignment(QwtAxis::XBottom, Qt::AlignCenter | Qt::AlignBottom);

    enableAxis(QwtAxis::YLeft, false);
    enableAxis(QwtAxis::YRight, true);

    axisWidget(QwtAxis::YRight)->setLayoutFlag(QwtScaleWidget::TitleInverted, false);

    setAxisTitle(QwtAxis::XBottom, tr("Время и дата"));
    setAxisTitle(QwtAxis::YRight, tr("Значение"));

    // TODO: [Evgeny] Align legend to left-center. It will looks better I guess.
    insertLegend(new QwtLegend, QwtPlot::LeftLegend);
    addLegendItem(tr("Штатный"), ClassLabel(ClassLabel::Enum::Regular).color());
    addLegendItem(tr("Аномальный"), ClassLabel(ClassLabel::Enum::Anomalous).color());
    addLegendItem(tr("Не размечен"), ClassLabel(ClassLabel::Enum::NoLabel).color());

    plotLayout()->setAlignCanvasToScales(true);
}

double SignalPlot::displayedTimeInterval() const
{
    return m_displayedTimeInterval;
}

void SignalPlot::setAxisTitle(QwtAxisId axisId, const QString &title)
{
    QFont font = this->font();
    QwtPlot::setAxisFont(axisId, font);

    QwtText qwtTitle(title);
    font.setBold(true);
    qwtTitle.setFont(font);

    QwtPlot::setAxisTitle(axisId, qwtTitle);
}

void SignalPlot::setDataRange(const double minY, const double maxY)
{
    setAxisScale(QwtAxis::YRight, minY, maxY);
    replot();
}

void SignalPlot::setDisplayedTimeInterval(const double intervalMsecs)
{
    m_displayedTimeInterval = intervalMsecs;
    recalculateTimeScale();
    replot();
}

void SignalPlot::onChunkCreated(const SignalChunk *chunk)
{
    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setItemAttribute(QwtPlotItem::ItemAttribute::Legend, false);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve->attach(this);
    curve->setAxes(QwtAxis::XBottom, QwtAxis::YRight);
    m_lastCurve = curve;

    connect(chunk, &SignalChunk::sizeChanged, this, [=](int size) {
        curve->setRawSamples(chunk->constDataX(), chunk->constDataY(), size);
        recalculateTimeScale();
        replot();
    });

    connect(chunk, &SignalChunk::colorChanged, this, [=](QColor color) {
        QPen pen = curve->pen();
        pen.setColor(color);
        curve->setPen(pen);
        replot();
    });

    connect(chunk, &SignalChunk::destroyed, this, [this, curve](QObject *) {
        if (curve == m_lastCurve) {
            m_lastCurve = nullptr;
        }

        curve->detach();
        delete curve;
    });

    curve->setRawSamples(chunk->constDataX(), chunk->constDataY(), chunk->size());
    recalculateTimeScale();

    QPen pen;
    pen.setColor(chunk->color());
    pen.setWidth(2);
    curve->setPen(pen);

    replot();
}

void SignalPlot::addLegendItem(const QString &text, const QColor color)
{
    QwtPlotCurve *emptyCurveForLegend = new QwtPlotCurve(text);
    emptyCurveForLegend->setVisible(false);
    emptyCurveForLegend->setPen(color);
    emptyCurveForLegend->attach(this);
}

bool SignalPlot::isCurveVisible(const QwtPlotCurve *curve) const
{
    return curve->sample(curve->dataSize() - 1).x() >= axisInterval(QwtAxis::XBottom).minValue();
}

void SignalPlot::recalculateTimeScale()
{
    double lastDisplayedTime;

    if (m_lastCurve && m_lastCurve->dataSize() > 0) {
        lastDisplayedTime = m_lastCurve->sample(m_lastCurve->dataSize() - 1).x();
    } else {
        lastDisplayedTime = QDateTime::currentMSecsSinceEpoch();
    }

    double firstDisplayedTime = lastDisplayedTime - m_displayedTimeInterval;
    setAxisScale(QwtAxis::XBottom, firstDisplayedTime, lastDisplayedTime);
}
