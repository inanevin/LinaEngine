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
#include "Widgets/WidgetsUtility.hpp"
#include "Core/EditorCommon.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Core/Application.hpp"
#include "Rendering/RenderSettings.hpp"
#include "World/Level.hpp"
#include "Rendering/Material.hpp"
#include "Modals/SelectMaterialModal.hpp"
#include "imgui/imgui.h"
#include "IconsFontAwesome5.h"

namespace LinaEditor
{
#define CURSORPOS_X_LABELS 12
#define CURSORPOS_XPERC_VALUES 0.30f

    void GlobalSettingsPanel::Setup()
    {
		LinaEngine::Application::GetEngineDispatcher().SubscribeAction<LinaEngine::World::Level*>("#levelsettings_levelinstall", LinaEngine::Action::ActionType::LevelInstalled,
			std::bind(&GlobalSettingsPanel::LevelInstalled, this, std::placeholders::_1));

		LinaEngine::Application::GetEngineDispatcher().SubscribeAction<LinaEngine::World::Level*>("#levelsettings_leveluninstall", LinaEngine::Action::ActionType::LevelUninstalled,
			std::bind(&GlobalSettingsPanel::LevelInstalled, this, std::placeholders::_1));

		m_show = true;

    }

    void GlobalSettingsPanel::Draw()
    {
		if (m_show)
		{
			float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
			float cursorPosLabels = CURSORPOS_X_LABELS;

			/// <summary>
		/// 
		/// </summary>
		/// 
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGui::SetNextWindowSize(ImVec2(700, 900), ImGuiCond_FirstUseEver);
			ImGui::Begin(GLOBALSETTINGS_ID, &m_show, flags);

			// Shadow.
			WidgetsUtility::DrawShadowedLine(5);

			WidgetsUtility::FramePaddingY(0);

			if (m_currentLevel != nullptr)
			{
				LinaEngine::World::LevelData& levelData = m_currentLevel->GetLevelData();

				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::IncrementCursorPosY(11);

				WidgetsUtility::AlignedText("Ambient Color");
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				WidgetsUtility::ColorButton("##lvlAmb", &levelData.m_ambientColor.r);
				LinaEngine::Application::GetRenderEngine().GetLightingSystem()->SetAmbientColor(levelData.m_ambientColor);
				// Material selection
				if (LinaEngine::Graphics::Material::MaterialExists(levelData.m_skyboxMaterialID))
				{
					levelData.m_selectedSkyboxMatID = levelData.m_skyboxMaterialID;
					levelData.m_selectedSkyboxMatPath = levelData.m_skyboxMaterialPath;
				}


				// Material selection.
				char matPathC[128] = "";
				strcpy(matPathC, levelData.m_skyboxMaterialPath.c_str());

				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText("Material");
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 35 - ImGui::GetCursorPosX());
				ImGui::InputText("##selectedMat", matPathC, IM_ARRAYSIZE(matPathC), ImGuiInputTextFlags_ReadOnly);
				ImGui::SameLine();
				WidgetsUtility::IncrementCursorPosY(5);


				// Material drag & drop.
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMATERIAL_ID))
					{
						IM_ASSERT(payload->DataSize == sizeof(uint32));
						levelData.m_skyboxMaterialID = LinaEngine::Graphics::Material::GetMaterial(*(uint32*)payload->m_data).GetID();
						levelData.m_skyboxMaterialPath = LinaEngine::Graphics::Material::GetMaterial(*(uint32*)payload->m_data).GetPath();
						m_currentLevel->SetSkyboxMaterial();
					}

					ImGui::EndDragDropTarget();
				}

				if (WidgetsUtility::IconButton("##selectmat", ICON_FA_MINUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
				{
					levelData.m_skyboxMaterialID = -1;
					levelData.m_skyboxMaterialPath = "";
					m_currentLevel->SetSkyboxMaterial();
				}
			}


			WidgetsUtility::PopStyleVar();
			/// <summary>
			/// 
			/// </summary>


			LinaEngine::Graphics::RenderSettings& renderSettings = LinaEngine::Application::GetRenderEngine().GetRenderSettings();

			// Shadow.
			WidgetsUtility::DrawShadowedLine(5);
			WidgetsUtility::FramePaddingY(0);
			WidgetsUtility::IncrementCursorPosY(12);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Lighting");

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("FXAA");

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Enabled");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##fxaaEnabled", &renderSettings.m_fxaaEnabled);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Reduce Min");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##fxaaReduceMin", &renderSettings.m_fxaaReduceMin, 0.05f);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Reduce Mul");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##fxaaReduceMul", &renderSettings.m_fxaaReduceMul, 0.05f);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Span Max");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##fxaaSpanMax", &renderSettings.m_fxaaSpanMax, 0.05f);

			WidgetsUtility::IncrementCursorPosY(6);

			WidgetsUtility::DrawBeveledLine();
			WidgetsUtility::IncrementCursorPosY(6);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Bloom");

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Enabled");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##bloomEnabled", &renderSettings.m_bloomEnabled);

			WidgetsUtility::IncrementCursorPosY(6);

			WidgetsUtility::DrawBeveledLine();
			WidgetsUtility::IncrementCursorPosY(6);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Post FX General");

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Gamma");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##gamma", &renderSettings.m_gamma, 0.05f);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Exposure");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##exposure", &renderSettings.m_exposure, 0.05f);

			WidgetsUtility::IncrementCursorPosY(6);
			WidgetsUtility::DrawBeveledLine();
			WidgetsUtility::IncrementCursorPosY(6);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Vignette Enabled");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##vigEnabled", &renderSettings.m_vignetteEnabled);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Vignette Amount");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##vigmat", &renderSettings.m_vignetteAmount, 0.05f);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Vignette Pow");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##vigpow", &renderSettings.m_vignettePow, 0.05f);

			WidgetsUtility::IncrementCursorPosY(6);
			WidgetsUtility::DrawBeveledLine();
			WidgetsUtility::IncrementCursorPosY(6);

			LinaEngine::Application::GetRenderEngine().UpdateRenderSettings();
			ImGui::SetCursorPosX(cursorPosLabels);

			if (ImGui::Button("Save Settings"))
				LinaEngine::Graphics::RenderSettings::SerializeRenderSettings(renderSettings, RENDERSETTINGS_FOLDERPATH, RENDERSETTINGS_FILE);

			ImGui::PopStyleVar();
			ImGui::End();


		}
    }

	void GlobalSettingsPanel::LevelInstalled(LinaEngine::World::Level* level)
	{
		m_currentLevel = level;
		m_currentLevel->SetSkyboxMaterial();
	}

	void GlobalSettingsPanel::LevelIUninstalled(LinaEngine::World::Level* level)
	{
		m_currentLevel = nullptr;
	}
}