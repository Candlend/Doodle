#define STB_IMAGE_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "ApplicationRunner.h"
#include "Application.h"

namespace Doodle
{

void ApplicationRunner::Run()
{
    m_app->Run();
    m_app->Deinitialize();
    m_app.reset();
    m_window.reset();
}

void ApplicationRunner::SetIcon(void *window, const std::string &iconSmall, const std::string &iconLarge)
{
    stbi_set_flip_vertically_on_load(false);
    // 加载图标
    int iconWidthSmall, iconHeightSmall, iconChannelsSmall;
    int iconWidthLarge, iconHeightLarge, iconChannelsLarge;

    // 加载小图标
    stbi_uc *iconPixelsSmall = stbi_load(iconSmall.c_str(), &iconWidthSmall, &iconHeightSmall, &iconChannelsSmall, 4);
    if (!iconPixelsSmall)
    {
        DOO_CORE_WARN("Failed to load small window icon!");
    }

    // 加载大图标
    stbi_uc *iconPixelsLarge = stbi_load(iconLarge.c_str(), &iconWidthLarge, &iconHeightLarge, &iconChannelsLarge, 4);
    if (!iconPixelsLarge)
    {
        DOO_CORE_WARN("Failed to load large window icon!");
    }

    // 设置图标
    GLFWimage images[2];
    if (iconPixelsSmall && iconPixelsLarge)
    {
        images[0].width = iconWidthSmall;
        images[0].height = iconHeightSmall;
        images[0].pixels = iconPixelsSmall;

        images[1].width = iconWidthLarge;
        images[1].height = iconHeightLarge;
        images[1].pixels = iconPixelsLarge;

        glfwSetWindowIcon(static_cast<GLFWwindow *>(window), 2, images);
    }

    // 释放内存
    if (iconPixelsSmall)
    {
        stbi_image_free(iconPixelsSmall);
    }
    if (iconPixelsLarge)
    {
        stbi_image_free(iconPixelsLarge);
    }
}
} // namespace Doodle