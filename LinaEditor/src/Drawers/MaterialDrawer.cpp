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

#include "Drawers/MaterialDrawer.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Model.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Drawers/ClassDrawer.hpp"
#include "Core/EditorCommon.hpp"
#include "ECS/Components/ModelNodeComponent.hpp"
#include "Core/GUILayer.hpp"
#include "Core/ResourceManager.hpp"
#include "Rendering/RenderConstants.hpp"
namespace Lina::Editor
{
    void MaterialDrawer::DrawMaterialSettings(Graphics::Material*& mat, float leftPaneSize)
    {
        const TypeID tid = GetTypeID<Graphics::Material>();
        WidgetsUtility::IncrementCursorPosY(12);

        // Store the current shader.
        const StringIDType sidBefore = mat->GetShaderHandle().m_sid;

        // Draw the free data.
        ClassDrawer::DrawClass(GetTypeID<Graphics::Material>(), entt::forward_as_meta(*mat), false);

        // Set shader if changed.
        const StringIDType sidNow = mat->GetShaderHandle().m_sid;
        if (sidNow != sidBefore)
            mat->SetShader(Resources::ResourceStorage::Get()->GetResource<Graphics::Shader>(sidNow));

        // Check if the material's shader is a skybox shader, if so draw skybox related parameters.
        uint8 shaderSpecification = static_cast<uint8>(mat->m_shaderHandle.m_value->GetSpecification());
        if (shaderSpecification > 0 && shaderSpecification < 4)
        {
            if (shaderSpecification == static_cast<uint8>(Graphics::ShaderSpecification::Sky_HDRICube))
            {
                Graphics::Texture* previousEnvironmentTexture = mat->m_environmentHDR.m_value;
                WidgetsUtility::PropertyLabel("Environment");
                WidgetsUtility::ResourceSelectionTexture("##mat_enviro_hdr", static_cast<void*>(&mat->m_environmentHDR));
                Graphics::Texture* newHDR = mat->m_environmentHDR.m_value;
                if (newHDR != nullptr && previousEnvironmentTexture != newHDR)
                    Graphics::RenderEngineBackend::Get()->CaptureCalculateHDRI(*newHDR);
            }
            else
            {
                WidgetsUtility::PropertyLabel("Indirect Contribution", true, "How much does the skybox contribute to indirect illumination of the scene. Set 0.0f for disabling.");
                WidgetsUtility::DragFloat("##mat_enviro_indirect", nullptr, &mat->m_skyboxIndirectContributionFactor);
            }
        }

        ImGui::SetCursorPosX(CURSOR_X_LABELS);

        if (WidgetsUtility::Button("Open Shader", ImVec2(leftPaneSize - CURSOR_X_LABELS * 2, 25 * GUILayer::Get()->m_globalScale)))
        {
            auto& textEditor = GUILayer::Get()->GetTextEditorPanel();

            Utility::File* file = Utility::FindFile(Resources::ResourceManager::Get()->GetRootFolder(), mat->m_shaderHandle.m_value->GetPath());
            textEditor.AddFile(file);
            textEditor.Open();
        }
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_PopupBg));
        WidgetsUtility::HorizontalDivider(0.0f, 1.0f);
        ImGui::PopStyleColor();
        WidgetsUtility::IncrementCursorPosY(12);

        if (mat->m_bools.size() > 0)
        {
            if (WidgetsUtility::Header(tid, "Bools"))
            {
                for (auto& [name, value] : mat->m_bools)
                {
                    const std::string usedName = name.find("material.") != std::string::npos ? name.substr(name.find(".") + 1) : name;
                    const std::string id       = "##_" + name;
                    WidgetsUtility::PropertyLabel(usedName.c_str());
                    const std::string label = "##_" + name;
                    ImGui::Checkbox(label.c_str(), &value);
                }
            }
        }

        if (mat->m_floats.size() > 0)
        {
            if (WidgetsUtility::Header(tid, "Floats"))
            {
                for (auto& [name, value] : mat->m_floats)
                {
                    const std::string usedName = name.find("material.") != std::string::npos ? name.substr(name.find(".") + 1) : name;
                    const std::string id       = "##_" + name;
                    WidgetsUtility::PropertyLabel(usedName.c_str());
                    WidgetsUtility::DragFloat(id.c_str(), nullptr, &value);
                }
            }
        }

        if (mat->m_ints.size() > 0)
        {
            if (WidgetsUtility::Header(tid, "Ints"))
            {
                for (auto& [name, value] : mat->m_ints)
                {
                    const std::string usedName = name.find("material.") != std::string::npos ? name.substr(name.find(".") + 1) : name;
                    const std::string id       = "##_" + name;
                    WidgetsUtility::PropertyLabel(usedName.c_str());

                    if (name.compare("material.surfaceType") == 0)
                        value = WidgetsUtility::SurfaceTypeComboBox("##material_surfaceType", value);
                    else if (name.compare("material.workflow") == 0)
                        value = WidgetsUtility::WorkflowComboBox("##material_workflow", value);
                    else
                        WidgetsUtility::DragInt(id.c_str(), nullptr, &value);
                }
            }
        }

        if (mat->m_colors.size() > 0)
        {
            if (WidgetsUtility::Header(tid, "Colors"))
            {
                for (auto& [name, value] : mat->m_colors)
                {
                    const std::string usedName = name.find("material.") != std::string::npos ? name.substr(name.find(".") + 1) : name;
                    const std::string id       = "##_" + name;
                    WidgetsUtility::PropertyLabel(usedName.c_str());
                    WidgetsUtility::ColorButton(id.c_str(), &value.r);
                }
            }
        }

        if (mat->m_vector2s.size() > 0)
        {
            if (WidgetsUtility::Header(tid, "Vector2s"))
            {
                for (auto& [name, value] : mat->m_vector2s)
                {
                    const std::string usedName = name.find("material.") != std::string::npos ? name.substr(name.find(".") + 1) : name;
                    const std::string id       = "##_" + name;
                    WidgetsUtility::PropertyLabel(usedName.c_str());
                    WidgetsUtility::DragVector2(id.c_str(), &value.x);
                }
            }
        }

        if (mat->m_vector3s.size() > 0)
        {
            if (WidgetsUtility::Header(tid, "Vector3s"))
            {
                for (auto& [name, value] : mat->m_vector3s)
                {
                    const std::string usedName = name.find("material.") != std::string::npos ? name.substr(name.find(".") + 1) : name;
                    const std::string id       = "##_" + name;
                    WidgetsUtility::PropertyLabel(usedName.c_str());
                    WidgetsUtility::DragVector3(id.c_str(), &value.x);
                }
            }
        }

        if (mat->m_vector4s.size() > 0)
        {
            if (WidgetsUtility::Header(tid, "Vector4s"))
            {
                for (auto& [name, value] : mat->m_vector4s)
                {
                    const std::string usedName = name.find("material.") != std::string::npos ? name.substr(name.find(".") + 1) : name;
                    const std::string id       = "##_" + name;
                    WidgetsUtility::PropertyLabel(usedName.c_str());
                    WidgetsUtility::DragVector4(id.c_str(), &value.x);
                }
            }
        }

        if (mat->m_sampler2Ds.size() > 0)
        {
            if (WidgetsUtility::Header(tid, "Textures"))
            {
                for (auto& [name, value] : mat->m_sampler2Ds)
                {
                    if (name.compare("material.brdfLUTMap") == 0 || name.compare("material.irradianceMap") == 0 ||
                        name.compare("material.prefilterMap") == 0 || name.find("material.pointShadowDepth") != std::string::npos || name.compare("material.environmentMap") == 0)
                        continue;
                    const std::string usedName = name.find("material.") != std::string::npos ? name.substr(name.find(".") + 1) : name;
                    const std::string id       = "##_" + usedName;
                    WidgetsUtility::PropertyLabel(usedName.c_str());

                    const StringIDType sidBefore = value.m_texture.m_sid;
                    WidgetsUtility::ResourceSelectionTexture(id, &value.m_texture);

                    if (value.m_texture.m_sid != sidBefore)
                    {
                        if (value.m_texture.m_value == nullptr)
                            mat->RemoveTexture(name);
                        else
                            mat->SetTexture(name, value.m_texture.m_value, value.m_bindMode);
                    }
                }
            }
        }

        WidgetsUtility::IncrementCursorPosY(6);
        ImGui::SetCursorPosX(CURSOR_X_LABELS);
        if (WidgetsUtility::Button("Save Settings", ImVec2(leftPaneSize - CURSOR_X_LABELS * 2, 25 * GUILayer::Get()->m_globalScale)))
        {
            bool isSkyboxMaterial = Graphics::RenderEngineBackend::Get()->GetSkyboxMaterial() == mat;
            mat->Save();

            // Make sure the skybox material is nulled out on render engine until we are done with reloading.
            if (isSkyboxMaterial)
                Graphics::RenderEngineBackend::Get()->SetSkyboxMaterial(nullptr);

            const std::string  path = mat->GetPath();
            const StringIDType sid  = mat->GetSID();
            Resources::ResourceStorage::Get()->Unload<Graphics::Material>(sid);
            Event::EventSystem::Get()->Trigger<Event::ERequestResourceReload>(Event::ERequestResourceReload{path, tid, sid});
            mat = Resources::ResourceStorage::Get()->GetResource<Graphics::Material>(sid);

            if (isSkyboxMaterial)
            {
                Graphics::RenderEngineBackend::Get()->SetSkyboxMaterial(mat);
                if (mat->m_shaderHandle.m_value->GetSpecification() == Graphics::ShaderSpecification::Sky_HDRICube && mat->m_environmentHDR.m_value != nullptr)
                {
                    mat->SetTexture(MAT_MAP_ENVIRONMENT, &Graphics::RenderEngineBackend::Get()->GetHDRICubemap(), Graphics::TextureBindMode::BINDTEXTURE_CUBEMAP);
                }
            }
        }
    }

    void MaterialDrawer::DrawMaterial(Graphics::Material* mat, Graphics::Model* previewModel, const Vector2& bgMin, const Vector2& bgMax)
    {
#pragma warning(disable : 4312)
        auto*  renderEngine   = Graphics::RenderEngineBackend::Get();
        uint32 previewTexture = renderEngine->RenderModelPreview(previewModel, Matrix::Identity(), nullptr, mat);
        ImGui::GetWindowDrawList()->AddImage((void*)previewTexture, ImVec2(bgMin.x, bgMin.y), ImVec2(bgMax.x, bgMax.y), ImVec2(0, 1), ImVec2(1, 0));
    }

} // namespace Lina::Editor