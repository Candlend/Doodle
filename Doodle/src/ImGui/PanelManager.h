#pragma once

#include "KeyCode.h"
#include "Log.h"
#include "glm/fwd.hpp"
#include "pch.h"
#include <glm/glm.hpp>
#include <imgui.h>

#include "Singleton.h"
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace Doodle
{

struct PanelData;
struct ImGuiPanel;

class PanelManager : public Singleton<PanelManager>
{

public:
    PanelManager() = default;
    ~PanelManager() = default;

    template <typename T> std::shared_ptr<T> CreatePanel()
    {
        std::shared_ptr<T> panel = std::make_shared<T>();
        m_panelMap[panel->GetPanelName()] = panel;
        return panel;
    }

    template <typename T> void AddPanel(const std::string &name, std::shared_ptr<T> panel)
    {
        m_panelMap[name] = std::dynamic_pointer_cast<ImGuiPanel>(panel);
    }

    void RemovePanel(const std::string &name)
    {
        m_panelMap.erase(name);
    }

    template <typename T> std::shared_ptr<T> GetPanel(const std::string &name)
    {
        return std::dynamic_pointer_cast<T>(m_panelMap[name]);
    }

    uint32_t GetFocusedPanelID() const
    {
        return m_focusedPanelID;
    }

    void SetFocusedPanelID(uint32_t id)
    {
        m_focusedPanelID = id;
    }

    PanelData *GetPanelData(uint32_t id);

    void SetPanelData(uint32_t id, const PanelData &data);

private:
    std::unordered_map<std::string, std::shared_ptr<ImGuiPanel>> m_panelMap;
    std::unordered_map<std::string, uint32_t> m_panelIdMap;
    std::unordered_map<uint32_t, PanelData> m_panelDataMap;
    uint32_t m_focusedPanelID = 0;
};

} // namespace Doodle