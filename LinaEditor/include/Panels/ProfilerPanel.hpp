/*
Class: ProfilerPanel

Responsible for displaying engine stats.

Timestamp: 10/22/2020 11:22:48 PM
*/

#pragma once

#ifndef ProfilerPanel_HPP
#define ProfilerPanel_HPP

#include "Panels/EditorPanel.hpp"

#include "Data/Deque.hpp"

namespace Lina::Editor
{
    class ProfilerPanel : public EditorPanel
    {

    public:
        ProfilerPanel() = default;
        ~ProfilerPanel() = default;

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;

    private:
        float m_lastMSDisplayTime = 0.0f;
    };
} // namespace Lina::Editor

#endif
