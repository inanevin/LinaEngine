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

#include "Panels/SystemsPanel.hpp"
#include "ECS/Registry.hpp"
#include "ECS/SystemList.hpp"
#include "ECS/System.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/Engine.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "Core/PhysicsBackend.hpp"
#include "Core/InputBackend.hpp"
#include "imgui/imgui.h"

namespace Lina::Editor
{

    void SystemsPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
        m_ecs = ECS::Registry::Get();
    }

    void SystemsPanel::DrawSystem(const ECS::System* system, const std::string& pipeline)
    {
        // Label.
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text(system->GetName().c_str());

        // Pipeline
        ImGui::TableNextColumn();
        ImGui::Text(pipeline.c_str());

        // Pool
        const std::string poolSize = std::to_string(system->GetPoolSize());
        ImGui::TableNextColumn();
        ImGui::Text(poolSize.c_str());
    }

    void SystemsPanel::Draw()
    {
        if (m_show)
        {
            if (Begin())
            {

                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_PopupBg));
                ImGui::BeginChild("systems_child", ImVec2(0, -30), true);

                static ImGuiTableFlags flags           = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg;
                const float            TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;

                if (ImGui::BeginTable("systensTable", 3, flags))
                {
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
                    ImGui::TableSetupColumn("Pipeline", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
                    ImGui::TableSetupColumn("Pool Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 6.0f);
                    ImGui::TableHeadersRow();

                    const auto& mainPipeline      = Engine::Get()->GetPipeline();
                    const auto& animationPipeline = Graphics::RenderEngineBackend::Get()->GetAnimationPipeline();
                    const auto& renderingPipeline = Graphics::RenderEngineBackend::Get()->GetRenderingPipeline();
                    const auto& physicsPipeline   = Physics::PhysicsEngineBackend::Get()->GetPipeline();
                    const auto& inputPipeline     = Input::InputEngineBackend::Get()->GetPipeline();

                    for (const auto* system : mainPipeline.GetSystems())
                        DrawSystem(system, "Main");
                    for (const auto* system : animationPipeline.GetSystems())
                        DrawSystem(system, "Animation");
                    for (const auto* system : renderingPipeline.GetSystems())
                        DrawSystem(system, "Rendering");
                    for (const auto* system : physicsPipeline.GetSystems())
                        DrawSystem(system, "Physics");
                    for (const auto* system : inputPipeline.GetSystems())
                        DrawSystem(system, "Input");

                    ImGui::EndTable();
                }

                ImGui::EndChild();
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                WidgetsUtility::HorizontalDivider(-ImGui::GetStyle().ItemSpacing.y, 4);
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();

                End();
            }
        }
    }

} // namespace Lina::Editor
