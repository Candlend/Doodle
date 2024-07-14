#pragma once

#include "pch.h"
#include <memory>

namespace Doodle
{

template <typename T> class RenderScope
{
public:
    // 支持 std::shared_ptr
    explicit RenderScope(std::shared_ptr<T> object) : m_object(object)
    {
        if (m_object)
        {
            m_object->Bind();
        }
    }

    // 支持 std::unique_ptr
    explicit RenderScope(std::unique_ptr<T> &object) : m_object(std::move(object))
    {
        if (m_object)
        {
            m_object->Bind();
        }
    }

    // 支持原始指针
    explicit RenderScope(T *object) : m_object(object)
    {
        if (m_object)
        {
            m_object->Bind();
        }
    }

    ~RenderScope()
    {
        if (m_object)
        {
            m_object->Unbind();
        }
    }

private:
    T *m_object; // 统一为原始指针
};

} // namespace Doodle
