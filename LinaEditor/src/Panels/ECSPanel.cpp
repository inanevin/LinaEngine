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
#include "Utility/Log.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "Rendering/Window.hpp"
#include "imgui.h"
#include "ImGuiFileBrowser.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include "imgui_internal.h"
#include "entt/meta/meta.hpp"
#include "entt/meta/factory.hpp"
#include "entt/meta/resolve.hpp"


namespace ImGui
{
	bool SelectableInput(const char* str_id, bool selected, ImGuiSelectableFlags flags, char* buf, size_t buf_size)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImVec2 pos_before = window->DC.CursorPos;

		PushID(str_id);
		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(g.Style.ItemSpacing.x, g.Style.FramePadding.y * 2.0f));
		bool ret = Selectable("##Selectable", selected, flags | ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_AllowItemOverlap);
		PopStyleVar();

		ImGuiID id = window->GetID("##Input");
		bool temp_input_is_active = TempInputIsActive(id);
		bool temp_input_start = ret ? IsMouseDoubleClicked(0) : false;

		if (temp_input_start)
			SetActiveID(id, window);

		if (temp_input_is_active || temp_input_start)
		{
			ImVec2 pos_after = window->DC.CursorPos;
			window->DC.CursorPos = pos_before;
			ret = TempInputText(window->DC.LastItemRect, id, "##Input", buf, (int)buf_size, ImGuiInputTextFlags_None);
			window->DC.CursorPos = pos_after;
		}
		else
		{
			window->DrawList->AddText(pos_before, GetColorU32(ImGuiCol_Text), buf);
		}

		PopID();
		return ret;
	}
}


namespace LinaEditor
{
	using namespace LinaEngine::ECS;
	using namespace LinaEngine;
	static bool openModal;
	const char* entityComponents[] = { "Transform", "Mesh Renderer", "Camera", "Directional Light", "Point Light", "Spot Light", "Free Look" };

	void ECSPanel::Setup(LinaEngine::ECS::ECSRegistry& registry, LinaEngine::Graphics::Window& appWindow)
	{
		m_ECS = &registry;
		m_AppWindow = &appWindow;

		registry.each([this](auto entity) {
			EditorEntity editorEntity;
			editorEntity.entity = entity;
			m_EditorEntities.push_back(editorEntity);

		});
	}

