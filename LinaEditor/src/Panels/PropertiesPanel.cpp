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

Class: PropertiesPanel
Timestamp: 6/7/2020 5:13:42 PM

*/


#include "Panels/PropertiesPanel.hpp"
#include "Core/GUILayer.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Material.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/RigidbodyComponent.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

namespace LinaEditor
{
	using namespace LinaEngine::ECS;
	using namespace LinaEngine;
	static bool openCompExistsModal;
	const char* entityComponents[] = { "Transform", "Mesh Renderer", "Camera", "Directional Light", "Point Light", "Spot Light", "Free Look" };
	const char* pixelFormats[]{
		"R",
		"RG",
		"RGB",
		"RGBA",
		"RGB16F",
		"RGBA16F",
		"DEPTH",
		"DEPTH_AND_STENCIL",
		"SRGB",
		"SRGBA" };

	const char* samplerFilters[]
	{
		"NEAREST",
		"LINEAR",
		"NEAREST_MIPMAP_NEAREST",
		"LINEAR_MIPMAP_NEAREST",
		"NEAREST_MIPMAP_LINEAR",
		"LINEAR_MIPMAP_LINEAR"
	};

	const char* wrapModes[]
	{
		"CLAMP_EDGE",
		"CLAMP_MIRROR",
		"CLAMP_BORDER",
		"REPEAT",
		"REPEAT_MIRROR"
	};

	const char* rigidbodyShapes[]
	{
		"SPHERE",
		"BOX",
		"CYLINDER",
		"CAPSULE"
	};

	static Graphics::PixelFormat selectedInternalPF;
	static Graphics::PixelFormat selectedPF;
	static Graphics::SamplerFilter selectedMinFilter;
	static Graphics::SamplerFilter selectedMagFilter;
	static Graphics::SamplerWrapMode selectedWrapS;
	static Graphics::SamplerWrapMode selectedWrapR;
	static Graphics::SamplerWrapMode selectedWrapT;
	static ECS::CollisionShape selectedCollisionShape;

	const int TEXTUREPREVIEW_MAX_WIDTH = 250;

	void PropertiesPanel::Setup()
	{
		m_ECS = m_GUILayer->GetECS();
		m_RenderEngine = m_GUILayer->GetRenderEngine();
	}

	void PropertiesPanel::Texture2DSelected(LinaEngine::Graphics::Texture* texture, int id, std::string& path)
	{

		m_SelectedTexture = texture;
		m_CurrentDrawType = DrawType::TEXTURE2D;
		m_SelectedTextureID = id;
		m_SelectedTexturePath = path;

		// Reset selected sampler params.
		Graphics::SamplerParameters& params = texture->GetSampler().GetSamplerParameters();
		m_CurrentInternalPF = (int)params.textureParams.internalPixelFormat;
		m_CurrentPF = (int)params.textureParams.pixelFormat;
		m_CurrentMinFilter = GetSamplerFilterID(params.textureParams.minFilter);
		m_CurrentMagFilter = GetSamplerFilterID(params.textureParams.magFilter);
		m_CurrentWrapS = GetWrapModeID(params.textureParams.wrapS);
		m_CurrentWrapR = GetWrapModeID(params.textureParams.wrapR);
		m_CurrentWrapT = GetWrapModeID(params.textureParams.wrapT);
		m_CurrentAnisotropy = params.anisotropy;
		m_CurrentGenerateMips = params.textureParams.generateMipMaps;
	}

