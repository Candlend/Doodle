#pragma once

#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "pch.h"
#include <glm/glm.hpp>

#include "ApplicationEvent.h"
#include "Component.h"
#include "EditorPanel.h"
#include "Event.h"
#include "EventManager.h"
#include "IEventHandler.h"
#include "Input.h"
#include "PanelManager.h"
#include "SceneCamera.h"
#include "SceneEvent.h"
#include "Singleton.h"
#include "Texture.h"
#include "ViewportPanel.h"

namespace Doodle
{

class EditorCamera : public SceneCamera, public IEventHandler<ViewportResizeEvent>, public Singleton<EditorCamera>
{
public:
    EditorCamera()
    {
        m_moveSpeed = 2.0f;
        m_rotateSpeed = 20.0f;
        m_transform.Position = glm::vec3(0.0f, 0.0f, 3.0f);
        m_transform.Rotation = glm::vec3(0.0f, 0.0f, 0.0f);

        m_lastMousePosition = glm::vec2(Input::GetMousePosition().first, Input::GetMousePosition().second);
        EventManager::Get()->AddListener<AppUpdateEvent>(this, &EditorCamera::OnUpdate);
    }

    ~EditorCamera()
    {
        EventManager::Get()->RemoveListener<AppUpdateEvent>(this, &EditorCamera::OnUpdate);
    }

    void OnUpdate()
    {
        if (!PanelManager::Get()->GetPanel<ViewportPanel>()->IsShowOnTop())
            return;

        float deltaTime = Application::Time::GetDeltaTime();

        // 获取当前鼠标位置
        glm::vec2 currentMousePosition = glm::vec2(Input::GetMousePosition().first, Input::GetMousePosition().second);
        glm::vec2 mouseDelta = currentMousePosition - m_lastMousePosition;

        // 处理相机移动
        if (Input::IsMouseButtonDown(MouseButton::Right))
        {
            if (Input::IsKeyDown(KeyCode::W))
            {
                m_transform.Position += m_transform.GetFront() * m_moveSpeed * deltaTime;
            }
            if (Input::IsKeyDown(KeyCode::S))
            {
                m_transform.Position -= m_transform.GetFront() * m_moveSpeed * deltaTime;
            }
            if (Input::IsKeyDown(KeyCode::A))
            {
                m_transform.Position -= m_transform.GetRight() * m_moveSpeed * deltaTime;
            }
            if (Input::IsKeyDown(KeyCode::D))
            {
                m_transform.Position += m_transform.GetRight() * m_moveSpeed * deltaTime;
            }
            if (Input::IsKeyDown(KeyCode::Q))
            {
                m_transform.Position -= m_transform.GetUp() * m_moveSpeed * deltaTime;
            }
            if (Input::IsKeyDown(KeyCode::E))
            {
                m_transform.Position += m_transform.GetUp() * m_moveSpeed * deltaTime;
            }

            // 处理相机旋转
            if (Input::IsKeyDown(KeyCode::LeftAlt)) // 检查是否按住 Alt 键
            {
                // 计算围绕焦点的旋转
                glm::vec3 direction = glm::normalize(m_transform.Position - m_focusPoint);
                float radius = glm::length(m_transform.Position - m_focusPoint);

                // 更新偏航和俯仰角
                m_transform.Rotation.y -= mouseDelta.x * m_rotateSpeed * deltaTime; // 偏航角
                m_transform.Rotation.x -= mouseDelta.y * m_rotateSpeed * deltaTime; // 俯仰角

                // 限制俯仰角
                if (m_transform.Rotation.x > 89.0f)
                    m_transform.Rotation.x = 89.0f;
                if (m_transform.Rotation.x < -89.0f)
                    m_transform.Rotation.x = -89.0f;

                // 计算新的位置
                glm::quat rotationQuat = glm::quat(
                    glm::vec3(glm::radians(m_transform.Rotation.x), glm::radians(m_transform.Rotation.y), 0.0f));
                glm::vec3 newPosition = m_focusPoint + rotationQuat * glm::vec3(0.0f, 0.0f, radius);

                m_transform.Position = newPosition;
                m_transform.LookAt(m_focusPoint); // 确保相机朝向焦点
            }
            else
            {
                // 正常的旋转
                m_transform.Rotation.y -= mouseDelta.x * m_rotateSpeed * deltaTime; // 偏航角
                m_transform.Rotation.x -= mouseDelta.y * m_rotateSpeed * deltaTime; // 俯仰角

                // 限制俯仰角
                if (m_transform.Rotation.x > 89.0f)
                    m_transform.Rotation.x = 89.0f;
                if (m_transform.Rotation.x < -89.0f)
                    m_transform.Rotation.x = -89.0f;
            }
        }

        m_lastMousePosition = currentMousePosition;
    }

    bool OnEvent(ViewportResizeEvent &event) override
    {
        int width = event.GetWidth();
        int height = event.GetHeight();
        if (width == 0 || height == 0)
            return false;
        SetViewportSize(event.GetWidth(), event.GetHeight());
        return false;
    }

    glm::vec3 GetPosition() const
    {
        return m_transform.Position;
    }

    glm::mat4 GetViewMatrix() const
    {
        return glm::inverse(m_transform.GetModelMatrix());
    }

    void SetViewMatrix(const glm::mat4 &view)
    {
        auto modelMatrix = glm::inverse(view);
        m_transform.Position = modelMatrix[3];
        m_transform.Rotation = glm::eulerAngles(glm::quat_cast(modelMatrix));

        m_transform.Rotation.x = glm::degrees(m_transform.Rotation.x);
        m_transform.Rotation.y = glm::degrees(m_transform.Rotation.y);
        m_transform.Rotation.z = glm::degrees(m_transform.Rotation.z);
    }

    void Focus(const glm::vec3 &position)
    {
        m_focusPoint = position;
        m_transform.LookAt(position);
        m_transform.Position = position - m_transform.GetFront() * 3.0f;
    }

private:
    float m_moveSpeed;
    float m_rotateSpeed;
    glm::vec2 m_lastMousePosition;

    TransformComponent m_transform;
    glm::vec3 m_focusPoint;
};

} // namespace Doodle