#pragma once

#include "pch.h"

#include "KeyCode.h"
#include "MouseButtonCode.h"
#include "Singleton.h"
#include <cstdint>
#include <mutex>
#include <unordered_map>

namespace Doodle
{

class DOO_API Input : public Singleton<Input>
{
public:
    friend class Window;
    void Initialize();
    void Deinitialize();
    static bool IsKeyPressed(KeyCode keycode);
    static bool IsMouseButtonPressed(MouseButtonCode button);
    static std::pair<float, float> GetMousePosition();
    static float GetMouseX();
    static float GetMouseY();

private:
    void Update();
    void InitCallback(void *window);
};

} // namespace Doodle
