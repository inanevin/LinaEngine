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

#include "Core/Application.hpp"
#include "Drawers/EntityDrawer.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Drawers/ComponentDrawer.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "IconsFontAwesome5.h"

namespace Lina::Editor
{
	void EntityDrawer::Initialize()
	{
		m_componentDrawer.Initialize();
	}
	void EntityDrawer::SetSelectedEntity(Lina::ECS::Entity entity)
	{
		m_shouldCopyEntityName = true;
		m_selectedEntity = entity;

		m_componentDrawer.ClearDrawList();
	}

	void EntityDrawer::DrawSelectedEntity()
	{
		Lina::ECS::Registry* ecs = Lina::ECS::Registry::Get();
		Lina::ECS::EntityDataComponent& data = ecs->get<Lina::ECS::EntityDataComponent>(m_selectedEntity);
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();
		bool isEditorCamera = m_selectedEntity == ecs->GetEntity("Editor Camera");

		if (isEditorCamera)
			ImGui::BeginDisabled();

		// Align.
		ImGui::SetCursorPosX(12); WidgetsUtility::IncrementCursorPosY(16);
		WidgetsUtility::PushScaledFont(0.8f);
		if (WidgetsUtility::IconButton("addcomp", ICON_FA_PLUS_SQUARE, 0.0f, 0.9f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
			AddComponentPopup();

		ImGui::SameLine();
		WidgetsUtility::PopScaledFont();

		// Setup char.
		static char entityName[64] = "";
		if (m_shouldCopyEntityName)
		{
			m_shouldCopyEntityName = false;
			memset(entityName, 0, sizeof entityName);
			std::string str = ecs->get<Lina::ECS::EntityDataComponent>(m_selectedEntity).m_name;
			std::copy(str.begin(), str.end(), entityName);
		}

		// Entity name input text.
		WidgetsUtility::FramePaddingX(5);
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 56);
		ImGui::InputText("##ename", entityName, IM_ARRAYSIZE(entityName));
		ecs->get<Lina::ECS::EntityDataComponent>(m_selectedEntity).m_name = entityName;
		WidgetsUtility::PopStyleVar();

		// Entity enabled toggle button.
		ImGui::SameLine();
		WidgetsUtility::IncrementCursorPosY(1.5f);
		ImVec4 toggleColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
		WidgetsUtility::ToggleButton("##eactive", &data.m_isEnabled, 0.8f, 1.4f, toggleColor, ImVec4(toggleColor.x, toggleColor.y, toggleColor.z, 0.7f));

		// Add Component Popup
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6, 4));
		if (ImGui::BeginPopup("addComponentPopup", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			AddComponentMap map = m_componentDrawer.GetCurrentAddComponentMap(m_selectedEntity);
			static char filterChr[128] = "";

			const char* addCompText = "Add Component";
			WidgetsUtility::CenteredText(addCompText);
			ImGui::SetNextItemWidth(250);
			ImGui::InputText("##filter", filterChr, IM_ARRAYSIZE(filterChr));

			for (auto& category : map)
			{
				std::string filterLowercase = Utility::ToLower(std::string(filterChr));

				if (ImGui::TreeNode(category.first.c_str()))
				{
					for (auto& pair : category.second)
					{
						std::string pairLowercase = Utility::ToLower(pair.first);
						if (pairLowercase.find(std::string(filterLowercase)) != std::string::npos)
						{
							bool selected = false;
							ImGui::Selectable(pair.first.c_str(), &selected);
							if (selected)
							{
								entt::resolve(pair.second).func("add"_hs).invoke({}, m_selectedEntity);
								ImGui::CloseCurrentPopup();
							}
						
						}
					}

					ImGui::TreePop();
				}
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();

		if (isEditorCamera)
			ImGui::EndDisabled();

		// Bevel.
		WidgetsUtility::IncrementCursorPosY(6.0f);
		WidgetsUtility::DrawBeveledLine();
		WidgetsUtility::FramePaddingX(4);
		
		m_componentDrawer.DrawEntityData(m_selectedEntity, &m_transformationFoldoutOpen, &m_physicsFoldoutOpen);

		// Visit each component an entity has and add the component to the draw list if its registered as a drawable component.
		Lina::ECS::Registry::Get()->visit(m_selectedEntity, [this](const auto component)
			{
				m_componentDrawer.PushComponentToDraw(component.hash(), m_selectedEntity);
			});

		m_componentDrawer.DrawAllComponents(m_selectedEntity);

		WidgetsUtility::PopStyleVar();

	}

	void EntityDrawer::AddComponentPopup()
	{
		ImGui::OpenPopup("addComponentPopup");
	}

	void EntityDrawer::DrawEntityData()
	{
	}



}