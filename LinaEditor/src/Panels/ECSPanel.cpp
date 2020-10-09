/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ECSPanel
Timestamp: 5/23/2020 4:15:24 PM

*/


#include "Panels/ECSPanel.hpp"
#include "Panels/ScenePanel.hpp"
#include "Panels/PropertiesPanel.hpp"
#include "Core/GUILayer.hpp"
#include "Utility/Log.hpp"
#include "Utility/EditorUtility.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include <stdio.h>



namespace LinaEditor
{
	using namespace LinaEngine::ECS;
	using namespace LinaEngine;
	
	void ECSPanel::Setup()
	{
		// Store references.
		m_ECS = m_guiLayer->GetECS();
		m_ScenePanel = m_guiLayer->GetScenePanel();
		m_PropertiesPanel = m_guiLayer->GetPropertiesPanel();
		
		// Refresh entity list.
		Refresh();
	}

	void ECSPanel::Refresh()
	{
		m_EntityList.clear();
		m_SelectedEntity = entt::null;
		m_ScenePanel->SetSelectedTransform(nullptr);
		m_PropertiesPanel->EntitySelected(entt::null);

		// add scene entitites to the list.
		m_ECS->each([this](auto entity)
			{
				m_EntityList.push_back(entity);
			});
	}

	void ECSPanel::Draw(float frameTime)
	{
		if (m_show)
		{
			
			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImVec2 panelSize = ImVec2(m_Size.x, m_Size.y);			
			ImGui::SetNextWindowSize(panelSize, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;

			if (ImGui::Begin("Entities", NULL, flags))
			{
				// Statics.
				static char selectedEntityName[256] = "Entity";

				// Handle Right Click.
				if (ImGui::BeginPopupContextWindow())
				{
					if (ImGui::BeginMenu("Create"))
					{
						if (ImGui::MenuItem("Entity"))
							m_EntityList.push_back(m_ECS->CreateEntity("Entity"));

						ImGui::EndMenu();
					}
					ImGui::EndPopup();
				}

				int entityCounter = 0;

				for (std::vector<ECSEntity>::iterator it = m_EntityList.begin(); it != m_EntityList.end(); ++it)
				{
					// Selection
					entityCounter++;
					ECSEntity& entity = *it;
					strcpy(selectedEntityName, m_ECS->GetEntityName(entity).c_str());
					if (EditorUtility::SelectableInput("entSelectable" + entityCounter, m_SelectedEntity == entity, ImGuiSelectableFlags_SelectOnClick, selectedEntityName, IM_ARRAYSIZE(selectedEntityName)))
					{
						m_SelectedEntity = entity;
						m_ScenePanel->SetSelectedTransform(m_ECS->has<TransformComponent>(m_SelectedEntity) ? &m_ECS->get<TransformComponent>(m_SelectedEntity) : nullptr);
						m_PropertiesPanel->EntitySelected(m_SelectedEntity);
						m_ECS->SetEntityName(entity, selectedEntityName);
					}

					// Deselect.
					if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						m_SelectedEntity = entt::null;
						m_PropertiesPanel->EntitySelected(m_SelectedEntity);
						m_ScenePanel->SetSelectedTransform(nullptr);
					}
				}

			}
			ImGui::End();
		}
	}



}