/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Panels/ProgressPanel.hpp"
#include "Core/EditorCommon.hpp"
#include "imgui/imgui.h"
#include "Core/WindowBackend.hpp"
#include "Widgets/WidgetsUtility.hpp"

namespace Lina::Editor
{
    constexpr float lineWidth = 2.0f;

    void ProgressPanel::Draw(const std::string& currentResource, float percentage)
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar;

        auto*        appWindow = Graphics::WindowBackend::Get();
        const ImVec2 size      = ImVec2(400, 50);
        const ImVec2 pos       = ImVec2(appWindow->GetWorkSize().x - size.x - 20, appWindow->GetWorkSize().y * 0.91f);
        const float  padding   = CURSOR_X_LABELS;
        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(size);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::GetStyleColorVec4(ImGuiCol_PopupBg));
        ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetStyleColorVec4(ImGuiCol_Header));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
        if (ImGui::Begin(ID_PROGRESSPANEL, NULL, flags))
        {
            WidgetsUtility::IncrementCursorPosY(4);
            ImGui::SetCursorPosX(CURSOR_X_LABELS);
            ImGui::Text("Processing:");
            ImGui::SameLine();
            ImGui::Text(currentResource.c_str());
            WidgetsUtility::IncrementCursorPosY(4);
            const ImVec2 p1        = ImVec2(ImGui::GetCursorScreenPos().x + padding, ImGui::GetCursorScreenPos().y);
            const ImVec2 p2        = ImVec2(p1.x + size.x - padding, p1.y);
            const ImVec4 lineBG    = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            const ImVec4 lineFill  = ImGui::GetStyleColorVec4(ImGuiCol_Header);
            const ImVec4 overlayBG = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            const ImVec2 overlayP2 = ImVec2(p1.x + ((size.x - padding) * percentage / 100.0f), p1.y);
            ImGui::GetWindowDrawList()->AddLine(p1, p2, ImGui::ColorConvertFloat4ToU32(lineBG), lineWidth);
            ImGui::GetWindowDrawList()->AddLine(p1, overlayP2, ImGui::ColorConvertFloat4ToU32(lineFill), lineWidth);

            ImGui::End();
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
} // namespace Lina::Editor