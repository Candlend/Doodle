#pragma once

#include "Event.h"
#include "MouseButton.h"

namespace Doodle {

	class DOO_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			: m_mouseX(x), m_mouseY(y) {}

		inline float GetX() const { return m_mouseX; }
		inline float GetY() const { return m_mouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_mouseX << ", " << m_mouseY;
			return ss.str();
		}

        EVENT_CLASS_TYPE(MouseMoved)
    private:
        float m_mouseX, m_mouseY;
	};

	class DOO_API MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_xOffset(xOffset), m_yOffset(yOffset) {}

		inline float GetXOffset() const { return m_xOffset; }
		inline float GetYOffset() const { return m_yOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

        EVENT_CLASS_TYPE(MouseScrolled)

    private:
        float m_xOffset, m_yOffset;
	};

	class DOO_API MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_button; }

        MouseButtonEvent(MouseButton button) : m_button(button)
        {
        }

    protected:
        int m_button;
	};

	class DOO_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
        MouseButtonPressedEvent(MouseButton button) : MouseButtonEvent(button)
        {
        }

        std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class DOO_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
        MouseButtonReleasedEvent(MouseButton button) : MouseButtonEvent(button)
        {
        }

        std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}

