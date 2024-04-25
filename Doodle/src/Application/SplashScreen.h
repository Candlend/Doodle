#pragma once

#include "pch.h"

using GLuint = unsigned int;

namespace Doodle
{

class DOO_API SplashScreen
{
public:
    SplashScreen();
    ~SplashScreen();

    bool LoadImage(const std::string &filename);

    void Begin(float minimumDuration);

    void End();

private:
    void *m_appWindow;
    void *m_splashScreenWindow;
    unsigned int m_textureID;
    int m_imageWidth;
    int m_imageHeight;
    float m_endTime;
};

} // namespace Doodle