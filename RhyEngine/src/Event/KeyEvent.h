#pragma once

#include "Event.h"
#include "KeyCode.h"

namespace RhyEngine
{

class RHY_API BaseKeyEvent : public BaseEvent
{
public:
    inline KeyCode GetKeyCode() const
    {
        return m_keyCode;
    }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
protected:
    explicit BaseKeyEvent(KeyCode keycode) : m_keyCode(keycode)
    {
    }

    KeyCode m_keyCode;
};

class RHY_API KeyPressedEvent : public BaseKeyEvent
{
public:
    KeyPressedEvent(KeyCode keycode, int repeatCount) : BaseKeyEvent(keycode), m_repeatCount(repeatCount)
    {
    }

    inline int GetRepeatCount() const
    {
        return m_repeatCount;
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << m_keyCode << " (" << m_repeatCount << " repeats)";
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyPressed)
private:
    int m_repeatCount;
};

class RHY_API KeyReleasedEvent : public BaseKeyEvent
{
public:
    explicit KeyReleasedEvent(KeyCode keycode) : BaseKeyEvent(keycode)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << m_keyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyReleased)
};

class RHY_API CharInputEvent : public BaseEvent
{
public:
    inline uint16_t GetChar() const
    {
        return m_inputChar;
    }

    explicit CharInputEvent(uint16_t inputChar)
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
	EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
private:
    uint16_t m_inputChar;
};
} // namespace RhyEngine
