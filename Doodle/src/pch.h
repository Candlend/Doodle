#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <glm/glm.hpp>
#include <rfl.hpp>

#include "Log.h"

#ifdef DOO_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#ifdef DOO_BUILD_PYTHON
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11; // NOLINT
#endif

// NOLINTBEGIN
namespace rfl
{
template <> struct Reflector<glm::vec2>
{
    struct ReflType
    {
        float x;
        float y;
    };

    static glm::vec2 to(const ReflType &v) noexcept
    {
        return {v.x, v.y};
    }

    static ReflType from(const glm::vec2 &v)
    {
        return {v.x, v.y};
    }
};

template <> struct Reflector<glm::vec3>
{
    struct ReflType
    {
        float x;
        float y;
        float z;
    };

    static glm::vec3 to(const ReflType &v) noexcept
    {
        return {v.x, v.y, v.z};
    }

    static ReflType from(const glm::vec3 &v)
    {
        return {v.x, v.y, v.z};
    }
};

template <> struct Reflector<glm::vec4>
{
    struct ReflType
    {
        float x;
        float y;
        float z;
        float w;
    };

    static glm::vec4 to(const ReflType &v) noexcept
    {
        return {v.x, v.y, v.z, v.w};
    }

    static ReflType from(const glm::vec4 &v)
    {
        return {v.x, v.y, v.z, v.w};
    }
};

template <> struct Reflector<glm::mat3>
{
    struct ReflType
    {
        std::array<float, 9> data;
    };

    static glm::mat3 to(const ReflType &v) noexcept
    {
        return {v.data[0], v.data[1], v.data[2], v.data[3], v.data[4], v.data[5], v.data[6], v.data[7], v.data[8]};
    }

    static ReflType from(const glm::mat3 &v)
    {
        return {v[0][0], v[0][1], v[0][2], v[1][0], v[1][1], v[1][2], v[2][0], v[2][1], v[2][2]};
    }
};

template <> struct Reflector<glm::mat4>
{
    struct ReflType
    {
        std::array<float, 16> data;
    };

    static glm::mat4 to(const ReflType &v) noexcept
    {
        return {v.data[0], v.data[1], v.data[2],  v.data[3],  v.data[4],  v.data[5],  v.data[6],  v.data[7],
                v.data[8], v.data[9], v.data[10], v.data[11], v.data[12], v.data[13], v.data[14], v.data[15]};
    }

    static ReflType from(const glm::mat4 &v)
    {
        return {v[0][0], v[0][1], v[0][2], v[0][3], v[1][0], v[1][1], v[1][2], v[1][3],
                v[2][0], v[2][1], v[2][2], v[2][3], v[3][0], v[3][1], v[3][2], v[3][3]};
    }
};

template <> struct Reflector<glm::ivec2>
{
    struct ReflType
    {
        int x;
        int y;
    };

    static glm::ivec2 to(const ReflType &v) noexcept
    {
        return {v.x, v.y};
    }

    static ReflType from(const glm::ivec2 &v)
    {
        return {v.x, v.y};
    }
};

template <> struct Reflector<glm::ivec3>
{
    struct ReflType
    {
        int x;
        int y;
        int z;
    };

    static glm::ivec3 to(const ReflType &v) noexcept
    {
        return {v.x, v.y, v.z};
    }

    static ReflType from(const glm::ivec3 &v)
    {
        return {v.x, v.y, v.z};
    }
};

template <> struct Reflector<glm::ivec4>
{
    struct ReflType
    {
        int x;
        int y;
        int z;
        int w;
    };

    static glm::ivec4 to(const ReflType &v) noexcept
    {
        return {v.x, v.y, v.z, v.w};
    }

    static ReflType from(const glm::ivec4 &v)
    {
        return {v.x, v.y, v.z, v.w};
    }
};

} // namespace rfl
// NOLINTEND