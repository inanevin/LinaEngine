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
#include "Modals/SelectComponentModal.hpp"
#include "Drawers/ComponentDrawer.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "IconsFontAwesome5.h"

namespace Lina::Editor
{
	void EntityDrawer::Setup()
	{
		m_componentDrawer.Setup();
	}
	void EntityDrawer::SetSelectedEntity(Lina::ECS::Entity entity)
	{
		m_shouldCopyEntityName = true;
		m_selectedEntity = entity;
		m_componentDrawer.ClearDrawList();
	}

	void EntityDrawer::DrawSelectedEntity()
	{
		Lina::ECS::Registry& ecs = Lina::Application::GetECSRegistry();
		Lina::ECS::EntityDataComponent& data = ecs.get<Lina::ECS::EntityDataComponent>(m_selectedEntity);

		// Align.
		ImGui::SetCursorPosX(12); WidgetsUtility::IncrementCursorPosY(16);
		WidgetsUtility::PushScaledFont(0.8f);
		WidgetsUtility::AlignedText(ICON_FA_CUBE);	ImGui::SameLine();
		WidgetsUtility::PopScaledFont();

		// Setup char.
		static char entityName[64] = "";
		if (m_shouldCopyEntityName)
		{
			m_shouldCopyEntityName = false;
			memset(entityName, 0, sizeof entityName);
			std::string str = ecs.get<Lina::ECS::EntityDataComponent>(m_selectedEntity).m_name;
			std::copy(str.begin(), str.end(), entityName);
		}

		// Entity name input text.
		WidgetsUtility::FramePaddingX(5);
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 56);
		ImGui::InputText("##ename", entityName, IM_ARRAYSIZE(entityName));
		ecs.get<Lina::ECS::EntityDataComponent>(m_selectedEntity).m_name = entityName;
		WidgetsUtility::PopStyleVar();

		// Entity enabled toggle button.
		ImGui::SameLine();	
		WidgetsUtility::IncrementCursorPosY(1.5f);
		ImVec4 toggleColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
		WidgetsUtility::ToggleButton("##eactive", &data.m_isEnabled, 0.8f, 1.4f, toggleColor, ImVec4(toggleColor.x, toggleColor.y, toggleColor.z, 0.7f));

		// Add component button.
		ImGui::SetCursorPosX(13);
		WidgetsUtility::IncrementCursorPosY(6);
		ImGui::Text("Add Component "); 
		ImGui::SameLine();
		WidgetsUtility::IncrementCursorPosY(4);

		// add component button.
		if (WidgetsUtility::IconButton("addcomp", ICON_FA_PLUS_SQUARE, 0.0f, 0.9f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
		{
			ImGui::OpenPopup("Select Component");
		}

		// Component selection modal.
		bool o = true;
		WidgetsUtility::FramePaddingY(8);
		WidgetsUtility::FramePaddingX(4);
		ImGui::SetNextWindowSize(ImVec2(280, 400));
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Size.x / 2.0f - 140, ImGui::GetMainViewport()->Size.y / 2.0f - 200));
		if (ImGui::BeginPopupModal("Select Component", &o, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			std::vector<std::string> types = m_componentDrawer.GetEligibleComponents(ecs, m_selectedEntity);
			std::vector<std::string> chosenComponents = SelectComponentModal::Draw(types);

			// Add the selected components to the entity.
			for (int i = 0; i < chosenComponents.size(); i++)
				m_componentDrawer.AddComponentToEntity(ecs, m_selectedEntity, chosenComponents[i]);

			ImGui::EndPopup();
		}
		WidgetsUtility::PopStyleVar(); WidgetsUtility::PopStyleVar();

		// Bevel.
		WidgetsUtility::IncrementCursorPosY(6.0f);
		WidgetsUtility::DrawBeveledLine();
		WidgetsUtility::FramePaddingX(4);

		// Visit each component an entity has and add the component to the draw list if its registered as a drawable component.
		ecs.visit(m_selectedEntity, [this](const auto component)
			{
				m_componentDrawer.AddIDToDrawList(component.hash());
			});

		// Draw the added components.
		m_componentDrawer.DrawComponents(ecs, m_selectedEntity);

		WidgetsUtility::PopStyleVar();

	}

}