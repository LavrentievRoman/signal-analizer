#pragma once

#include <QtCore/QtGlobal>

class QColor;


class ClassLabel  // (рус. Метка класса)
{
public:
    enum class Enum : quint64
    {
        // Values are copied from https://wiki.qt.io/Colors_and_Font_Guidelines
        NoLabel   = 0x1e1b18,  // (рус. Не размечен)
        Anomalous = 0xb40000,  // (рус. Аномальный)
        Regular   = 0x328930,  // (рус. Штатный)
    };

    ClassLabel() = delete;
    explicit ClassLabel(Enum value);

    ClassLabel::Enum value() const;
    QColor color() const;

private:
    Enum m_value;

};
