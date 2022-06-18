/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Widgets/Snackbar.hpp"
#include "Core/CommonApplication.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/GUILayer.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/Window.hpp"
#include "Math/Math.hpp"
#include "Core/Engine.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "IconsFontAwesome5.h"
#include "imgui/imgui.h"

namespace Lina::Editor
{
    float       m_swayDuration    = 0.2f;
    float       m_timer           = 0.0f;
    float       m_stayAliveTime   = 4.0f;
    std::string m_currentText     = "";
    LogLevel    m_currentLogLevel = LogLevel::None;
    ImVec2      size              = ImVec2(350, 50);

    void Tick(const EGUILayerRender& ev)
    {
        m_timer += (float)Engine::Get()->GetRawDelta();

        if (m_timer > m_swayDuration)
        {
            if (m_timer - m_swayDuration > m_stayAliveTime)
            {
                Event::EventSystem::Get()->Disconnect<EGUILayerRender, &Tick>();
            }
        }

        float ratio = m_timer / m_swayDuration;
        ratio       = Math::Clamp(ratio, 0.0f, 1.0f);
        auto*         appWindow = Graphics::Window::Get();
        const Vector2 windowPos = appWindow->GetPos();
        const ImVec2  min       = ImVec2(windowPos.x - size.x + (ratio * (size.x + 20 * GUILayer::Get()->GetDPIScale())), windowPos.y + (float)appWindow->GetHeight() * 0.91f);
        const ImVec2  max       = ImVec2(min.x + size.x, min.y + size.y);


        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin("##snackbar", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration);

        const ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        ImVec4       borderCol = ImGui::GetStyleColorVec4(ImGuiCol_Button);

        char* icon = ICON_FA_INFO;
        if (m_currentLogLevel == LogLevel::Error)
        {
            borderCol = ImVec4(0.8f, 0.35f, 0.35f, 1.0f);
            icon      = ICON_FA_TIMES;
        }
        else if (m_currentLogLevel == LogLevel::Warn)
        {
            borderCol = ImVec4(0.7f, 0.4f, 0.1f, 1.0f);
            icon      = ICON_FA_EXCLAMATION_TRIANGLE;
        }

        // Background & border.
        ImGui::GetForegroundDrawList()->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_PopupBg)), 3.0f);
        ImGui::GetForegroundDrawList()->AddRect(min, max, ImGui::ColorConvertFloat4ToU32(borderCol), 3.0f, 0, 2.0f);

        // Icon text.
        float textHeight = ImGui::CalcTextSize(m_currentText.c_str()).y;
        WidgetsUtility::PushIconFontDefault();
        ImGui::GetForegroundDrawList()->AddText(ImVec2(min.x + 20 * GUILayer::Get()->GetDPIScale(), min.y + size.y / 2.0f - textHeight / 2.0f), ImGui::ColorConvertFloat4ToU32(borderCol), icon);
        ImGui::PopFont();

        // Info text.
        ImGui::PushFont(GUILayer::Get()->GetMediumFont());
        textHeight = ImGui::CalcTextSize(m_currentText.c_str()).y;
        ImGui::GetForegroundDrawList()->AddText(ImVec2(min.x + 50 * GUILayer::Get()->GetDPIScale(), min.y + size.y / 2.0f - textHeight / 2.0f), ImGui::ColorConvertFloat4ToU32(textColor), m_currentText.c_str());
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void Snackbar::PushSnackbar(LogLevel level, const std::string& text)
    {
        m_timer           = 0.0f;
        m_currentText     = text;
        m_currentLogLevel = level;
        Event::EventSystem::Get()->Connect<EGUILayerRender, &Tick>();
        ImGui::PushFont(GUILayer::Get()->GetMediumFont());
        size.x = ImGui::CalcTextSize(text.c_str()).x + 80 * GUILayer::Get()->GetDPIScale();
        size.y = 50 * GUILayer::Get()->GetDPIScale();
        ImGui::PopFont();
    }
} // namespace Lina::Editor