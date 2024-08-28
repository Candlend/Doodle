#pragma once

#include "CameraController.h"
#include "LogWindow.h"
#include "SceneHierarchy.h"
#include <Doodle.h>
#include <memory>

using namespace Doodle;

class Sandbox : public Application
{
public:
    void Initialize() override
    {
        LogWindow::Register();
        SceneHierarchy::Register();
        CameraController::Register();

        m_scene = SceneManager::Get().CreateScene("Main");
        m_scene->Begin();

        Application::Initialize();
        ActivateImGuiContext();
    }

    void Deinitialize() override
    {
        m_scene->End();
        Application::Deinitialize();
    }

private:
    std::shared_ptr<Scene> m_scene;
};
