#pragma once

#include "ApplicationRunner.h"
#include "Component.h"
#include "KeyCode.h"
#include "glm/fwd.hpp"
#include <Doodle.h>
#include <memory>

using namespace Doodle;

class CameraController : public RegisterModule<CameraController, 10>
{
public:
    void Initialize() override
    {
        m_moveSpeed = 2.0f;
        m_rotateSpeed = 20.0f;
        m_cameraEntity = SceneManager::Get().GetActiveScene()->GetMainCameraEntity();
        auto &camera = m_cameraEntity->GetComponent<CameraComponent>().Camera;
        camera->SetViewportSize(ApplicationRunner::Get().GetWindow().GetWidth(),
                                ApplicationRunner::Get().GetWindow().GetHeight());

        m_lastMousePosition = glm::vec2(Input::GetMousePosition().first, Input::GetMousePosition().second);

        EventManager::Get().AddListener(this, &CameraController::OnWindowResizeEvent);
    }

    void OnUpdate() override
    {
        m_activeScene = SceneManager::Get().GetActiveScene();
        auto &transform = m_cameraEntity->GetComponent<Transform>();

        float deltaTime = Application::Time::GetDeltaTime();
        if (Input::IsKeyPressed(KeyCode::W))
        {
            transform.Position += transform.GetFront() * m_moveSpeed * deltaTime;
        }
        if (Input::IsKeyPressed(KeyCode::S))
        {
            transform.Position -= transform.GetFront() * m_moveSpeed * deltaTime;
        }
        if (Input::IsKeyPressed(KeyCode::A))
        {
            transform.Position -= transform.GetRight() * m_moveSpeed * deltaTime;
        }
        if (Input::IsKeyPressed(KeyCode::D))
        {
            transform.Position += transform.GetRight() * m_moveSpeed * deltaTime;
        }
        if (Input::IsKeyPressed(KeyCode::Q))
        {
            transform.Position -= transform.GetUp() * m_moveSpeed * deltaTime;
        }
        if (Input::IsKeyPressed(KeyCode::E))
        {
            transform.Position += transform.GetUp() * m_moveSpeed * deltaTime;
        }
        // 处理相机旋转
        if (Input::IsMouseButtonPressed(MouseButtonCode::Right))
        {
            glm::vec2 currentMousePosition =
                glm::vec2(Input::GetMousePosition().first, Input::GetMousePosition().second);
            glm::vec2 mouseDelta = currentMousePosition - m_lastMousePosition;

            transform.Rotation.y -= mouseDelta.x * m_rotateSpeed * deltaTime; // 偏航角
            transform.Rotation.x -= mouseDelta.y * m_rotateSpeed * deltaTime; // 俯仰角

            if (transform.Rotation.x > 89.0f)
                transform.Rotation.x = 89.0f;
            if (transform.Rotation.x < -89.0f)
                transform.Rotation.x = -89.0f;
        }

        m_lastMousePosition = glm::vec2(Input::GetMousePosition().first, Input::GetMousePosition().second);
    }

    void OnLayout() override
    {
        auto &transform = m_cameraEntity->GetComponent<Transform>();

        ImGui::Begin("相机控制器");
        ImGui::SliderFloat("移动速度", &m_moveSpeed, 0.1f, 10.0f);
        ImGui::SliderFloat("旋转速度", &m_rotateSpeed, 1.0f, 100.0f);
        if (ImGui::Button("重置相机"))
        {
            transform.Reset();
            transform.Position.z = 3.0f;
        }
        ImGui::End();
    }

    void Deinitialize() override
    {
        EventManager::Get().RemoveListener(this, &CameraController::OnWindowResizeEvent);
    }

    bool OnWindowResizeEvent(const WindowResizeEvent &event)
    {
        auto &camera = m_cameraEntity->GetComponent<CameraComponent>().Camera;
        camera->SetViewportSize(event.GetWidth(), event.GetHeight());
        return false;
    }

private:
    float m_moveSpeed;
    float m_rotateSpeed;
    glm::vec2 m_lastMousePosition;
    std::shared_ptr<Scene> m_activeScene;
    std::shared_ptr<Entity> m_cameraEntity;
};
