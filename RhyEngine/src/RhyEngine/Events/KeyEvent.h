#pragma once

#include "pch.h"
#include "Event.h"

namespace RhyEngine {

	class RHY_API BaseKeyEvent : public BaseEvent
	{
	public:
		inline int GetKeyCode() const { return m_keyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		explicit BaseKeyEvent(int keycode)
			: m_keyCode(keycode) {}

		int m_keyCode;
	};

	class RHY_API KeyPressedEvent : public BaseKeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount)
			: BaseKeyEvent(keycode), m_repeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return m_repeatCount; }

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
		explicit KeyReleasedEvent(int keycode)
			: BaseKeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}