	void ECSPanel::Draw()
	{
		if (m_Show)
		{
			if (openModal)
				ImGui::OpenPopup("Component Exists!");

			if (ImGui::BeginPopupModal("Component Exists!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("This component already exists!\n\n");
				if (ImGui::Button("OK", ImVec2(120, 0))) { openModal = false; ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			ImVec2 panelSize = ImVec2(700, 400);
			
			ImGui::SetNextWindowSize(panelSize, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(0.2f);
			ImGui::SetNextWindowPos(ImVec2(0,  m_AppWindow->GetHeight() - panelSize.y / 2.0f));
			if (ImGui::Begin("ECS Panel", &m_Show))
			{
				static int componentsComboCurrentItem = 0;
				static char selectedEntityName[256] = "Entity";


				ImGui::BeginChild("left pane", ImVec2(150, 0), true);

				// Handle Right Click.
				if (ImGui::BeginPopupContextWindow())
				{
					if (ImGui::BeginMenu("Create"))
					{
						if (ImGui::MenuItem("Entity"))
						{
							CreateNewEntity();
						}
						ImGui::EndMenu();
					}
					ImGui::EndPopup();
				}

				static bool anyItemHovered = false;

				for (int i = 0; i < m_EditorEntities.size(); i++)
				{
					strcpy(selectedEntityName, m_EditorEntities[i].name.c_str());
					if (ImGui::SelectableInput("entSelectable" + i, m_SelectedEntity == i, ImGuiSelectableFlags_SelectOnClick, selectedEntityName, IM_ARRAYSIZE(selectedEntityName)))
					{
						m_SelectedEntity = i;
						m_EditorEntities[i].name = selectedEntityName;
					}

					if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						m_SelectedEntity = -1;
					}

				}

				ImGui::EndChild();
				ImGui::SameLine();

				// Right
				ImGui::BeginGroup();
				ImGui::BeginChild("Component View", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us

				if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
				{
					std::string selectedName = m_SelectedEntity == -1 ? "Component View" : "Component View: " + m_EditorEntities[m_SelectedEntity].name;
					char titleName[256];
					strcpy(titleName, selectedName.c_str());

					if (ImGui::BeginTabItem(titleName))
					{
						if (m_SelectedEntity == -1)
							ImGui::TextWrapped("No entity is selected.");
						else
							DrawComponents(m_EditorEntities[m_SelectedEntity].entity);
						ImGui::EndTabItem();
					}

					ImGui::EndTabBar();
				}
				ImGui::EndChild();

				if (m_SelectedEntity != -1)
				{
					if (ImGui::Button("Add Component"))
					{
						AddComponentToEntity(componentsComboCurrentItem);
					}

					ImGui::SameLine();

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
					}
				}


				ImGui::EndGroup();
			}
			ImGui::End();
		}
	}

	void ECSPanel::CreateNewEntity()
	{
		EditorEntity editorEntity;
		editorEntity.entity = m_ECS->create();
		m_EditorEntities.push_back(editorEntity);
	}

	void ECSPanel::AddComponentToEntity(int componentID)
	{
		if (m_SelectedEntity == -1 || m_SelectedEntity >= m_EditorEntities.size()) return;


		bool open = true;
		// Add the indexed component to target entity.
		if (componentID == 0)
		{
			if (m_ECS->has<LinaEngine::ECS::TransformComponent>(m_EditorEntities[m_SelectedEntity].entity))
				openModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::TransformComponent>(m_EditorEntities[m_SelectedEntity].entity);
			}
		}
		else if (componentID == 1)
		{
			if (m_ECS->has<LinaEngine::ECS::MeshRendererComponent>(m_EditorEntities[m_SelectedEntity].entity))
				openModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::MeshRendererComponent>(m_EditorEntities[m_SelectedEntity].entity);
			}
		}
		else if (componentID == 2)
		{
			if (m_ECS->has<LinaEngine::ECS::CameraComponent>(m_EditorEntities[m_SelectedEntity].entity))
				openModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::CameraComponent>(m_EditorEntities[m_SelectedEntity].entity);
			}

		}
		else if (componentID == 3)
		{
			if (m_ECS->has<LinaEngine::ECS::DirectionalLightComponent>(m_EditorEntities[m_SelectedEntity].entity))
				openModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::DirectionalLightComponent>(m_EditorEntities[m_SelectedEntity].entity);
			}

		}
		else if (componentID == 4)
		{
			if (m_ECS->has<LinaEngine::ECS::PointLightComponent>(m_EditorEntities[m_SelectedEntity].entity))
				openModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::PointLightComponent>(m_EditorEntities[m_SelectedEntity].entity);
			}

		}
		else if (componentID == 5)
		{
			if (m_ECS->has<LinaEngine::ECS::SpotLightComponent>(m_EditorEntities[m_SelectedEntity].entity))
				openModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::SpotLightComponent>(m_EditorEntities[m_SelectedEntity].entity);
			}

		}
		else if (componentID == 6)
		{
			if (m_ECS->has<LinaEngine::ECS::FreeLookComponent>(m_EditorEntities[m_SelectedEntity].entity))
				openModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::FreeLookComponent>(m_EditorEntities[m_SelectedEntity].entity);
			}
		}

	}

	void ECSPanel::DrawComponents(LinaEngine::ECS::ECSEntity& entity)
	{
		if (!m_ECS->any<TransformComponent, MeshRendererComponent, CameraComponent, DirectionalLightComponent, SpotLightComponent, PointLightComponent, FreeLookComponent>(entity))
			ImGui::TextWrapped("This entity doesn't contain any components");

		if (m_ECS->has<TransformComponent>(entity))
		{
			TransformComponent* transform = &m_ECS->get<TransformComponent>(entity);

			if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();

				float location[3] = { transform->transform.location.x, transform->transform.location.y, transform->transform.location.z };
				float rotation[3] = { transform->transform.rotation.GetEuler().x,transform->transform.rotation.GetEuler().y,transform->transform.rotation.GetEuler().z };
				float scale[3] = { transform->transform.scale.x, transform->transform.scale.y, transform->transform.scale.z};
				ImGui::DragFloat3("Location", location);
				ImGui::Separator();
				ImGui::DragFloat3("Rotation", rotation);
				ImGui::Separator();
				ImGui::DragFloat3("Scale", scale);
				transform->transform.location = Vector3(location[0], location[1], location[2]);
				transform->transform.scale = Vector3(scale[0], scale[1], scale[2]);
				transform->transform.rotation = Quaternion::Euler(Vector3(rotation[0], rotation[1], rotation[2]));
				LINA_CLIENT_INFO("{0}", transform->transform.location.ToString());
				ImGui::Unindent();
			}
		}



	}

}