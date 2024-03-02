#pragma once

#include <QtCore/QObject>
#include <QtCore/QVector>

#include "ClassLabel.h"


// Like SignalFragment, but with two arrays instead of array of pairs
// and it has maximum size called full size.
class SignalChunk final : public QObject
{
    Q_OBJECT

public:
    explicit SignalChunk(int fullSize);
    explicit SignalChunk(int fullSize, double x0, double y0);
    ~SignalChunk() override = default;

    // Element access
    double firstX() const;
    double firstY() const;
    double lastX() const;
    double lastY() const;
    const double *constDataX() const;
    const double *constDataY() const;
    const QVector<double> &constVectorX() const;
    const QVector<double> &constVectorY() const;

    // Label access
    QColor color() const;
    bool hasNoLabel() const;
    bool isAnomalous() const;
    bool isRegular() const;

    // Capacity
    bool isEmpty() const;
    int size() const;
    bool isFull() const;
    int fullSize() const;

    // Modifiers
    /**
     * Inserts point with 'x' and 'y' values at the end of the chunk.
     * Note that this method doesn't emit 'sizeChanged' signal.
     */
    void append(double x, double y);
    void labelRegular();
    void labelAnomalous();

signals:
    void sizeChanged(int newSize);
    void colorChanged(QColor newColor);

private:
    // _xs and _ys has same size less or equals to _fullSize
    QVector<double> _xs;
    QVector<double> _ys;
    const int _fullSize;
    ClassLabel _label;

};
