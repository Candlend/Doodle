#pragma once

#include "pch.h"

#include "ImGuiPopupModel.h"

namespace Doodle
{

class ConfirmPopupModal : public ImGuiPopupModal
{
public:
    const char *GetModalName() const override
    {
        return m_modalName.c_str();
    }

    void OnLayOut() override
    {
        if (m_onPopupLayOut)
        {
            m_onPopupLayOut();
            return;
        }
        ImGui::Text("Do you confirm that you wish to proceed with this action?");
    }

    void Confirm() override
    {
        if (m_onConfirm)
        {
            m_onConfirm();
        }
    }

    void Cancel() override
    {
        if (m_onCancel)
        {
            m_onCancel();
        }
    }

    void Bind(const std::string &modalName, std::function<void()> onConfirm, std::function<void()> onCancel = nullptr,
              std::function<void()> onPopupLayOut = nullptr)
    {
        m_modalName = modalName;
        m_onConfirm = onConfirm;
        m_onCancel = onCancel;
        m_onPopupLayOut = onPopupLayOut;
    }

private:
    std::string m_modalName = "Confirm";
    std::function<void()> m_onConfirm;
    std::function<void()> m_onCancel;
    std::function<void()> m_onPopupLayOut;
};
} // namespace Doodle
