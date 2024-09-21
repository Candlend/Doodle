#pragma once

#include "Log.h"
#include "pch.h"
#include <memory>

namespace Doodle
{

template <typename T> class RenderScope
{
public:
    RenderScope(T *object) : m_object(object)
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
