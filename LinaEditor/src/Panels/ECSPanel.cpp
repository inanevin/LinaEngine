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
#include "Panels/MaterialPanel.hpp"
#include "Panels/ScenePanel.hpp"
#include "Utility/Log.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "REndering/Material.hpp"
#include "Rendering/Window.hpp"
#include "imgui.h"
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
	static bool openCompExistsModal;
	const char* entityComponents[] = { "Transform", "Mesh Renderer", "Camera", "Directional Light", "Point Light", "Spot Light", "Free Look" };

	static void ColorButton(ImVec4& color)
	{
		static bool alpha_preview = true;
		static bool alpha_half_preview = false;
		static bool drag_and_drop = true;
		static bool options_menu = true;
		static bool hdr = true;
		ImGuiColorEditFlags misc_flags = (hdr ? (ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float) : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
		// Generate a dummy default palette. The palette will persist and can be edited.
		static bool saved_palette_init = true;
		static ImVec4 saved_palette[32] = {};
		if (saved_palette_init)
		{
			for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
			{
				ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
					saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
				saved_palette[n].w = 1.0f; // Alpha
			}
			saved_palette_init = false;
		}

		static ImVec4 backup_color;
		bool open_popup = ImGui::ColorButton("MyColor##3b", color, misc_flags);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		open_popup |= ImGui::Button("Light Color");
		if (open_popup)
		{
			ImGui::OpenPopup("mypicker");
			backup_color = color;
		}
		if (ImGui::BeginPopup("mypicker"))
		{
			ImGui::PushItemWidth(160);

			ImGui::Text("Color Picker!");
			ImGui::Separator();
			ImGui::ColorPicker4("##picker", (float*)&color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
			ImGui::SameLine();

			ImGui::BeginGroup(); // Lock X position
			ImGui::Text("Current");
			ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
			ImGui::Text("Previous");
			if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
				color = backup_color;
		/*	ImGui::Separator();
			ImGui::Text("Palette");
			for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
			{
				ImGui::PushID(n);
				if ((n % 8) != 0)
					ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

				ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
				if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
					color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

				// Allow user to drop colors into each palette entry. Note that ColorButton() is already a
				// drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
						memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
						memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
					ImGui::EndDragDropTarget();
				}

				ImGui::PopID();
			}*/
			ImGui::EndGroup();
			ImGui::EndPopup();
		}
	}

	void ECSPanel::Setup(LinaEngine::ECS::ECSRegistry& registry, ScenePanel& scenePanel, LinaEngine::Graphics::Window& appWindow, MaterialPanel& materialPanel)
	{
		m_ScenePanel = &scenePanel;
		m_ECS = &registry;
		m_AppWindow = &appWindow;
		m_SelectedEntity = entt::null;
		m_MaterialPanel = &materialPanel;

		// add scene entitites to the list.
		m_ECS->each([this](auto entity)
		{
			m_EntityList.push_back(entity);
		});

	}

	void ECSPanel::Draw()
	{
		if (m_Show)
		{
			// Component already exists popup modal.
			if (openCompExistsModal)
				ImGui::OpenPopup("Component Exists!");
			if (ImGui::BeginPopupModal("Component Exists!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("This component already exists!\n\n");
				if (ImGui::Button("OK", ImVec2(120, 0))) { openCompExistsModal = false; ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImVec2 panelSize = ImVec2(m_Size.x, m_Size.y);			
			ImGui::SetNextWindowSize(panelSize, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(0.2f);

			if (ImGui::Begin("ECS", &m_Show))
			{
				// Statics.
				static int componentsComboCurrentItem = 0;
				static char selectedEntityName[256] = "Entity";

				// Left pane group.
				ImGui::BeginChild("left pane", ImVec2(150, 0), true);

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
					if (ImGui::SelectableInput("entSelectable" + entityCounter, m_SelectedEntity == entity, ImGuiSelectableFlags_SelectOnClick, selectedEntityName, IM_ARRAYSIZE(selectedEntityName)))
					{
						m_SelectedEntity = entity;
						m_ScenePanel->SetSelectedTransform(m_ECS->has<TransformComponent>(m_SelectedEntity) ? &m_ECS->get<TransformComponent>(m_SelectedEntity) : nullptr);
						m_ECS->SetEntityName(entity, selectedEntityName);
					}

					// Deselect.
					if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						m_SelectedEntity = entt::null;
						m_ScenePanel->SetSelectedTransform(nullptr);
					}

				}
		

				// End this pane.
				ImGui::EndChild();
				ImGui::SameLine();

				// Right pane.
				ImGui::BeginGroup();
				ImGui::BeginChild("Component View", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
				if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
				{
					// Component view tab
					bool isValid = m_ECS->valid(m_SelectedEntity);
					std::string selectedName = m_ECS->valid(m_SelectedEntity) ? "Component View" : ("Component View: " + m_ECS->GetEntityName(m_SelectedEntity));
					char titleName[256];
					strcpy(titleName, selectedName.c_str());
					if (ImGui::BeginTabItem(titleName))
					{
						if (!isValid)
							ImGui::TextWrapped("No entity is selected.");
						else
							DrawComponents(m_SelectedEntity);
						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}
				ImGui::EndChild();

				// Buttons down below.
				if (m_ECS->valid(m_SelectedEntity))
				{
					if (ImGui::Button("Add Component"))
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
					}
				}


				ImGui::EndGroup();
			}
			ImGui::End();
		}
	}

	void ECSPanel::AddComponentToEntity(int componentID)
	{
		if (!m_ECS->valid(m_SelectedEntity)) return;

		// Add the indexed component to target entity.
		if (componentID == 0)
		{
			if (m_ECS->has<LinaEngine::ECS::TransformComponent>(m_SelectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::TransformComponent>(m_SelectedEntity);
			}
		}
		else if (componentID == 1)
		{
			if (m_ECS->has<LinaEngine::ECS::MeshRendererComponent>(m_SelectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::MeshRendererComponent>(m_SelectedEntity);
			}
		}
		else if (componentID == 2)
		{
			if (m_ECS->has<LinaEngine::ECS::CameraComponent>(m_SelectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::CameraComponent>(m_SelectedEntity);
			}

		}
		else if (componentID == 3)
		{
			if (m_ECS->has<LinaEngine::ECS::DirectionalLightComponent>(m_SelectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::DirectionalLightComponent>(m_SelectedEntity);
			}

		}
		else if (componentID == 4)
		{
			if (m_ECS->has<LinaEngine::ECS::PointLightComponent>(m_SelectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::PointLightComponent>(m_SelectedEntity);
			}

		}
		else if (componentID == 5)
		{
			if (m_ECS->has<LinaEngine::ECS::SpotLightComponent>(m_SelectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::SpotLightComponent>(m_SelectedEntity);
			}

		}
		else if (componentID == 6)
		{
			if (m_ECS->has<LinaEngine::ECS::FreeLookComponent>(m_SelectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ECS->emplace<LinaEngine::ECS::FreeLookComponent>(m_SelectedEntity);
			}
		}

	}

	void ECSPanel::DrawComponents(LinaEngine::ECS::ECSEntity& entity)
	{
		// Check if entity has any component.
		if (!m_ECS->any<TransformComponent, MeshRendererComponent, CameraComponent, DirectionalLightComponent, SpotLightComponent, PointLightComponent, FreeLookComponent>(entity))
			ImGui::TextWrapped("This entity doesn't contain any components");

		// Draw transform component.
		if (m_ECS->has<TransformComponent>(entity))
		{
			float dragSensitivity = 0.05f;
			float dragSensitivityRotation = 0.25f;
			TransformComponent* transform = &m_ECS->get<TransformComponent>(entity);

			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();

				float location[3] = { transform->transform.location.x, transform->transform.location.y, transform->transform.location.z };
				float rot[3] = { transform->transform.rotation.GetEuler().x, transform->transform.rotation.GetEuler().y, transform->transform.rotation.GetEuler().z };
				float scale[3] = { transform->transform.scale.x, transform->transform.scale.y, transform->transform.scale.z};
				ImGui::DragFloat3("Location", location, dragSensitivity);
				ImGui::DragFloat3("Rotation", rot, dragSensitivityRotation);
				ImGui::DragFloat3("Scale", scale, dragSensitivity);
				transform->transform.location = Vector3(location[0], location[1], location[2]);
				transform->transform.scale = Vector3(scale[0], scale[1], scale[2]);			
				transform->transform.rotation = Quaternion::Euler(rot[0], rot[1], rot[2]);

				ImGui::Unindent();
			}

		}


		// Draw Point Light component.
		if (m_ECS->has<PointLightComponent>(entity))
		{
			PointLightComponent* light = &m_ECS->get<PointLightComponent>(entity);
			MeshRendererComponent* lightRenderer = m_ECS->has<MeshRendererComponent>(entity) ? &m_ECS->get<MeshRendererComponent>(entity) : nullptr;

			float dragSensitivity = 0.05f;
			if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();

				ImVec4 col = ImVec4(light->color.r, light->color.g, light->color.b, light->color.a);
				float d = light->distance;
				ColorButton(col);
				ImGui::DragFloat("Distance ", &d, dragSensitivity);
				light->distance = d;
				light->color = Color(col.x, col.y, col.z, col.w);
				
				if (lightRenderer != nullptr)
					lightRenderer->material->SetColor(MAT_OBJECTCOLORPROPERTY, light->color);

				ImGui::Unindent();
			}

		}

		// Draw spot light component.
		if (m_ECS->has<SpotLightComponent>(entity))
		{
			SpotLightComponent* light = &m_ECS->get<SpotLightComponent>(entity);
			MeshRendererComponent* lightRenderer = m_ECS->has<MeshRendererComponent>(entity) ? &m_ECS->get<MeshRendererComponent>(entity) : nullptr;

			float dragSensitivity = 0.005f;
			if (ImGui::CollapsingHeader("Spot Light", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();

				ImVec4 col = ImVec4(light->color.r, light->color.g, light->color.b, light->color.a);
				float d = light->distance;
				float cutOff = light->cutOff;
				float outerCutOff = light->outerCutOff;
				ColorButton(col);
				ImGui::DragFloat("Distance ", &d, dragSensitivity);
				ImGui::DragFloat("CutOff ", &cutOff, dragSensitivity);
				ImGui::DragFloat("Outer Cutoff ", &outerCutOff, dragSensitivity);
				light->distance = d;
				light->cutOff = cutOff;
				light->outerCutOff = outerCutOff;
				light->color = Color(col.x, col.y, col.z, col.w);


				if (lightRenderer != nullptr)
					lightRenderer->material->SetColor(MAT_OBJECTCOLORPROPERTY, light->color);

				ImGui::Unindent();
			}
		}

		// Draw directional light component.
		if (m_ECS->has<DirectionalLightComponent>(entity))
		{
			DirectionalLightComponent* light = &m_ECS->get<DirectionalLightComponent>(entity);
			MeshRendererComponent* lightRenderer = m_ECS->has<MeshRendererComponent>(entity) ? &m_ECS->get<MeshRendererComponent>(entity) : nullptr;

			float dragSensitivity = 0.005f;
			if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();

				ImVec4 col = ImVec4(light->color.r, light->color.g, light->color.b, light->color.a);
				float direction[3] = { light->direction.x, light->direction.y, light->direction.z };
				ColorButton(col);
				ImGui::DragFloat3("Direction ", direction, dragSensitivity);
				light->direction = Vector3(direction[0], direction[1], direction[2]);
				light->color = Color(col.x, col.y, col.z, col.w);
				if (lightRenderer != nullptr)
					lightRenderer->material->SetColor(MAT_OBJECTCOLORPROPERTY, light->color);

				ImGui::Unindent();
			}
		}

		// Draw mesh renderer component
		if (m_ECS->has<MeshRendererComponent>(entity))
		{
			MeshRendererComponent* renderer = m_ECS->has<MeshRendererComponent>(entity) ? &m_ECS->get<MeshRendererComponent>(entity) : nullptr;

		
			float dragSensitivity = 0.005f;
			if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();

				if (renderer->material != nullptr)
				{
					m_MaterialPanel->SetCurrentMaterial(renderer->material);
				}

				//ImVec4 col = ImVec4(light->color.r, light->color.g, light->color.b, light->color.a);
				//float direction[3] = { light->direction.x, light->direction.y, light->direction.z };
				//ColorButton(col);
				//ImGui::DragFloat3("Direction ", direction, dragSensitivity);
				//light->direction = Vector3(direction[0], direction[1], direction[2]);
				//light->color = Color(col.x, col.y, col.z, col.w);
				//if (renderer != nullptr)
					//renderer->material->SetColor(MAT_OBJECTCOLORPROPERTY, light->color);

				ImGui::Unindent();
			}
		}

	}

}