#include <QtGui/QColor>

#include "ClassLabel.h"

ClassLabel::ClassLabel(Enum value)
    : m_value(value)
{
}

ClassLabel::Enum ClassLabel::value() const
{
    return m_value;
}

QColor ClassLabel::color() const
{
    return QColor(static_cast<quint64>(m_value));
}
