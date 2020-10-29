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

#include "Panels/LevelSettingsPanel.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/Application.hpp"
#include "Core/EditorCommon.hpp"
#include "World/Level.hpp"
#include "Rendering/Material.hpp"
#include "Modals/SelectMaterialModal.hpp"
#include "imgui/imgui.h"
#include "IconsFontAwesome5.h"

namespace LinaEditor
{
#define CURSORPOS_X_LABELS 30
#define CURSORPOS_XPERC_VALUES 0.52f

	void LevelSettingsPanel::Setup()
	{
		LinaEngine::Application::GetEngineDispatcher().SubscribeAction<LinaEngine::World::Level*>("#levelsettings_levelinstall", LinaEngine::Action::ActionType::LevelInstalled, 
			std::bind(&LevelSettingsPanel::LevelInstalled,this, std::placeholders::_1));

		LinaEngine::Application::GetEngineDispatcher().SubscribeAction<LinaEngine::World::Level*>("#levelsettings_leveluninstall", LinaEngine::Action::ActionType::LevelUninstalled,
			std::bind(&LevelSettingsPanel::LevelInstalled, this, std::placeholders::_1));
	
	}

	void LevelSettingsPanel::Draw()
	{
		if (m_show)
		{
			float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
			float cursorPosLabels = CURSORPOS_X_LABELS;

			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowBgAlpha(1.0f);

			ImGui::Begin(LEVELSETTINGS_ID, &m_show, flags);

			if (m_currentLevel != nullptr)
			{
				LinaEngine::World::LevelData& levelData = m_currentLevel->GetLevelData();

				// Material selection
				if (LinaEngine::Graphics::Material::MaterialExists(levelData.m_skyboxMaterialID))
				{
					levelData.m_selectedSkyboxMatID = levelData.m_skyboxMaterialID;
					levelData.m_selectedSkyboxMatPath = levelData.m_skyboxMaterialPath;
				}

				char matPathC[128] = "";
				strcpy(matPathC, levelData.m_selectedSkyboxMatPath.c_str());

				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText("Material");
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 35 - ImGui::GetCursorPosX());
				ImGui::InputText("##selectedMat", matPathC, IM_ARRAYSIZE(matPathC), ImGuiInputTextFlags_ReadOnly);
				ImGui::SameLine();
				WidgetsUtility::IncrementCursorPosY(5);

				if (WidgetsUtility::IconButton("##selectmat", ICON_FA_PLUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
					ImGui::OpenPopup("Select Material");

				bool materialPopupOpen = true;
				WidgetsUtility::FramePaddingY(8);
				WidgetsUtility::FramePaddingX(4);
				ImGui::SetNextWindowSize(ImVec2(280, 400));
				ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Size.x / 2.0f - 140, ImGui::GetMainViewport()->Size.y / 2.0f - 200));
				if (ImGui::BeginPopupModal("Select Material", &materialPopupOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
				{
					SelectMaterialModal::Draw(LinaEngine::Graphics::Material::GetLoadedMaterials(), &levelData.m_selectedSkyboxMatID, levelData.m_selectedSkyboxMatPath);
					ImGui::EndPopup();
				}
				WidgetsUtility::PopStyleVar(); WidgetsUtility::PopStyleVar();

				levelData.m_skyboxMaterialID = levelData.m_selectedSkyboxMatID;
				levelData.m_skyboxMaterialPath = levelData.m_selectedSkyboxMatPath;
			}

			ImGui::End();

		}
	}
	void LevelSettingsPanel::LevelInstalled(LinaEngine::World::Level* level)
	{
		m_currentLevel = level;
	}

	void LevelSettingsPanel::LevelIUninstalled(LinaEngine::World::Level* level)
	{
		m_currentLevel = nullptr;
	}
}