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

#include "Panels/GlobalSettingsPanel.hpp"

#include "Core/Application.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "IconsFontAwesome5.h"
#include "Rendering/Material.hpp"
#include "Rendering/RenderSettings.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "World/Level.hpp"
#include "Drawers/ClassDrawer.hpp"
#include "imgui/imgui.h"

namespace Lina::Editor
{
#define CURSORPOS_X_LABELS     12
#define CURSORPOS_XPERC_VALUES 0.30f

    void GlobalSettingsPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
        m_show = true;
    }

    void GlobalSettingsPanel::Draw()
    {
        if (m_show)
        {
            if (Begin())
            {

                //World::LevelData& levelData = m_currentLevel->GetLevelData();
                //auto*             storage   = Resources::ResourceStorage::Get();

                auto*              currentLevel = World::Level::GetCurrent();
                const StringIDType sidBefore    = currentLevel->m_skyboxMaterial.m_sid;
                if (ClassDrawer::DrawClass(GetTypeID<World::Level>(), entt::forward_as_meta(*World::Level::GetCurrent()), true))
                {
                    Graphics::RenderEngineBackend::Get()->SetSkyboxMaterial(currentLevel->m_skyboxMaterial.m_value);
                    Graphics::RenderEngineBackend::Get()->GetLightingSystem()->SetAmbientColor(currentLevel->m_ambientColor);
                }

                if (ClassDrawer::DrawClass(GetTypeID<Graphics::RenderSettings>(), entt::forward_as_meta(Engine::Get()->GetEngineSettings().GetRenderSettings()), true))
                {

                }

                //if (WidgetsUtility::Header("Level Settings", &levelSettingsOpen))
                // {
                // WidgetsUtility::PropertyLabel("Ambient Color");
                // WidgetsUtility::ColorButton("##lvlAmb", &levelData.m_ambientColor.r);
                // Graphics::RenderEngineBackend::Get()->GetLightingSystem()->SetAmbientColor(levelData.m_ambientColor);
                //
                // WidgetsUtility::PropertyLabel("Skybox Material");
                //
                // WidgetsUtility::ResourceSelectionMaterial(&levelData.m_skyboxMaterial);

                // Material selection
                //  if (Graphics::Material::MaterialExists(levelData.m_skyboxMaterialID))
                //  {
                //      levelData.m_selectedSkyboxMatID   = levelData.m_skyboxMaterialID;
                //      levelData.m_selectedSkyboxMatPath = levelData.m_skyboxMaterialPath;
                //  }
                //
                //  // Material selection.
                //  char matPathC[128] = "";
                //  strcpy_s(matPathC, levelData.m_skyboxMaterialPath.c_str());
                //
                //  WidgetsUtility::PropertyLabel("Material");
                //  bool                removed = false;
                //  Graphics::Material* mat     = WidgetsUtility::MaterialComboBox("##globalSettings_levelMat", levelData.m_selectedSkyboxMatPath, &removed);
                //
                //  if (removed)
                //  {
                //      levelData.m_skyboxMaterialID   = -1;
                //      levelData.m_skyboxMaterialPath = "";
                //      m_currentLevel->SetSkyboxMaterial();
                //  }
                //
                //  if (mat != nullptr)
                //  {
                //      levelData.m_skyboxMaterialID   = mat->GetID();
                //      levelData.m_skyboxMaterialPath = mat->GetPath();
                //  }
                //
                //  // Material drag & drop.
                //  if (ImGui::BeginDragDropTarget())
                //  {
                //      if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMATERIAL_ID))
                //      {
                //          IM_ASSERT(payload->DataSize == sizeof(uint32));
                //          levelData.m_skyboxMaterialID   = Graphics::Material::GetMaterial(*(uint32*)payload->Data).GetID();
                //          levelData.m_skyboxMaterialPath = Graphics::Material::GetMaterial(*(uint32*)payload->Data).GetPath();
                //          m_currentLevel->SetSkyboxMaterial();
                //      }
                //
                //      ImGui::EndDragDropTarget();
                //  }
                //
                //  if (Graphics::Material::MaterialExists(levelData.m_skyboxMaterialID))
                //  {
                //      auto& mat = Graphics::Material::GetMaterial(levelData.m_skyboxMaterialID);
                //      // TODO: HDRI SHADER SELECTION
                //  }
                // }

                //  Graphics::RenderSettings& renderSettings = Graphics::RenderEngineBackend::Get()->GetRenderSettings();

                // if (WidgetsUtility::Header("Post Process"))
                // {
                //     static bool toneSettingsOpen     = false;
                //     static bool fxaaSettingsOpen     = false;
                //     static bool bloomSettingsOpen    = false;
                //     static bool vignetteSettingsOpen = false;

                //  if (WidgetsUtility::CaretTitle("Tonemapping", &toneSettingsOpen))
                //  {
                //      WidgetsUtility::PropertyLabel("Gamma");
                //      ImGui::DragFloat("##gamma", &renderSettings.m_gamma, 0.05f);
                //
                //      WidgetsUtility::PropertyLabel("Exposure");
                //      ImGui::DragFloat("##exposure", &renderSettings.m_exposure, 0.05f);
                //  }
                //
                //  if (WidgetsUtility::CaretTitle("FXAA", &fxaaSettingsOpen))
                //  {
                //      WidgetsUtility::PropertyLabel("Enabled");
                //      ImGui::Checkbox("##fxaaEnabled", &renderSettings.m_fxaaEnabled);
                //
                //      WidgetsUtility::PropertyLabel("Reduce Min");
                //      ImGui::DragFloat("##fxaaReduceMin", &renderSettings.m_fxaaReduceMin, 0.05f);
                //
                //      WidgetsUtility::PropertyLabel("Reduce Mul");
                //      ImGui::DragFloat("##fxaaReduceMul", &renderSettings.m_fxaaReduceMul, 0.05f);
                //
                //      WidgetsUtility::PropertyLabel("Span Max");
                //      ImGui::DragFloat("##fxaaSpanMax", &renderSettings.m_fxaaSpanMax, 0.05f);
                //  }
                //
                //  if (WidgetsUtility::CaretTitle("Bloom", &bloomSettingsOpen))
                //  {
                //      WidgetsUtility::PropertyLabel("Enabled");
                //      ImGui::Checkbox("##bloomEnabled", &renderSettings.m_bloomEnabled);
                //  }
                //
                //  if (WidgetsUtility::CaretTitle("Vignette", &vignetteSettingsOpen))
                //  {
                //      WidgetsUtility::PropertyLabel("Enabled");
                //      ImGui::Checkbox("##vigEnabled", &renderSettings.m_vignetteEnabled);
                //
                //      WidgetsUtility::PropertyLabel("Amount");
                //      ImGui::DragFloat("##vigmat", &renderSettings.m_vignetteAmount, 0.05f);
                //
                //      WidgetsUtility::PropertyLabel("Pow");
                //      ImGui::DragFloat("##vigpow", &renderSettings.m_vignettePow, 0.05f);
                //  }
                //  }

                Graphics::RenderEngineBackend::Get()->UpdateRenderSettings();
                const ImVec2 buttonSize = ImVec2(90, 30);
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() - VALUE_OFFSET_FROM_WINDOW - buttonSize.x);

                //if (WidgetsUtility::Button("Save Settings", buttonSize))
                //  Graphics::RenderSettings::SerializeRenderSettings(renderSettings, RENDERSETTINGS_FOLDERPATH, RENDERSETTINGS_FILE);

                End();
            }
        }
    }

} // namespace Lina::Editor