	void PropertiesPanel::Draw()
	{
		if (m_CurrentDrawType != DrawType::NONE)
		{
			if (m_SelectedEntity == entt::null && m_SelectedTexture == nullptr && m_SelectedMesh == nullptr && m_SelectedMaterial == nullptr)
				m_CurrentDrawType = DrawType::NONE;
		}

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

			if (ImGui::Begin("Properties", &m_Show, flags))
			{
				if (m_CurrentDrawType == DrawType::ENTITIES)
					DrawEntityProperties();
				else if (m_CurrentDrawType == DrawType::TEXTURE2D)
					DrawTextureProperties();
				else if (m_CurrentDrawType == DrawType::MESH)
					DrawMeshProperties();
				else if (m_CurrentDrawType == DrawType::MATERIAL)
					DrawMaterialProperties();
			}

			ImGui::End();
		}
	}

	void PropertiesPanel::DrawEntityProperties()
	{
		static int componentsComboCurrentItem = 0;

		// Buttons down below.
		if (m_ECS->valid(m_SelectedEntity))
		{
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


	}

	void PropertiesPanel::AddComponentToEntity(int componentID)
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

	void PropertiesPanel::DrawComponents(LinaEngine::ECS::ECSEntity& entity)
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
				float scale[3] = { transform->transform.scale.x, transform->transform.scale.y, transform->transform.scale.z };
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
				EditorUtility::ColorButton(&col.x);
				ImGui::DragFloat("Distance ", &d, dragSensitivity);
				light->distance = d;
				light->color = Color(col.x, col.y, col.z, col.w);

				if (lightRenderer != nullptr)
					m_RenderEngine->GetMaterial(lightRenderer->materialID).SetColor(MAT_OBJECTCOLORPROPERTY, light->color);

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
				EditorUtility::ColorButton(&col.x);
				ImGui::DragFloat("Distance ", &d, dragSensitivity);
				ImGui::DragFloat("CutOff ", &cutOff, dragSensitivity);
				ImGui::DragFloat("Outer Cutoff ", &outerCutOff, dragSensitivity);
				light->distance = d;
				light->cutOff = cutOff;
				light->outerCutOff = outerCutOff;
				light->color = Color(col.x, col.y, col.z, col.w);


				if (lightRenderer != nullptr)
					m_RenderEngine->GetMaterial(lightRenderer->materialID).SetColor(MAT_OBJECTCOLORPROPERTY, light->color);

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

				EditorUtility::ColorButton(&col.x);
				ImGui::DragFloat3("Direction ", direction, dragSensitivity);
				light->direction = Vector3(direction[0], direction[1], direction[2]);
				light->color = Color(col.x, col.y, col.z, col.w);
				if (lightRenderer != nullptr)
					m_RenderEngine->GetMaterial(lightRenderer->materialID).SetColor(MAT_OBJECTCOLORPROPERTY, light->color);

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

		// Draw rigidbody component
		if (m_ECS->has<RigidbodyComponent>(entity))
		{
			if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();

			//	if (m_ECS->has<TransformComponent>(entity))
				//	m_gizmoLayer->RegisterGizmo(entity, LinaEngine::Graphics::Primitives::CUBE, m_ECS->get<TransformComponent>(entity));

					RigidbodyComponent& rb = m_ECS->get<RigidbodyComponent>(entity);

				m_currentCollisionShape = (int)rb.m_collisionShape;

				// Draw collision shape.
				static ImGuiComboFlags flags = 0;
				static ECS::CollisionShape selectedCollisionShape = rb.m_collisionShape;
				const char* collisionShapeLabel = rigidbodyShapes[m_currentCollisionShape];

				if (ImGui::BeginCombo("Collision Shape", collisionShapeLabel, flags))
				{
					for (int i = 0; i < IM_ARRAYSIZE(rigidbodyShapes); i++)
					{
						const bool is_selected = (m_currentCollisionShape == i);
						if (ImGui::Selectable(rigidbodyShapes[i], is_selected))
						{
							selectedCollisionShape = (ECS::CollisionShape)i;
							m_currentCollisionShape = i;
							rb.m_collisionShape = selectedCollisionShape;
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();

				}

				ImGui::InputFloat("Mass", &rb.m_mass);

				if (rb.m_collisionShape == ECS::CollisionShape::BOX || rb.m_collisionShape == ECS::CollisionShape::CYLINDER)
				{
					ImGui::InputFloat3("Half Extents", &rb.m_halfExtents.x);
				}
				else if (rb.m_collisionShape == ECS::CollisionShape::SPHERE)
				{
					ImGui::InputFloat("Radius", &rb.m_radius);
				}
				else if (rb.m_collisionShape == ECS::CollisionShape::CAPSULE)
				{
					ImGui::InputFloat("Radius", &rb.m_radius);
					ImGui::InputFloat("Height", &rb.m_capsuleHeight);
				}


				ImGui::Unindent();
			}


		}

	}

	void PropertiesPanel::DrawTextureProperties()
	{
		Graphics::SamplerParameters& params = m_SelectedTexture->GetSampler().GetSamplerParameters();
		static ImGuiComboFlags flags = 0;
		static Graphics::PixelFormat selectedInternalPF = params.textureParams.internalPixelFormat;
		static Graphics::PixelFormat selectedPF = params.textureParams.pixelFormat;
		static Graphics::SamplerFilter selectedMinFilter = params.textureParams.minFilter;
		static Graphics::SamplerFilter selectedMagFilter = params.textureParams.magFilter;
		static Graphics::SamplerWrapMode selectedWrapS = params.textureParams.wrapS;
		static Graphics::SamplerWrapMode selectedWrapR = params.textureParams.wrapR;
		static Graphics::SamplerWrapMode selectedWrapT = params.textureParams.wrapT;
		const char* internalPFLabel = pixelFormats[m_CurrentInternalPF];
		const char* pfLabel = pixelFormats[m_CurrentPF];
		const char* minFilterLabel = samplerFilters[m_CurrentMinFilter];
		const char* magFilterLabel = samplerFilters[m_CurrentMagFilter];
		const char* wrapSLabel = wrapModes[m_CurrentWrapS];
		const char* wrapRLabel = wrapModes[m_CurrentWrapR];
		const char* wrapTLabel = wrapModes[m_CurrentWrapT];


		ImGui::Checkbox("Generate Mipmaps?", &m_CurrentGenerateMips);
		ImGui::DragInt("Anisotropy", &m_CurrentAnisotropy, 0.05f, 0, 8);

		// Internal Pixel Format ComboBox
		if (ImGui::BeginCombo("Internal Pixel Format", internalPFLabel, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(pixelFormats); n++)
			{
				const bool is_selected = (m_CurrentInternalPF == n);
				if (ImGui::Selectable(pixelFormats[n], is_selected))
				{
					selectedInternalPF = (Graphics::PixelFormat)n;
					m_CurrentInternalPF = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		// Pixel Format ComboBox
		if (ImGui::BeginCombo("Pixel Format", pfLabel, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(pixelFormats); n++)
			{
				const bool is_selected = (m_CurrentPF == n);
				if (ImGui::Selectable(pixelFormats[n], is_selected))
				{
					selectedPF = (Graphics::PixelFormat)n;
					m_CurrentPF = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		// Mın Filter ComboBox
		if (ImGui::BeginCombo("Min Filter", minFilterLabel, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(samplerFilters); n++)
			{
				const bool is_selected = (m_CurrentMinFilter == n);
				if (ImGui::Selectable(samplerFilters[n], is_selected))
				{
					selectedMinFilter = GetSamplerFilterFromID(n);
					m_CurrentMinFilter = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		// Mın Filter ComboBox
		if (ImGui::BeginCombo("Mag Filter", magFilterLabel, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(samplerFilters); n++)
			{
				const bool is_selected = (m_CurrentMagFilter == n);
				if (ImGui::Selectable(samplerFilters[n], is_selected))
				{
					m_CurrentMagFilter = n;
					selectedMagFilter = GetSamplerFilterFromID(n);
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		// Wrap S ComboBox
		if (ImGui::BeginCombo("Wrap S", wrapSLabel, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(wrapModes); n++)
			{
				const bool is_selected = (m_CurrentWrapS == n);
				if (ImGui::Selectable(wrapModes[n], is_selected))
				{
					selectedWrapS = GetWrapModeFromID(n);
					m_CurrentWrapS = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		// Wrap T ComboBox
		if (ImGui::BeginCombo("Wrap R", wrapRLabel, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(wrapModes); n++)
			{
				const bool is_selected = (m_CurrentWrapR == n);
				if (ImGui::Selectable(wrapModes[n], is_selected))
				{
					selectedWrapS = GetWrapModeFromID(n);
					m_CurrentWrapR = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		// Wrap T ComboBox
		if (ImGui::BeginCombo("Wrap T", wrapTLabel, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(wrapModes); n++)
			{
				const bool is_selected = (m_CurrentWrapT == n);
				if (ImGui::Selectable(wrapModes[n], is_selected))
				{
					selectedWrapS = GetWrapModeFromID(n);
					m_CurrentWrapT = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		// Apply button
		if (ImGui::Button("Apply"))
		{
			params.anisotropy = m_CurrentAnisotropy;
			params.textureParams.generateMipMaps = m_CurrentGenerateMips;
			params.textureParams.internalPixelFormat = selectedInternalPF;
			params.textureParams.pixelFormat = selectedPF;
			params.textureParams.minFilter = selectedMinFilter;
			params.textureParams.magFilter = selectedMagFilter;
			params.textureParams.wrapR = selectedWrapR;
			params.textureParams.wrapS = selectedWrapS;
			params.textureParams.wrapT = selectedWrapT;
			Graphics::SamplerParameters newParams = params;
			m_RenderEngine->UnloadTextureResource(m_SelectedTextureID);
			m_SelectedTexture = &m_RenderEngine->CreateTexture2D(m_SelectedTextureID, m_SelectedTexturePath, newParams);
		}

		// Setup data for drawing texture.
		float currentWindowX = ImGui::GetCurrentWindow()->Size.x;

		if (currentWindowX > TEXTUREPREVIEW_MAX_WIDTH)
			currentWindowX = TEXTUREPREVIEW_MAX_WIDTH;

		float currentWindowY = ImGui::GetCurrentWindow()->Size.y;
		Vector2 textureSize = m_SelectedTexture->GetSize();
		float textureAspect = textureSize.x / textureSize.y;
		float desiredH = currentWindowX / textureAspect;
		ImVec2 pMin = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
		ImVec2 pMax = ImVec2(ImGui::GetCursorScreenPos().x + currentWindowX, ImGui::GetCursorScreenPos().y + desiredH);
		ImVec2 size = ImGui::GetCurrentWindow()->Size;

		// Draw texture
		ImGui::GetWindowDrawList()->AddImage((void*)m_SelectedTexture->GetID(), pMin, pMax, ImVec2(0, 1), ImVec2(1, 0));


	}

	int PropertiesPanel::GetSamplerFilterID(Graphics::SamplerFilter filter)
	{
		if (filter == Graphics::SamplerFilter::FILTER_NEAREST)
			return 0;
		else if (filter == Graphics::SamplerFilter::FILTER_LINEAR)
			return 1;
		else if (filter == Graphics::SamplerFilter::FILTER_NEAREST_MIPMAP_NEAREST)
			return 2;
		else if (filter == Graphics::SamplerFilter::FILTER_LINEAR_MIPMAP_NEAREST)
			return 3;
		else if (filter == Graphics::SamplerFilter::FILTER_NEAREST_MIPMAP_LINEAR)
			return 4;
		else if (filter == Graphics::SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR)
			return 5;
	}

	int PropertiesPanel::GetWrapModeID(Graphics::SamplerWrapMode wrapMode)
	{
		if (wrapMode == Graphics::SamplerWrapMode::WRAP_CLAMP_EDGE)
			return 0;
		else if (wrapMode == Graphics::SamplerWrapMode::WRAP_CLAMP_MIRROR)
			return 1;
		else if (wrapMode == Graphics::SamplerWrapMode::WRAP_CLAMP_BORDER)
			return 2;
		else if (wrapMode == Graphics::SamplerWrapMode::WRAP_REPEAT)
			return 3;
		else if (wrapMode == Graphics::SamplerWrapMode::WRAP_REPEAT_MIRROR)
			return 4;
	}

	Graphics::SamplerFilter PropertiesPanel::GetSamplerFilterFromID(int id)
	{
		if (id == 0)
			return Graphics::SamplerFilter::FILTER_NEAREST;
		else if (id == 1)
			return Graphics::SamplerFilter::FILTER_LINEAR;
		else if (id == 2)
			return Graphics::SamplerFilter::FILTER_NEAREST_MIPMAP_NEAREST;
		else if (id == 3)
			return Graphics::SamplerFilter::FILTER_LINEAR_MIPMAP_NEAREST;
		else if (id == 4)
			return Graphics::SamplerFilter::FILTER_NEAREST_MIPMAP_LINEAR;
		else if (id == 5)
			return Graphics::SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
	}

	Graphics::SamplerWrapMode PropertiesPanel::GetWrapModeFromID(int id)
	{
		if (id == 0)
			return Graphics::SamplerWrapMode::WRAP_CLAMP_EDGE;
		else if (id == 1)
			return Graphics::SamplerWrapMode::WRAP_CLAMP_MIRROR;
		else if (id == 2)
			return Graphics::SamplerWrapMode::WRAP_CLAMP_BORDER;
		else if (id == 2)
			return Graphics::SamplerWrapMode::WRAP_REPEAT;
		else if (id == 4)
			return Graphics::SamplerWrapMode::WRAP_REPEAT_MIRROR;
	}

	void PropertiesPanel::DrawMeshProperties()
	{
		Graphics::MeshParameters params = m_SelectedMesh->GetParameters();

		ImGui::Checkbox("Triangulate", &m_CurrentMeshParams.triangulate);
		ImGui::Checkbox("Generate Smooth Normals", &m_CurrentMeshParams.smoothNormals);
		ImGui::Checkbox("Calculate Tangent Space", &m_CurrentMeshParams.calculateTangentSpace);

		if (ImGui::Button("Apply"))
		{
			Graphics::MeshParameters params = m_CurrentMeshParams;
			m_RenderEngine->UnloadMeshResource(m_SelectedMeshID);
			m_SelectedMesh = &m_RenderEngine->CreateMesh(m_SelectedMeshID, m_SelectedMeshPath, params);
		}
	}
	void PropertiesPanel::DrawMaterialProperties()
	{
		ImGui::Text("test");
	}
}