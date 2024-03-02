#pragma once

#include <QtCore/QObject>

class TimeInterval : public QObject
{
    Q_OBJECT

public:
    enum Units
    {
        UNDEFINED = -1,
        Milliseconds,
        Seconds,
        Minutes,
        Hours,
        Days,
        Weeks,
        Months,
        Years
    };

    inline TimeInterval(double intervalMsecs = 0.0);
    ~TimeInterval() override = default;

    inline bool isValid() const;
    inline double value() const;
    inline void setValue(double msecs);

    inline static constexpr double toMsecs(double value, Units units);
    inline static constexpr double fromMsecs(double value, Units units);

    inline static QString unitsToString(Units units);
    inline static Units unitsFromString(const QString &string);

    static constexpr double millisecond()   {return 1.0;};
    static constexpr double second()        {return 1000 * millisecond();};
    static constexpr double minute()        {return 60 * second();};
    static constexpr double hour()          {return 60 * minute();};
    static constexpr double day()           {return 24 * hour();};
    static constexpr double week()          {return 7 * day();};
    static constexpr double month()         {return 30 * day();};
    static constexpr double year()          {return 365 * day();};

signals:
    void changed(double intervalMsecs);

private:
    double m_msecs;
};

inline TimeInterval::TimeInterval(double intervalMsecs)
    : m_msecs(intervalMsecs)
{}

inline bool TimeInterval::isValid() const
{
    return (m_msecs > 0);
}

inline double TimeInterval::value() const
{
    return m_msecs;
}

inline void TimeInterval::setValue(double msecs)
{
    if (m_msecs != msecs) {
        m_msecs = msecs;
        emit changed(m_msecs);
    }
}

inline constexpr double TimeInterval::toMsecs(double value, Units units)
{
    switch (units) {
        case Milliseconds:  return value * millisecond();
        case Seconds:       return value * second();
        case Minutes:       return value * minute();
        case Hours:         return value * hour();
        case Days:          return value * day();
        case Weeks:         return value * week();
        case Months:        return value * month();
        case Years:         return value * year();
        default:            return value;
    }
}

inline constexpr double TimeInterval::fromMsecs(double value, Units units)
{
    switch (units) {
        case Milliseconds:  return value / millisecond();
        case Seconds:       return value / second();
        case Minutes:       return value / minute();
        case Hours:         return value / hour();
        case Days:          return value / day();
        case Weeks:         return value / week();
        case Months:        return value / month();
        case Years:         return value / year();
        default:            return value;
    }
}

inline QString TimeInterval::unitsToString(Units units)
{
    switch (units) {
        case Milliseconds:  return "Milliseconds";
        case Seconds:       return "Seconds";
        case Minutes:       return "Minutes";
        case Hours:         return "Hours";
        case Days:          return "Days";
        case Weeks:         return "Weeks";
        case Months:        return "Months";
        case Years:         return "Years";
        default:            return "UNDEFINED";
    }
}

inline TimeInterval::Units TimeInterval::unitsFromString(const QString &string)
{
    if (string == "Milliseconds") {
        return Milliseconds;
    } else if (string == "Seconds") {
        return Seconds;
    } else if (string == "Minutes") {
        return Minutes;
    } else if (string == "Hours") {
        return Hours;
    } else if (string == "Days") {
        return Days;
    } else if (string == "Weeks") {
        return Weeks;
    } else if (string == "Months") {
        return Months;
    } else if (string == "Years") {
        return Years;
    } else {
        return UNDEFINED;
    }
}
