
#include "pch.h"
#include <GLFW/glfw3.h>
#include <imgui.h>

#include <ImGuizmo.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <filesystem>
#include <glad/glad.h>

#include "Application.h"
#include "ApplicationEvent.h"
#include "ApplicationRunner.h"
#include "Event.h"
#include "IconsFontAwesome6Pro.h"
#include "IconsFontAwesome6ProBrands.h"
#include "ImGuiCustomStyle.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "KeyCode.h"
#include "KeyEvent.h"
#include "MouseEvent.h"


namespace Doodle
{

void ImGuiManager::RegisterFont(int sizeInPixels, std::string englishFont, std::string chineseFont, std::string iconFont,
                                std::string brandFont)
{
    static const ImWchar ICONS_RANGES[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};

    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig config;
    std::string path = (std::filesystem::path("assets") / "fonts" / englishFont).string();
    io.Fonts->AddFontFromFileTTF(path.c_str(), sizeInPixels, &config, io.Fonts->GetGlyphRangesDefault());
    DOO_CORE_ASSERT(std::filesystem::exists(path), "Font file does not exist: {0}", path);
    config.MergeMode = true;
    path = (std::filesystem::path("assets") / "fonts" / chineseFont).string();
    DOO_CORE_ASSERT(std::filesystem::exists(path), "Font file does not exist: {0}", path);
    io.Fonts->AddFontFromFileTTF(path.c_str(), sizeInPixels, &config, io.Fonts->GetGlyphRangesChineseFull());
    path = (std::filesystem::path("assets") / "fonts" / iconFont).string();
    DOO_CORE_ASSERT(std::filesystem::exists(path), "Font file does not exist: {0}", path);
    io.Fonts->AddFontFromFileTTF(path.c_str(), sizeInPixels, &config, ICONS_RANGES);
    path = (std::filesystem::path("assets") / "fonts" / brandFont).string();
    DOO_CORE_ASSERT(std::filesystem::exists(path), "Font file does not exist: {0}", path);
    auto *font = io.Fonts->AddFontFromFileTTF(path.c_str(), sizeInPixels, &config, ICONS_RANGES);
    m_fonts.push_back(font);
}

void ImGuiManager::Initialize()
{
    IMGUI_CHECKVERSION();

    m_imnodesContext = ImNodes::CreateContext();
    m_imguiContext = ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    SetupImGuiStyle();
    const int SIZE_IN_PIXELS = 20;
    RegisterFont(SIZE_IN_PIXELS, "MiSans-Light.ttf", "MiSans-Light.ttf", "fa-light-300.ttf", "fa-brands-400.ttf");
    RegisterFont(SIZE_IN_PIXELS, "MiSans-Regular.ttf", "MiSans-Regular.ttf", "fa-regular-400.ttf", "fa-brands-400.ttf");
    RegisterFont(SIZE_IN_PIXELS, "MiSans-Bold.ttf", "MiSans-Bold.ttf", "fa-solid-900.ttf", "fa-brands-400.ttf");

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
    // ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        // style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    const char *glslVersion = "#version 410";
    auto &window = ApplicationRunner::GetWindow();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(window.GetNativeWindow()), true);
    ImGui_ImplOpenGL3_Init(glslVersion);

}

void ImGuiManager::Deinitialize()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImNodes::DestroyContext();
}

void ImGuiManager::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void ImGuiManager::EndFrame()
{
    ImGuiIO &io = ImGui::GetIO();
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    auto &window = ApplicationRunner::GetWindow();
    int displayW, displayH;
    glfwGetFramebufferSize(static_cast<GLFWwindow *>(window.GetNativeWindow()), &displayW, &displayH);
    glViewport(0, 0, displayW, displayH);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backupCurrentContext);
    }
}

void ImGuiManager::ShowDockspace(){
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void ImGuiManager::DrawLayout()
{
    BeginFrame();

    ShowDockspace();

    AppLayoutEvent event;
    EventManager::Get().Dispatch(event);

    EndFrame();
}

} // namespace Doodle