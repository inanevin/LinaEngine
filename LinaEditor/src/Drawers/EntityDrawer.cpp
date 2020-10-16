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

Class: EntityDrawer
Timestamp: 10/12/2020 1:02:39 AM

*/


#include "Drawers/EntityDrawer.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "IconsFontAwesome5.h"
#include "Modals/SelectComponentModal.hpp"
#include "Drawers/ComponentDrawer.hpp"


namespace LinaEditor
{

	void EntityDrawer::DrawEntity(LinaEngine::ECS::ECSEntity entity, bool* copySelectedEntityName)
	{
		// Align.
		ImGui::SetCursorPosX(12); WidgetsUtility::IncrementCursorPosY(16);
		WidgetsUtility::PushScaledFont(0.8f);
		WidgetsUtility::AlignedText(ICON_FA_CUBE);	ImGui::SameLine();
		WidgetsUtility::PopScaledFont();

		// Setup char.
		static char entityName[64] = "";
		if (*copySelectedEntityName)
		{
			*copySelectedEntityName = false;
			memset(entityName, 0, sizeof entityName);
			std::string str = m_ecs->GetEntityName(entity);
			std::copy(str.begin(), str.end(), entityName);
		}

		// Entity name input text.
		WidgetsUtility::FramePaddingX(5);
		WidgetsUtility::IncrementCursorPosY(-5);  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 56);
		ImGui::InputText("##ename", entityName, IM_ARRAYSIZE(entityName));
		m_ecs->SetEntityName(entity, entityName);
		WidgetsUtility::PopStyleVar();

		// Entity enabled toggle button.
		ImGui::SameLine();	WidgetsUtility::IncrementCursorPosY(1.5f);
		static bool b = false;	ImVec4 toggleColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
		WidgetsUtility::ToggleButton("##eactive", &b, 0.8f, 1.4f, toggleColor, ImVec4(toggleColor.x, toggleColor.y, toggleColor.z, 0.7f));

		// Add component button.
		ImGui::SetCursorPosX(13);
		WidgetsUtility::IncrementCursorPosY(6);
		ImGui::Text("Add "); ImGui::SameLine();
		WidgetsUtility::IncrementCursorPosY(4);

		// add component button.
		if (WidgetsUtility::IconButton("addcomp", ICON_FA_PLUS_SQUARE, 0.0f, 0.9f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
		{
			ImGui::OpenPopup("Select Component");
		}

		// Component selection modal.
		bool o = true;
		WidgetsUtility::FramePaddingY(8); WidgetsUtility::FramePaddingX(4);
		ImGui::SetNextWindowSize(ImVec2(280, 400));
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Size.x / 2.0f - 140, ImGui::GetMainViewport()->Size.y / 2.0f - 200));
		if (ImGui::BeginPopupModal("Select Component", &o, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			std::vector<std::string> types = ComponentDrawer::GetEligibleComponents(m_ecs, entity);
			std::vector<std::string> chosenComponents = SelectComponentModal::Draw(types);

			// Add the selected components to the entity.
			for (int i = 0; i < chosenComponents.size(); i++)
				ComponentDrawer::AddComponentToEntity(m_ecs, entity, chosenComponents[i]);
			
			ImGui::EndPopup();
		}
		WidgetsUtility::PopStyleVar(); WidgetsUtility::PopStyleVar();

		// Bevel.
		WidgetsUtility::IncrementCursorPosY(6.0f);
		WidgetsUtility::DrawBeveledLine();
		WidgetsUtility::FramePaddingX(4);

		// Visit each component an entity has and add the component to the draw list if its registered as a drawable component.
		m_ecs->visit(entity, [entity](const auto component)
		{
				if (ComponentDrawer::s_componentDrawFuncMap.find(component) != ComponentDrawer::s_componentDrawFuncMap.end())
					ComponentDrawer::AddIDToDrawList(component);
		});

		// Draw the added components.
		ComponentDrawer::DrawComponents(m_ecs, entity);

		WidgetsUtility::PopStyleVar();

	}

}