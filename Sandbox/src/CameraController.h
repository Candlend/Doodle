#pragma once

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
    }

    void OnUpdate() override
    {
        m_activeScene = SceneManager::Get().GetActiveScene();
        auto &transform = m_activeScene->GetMainCameraEntity()->GetComponent<Transform>();

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
            // 获取当前鼠标位置
            glm::vec2 currentMousePosition =
                glm::vec2(Input::GetMousePosition().first, Input::GetMousePosition().second);

            // 计算鼠标移动量
            glm::vec2 mouseDelta = currentMousePosition - m_lastMousePosition;

            // 更新偏航角和俯仰角
            transform.Rotation.y -= mouseDelta.x * m_rotateSpeed * deltaTime; // 偏航角
            transform.Rotation.x -= mouseDelta.y * m_rotateSpeed * deltaTime; // 俯仰角

            // 限制俯仰角，避免翻转
            if (transform.Rotation.x > 89.0f)
                transform.Rotation.x = 89.0f;
            if (transform.Rotation.x < -89.0f)
                transform.Rotation.x = -89.0f;
        }

        m_lastMousePosition = glm::vec2(Input::GetMousePosition().first, Input::GetMousePosition().second);
    }

    void OnLayout() override
    {
        ImGui::Begin("相机控制器");
        ImGui::SliderFloat("移动速度", &m_moveSpeed, 0.1f, 10.0f);
        ImGui::SliderFloat("旋转速度", &m_rotateSpeed, 1.0f, 100.0f);
        if (ImGui::Button("重置相机"))
        {
            m_activeScene->GetMainCameraEntity()->GetComponent<Transform>().Reset();
            m_activeScene->GetMainCameraEntity()->GetComponent<Transform>().Position.z = 3.0f;
        }
        ImGui::End();
    }

    void Deinitialize() override
    {
    }

private:
    float m_moveSpeed;
    float m_rotateSpeed;
    glm::vec2 m_lastMousePosition;
    std::shared_ptr<Scene> m_activeScene;
};