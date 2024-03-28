
#include "pch.h"

#include <GLFW/glfw3.h>
#include <cstdint>
#include <glad/glad.h>
#include <imgui.h>

#include "Application.h"
#include "Event.h"
#include "ImGuiLayer.h"
#include "Input.h"
#include "KeyCode.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "backends/imgui_impl_opengl3.h"

namespace RhyEngine
{

static ImGuiKey KeyCode2ImGuiKey(KeyCode keycode)
{
    switch (keycode)
    {
    case Tab:
        return ImGuiKey_Tab;
    case Left:
        return ImGuiKey_LeftArrow;
    case Right:
        return ImGuiKey_RightArrow;
    case Up:
        return ImGuiKey_UpArrow;
    case Down:
        return ImGuiKey_DownArrow;
    case PageUp:
        return ImGuiKey_PageUp;
    case PageDown:
        return ImGuiKey_PageDown;
    case Home:
        return ImGuiKey_Home;
    case End:
        return ImGuiKey_End;
    case Insert:
        return ImGuiKey_Insert;
    case Delete:
        return ImGuiKey_Delete;
    case Backspace:
        return ImGuiKey_Backspace;
    case Space:
        return ImGuiKey_Space;
    case Enter:
        return ImGuiKey_Enter;
    case Escape:
        return ImGuiKey_Escape;
    case Apostrophe:
        return ImGuiKey_Apostrophe;
    case Comma:
        return ImGuiKey_Comma;
    case Minus:
        return ImGuiKey_Minus;
    case Period:
        return ImGuiKey_Period;
    case Slash:
        return ImGuiKey_Slash;
    case Semicolon:
        return ImGuiKey_Semicolon;
    case Equal:
        return ImGuiKey_Equal;
    case LeftBracket:
        return ImGuiKey_LeftBracket;
    case Backslash:
        return ImGuiKey_Backslash;
    case RightBracket:
        return ImGuiKey_RightBracket;
    case GraveAccent:
        return ImGuiKey_GraveAccent;
    case CapsLock:
        return ImGuiKey_CapsLock;
    case ScrollLock:
        return ImGuiKey_ScrollLock;
    case NumLock:
        return ImGuiKey_NumLock;
    case PrintScreen:
        return ImGuiKey_PrintScreen;
    case Pause:
        return ImGuiKey_Pause;
    case Keypad0:
        return ImGuiKey_Keypad0;
    case Keypad1:
        return ImGuiKey_Keypad1;
    case Keypad2:
        return ImGuiKey_Keypad2;
    case Keypad3:
        return ImGuiKey_Keypad3;
    case Keypad4:
        return ImGuiKey_Keypad4;
    case Keypad5:
        return ImGuiKey_Keypad5;
    case Keypad6:
        return ImGuiKey_Keypad6;
    case Keypad7:
        return ImGuiKey_Keypad7;
    case Keypad8:
        return ImGuiKey_Keypad8;
    case Keypad9:
        return ImGuiKey_Keypad9;
    case KeypadDecimal:
        return ImGuiKey_KeypadDecimal;
    case KeypadDivide:
        return ImGuiKey_KeypadDivide;
    case KeypadMultiply:
        return ImGuiKey_KeypadMultiply;
    case KeypadSubtract:
        return ImGuiKey_KeypadSubtract;
    case KeypadAdd:
        return ImGuiKey_KeypadAdd;
    case KeypadEnter:
        return ImGuiKey_KeypadEnter;
    case KeypadEqual:
        return ImGuiKey_KeypadEqual;
    case LeftShift:
        return ImGuiKey_LeftShift;
    case LeftControl:
        return ImGuiKey_LeftCtrl;
    case LeftAlt:
        return ImGuiKey_LeftAlt;
    case LeftSuper:
        return ImGuiKey_LeftSuper;
    case RightShift:
        return ImGuiKey_RightShift;
    case RightControl:
        return ImGuiKey_RightCtrl;
    case RightAlt:
        return ImGuiKey_RightAlt;
    case RightSuper:
        return ImGuiKey_RightSuper;
    case Menu:
        return ImGuiKey_Menu;
    case Alpha0:
        return ImGuiKey_0;
    case Alpha1:
        return ImGuiKey_1;
    case Alpha2:
        return ImGuiKey_2;
    case Alpha3:
        return ImGuiKey_3;
    case Alpha4:
        return ImGuiKey_4;
    case Alpha5:
        return ImGuiKey_5;
    case Alpha6:
        return ImGuiKey_6;
    case Alpha7:
        return ImGuiKey_7;
    case Alpha8:
        return ImGuiKey_8;
    case Alpha9:
        return ImGuiKey_9;
    case A:
        return ImGuiKey_A;
    case B:
        return ImGuiKey_B;
    case C:
        return ImGuiKey_C;
    case D:
        return ImGuiKey_D;
    case E:
        return ImGuiKey_E;
    case F:
        return ImGuiKey_F;
    case G:
        return ImGuiKey_G;
    case H:
        return ImGuiKey_H;
    case I:
        return ImGuiKey_I;
    case J:
        return ImGuiKey_J;
    case K:
        return ImGuiKey_K;
    case L:
        return ImGuiKey_L;
    case M:
        return ImGuiKey_M;
    case N:
        return ImGuiKey_N;
    case O:
        return ImGuiKey_O;
    case P:
        return ImGuiKey_P;
    case Q:
        return ImGuiKey_Q;
    case R:
        return ImGuiKey_R;
    case S:
        return ImGuiKey_S;
    case T:
        return ImGuiKey_T;
    case U:
        return ImGuiKey_U;
    case V:
        return ImGuiKey_V;
    case W:
        return ImGuiKey_W;
    case X:
        return ImGuiKey_X;
    case Y:
        return ImGuiKey_Y;
    case Z:
        return ImGuiKey_Z;
    case F1:
        return ImGuiKey_F1;
    case F2:
        return ImGuiKey_F2;
    case F3:
        return ImGuiKey_F3;
    case F4:
        return ImGuiKey_F4;
    case F5:
        return ImGuiKey_F5;
    case F6:
        return ImGuiKey_F6;
    case F7:
        return ImGuiKey_F7;
    case F8:
        return ImGuiKey_F8;
    case F9:
        return ImGuiKey_F9;
    case F10:
        return ImGuiKey_F10;
    case F11:
        return ImGuiKey_F11;
    case F12:
        return ImGuiKey_F12;
    case F13:
        return ImGuiKey_F13;
    case F14:
        return ImGuiKey_F14;
    case F15:
        return ImGuiKey_F15;
    case F16:
        return ImGuiKey_F16;
    case F17:
        return ImGuiKey_F17;
    case F18:
        return ImGuiKey_F18;
    case F19:
        return ImGuiKey_F19;
    case F20:
        return ImGuiKey_F20;
    case F21:
        return ImGuiKey_F21;
    case F22:
        return ImGuiKey_F22;
    case F23:
        return ImGuiKey_F23;
    case F24:
        return ImGuiKey_F24;
    default:
        return ImGuiKey_None;
    }
}

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
    m_eventManager.AddListener(this, &ImGuiLayer::OnMouseButtonPressedEvent);
    m_eventManager.AddListener(this, &ImGuiLayer::OnMouseButtonReleasedEvent);
    m_eventManager.AddListener(this, &ImGuiLayer::OnMouseMovedEvent);
    m_eventManager.AddListener(this, &ImGuiLayer::OnMouseScrolledEvent);
    m_eventManager.AddListener(this, &ImGuiLayer::OnKeyPressedEvent);
    m_eventManager.AddListener(this, &ImGuiLayer::OnKeyReleasedEvent);
    m_eventManager.AddListener(this, &ImGuiLayer::OnCharInputEvent);
    m_eventManager.AddListener(this, &ImGuiLayer::OnWindowResizeEvent);
}

