#pragma once

#include "pch.h"

#include "Singleton.h"

namespace Doodle
{

struct PanelData;
struct EditorPanel;

class DOO_API PanelManager : public Singleton<PanelManager>
{

public:
    PanelManager() = default;
    ~PanelManager() = default;

    template <typename T> std::shared_ptr<T> CreatePanel()
    {
        std::shared_ptr<T> panel = std::make_shared<T>();
        m_panelMap[panel->GetName()] = panel;
        return panel;
    }

    template <typename T> void AddPanel(const std::string &name, std::shared_ptr<T> panel)
    {
        m_panelMap[name] = std::dynamic_pointer_cast<EditorPanel>(panel);
    }

    void RemovePanel(const std::string &name)
    {
        m_panelMap.erase(name);
    }

    template <typename T> std::shared_ptr<T> GetPanel()
    {
        return std::dynamic_pointer_cast<T>(m_panelMap[T::GetStaticName()]);
    }

    std::shared_ptr<EditorPanel> GetPanel(const std::string &name)
    {
        return m_panelMap[name];
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

    PanelData *GetPanelData(const std::string &name)
    {
        return GetPanelData(m_panelIdMap[name]);
    }

    void SetPanelData(uint32_t id, const PanelData &data);

    std::unordered_map<uint32_t, PanelData> &GetPanelDataMap()
    {
        return m_panelDataMap;
    }

    std::unordered_map<std::string, std::shared_ptr<EditorPanel>> &GetPanels()
    {
        return m_panelMap;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<EditorPanel>> m_panelMap;
    std::unordered_map<std::string, uint32_t> m_panelIdMap;
    std::unordered_map<uint32_t, PanelData> m_panelDataMap;
    uint32_t m_focusedPanelID = 0;
};

} // namespace Doodle