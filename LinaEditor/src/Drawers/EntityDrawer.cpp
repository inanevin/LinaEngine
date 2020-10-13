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
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/RigidbodyComponent.hpp"

using namespace LinaEngine::ECS;

namespace LinaEditor
{


	void EntityDrawer::DrawEntity(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity, bool* copySelectedEntityName)
	{
		static int componentsComboCurrentItem = 0;
		// Shadow.
		WidgetsUtility::DrawShadowedLine(5);

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
			std::string str = ecs->GetEntityName(entity);
			std::copy(str.begin(), str.end(), entityName);
		}

		// Entity name input text.
		WidgetsUtility::FramePaddingX(5);
		WidgetsUtility::IncrementCursorPosY(-5);  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 56);
		ImGui::InputText("##ename", entityName, IM_ARRAYSIZE(entityName));
		ecs->SetEntityName(entity, entityName);
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

		if (WidgetsUtility::IconButton("addcomp", ICON_FA_PLUS_SQUARE, 0.0f, 0.9f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
		{
			ImGui::OpenPopup("Select Component");
		}

		bool o = true;
		WidgetsUtility::FramePaddingY(8);
		WidgetsUtility::FramePaddingX(4);
		ImGui::SetNextWindowSize(ImVec2(280, 400));
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Size.x / 2.0f - 140, ImGui::GetMainViewport()->Size.y / 2.0f - 200));
		if (ImGui::BeginPopupModal("Select Component", &o, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			std::vector<std::string> types = GetEligibleComponents(ecs, entity);
			std::vector<std::string> chosenComponents = SelectComponentModal::Draw(types);

			// Add the selected components to the entity.
			for (int i = 0; i < chosenComponents.size(); i++)
				AddComponentToEntity(ecs, entity, chosenComponents[i]);

;			ImGui::EndPopup();
		}
		WidgetsUtility::PopStyleVar();
		WidgetsUtility::PopStyleVar();

		// Bevel.
		WidgetsUtility::IncrementCursorPosY(6.0f);
		WidgetsUtility::DrawBeveledLine();


		ImGui::SetCursorPosX(12); WidgetsUtility::IncrementCursorPosY(15);
		static bool open = false;
		WidgetsUtility::DrawComponentTitle("Transformation", ICON_FA_ARROWS_ALT, &open, ImGui::GetStyleColorVec4(ImGuiCol_Header));

		if (open)
		{

		}

		WidgetsUtility::DrawBeveledLine();
		WidgetsUtility::IncrementCursorPosY(15);
		WidgetsUtility::CenterCursorX();

		//if (ImGui::Button(ICON_FA_PLUS, ImVec2(WidgetsUtility::DebugFloat(), WidgetsUtility::DebugFloat())))
		//{
		//
		//}

		/*ImGui::BeginChild("Component View", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			// Component view tab
			bool isValid = m_ecs->valid(m_selectedEntity);
			std::string selectedName = m_ecs->valid(m_selectedEntity) ? "Component View" : ("Component View: " + m_ecs->GetEntityName(m_selectedEntity));
			char titleName[256];
			strcpy(titleName, selectedName.c_str());
			if (ImGui::BeginTabItem(titleName))
			{
				if (!isValid)
					ImGui::TextWrapped("No entity is selected.");
				else
					DrawComponents(m_selectedEntity);
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();

		if (ImGui::Button("Add "))
			AddComponentToEntity(componentsComboCurrentItem);

		ImGui::SameLine();

		// Combo box for components.
		static ImGuiComboFlags flags = 0;
		const char* combo_label = entityComponents[componentsComboCurrentItem];  // Label to preview before opening the combo (technically could be anything)(
		if (ImGui::BeginCombo("ComponentsCombo", combo_label, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(entityComponents); n++)
			{
				const bool is_selected = (componentsComboCurrentItem == n);
				if (ImGui::Selectable(entityComponents[n], is_selected))
					componentsComboCurrentItem = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}*/

	}

	// Use reflection for gods sake later on.
	std::vector<std::string> EntityDrawer::GetEligibleComponents(LinaEngine::ECS::ECSRegistry* reg, LinaEngine::ECS::ECSEntity entity)
	{
		std::vector<std::string> eligibleTypes;

		if (!reg->has<TransformComponent>(entity))
			eligibleTypes.push_back("Transformation");
		if (!reg->has<MeshRendererComponent>(entity))
			eligibleTypes.push_back("MeshRenderer");
		if (!reg->has<CameraComponent>(entity))
			eligibleTypes.push_back("Camera");
		if (!reg->has<FreeLookComponent>(entity))
			eligibleTypes.push_back("FreeLook");
		if (!reg->has<PointLightComponent>(entity))
			eligibleTypes.push_back("PointLight");
		if (!reg->has<SpotLightComponent>(entity))
			eligibleTypes.push_back("SpotLight");
		if (!reg->has<DirectionalLightComponent>(entity))
			eligibleTypes.push_back("DirectionalLight");
		if (!reg->has<RigidbodyComponent>(entity))
			eligibleTypes.push_back("Rigidbody");

		return eligibleTypes;
	}

	void EntityDrawer::AddComponentToEntity(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity, const std::string& comp)
	{

		LINA_CLIENT_TRACE(comp);
		if (comp.compare("Transformation") == 0)
			ecs->emplace<TransformComponent>(entity, TransformComponent());
		else if (comp.compare("MeshRenderer") == 0)
			ecs->emplace<MeshRendererComponent>(entity, MeshRendererComponent());
		else if (comp.compare("Camera") == 0)
			ecs->emplace<CameraComponent>(entity, CameraComponent());
		else if (comp.compare("FreeLook") == 0)
			ecs->emplace<FreeLookComponent>(entity, FreeLookComponent());
		else if (comp.compare("PointLight") == 0)
			ecs->emplace<PointLightComponent>(entity, PointLightComponent());
		else if (comp.compare("SpotLight") == 0)
			ecs->emplace<SpotLightComponent>(entity, SpotLightComponent());
		else if (comp.compare("DirectionalLight") == 0)
			ecs->emplace<DirectionalLightComponent>(entity, DirectionalLightComponent());
		else if (comp.compare("Rigidbody") == 0)
			ecs->emplace<RigidbodyComponent>(entity,RigidbodyComponent());

	}
}