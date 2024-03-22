#include "RhyEngine/Event/MouseEvent.h"
#include "pch.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>

#include "ImGuiLayer.h"
#include "Platform/OpenGL/imgui_impl_opengl3.h"
#include "RhyEngine/Application.h"
#include "RhyEngine/Event/Event.h"

namespace RhyEngine
{

#define ADD_EVENT_LISTENER(event, callback) static std::function<void(BaseEvent &)> s_##event##Callback = m_eventManager.AddListener(EventType::event, [this](BaseEvent &e) {callback(e);});
#define REMOVE_EVENT_LISTENER(event, callback) m_eventManager.RemoveListener(EventType::event, event##Callback);

ImGuiLayer::ImGuiLayer() : BaseLayer("ImGuiLayer")
{
}

ImGuiLayer::~ImGuiLayer()
{
}

void ImGuiLayer::OnAttach()
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO &io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::OnDetach()
{
}

bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[e.GetMouseButton()] = true;

    return false;
}

bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[e.GetMouseButton()] = false;

    return false;
}

bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(e.GetX(), e.GetY());

    return false;
}

bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseWheelH += e.GetXOffset();
    io.MouseWheel += e.GetYOffset();

    return false;
}

bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    io.KeysDown[e.GetKeyCode()] = true;

    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
    return false;
}

bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    io.KeysDown[e.GetKeyCode()] = false;

    return false;
}

bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    glViewport(0, 0, e.GetWidth(), e.GetHeight());

    return false;
}

void ImGuiLayer::OnUpdate()
{
    ImGuiIO &io = ImGui::GetIO();
    Application &app = Application::GetInstance();
    io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

    float time = static_cast<float>(glfwGetTime());
    io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
    m_time = time;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    static bool s_Show = true;
    ImGui::ShowDemoWindow(&s_Show);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace RhyEngine