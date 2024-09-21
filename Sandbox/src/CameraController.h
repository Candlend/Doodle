#pragma once

#include <glm/fwd.hpp>
#include <string>

#include "ApplicationRunner.h"
#include "Component.h"
#include "Input.h"
#include "KeyCode.h"
#include "Log.h"
#include "PanelManager.h"
#include "SceneManager.h"
#include "ViewportPanel.h"
#include "imgui.h"

using namespace Doodle;

class CameraController : public Scriptable, public IEventHandler<ViewportResizeEvent>
{
    COMPONENT_CLASS_TYPE(CameraController)

public:
    void Initialize() override
    {
        m_moveSpeed = 2.0f;
        m_rotateSpeed = 20.0f;

        std::shared_ptr<SceneCamera> camera = GetComponent<CameraComponent>();

        m_lastMousePosition = glm::vec2(Input::GetMousePosition().first, Input::GetMousePosition().second);
    }

    void OnUpdate() override
    {
        if (!PanelManager::Get()->GetPanel<ViewportPanel>()->IsShowOnTop())
            return;

        if (Input::IsMouseButtonDown(MouseButton::Right))
        {
            auto &transform = GetComponent<TransformComponent>();

            float deltaTime = Application::Time::GetDeltaTime();
            if (Input::IsKeyDown(KeyCode::W))
            {
                auto localPosition = transform.GetLocalPosition() + transform.GetFront() * m_moveSpeed * deltaTime;
                transform.SetLocalPosition(localPosition);
            }
            if (Input::IsKeyDown(KeyCode::S))
            {
                auto localPosition = transform.GetLocalPosition() - transform.GetFront() * m_moveSpeed * deltaTime;
                transform.SetLocalPosition(localPosition);
            }
            if (Input::IsKeyDown(KeyCode::A))
            {
                auto localPosition = transform.GetLocalPosition() - transform.GetRight() * m_moveSpeed * deltaTime;
                transform.SetLocalPosition(localPosition);
            }
            if (Input::IsKeyDown(KeyCode::D))
            {
                auto localPosition = transform.GetLocalPosition() + transform.GetRight() * m_moveSpeed * deltaTime;
                transform.SetLocalPosition(localPosition);
            }
            if (Input::IsKeyDown(KeyCode::Q))
            {
                auto localPosition = transform.GetLocalPosition() - transform.GetUp() * m_moveSpeed * deltaTime;
                transform.SetLocalPosition(localPosition);
            }
            if (Input::IsKeyDown(KeyCode::E))
            {
                auto localPosition = transform.GetLocalPosition() + transform.GetUp() * m_moveSpeed * deltaTime;
                transform.SetLocalPosition(localPosition);
            }
            // 处理相机旋转
            glm::vec2 currentMousePosition =
                glm::vec2(Input::GetMousePosition().first, Input::GetMousePosition().second);
            glm::vec2 mouseDelta = currentMousePosition - m_lastMousePosition;

            auto localRotation = transform.GetLocalRotation();
            localRotation.y -= mouseDelta.x * m_rotateSpeed * deltaTime; // 偏航角
            localRotation.x -= mouseDelta.y * m_rotateSpeed * deltaTime; // 俯仰角

            if (localRotation.x > 89.0f)
                localRotation.x = 89.0f;
            if (localRotation.x < -89.0f)
                localRotation.x = -89.0f;

            transform.SetLocalRotation(localRotation);
        }

        m_lastMousePosition = glm::vec2(Input::GetMousePosition().first, Input::GetMousePosition().second);
    }

    void OnInspectorLayout() override
    {
        auto &transform = GetComponent<TransformComponent>();

        ImGui::DragFloat("Move Speed", &m_moveSpeed, 0.1f, 0.0f, 10.0f);
        ImGui::DragFloat("Rotate Speed", &m_rotateSpeed, 1.0f, 0.0f, 100.0f);
    }

    void Deinitialize() override
    {
    }

    bool OnEvent(ViewportResizeEvent &event) override
    {
        int width = event.GetWidth();
        int height = event.GetHeight();
        if (width == 0 || height == 0)
            return false;
        auto &camera = GetComponent<CameraComponent>().Camera;
        camera->SetViewportSize(event.GetWidth(), event.GetHeight());
        return false;
    }

private:
    float m_moveSpeed;
    float m_rotateSpeed;
    glm::vec2 m_lastMousePosition;
};
