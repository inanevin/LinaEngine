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
#include "Widgets/WidgetsUtility.hpp"
#include <stdio.h>



namespace LinaEditor
{
	using namespace LinaEngine::ECS;
	using namespace LinaEngine;
	
	void ECSPanel::Setup()
	{
		// Store references.
		m_ecs = m_guiLayer->GetECS();
		m_scenePanel = m_guiLayer->GetScenePanel();
		m_propertiesPanel = m_guiLayer->GetPropertiesPanel();
		
		// Refresh entity list.
		Refresh();
	}

	void ECSPanel::Refresh()
	{
		m_entityList.clear();
		m_selectedEntity = entt::null;
		m_scenePanel->SetSelectedTransform(nullptr);
		m_propertiesPanel->EntitySelected(entt::null);

		// add scene entitites to the list.
		m_ecs->each([this](auto entity)
			{
				m_entityList.push_back(entity);
			});
	}

	void ECSPanel::Draw(float frameTime)
	{
		if (m_show)
		{
			
			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImVec2 panelSize = ImVec2(m_size.x, m_size.y);			
			ImGui::SetNextWindowSize(panelSize, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;;

			if (ImGui::Begin("Entities", &m_show, flags))
			{
				WidgetsUtility::DrawShadowedLine(5);

				// Statics.
				static char selectedEntityName[256] = "Entity";

				// Handle Right Click.
				if (ImGui::BeginPopupContextWindow())
				{
					if (ImGui::BeginMenu("Create"))
					{
						if (ImGui::MenuItem("Entity"))
							m_entityList.push_back(m_ecs->CreateEntity("Entity"));

						ImGui::EndMenu();
					}
					ImGui::EndPopup();
				}

				int entityCounter = 0;

				for (std::vector<ECSEntity>::iterator it = m_entityList.begin(); it != m_entityList.end(); ++it)
				{
					// Selection
					entityCounter++;
					ECSEntity& entity = *it;
					strcpy(selectedEntityName, m_ecs->GetEntityName(entity).c_str());
					if (WidgetsUtility::SelectableInput("entSelectable" + entityCounter, m_selectedEntity == entity, ImGuiSelectableFlags_SelectOnClick, selectedEntityName, IM_ARRAYSIZE(selectedEntityName)))
					{
						m_selectedEntity = entity;
						m_scenePanel->SetSelectedTransform(m_ecs->has<TransformComponent>(m_selectedEntity) ? &m_ecs->get<TransformComponent>(m_selectedEntity) : nullptr);
						m_propertiesPanel->EntitySelected(m_selectedEntity);
						m_ecs->SetEntityName(entity, selectedEntityName);
					}

					// Deselect.
					if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						m_selectedEntity = entt::null;
						m_propertiesPanel->EntitySelected(m_selectedEntity);
						m_scenePanel->SetSelectedTransform(nullptr);
					}
				}

			}

			ImGui::End();
		}
	}



}