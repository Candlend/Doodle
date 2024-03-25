#pragma once

#include "Layer.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "ApplicationEvent.h"
#include <unordered_map>

namespace RhyEngine {

	class RHY_API ImGuiLayer : public BaseLayer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(BaseEvent& event);
	private:
		void SetupKeyboard();

		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnCharInputEvent(CharInputEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);

		float m_time = 0.0f;
	};

}