#pragma once

#include "pch.h"
#include <glm/glm.hpp>

namespace Doodle
{

bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &outTranslation, glm::vec3 &outRotation,
                        glm::vec3 &outScale);

glm::mat4 RemoveScaling(const glm::mat4 &modelMatrix);
}