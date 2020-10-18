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

#include "Panels/ECSPanel.hpp"
#include "Panels/ScenePanel.hpp"
#include "Core/GUILayer.hpp"
#include "Utility/Log.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/EditorApplication.hpp"

namespace LinaEditor
{
	using namespace LinaEngine::ECS;
	using namespace LinaEngine;
	
	void ECSPanel::Setup()
	{
		// Store references.
		m_ecs = m_guiLayer->GetECS();
		
		// Refresh entity list.
		Refresh();
	}

	void ECSPanel::Refresh()
	{
		m_entityList.clear();
		m_selectedEntity = entt::null;
		EditorApplication::GetEditorDispatcher().DispatchAction<void*>(LinaEngine::Action::ActionType::Unselect, 0);

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
						EditorApplication::GetEditorDispatcher().DispatchAction<ECSEntity>(LinaEngine::Action::ActionType::EntitySelected, entity);
						m_ecs->SetEntityName(entity, selectedEntityName);
					}

					// Deselect.
					if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						EditorApplication::GetEditorDispatcher().DispatchAction<void*>(LinaEngine::Action::ActionType::Unselect, 0);
						m_selectedEntity = entt::null;
					}
				}

			}

			ImGui::End();
		}
	}



}