#include "core/model/signal-fragment.h"

SignalFragment::SignalFragment(std::initializer_list<SignalPoint> values)
    : QList<SignalPoint>(values)
{
}

SignalFragment::~SignalFragment()
{
}
