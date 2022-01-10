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

#include "Panels/PreviewPanel.hpp"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Rendering/Model.hpp"
#include "Core/Engine.hpp"
#include "Drawers/TextureDrawer.hpp"
#include "Drawers/ModelDrawer.hpp"
#include "Drawers/MaterialDrawer.hpp"

namespace Lina::Editor
{
    void PreviewPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
        m_leftPaneWidth    = 380;
        m_leftPaneMinWidth = 380;
        m_leftPaneMaxWidth = 800;
    }
    void PreviewPanel::Draw()
    {
        if (m_show)
        {
            if (Begin())
            {
                m_rightPaneSize = Vector2(ImGui::GetWindowWidth() - m_leftPaneWidth, ImGui::GetWindowHeight());
                ImGui::BeginChild("previewPanel_leftPane", ImVec2(m_leftPaneWidth, 0));

                if (m_previewType == PreviewType::Texture)
                    TextureDrawer::DrawTextureSettings(m_targetTexture, m_leftPaneWidth);
                else if (m_previewType == PreviewType::Model)
                    ModelDrawer::DrawModelSettings(m_targetModel, m_leftPaneWidth);
                else if (m_previewType == PreviewType::Material)
                    MaterialDrawer::DrawMaterialSettings(m_targetMaterial, m_leftPaneWidth);

                ImGui::EndChild();

                ImGui::SameLine();

                ImGui::BeginChild("previewPanel_rightPane");

                DrawPreviewArea();

                ImGui::EndChild();

                WidgetsUtility::VerticalResizeDivider(true, &m_resizeDividerPressedPos, &m_leftPaneWidth, m_leftPaneMinWidth, m_leftPaneMaxWidth, &m_lockWindowPos, &m_draggingVerticalDivider);

                End();
            }
        }
    }
    void PreviewPanel::Open()
    {
        m_previewCameraPosition = Vector3(0, 5, -10);
        m_previewCameraRotation = Quaternion::LookAt(m_previewCameraPosition, Vector3(0, 0, 0), Vector3::Up);
        m_mouseAngles.x         = m_previewCameraRotation.GetEuler().y;
        m_mouseAngles.y         = m_previewCameraRotation.GetEuler().x;
        EditorPanel::Open();
    }

    void PreviewPanel::SetTargetModel(Graphics::Model* model)
    {
        m_targetModel = model;
        m_title       = "Model Preview: " + model->GetPath();
        m_previewType = PreviewType::Model;
        Open();
    }

    void PreviewPanel::SetTargetTexture(Graphics::Texture* texture)
    {
        m_targetTexture = texture;
        m_title         = "Texture Preview: " + texture->GetPath();
        m_previewType   = PreviewType::Texture;
        Open();
    }

    void PreviewPanel::SetTargetMaterial(Graphics::Material* mat)
    {
        m_targetMaterial = mat;
        m_title          = "Material Preview: " + mat->GetPath();
        m_previewType    = PreviewType::Material;
        Open();
    }

    void PreviewPanel::SetTargetShader(Graphics::Shader* shader)
    {
        m_targetShader = shader;
        m_title        = "Shader Preview: " + shader->GetPath();
        m_previewType  = PreviewType::Shader;
        Open();
    }

    void PreviewPanel::DrawPreviewArea()
    {
        const ImVec2 padding   = ImVec2(ImGui::GetStyle().ItemSpacing.x + 4, ImGui::GetStyle().ItemSpacing.y + 4);
        const ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        const ImVec2 bgMin     = ImVec2(cursorPos.x + padding.x - 4, cursorPos.y + padding.x);
        const ImVec2 bgMax     = ImVec2(cursorPos.x + m_rightPaneSize.x - padding.x * 2, cursorPos.y + m_rightPaneSize.y - padding.x * 4.0f);
        const ImVec2 bgSize    = ImVec2(bgMax.x - bgMin.x, bgMax.y - bgMin.y);
        const ImVec4 bgCol     = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);
        ImGui::GetWindowDrawList()->AddRectFilled(bgMin, bgMax, ImGui::ColorConvertFloat4ToU32(bgCol), 3.0f);
        const Vector2 bgMinLina = Vector2(bgMin.x, bgMin.y);
        const Vector2 bgMaxLina = Vector2(bgMax.x, bgMax.y);

        auto* renderEngine         = Graphics::RenderEngineBackend::Get();
        float currentAspect        = 0.0f;
        bool  previewCameraEnabled = m_previewType != PreviewType::Texture;

        if (previewCameraEnabled)
        {
            // Handle camera movement & rotation.
            auto& editorCamSystem = EditorApplication::Get()->GetCameraSystem();
            auto& data            = ECS::Registry::Get()->get<ECS::EntityDataComponent>(editorCamSystem.GetEditorCamera());
            WidgetsUtility::SaveEditorCameraBeforeSnapshot(bgSize.x / bgSize.y);

            if (ImGui::IsWindowHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle)))
                ImGui::SetWindowFocus();

            const float delta = (float)Engine::Get()->GetRawDelta();

            if (ImGui::IsWindowFocused())
                editorCamSystem.RotateBehaviour(delta, m_previewCameraRotation, m_mouseAngles);

            data.SetRotation(m_previewCameraRotation);

            if (ImGui::IsWindowFocused())
                editorCamSystem.MoveBehaviour(delta, m_mouseDragStart, m_previewCameraPosition, m_previewCameraRotation);

            data.SetLocation(m_previewCameraPosition);
        }

        // Draw the actual target.
        if (m_previewType == PreviewType::Texture)
            TextureDrawer::DrawTexture(m_targetTexture, bgMinLina, bgMaxLina);
        else if (m_previewType == PreviewType::Model)
            ModelDrawer::DrawModel(m_targetModel, bgMinLina, bgMaxLina);
        else if (m_previewType == PreviewType::Material)
            MaterialDrawer::DrawMaterial(m_targetMaterial, bgMinLina, bgMaxLina);

        if (previewCameraEnabled)
        {
            WidgetsUtility::ResetEditorCamera();

            // Draw tools.
            const ImRect confineSpace = ImRect(bgMin, bgMax);
            WidgetsUtility::TransformOperationTools("##modelpanel_transformops", confineSpace);
        }
    }
} // namespace Lina::Editor