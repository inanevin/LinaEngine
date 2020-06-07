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
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include <stdio.h>



namespace LinaEditor
{
	using namespace LinaEngine::ECS;
	using namespace LinaEngine;
	static bool openCompExistsModal;
	const char* entityComponents[] = { "Transform", "Mesh Renderer", "Camera", "Directional Light", "Point Light", "Spot Light", "Free Look" };

	
	void ECSPanel::Setup()
	{
		// Store references.
		m_ECS = m_GUILayer->GetECS();
		m_ScenePanel = m_GUILayer->GetScenePanel();
		m_PropertiesPanel = m_GUILayer->GetPropertiesPanel();

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
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

			if (ImGui::Begin("Entities", &m_Show, flags))
			{
				// Statics.
				static int componentsComboCurrentItem = 0;
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
						m_ECS->SetEntityName(entity, selectedEntityName);
					}

					// Deselect.
					if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						m_SelectedEntity = entt::null;
						m_ScenePanel->SetSelectedTransform(nullptr);
					}

				}

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
				EditorUtility::ColorButton(Vector4(col.x, col.y, col.z, col.w));
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
				EditorUtility::ColorButton(Vector4(col.x, col.y, col.z, col.w));
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
				
				EditorUtility::ColorButton(Vector4(col.x, col.y, col.z, col.w));
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
				ImGui::Unindent();
			}
		}

	}

}