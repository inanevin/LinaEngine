#include "Panels/SystemsPanel.hpp"
#include "ECS/Registry.hpp"
#include "ECS/SystemList.hpp"
#include "ECS/System.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/Engine.hpp"
#include "Core/RenderEngine.hpp"
#include "Core/PhysicsEngine.hpp"
#include "Input/InputEngine.hpp"
#include "imgui/imgui.h"

namespace Lina::Editor
{

    void SystemsPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
    }

    void SystemsPanel::DrawSystem(const ECS::System* system, const String& pipeline)
    {
        // Label.
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text(system->GetName().c_str());

        // Pipeline
        ImGui::TableNextColumn();
        ImGui::Text(pipeline.c_str());

        // Pool
        const String poolSize = TO_STRING(system->GetPoolSize());
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
                    const auto& animationPipeline = Graphics::RenderEngine::Get()->GetAnimationPipeline();
                    const auto& renderingPipeline = Graphics::RenderEngine::Get()->GetRenderingPipeline();
                    const auto& physicsPipeline   = Physics::PhysicsEngine::Get()->GetPipeline();
                    const auto& inputPipeline     = Input::InputEngine::Get()->GetPipeline();

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
