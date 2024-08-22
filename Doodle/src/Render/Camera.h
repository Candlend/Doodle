#pragma once

#include "pch.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Doodle
{

class DOO_API Camera
{
public:
    Camera()
    {
        // 初始化相机参数
        m_position = glm::vec3(0.0f, 0.0f, 3.0f);
        m_front = glm::vec3(0.0f, 0.0f, -1.0f);
        m_up = glm::vec3(0.0f, 1.0f, 0.0f);
        m_fov = 45.0f;
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

    glm::mat4 GetProjectionMatrix(float aspectRatio) const
    {
        return glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 100.0f);
    }

    // TODO 其他相机控制方法...

private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    float m_fov;
};

} // namespace Doodle