#pragma once

#include <initializer_list>

#include <QtCore/QStringList>

#include "core/model/signal-types.h"


class SignalFragment : public QList<SignalPoint>
{
public:
    explicit SignalFragment(std::initializer_list<SignalPoint> values);
    virtual ~SignalFragment();

};
