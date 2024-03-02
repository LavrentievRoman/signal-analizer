#include <cassert>

#include <QtGui/QColor>

#include "core/model/signal-chunk.h"

SignalChunk::SignalChunk(int fullSize)
    : _xs()
    , _ys()
    , _fullSize(fullSize)
    , _label(ClassLabel::Enum::NoLabel)
{
    _xs.reserve(fullSize);
    _ys.reserve(fullSize);
}

SignalChunk::SignalChunk(int fullSize, double x0, double y0)
    : _xs()
    , _ys()
    , _fullSize(fullSize)
    , _label(ClassLabel::Enum::NoLabel)
{
    _xs.reserve(fullSize);
    _ys.reserve(fullSize);

    _xs.append(x0);
    _ys.append(y0);
}

double SignalChunk::firstX() const
{
    return _xs.first();
}

double SignalChunk::firstY() const
{
    return _ys.first();
}

double SignalChunk::lastX() const
{
    return _xs.last();
}

double SignalChunk::lastY() const
{
    return _ys.last();
}

const double *SignalChunk::constDataX() const
{
    return _xs.constData();
}

const double *SignalChunk::constDataY() const
{
    return _ys.constData();
}

const QVector<double> &SignalChunk::constVectorX() const
{
    return _xs;
}

const QVector<double> &SignalChunk::constVectorY() const
{
    return _ys;
}

QColor SignalChunk::color() const
{
    return _label.color();
}

bool SignalChunk::hasNoLabel() const
{
    return _label.value() == ClassLabel::Enum::NoLabel;
}

bool SignalChunk::isAnomalous() const
{
    return _label.value() == ClassLabel::Enum::Anomalous;
}

bool SignalChunk::isRegular() const
{
    return _label.value() == ClassLabel::Enum::Regular;
}

bool SignalChunk::isEmpty() const
{
    assert(_xs.size() == _ys.size());

    return _xs.isEmpty();
}

int SignalChunk::size() const
{
    assert(_xs.size() == _ys.size());

    return _xs.size();
}

bool SignalChunk::isFull() const
{
    assert(_xs.size() == _ys.size());

    return _xs.size() == _fullSize;
}

int SignalChunk::fullSize() const
{
    return _fullSize;
}

void SignalChunk::append(double x, double y)
{
    assert(_xs.size() < _fullSize && _ys.size() < _fullSize);

    _xs.append(x);
    _ys.append(y);
}

void SignalChunk::labelRegular()
{
    assert(_xs.size() == _fullSize);
    assert(hasNoLabel());

    _label = ClassLabel(ClassLabel::Enum::Regular);
    emit colorChanged(color());
}

void SignalChunk::labelAnomalous()
{
    assert(_xs.size() == _fullSize);
    assert(hasNoLabel());

    _label = ClassLabel(ClassLabel::Enum::Anomalous);
    emit colorChanged(color());
}
