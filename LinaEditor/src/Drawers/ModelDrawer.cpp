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

#include "Drawers/ModelDrawer.hpp"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Rendering/Model.hpp"
#include "Core/Engine.hpp"
#include "Drawers/ClassDrawer.hpp"

namespace Lina::Editor
{
    void ModelDrawer::DrawModelSettings(Graphics::Model*& model, float leftPaneWidth)
    {
        auto* assetData = model->GetAssetData();
        WidgetsUtility::IncrementCursorPosY(ImGui::GetStyle().ItemSpacing.y + 9.5f);
        const std::string numMeshes    = std::to_string(model->GetNumMeshes());
        const std::string numBones     = std::to_string(model->GetNumBones());
        const std::string numMaterials = std::to_string(model->GetNumMaterials());
        const std::string numAnims     = std::to_string(model->GetNumAnims());
        const std::string numVertices  = std::to_string(model->GetNumVertices());
        const std::string numNodes     = std::to_string(model->GetNumNodes());

        WidgetsUtility::PropertyLabel("Num Meshes");
        ImGui::Text(numMeshes.c_str());

        WidgetsUtility::PropertyLabel("Num Bones");
        ImGui::Text(numBones.c_str());

        WidgetsUtility::PropertyLabel("Num Materials");
        ImGui::Text(numMaterials.c_str());

        WidgetsUtility::PropertyLabel("Num Anims");
        ImGui::Text(numAnims.c_str());

        WidgetsUtility::PropertyLabel("Num Vertices");
        ImGui::Text(numVertices.c_str());

        WidgetsUtility::PropertyLabel("Num Nodes");
        ImGui::Text(numNodes.c_str());

        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_PopupBg));
        WidgetsUtility::HorizontalDivider(0.0f, 1.0f);
        ImGui::PopStyleColor();
        WidgetsUtility::IncrementCursorPosY(12);

        ClassDrawer::DrawClass(GetTypeID<Graphics::ModelAssetData>(), entt::forward_as_meta(*assetData), false);

        WidgetsUtility::IncrementCursorPosY(6);
        ImGui::SetCursorPosX(CURSOR_X_LABELS);
        if (WidgetsUtility::Button("Save Settings", ImVec2(leftPaneWidth - CURSOR_X_LABELS * 2, 25)))
        {
            Resources::SaveArchiveToFile(assetData->GetPath(), *assetData);
            const std::string  path = model->GetPath();
            const StringIDType sid  = model->GetSID();
            const TypeID       tid  = GetTypeID<Graphics::Model>();
            Resources::ResourceStorage::Get()->Unload<Graphics::Model>(sid);
            Event::EventSystem::Get()->Trigger<Event::ERequestResourceReload>(Event::ERequestResourceReload{path, tid, sid});
            model = Resources::ResourceStorage::Get()->GetResource<Graphics::Model>(sid);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_PopupBg));
        WidgetsUtility::HorizontalDivider(0.0f, 0.75f);
        ImGui::PopStyleColor();
        WidgetsUtility::IncrementCursorPosY(12);

        if (WidgetsUtility::CaretTitle("Materials", "##model_drawer_materials"))
        {
            auto& mats = model->GetImportedMaterials();
            for (auto& mat : mats)
            {
                WidgetsUtility::PropertyLabel(mat.m_name.c_str(), false);
            }
        }

        DisplayNode(model->GetRootNode());
    }

    void ModelDrawer::DisplayNode(Graphics::ModelNode* node)
    {
        const char* nodeName = node->GetName().c_str();
        auto&       children = node->GetChildren();

        if (children.size() > 0)
        {
            if (WidgetsUtility::CaretTitle(nodeName, nodeName))
            {
                for (auto* child : children)
                    DisplayNode(child);
            }
        }
        else
        {
            WidgetsUtility::PropertyLabel(nodeName, false);
        }
    }

    void ModelDrawer::DrawModel(Graphics::Model* model, const Vector2& bgMin, const Vector2& bgMax)
    {
#pragma warning(disable : 4312)
        auto*  renderEngine   = Graphics::RenderEngineBackend::Get();
        uint32 previewTexture = renderEngine->RenderModelPreview(model);
        ImGui::GetWindowDrawList()->AddImage((void*)previewTexture, ImVec2(bgMin.x, bgMin.y), ImVec2(bgMax.x, bgMax.y), ImVec2(0, 1), ImVec2(1, 0));
    }
} // namespace Lina::Editor