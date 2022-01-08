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

#include "Panels/ModelPanel.hpp"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Rendering/Model.hpp"

namespace Lina::Editor
{
    void ModelPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
        m_leftPaneWidth    = 380;
        m_leftPaneMinWidth = 380;
        m_leftPaneMaxWidth = 800;
    }
    void ModelPanel::Draw()
    {
        if (m_show)
        {
            if (Begin())
            {
                m_rightPaneSize = Vector2(ImGui::GetWindowWidth() - m_leftPaneWidth, ImGui::GetWindowHeight());
                ImGui::BeginChild("modelPanel_leftPane", ImVec2(m_leftPaneWidth, 0));
                DrawModelSettings();
                ImGui::EndChild();

                ImGui::SameLine();

                ImGui::BeginChild("modelPanel_rightPane");
                DrawModel();
                ImGui::EndChild();

                WidgetsUtility::VerticalResizeDivider(true, &m_resizeDividerPressedPos, &m_leftPaneWidth, m_leftPaneMinWidth, m_leftPaneMaxWidth, &m_lockWindowPos, &m_draggingVerticalDivider);

                End();
            }
        }
    }
    void ModelPanel::SetTargetModel(Graphics::Model* model)
    {
        m_targetModel = model;
        m_title       = "Model Panel: " + model->GetPath();
    }

    void ModelPanel::DrawModelSettings()
    {
        auto* assetData = m_targetModel->GetAssetData();
        WidgetsUtility::IncrementCursorPosY(ImGui::GetStyle().ItemSpacing.y + 9.5f);

        WidgetsUtility::PropertyLabel("Triangulate");
        ImGui::Checkbox("##_triangulate", &assetData->m_triangulate);

        WidgetsUtility::PropertyLabel("Calculate Tangents");
        ImGui::Checkbox("##_calctangent", &assetData->m_calculateTangentSpace);

        WidgetsUtility::PropertyLabel("Flip UVs");
        ImGui::Checkbox("##_flipuv", &assetData->m_flipUVs);

        WidgetsUtility::PropertyLabel("Flip Winding");
        ImGui::Checkbox("##_flipwinding", &assetData->m_flipWinding);

        WidgetsUtility::PropertyLabel("Smooth Normals");
        ImGui::Checkbox("##_smoothnormals", &assetData->m_smoothNormals);

        WidgetsUtility::PropertyLabel("Import Scale");
        WidgetsUtility::DragFloat("##_globalscale", nullptr, &assetData->m_globalScale);

        ImGui::SetCursorPosX(CURSOR_X_LABELS);
        if (WidgetsUtility::Button("Save Settings", ImVec2(m_leftPaneWidth - CURSOR_X_LABELS * 2, 25)))
        {
            Resources::SaveArchiveToFile(assetData->GetPath(), *assetData);
            const std::string  path = m_targetModel->GetPath();
            const StringIDType sid  = m_targetModel->GetSID();
            const TypeID       tid  = GetTypeID<Graphics::Model>();
            Resources::ResourceStorage::Get()->Unload<Graphics::Model>(sid);
            Event::EventSystem::Get()->Trigger<Event::ERequestResourceReload>(Event::ERequestResourceReload{path, tid, sid});
            m_targetModel = Resources::ResourceStorage::Get()->GetResource<Graphics::Model>(sid);
        }
    }

    void ModelPanel::DrawModel()
    {
#pragma warning(disable : 4312)

        const ImVec2 padding   = ImVec2(ImGui::GetStyle().ItemSpacing.x + 4, ImGui::GetStyle().ItemSpacing.y + 4);
        const ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        const ImVec2 bgMin     = ImVec2(cursorPos.x + padding.x - 4, cursorPos.y + padding.x);
        const ImVec2 bgMax     = ImVec2(cursorPos.x + m_rightPaneSize.x - padding.x * 2, cursorPos.y + m_rightPaneSize.y - padding.x * 2.0f);
        const ImVec2 bgSize    = ImVec2(bgMax.x - bgMin.x, bgMax.y - bgMin.y);
        const ImVec4 bgCol     = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);
        ImGui::GetWindowDrawList()->AddRectFilled(bgMin, bgMax, ImGui::ColorConvertFloat4ToU32(bgCol), 3.0f);

        const float drawPadding = 5;
        ImVec2      drawSize    = ImVec2(bgSize.x - drawPadding, bgSize.y - drawPadding);

        auto&            camSystem = EditorApplication::Get()->GetCameraSystem();
        auto&            data      = ECS::Registry::Get()->get<ECS::EntityDataComponent>(camSystem.GetEditorCamera());
        const Vector3    location  = data.GetLocation();
        const Quaternion rotation  = data.GetRotation();

        data.SetLocation(Vector3(0, 5, -10));
        data.SetRotation(Quaternion::LookAt(data.GetLocation(), Vector3(0, 0, 0), Vector3::Up));

        auto* renderEngine = Graphics::RenderEngineBackend::Get();

        const ImVec2 imageMin    = ImVec2(bgMin.x + bgSize.x / 2.0f - drawSize.x / 2.0f, bgMin.y + bgSize.y / 2.0f - drawSize.y / 2.0f);
        const ImVec2 imageMax    = ImVec2(imageMin.x + drawSize.x, imageMin.y + drawSize.y);
        const ImVec2 imageSize   = ImVec2(imageMax.x - imageMin.x, imageMax.y - imageMin.y);
        const float  aspectRatio = imageSize.x / imageSize.y;

        auto* cameraSystem = renderEngine->GetCameraSystem();
        if (cameraSystem->GetAspectRatio() != aspectRatio)
            cameraSystem->SetAspectRatio(aspectRatio);

        uint32 previewTexture = renderEngine->RenderModelPreview(m_targetModel);
        ImGui::GetWindowDrawList()->AddImage((void*)previewTexture, imageMin, imageMax, ImVec2(0, 1), ImVec2(1, 0));
        // Reset
        data.SetLocation(location);
        data.SetRotation(rotation);
    }
} // namespace Lina::Editor