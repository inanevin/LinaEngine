#include "Drawers/ModelDrawer.hpp"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/RenderEngine.hpp"
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
        const String numMeshes    = TO_STRING(model->GetNumMeshes());
        const String numBones     = TO_STRING(model->GetNumBones());
        const String numMaterials = TO_STRING(model->GetNumMaterials());
        const String numAnims     = TO_STRING(model->GetNumAnims());
        const String numVertices  = TO_STRING(model->GetNumVertices());
        const String numNodes     = TO_STRING(model->GetNumNodes());

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
        if (WidgetsUtility::Button("Save Settings", ImVec2(leftPaneWidth - CURSOR_X_LABELS * 2, 25 * GUILayer::Get()->GetDPIScale())))
        {
            Resources::SaveArchiveToFile(assetData->GetPath(), *assetData);
            const String  path = model->GetPath();
            const StringID sid  = model->GetSID();
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

    void ModelDrawer::DrawModel(Graphics::Model* model, Matrix& matrix, const Vector2& bgMin, const Vector2& bgMax)
    {
#pragma warning(disable : 4312)
        auto*  renderEngine   = Graphics::RenderEngine::Get();
        uint32 previewTexture = renderEngine->RenderModelPreview(model, matrix);
        ImGui::GetWindowDrawList()->AddImage((void*)previewTexture, ImVec2(bgMin.x, bgMin.y), ImVec2(bgMax.x, bgMax.y), ImVec2(0, 1), ImVec2(1, 0));
    }
} // namespace Lina::Editor