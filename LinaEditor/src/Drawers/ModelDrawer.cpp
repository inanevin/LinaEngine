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

namespace Lina::Editor
{
    void ModelDrawer::DrawModelSettings(Graphics::Model*& model, float leftPaneWidth)
    {
        auto* assetData = model->GetAssetData();
        WidgetsUtility::IncrementCursorPosY(ImGui::GetStyle().ItemSpacing.y + 9.5f);

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
    }

    void ModelDrawer::DrawModel(Graphics::Model* model, const Vector2& bgMin, const Vector2& bgMax)
    {
#pragma warning(disable : 4312)
        auto*         renderEngine = Graphics::RenderEngineBackend::Get();
        uint32 previewTexture = renderEngine->RenderModelPreview(model);
        ImGui::GetWindowDrawList()->AddImage((void*)previewTexture, ImVec2(bgMin.x, bgMin.y), ImVec2(bgMax.x, bgMax.y), ImVec2(0, 1), ImVec2(1, 0));
    }
} // namespace Lina::Editor