void ImGuiLayer::OnDetach()
{
    m_eventManager.RemoveListener(this, &ImGuiLayer::OnMouseButtonPressedEvent);
    m_eventManager.RemoveListener(this, &ImGuiLayer::OnMouseButtonReleasedEvent);
    m_eventManager.RemoveListener(this, &ImGuiLayer::OnMouseMovedEvent);
    m_eventManager.RemoveListener(this, &ImGuiLayer::OnMouseScrolledEvent);
    m_eventManager.RemoveListener(this, &ImGuiLayer::OnKeyPressedEvent);
    m_eventManager.RemoveListener(this, &ImGuiLayer::OnKeyReleasedEvent);
    m_eventManager.RemoveListener(this, &ImGuiLayer::OnCharInputEvent);
    m_eventManager.RemoveListener(this, &ImGuiLayer::OnWindowResizeEvent);
}

static void UpdateKeyModifiers(ImGuiIO &io)
{
    io.AddKeyEvent(ImGuiMod_Ctrl, Input::IsKeyPressed(LeftControl) || Input::IsKeyPressed(RightControl));
    io.AddKeyEvent(ImGuiMod_Shift, Input::IsKeyPressed(LeftShift) || Input::IsKeyPressed(RightShift));
    io.AddKeyEvent(ImGuiMod_Alt, Input::IsKeyPressed(LeftAlt) || Input::IsKeyPressed(RightAlt));
    io.AddKeyEvent(ImGuiMod_Super, Input::IsKeyPressed(LeftSuper) || Input::IsKeyPressed(RightSuper));
}

bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    UpdateKeyModifiers(io);
    io.AddMouseButtonEvent(e.GetMouseButton(), true);
    return false;
}

bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    UpdateKeyModifiers(io);
    io.AddMouseButtonEvent(e.GetMouseButton(), false);
    return false;
}

bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    io.AddMousePosEvent(e.GetX(), e.GetY());
    return false;
}

bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseWheelEvent(e.GetXOffset(), e.GetYOffset());
    return false;
}

bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    UpdateKeyModifiers(io);
    io.AddKeyEvent(KeyCode2ImGuiKey(e.GetKeyCode()), true);
    return false;
}

bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    UpdateKeyModifiers(io);
    io.AddKeyEvent(KeyCode2ImGuiKey(e.GetKeyCode()), false);
    return false;
}

bool ImGuiLayer::OnCharInputEvent(CharInputEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    io.AddInputCharacter(e.GetChar());
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
    Application &app = Application::Get();
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