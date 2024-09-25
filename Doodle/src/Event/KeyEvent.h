#pragma once

#include "Event.h"
#include "KeyCode.h"

namespace Doodle
{

class DOO_API KeyEvent : public Event
{
public:
    inline KeyCode GetKeyCode() const
    {
        return m_keyCode;
    }

protected:
    KeyEvent(KeyCode keycode) : m_keyCode(keycode)
    {
    }

    KeyCode m_keyCode;
};

class DOO_API KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(KeyCode keycode, bool repeated) : KeyEvent(keycode), m_repeated(repeated)
    {
    }

    inline int IsRepeated() const
    {
        return m_repeated;
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << static_cast<int>(m_keyCode) << m_repeated;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyPressed)
private:
    bool m_repeated;
};

class DOO_API KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << static_cast<int>(m_keyCode);
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyReleased)
};

class DOO_API CharInputEvent : public Event
{
public:
    inline uint16_t GetChar() const
    {
        return m_inputChar;
    }

    CharInputEvent(uint16_t inputChar)
    {
		m_inputChar = inputChar;
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "CharInputEvent: " << static_cast<char>(m_inputChar);
        return ss.str();
    }

    EVENT_CLASS_TYPE(CharInput)
private:
    uint16_t m_inputChar;
};
} // namespace Doodle
