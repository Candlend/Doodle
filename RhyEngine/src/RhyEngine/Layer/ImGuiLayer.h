#pragma once

#include "Layer.h"
#include "RhyEngine/Event/KeyEvent.h"
#include "RhyEngine/Event/MouseEvent.h"
#include "RhyEngine/Event/ApplicationEvent.h"

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
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		// bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);
		float m_time = 0.0f;
	};

}