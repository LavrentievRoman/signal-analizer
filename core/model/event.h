#pragma once

#include <array>
#include <optional>

#include <QtCore/QDateTime>


class Event final
{
public:
    ~Event();  // non-virtual

    static Event generate(QDateTime begin, QDateTime end);

    QDateTime begin() const;
    QDateTime end() const;
    QDateTime generationDateTime() const;

    std::optional<QDateTime> at(int column) const;

private:
    explicit Event(QDateTime begin, QDateTime end, QDateTime gen);

private:
    std::array<QDateTime, 3> _data;

};
