#include <glfw/glfw3.h>
#include <glad/glad.h>
#include <stb_image.h>

#include "SplashScreen.h"

namespace Doodle
{

SplashScreen::SplashScreen()
{
    m_appWindow = nullptr;
    m_splashScreenWindow = nullptr;
    m_textureID = 0;
    m_imageWidth = 0;
    m_imageHeight = 0;
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Hide the window until we're ready to show it
}

bool SplashScreen::Load(const std::string &filename)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename.c_str(), &m_imageWidth, &m_imageHeight, nullptr, 4);
    if (!data)
    {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return false;
    }

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_imageWidth, m_imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return true;
}

void SplashScreen::Begin(float minimumDuration)
{
    if (m_textureID == 0)
    {
        std::cerr << "Cannot show splash screen since no image has been loaded" << std::endl;
        return;
    }

    m_endTime = glfwGetTime() + minimumDuration;
    m_appWindow = glfwGetCurrentContext();

    glfwHideWindow(static_cast<GLFWwindow *>(m_appWindow));
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    m_splashScreenWindow =
        glfwCreateWindow(m_imageWidth, m_imageHeight, "SplashScreen", nullptr, static_cast<GLFWwindow *>(m_appWindow));
    glfwSetWindowPos(static_cast<GLFWwindow *>(m_splashScreenWindow), (glfwGetVideoMode(glfwGetPrimaryMonitor())->width - m_imageWidth) / 2,
                     (glfwGetVideoMode(glfwGetPrimaryMonitor())->height - m_imageHeight) / 2);
    glfwMakeContextCurrent(static_cast<GLFWwindow *>(m_splashScreenWindow));

    // Set up OpenGL state
    glViewport(0, 0, m_imageWidth, m_imageHeight);
    glClearColor(0, 0, 0, 1);
    glEnable(GL_TEXTURE_2D);

    // Draw the image
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();

    glfwSwapBuffers(static_cast<GLFWwindow *>(m_splashScreenWindow));
    glFlush();

    // Set the context back to main for rest of setup
    glfwMakeContextCurrent(static_cast<GLFWwindow *>(m_appWindow));
}

void SplashScreen::End()
{
    if (!m_appWindow || !m_splashScreenWindow)
    {
        return;
    }

    while (m_endTime > glfwGetTime())
    {
        glfwPollEvents(); // Keep the window responsive
    }

    glfwDestroyWindow(static_cast<GLFWwindow *>(m_splashScreenWindow));
    m_splashScreenWindow = nullptr;

    glfwShowWindow(static_cast<GLFWwindow *>(m_appWindow));
}

SplashScreen::~SplashScreen()
{
    if (m_textureID)
    {
        glDeleteTextures(1, &m_textureID);
    }
}

} // namespace Doodle
