#include "pch.h"
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include "MathUtils.h"

namespace Doodle
{

bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &translation, glm::vec3 &rotation, glm::vec3 &scale)
{
    // From glm::decompose in matrix_decompose.inl

    using namespace glm;
    using T = float;

    mat4 localMatrix(transform);

    // Normalize the matrix.
    if (epsilonEqual(localMatrix[3][3], static_cast<float>(0), epsilon<T>()))
        return false;

    // First, isolate perspective.  This is the messiest.
    if (epsilonNotEqual(localMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(localMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(localMatrix[2][3], static_cast<T>(0), epsilon<T>()))
    {
        // Clear the perspective partition
        localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = static_cast<T>(0);
        localMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    translation = vec3(localMatrix[3]);
    localMatrix[3] = vec4(0, 0, 0, localMatrix[3].w);

    vec3 row[3], pdum3;

    // Now get scale and shear.
    for (length_t i = 0; i < 3; ++i)
        for (length_t j = 0; j < 3; ++j)
            row[i][j] = localMatrix[i][j];

    // Compute X scale factor and normalize first row.
    scale.x = length(row[0]);
    row[0] = detail::scale(row[0], static_cast<T>(1));
    scale.y = length(row[1]);
    row[1] = detail::scale(row[1], static_cast<T>(1));
    scale.z = length(row[2]);
    row[2] = detail::scale(row[2], static_cast<T>(1));

    // At this point, the matrix (in rows[]) is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
#if 0
        Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
        if (dot(Row[0], Pdum3) < 0)
        {
            for (length_t i = 0; i < 3; i++)
            {
                scale[i] *= static_cast<T>(-1);
                Row[i] *= static_cast<T>(-1);
            }
        }
#endif

    rotation.y = asin(-row[0][2]);
    if (cos(rotation.y) != 0)
    {
        rotation.x = atan2(row[1][2], row[2][2]);
        rotation.z = atan2(row[0][1], row[0][0]);
    }
    else
    {
        rotation.x = atan2(-row[2][0], row[1][1]);
        rotation.z = 0;
    }
    rotation = degrees(rotation);

    return true;
}

glm::mat4 RemoveScaling(const glm::mat4 &modelMatrix)
{
    // 提取缩放部分
    glm::vec3 scale;
    scale.x = glm::length(glm::vec3(modelMatrix[0][0], modelMatrix[0][1], modelMatrix[0][2]));
    scale.y = glm::length(glm::vec3(modelMatrix[1][0], modelMatrix[1][1], modelMatrix[1][2]));
    scale.z = glm::length(glm::vec3(modelMatrix[2][0], modelMatrix[2][1], modelMatrix[2][2]));

    // 创建一个单位矩阵
    glm::mat4 noScaleMatrix = glm::mat4(1.0f);

    // 将旋转部分提取出来
    noScaleMatrix[0] = modelMatrix[0] / scale.x;
    noScaleMatrix[1] = modelMatrix[1] / scale.y;
    noScaleMatrix[2] = modelMatrix[2] / scale.z;

    // 将位移部分保留
    noScaleMatrix[3] = modelMatrix[3];

    return noScaleMatrix;
}

} // namespace Doodle
