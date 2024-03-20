#pragma once

#include "Layer.h"

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
		float m_time = 0.0f;
	};

}