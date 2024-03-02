#include "core/model/event.h"

Event::~Event()
{
}

Event Event::generate(QDateTime begin, QDateTime end)
{
    // here we can use ProxyClock instead of real current time if needed
    return Event(begin, end, QDateTime::currentDateTime());
}

QDateTime Event::begin() const
{
    return _data[1];
}

QDateTime Event::end() const
{
    return _data[2];
}

QDateTime Event::generationDateTime() const
{
    return _data[0];
}

std::optional<QDateTime> Event::at(int column) const
{
    if (column < 0 || column >= 3) {
        return {};
    }

    return _data[column];
}

Event::Event(QDateTime begin, QDateTime end, QDateTime gen)
    : _data {gen, begin, end}
{
}
