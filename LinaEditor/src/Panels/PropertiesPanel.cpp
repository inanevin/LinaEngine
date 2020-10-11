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
#include "Widgets/WidgetsUtility.hpp"
#include "IconsFontAwesome5.h"


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
		m_ecs = m_guiLayer->GetECS();
		m_RenderEngine = m_guiLayer->GetRenderEngine();


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

	void PropertiesPanel::Draw(float frameTime)
	{
		if (m_CurrentDrawType != DrawType::NONE)
		{
			if (m_selectedEntity == entt::null && m_SelectedTexture == nullptr && m_SelectedMesh == nullptr && m_SelectedMaterial == nullptr)
				m_CurrentDrawType = DrawType::NONE;
		}

		if (m_show)
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
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

		
			
			ImGui::Begin("Properties", &m_show, flags);
	
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 0));

			if (m_CurrentDrawType == DrawType::ENTITIES)
				DrawEntityProperties();
			else if (m_CurrentDrawType == DrawType::TEXTURE2D)
				DrawTextureProperties();
			else if (m_CurrentDrawType == DrawType::MESH)
				DrawMeshProperties();
			else if (m_CurrentDrawType == DrawType::MATERIAL)
				DrawMaterialProperties();

			ImGui::PopStyleVar();


			ImGui::End();

		}
	}

	void PropertiesPanel::DrawEntityProperties()
	{
		static int componentsComboCurrentItem = 0;

		// Buttons down below.
		if (m_ecs->valid(m_selectedEntity))
		{

			// Shadow.
			WidgetsUtility::DrawShadowedLine(5);

			// Align.
			ImGui::SetCursorPosX(12); WidgetsUtility::IncrementCursorPosY(15);	
			WidgetsUtility::AlignedText(ICON_FA_CUBE);	ImGui::SameLine();

			// Setup char.
			static char entityName[64] = "";
			if (m_copySelectedEntityName)
			{
				m_copySelectedEntityName = false;
				memset(entityName, 0, sizeof entityName);
				std::string str = m_ecs->GetEntityName(m_selectedEntity);
				std::copy(str.begin(), str.end(), entityName);
			}

			// Entity name input text.
			WidgetsUtility::IncrementCursorPosY(-1.6f);  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 56);
			ImGui::InputText("##hidelabel", entityName, IM_ARRAYSIZE(entityName));
			m_ecs->SetEntityName(m_selectedEntity, entityName);

			// Entity enabled toggle button.
			ImGui::SameLine();	WidgetsUtility::IncrementCursorPosY(1.5f);
			static bool b = false;	ImVec4 toggleColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
			WidgetsUtility::ToggleButton("##hideLabel", &b, 0.8f, 1.4f, toggleColor, ImVec4(toggleColor.x, toggleColor.y, toggleColor.z, 0.7f));
			
			// Bevel.
			WidgetsUtility::IncrementCursorPosY(6.0f);
			WidgetsUtility::DrawBeveledLine();

			ImGui::SetCursorPosX(12); WidgetsUtility::IncrementCursorPosY(15);
			static bool open = false;
			WidgetsUtility::DrawComponentTitle("Transformation" , ICON_FA_ARROWS_ALT, &open, ImGui::GetStyleColorVec4(ImGuiCol_Header));

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
			}*/
		}


	}

	void PropertiesPanel::AddComponentToEntity(int componentID)
	{
		if (!m_ecs->valid(m_selectedEntity)) return;

		// Add the indexed component to target entity.
		if (componentID == 0)
		{
			if (m_ecs->has<LinaEngine::ECS::TransformComponent>(m_selectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ecs->emplace<LinaEngine::ECS::TransformComponent>(m_selectedEntity);
			}
		}
		else if (componentID == 1)
		{
			if (m_ecs->has<LinaEngine::ECS::MeshRendererComponent>(m_selectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ecs->emplace<LinaEngine::ECS::MeshRendererComponent>(m_selectedEntity);
			}
		}
		else if (componentID == 2)
		{
			if (m_ecs->has<LinaEngine::ECS::CameraComponent>(m_selectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ecs->emplace<LinaEngine::ECS::CameraComponent>(m_selectedEntity);
			}

		}
		else if (componentID == 3)
		{
			if (m_ecs->has<LinaEngine::ECS::DirectionalLightComponent>(m_selectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ecs->emplace<LinaEngine::ECS::DirectionalLightComponent>(m_selectedEntity);
			}

		}
		else if (componentID == 4)
		{
			if (m_ecs->has<LinaEngine::ECS::PointLightComponent>(m_selectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ecs->emplace<LinaEngine::ECS::PointLightComponent>(m_selectedEntity);
			}

		}
		else if (componentID == 5)
		{
			if (m_ecs->has<LinaEngine::ECS::SpotLightComponent>(m_selectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ecs->emplace<LinaEngine::ECS::SpotLightComponent>(m_selectedEntity);
			}

		}
		else if (componentID == 6)
		{
			if (m_ecs->has<LinaEngine::ECS::FreeLookComponent>(m_selectedEntity))
				openCompExistsModal = true;
			else
			{
				auto& e = m_ecs->emplace<LinaEngine::ECS::FreeLookComponent>(m_selectedEntity);
			}
		}

	}

	void PropertiesPanel::DrawComponents(LinaEngine::ECS::ECSEntity& entity)
	{
		// Draw transform component.
		if (TransformComponent* transform = m_ecs->try_get<TransformComponent>(entity))
		{
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();
				float a = transform->transform.m_location[0];
				float rot[3] = { transform->transform.m_rotation.GetEuler().x, transform->transform.m_rotation.GetEuler().y, transform->transform.m_rotation.GetEuler().z };
				ImGui::Text("Location"); ImGui::SameLine(); ImGui::InputFloat3("", transform->transform.m_location.Get());
				ImGui::Text("Rotation"); ImGui::SameLine(); ImGui::InputFloat3("", rot);
				ImGui::Text("Scale"); ImGui::SameLine(); ImGui::InputFloat3("", transform->transform.m_scale.Get());			
				transform->transform.m_rotation = Quaternion::Euler(rot[0], rot[1], rot[2]);
				ImGui::Unindent();
			}

		}


		// Draw Point Light component.
		if (m_ecs->has<PointLightComponent>(entity))
		{
			PointLightComponent* light = &m_ecs->get<PointLightComponent>(entity);
			MeshRendererComponent* lightRenderer = m_ecs->has<MeshRendererComponent>(entity) ? &m_ecs->get<MeshRendererComponent>(entity) : nullptr;

			float dragSensitivity = 0.05f;
			if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();

				ImVec4 col = ImVec4(light->color.r, light->color.g, light->color.b, light->color.a);
				float d = light->distance;
				WidgetsUtility::ColorButton(&col.x);
				ImGui::DragFloat("Distance ", &d, dragSensitivity);
				light->distance = d;
				light->color = Color(col.x, col.y, col.z, col.w);

				if (lightRenderer != nullptr)
					m_RenderEngine->GetMaterial(lightRenderer->materialID).SetColor(MAT_OBJECTCOLORPROPERTY, light->color);

				ImGui::Unindent();
			}

		}

		// Draw spot light component.
		if (m_ecs->has<SpotLightComponent>(entity))
		{
			SpotLightComponent* light = &m_ecs->get<SpotLightComponent>(entity);
			MeshRendererComponent* lightRenderer = m_ecs->has<MeshRendererComponent>(entity) ? &m_ecs->get<MeshRendererComponent>(entity) : nullptr;

			float dragSensitivity = 0.005f;
			if (ImGui::CollapsingHeader("Spot Light", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();

				ImVec4 col = ImVec4(light->color.r, light->color.g, light->color.b, light->color.a);
				float d = light->distance;
				float cutOff = light->cutOff;
				float outerCutOff = light->outerCutOff;
				WidgetsUtility::ColorButton(&col.x);
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
		if (m_ecs->has<DirectionalLightComponent>(entity))
		{
			DirectionalLightComponent* light = &m_ecs->get<DirectionalLightComponent>(entity);
			MeshRendererComponent* lightRenderer = m_ecs->has<MeshRendererComponent>(entity) ? &m_ecs->get<MeshRendererComponent>(entity) : nullptr;

			float dragSensitivity = 0.005f;
			if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();

				ImVec4 col = ImVec4(light->color.r, light->color.g, light->color.b, light->color.a);
				float projectionSettings[4] = { light->shadowProjectionSettings.x, light->shadowProjectionSettings.y, light->shadowProjectionSettings.z, light->shadowProjectionSettings.w };
				float dir[3] = { light->direction.x, light->direction.y, light->direction.z};

				WidgetsUtility::ColorButton(&col.x);
				ImGui::InputFloat4("Shadow Projection ", projectionSettings, dragSensitivity);
				ImGui::InputFloat("Shadow Near", &light->shadowNearPlane);
				ImGui::InputFloat("Shadow Far", &light->shadowFarPlane);
				ImGui::InputFloat3("Direction", dir);
				light->color = Color(col.x, col.y, col.z, col.w);
				light->shadowProjectionSettings = Vector4(projectionSettings[0], projectionSettings[1], projectionSettings[2], projectionSettings[3]);
				light->direction = Vector3(dir[0], dir[1], dir[2]);
				if (lightRenderer != nullptr)
					m_RenderEngine->GetMaterial(lightRenderer->materialID).SetColor(MAT_OBJECTCOLORPROPERTY, light->color);

				ImGui::Unindent();
			}
		}

		// Draw mesh renderer component
		if (m_ecs->has<MeshRendererComponent>(entity))
		{
			MeshRendererComponent* renderer = m_ecs->has<MeshRendererComponent>(entity) ? &m_ecs->get<MeshRendererComponent>(entity) : nullptr;

			float dragSensitivity = 0.005f;
			if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();


				ImGui::Unindent();
			}
		}

		// Draw rigidbody component
		if (m_ecs->has<RigidbodyComponent>(entity))
		{
			if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();

				RigidbodyComponent& rb = m_ecs->get<RigidbodyComponent>(entity);

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

				if (ImGui::Button("Apply"))
				{
					m_ecs->replace<LinaEngine::ECS::RigidbodyComponent>(entity, rb);
				}
				ImGui::Unindent();
			}


		}

	}

	void PropertiesPanel::DrawVector2(const char* label, Vector2& v)
	{
		float location[2] = { v.x, v.y };
		ImGui::Text(label);
		ImGui::SameLine();
		ImGui::InputFloat2("", location);
		v.x = location[0];
		v.y = location[1];
	}

	void PropertiesPanel::DrawVector3(const char* label, Vector3& v)
	{
	}

	void PropertiesPanel::DrawVector4(const char* label, Vector4& v)
	{
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
			m_SelectedTexture = &m_RenderEngine->CreateTexture2D(m_SelectedTexturePath, newParams);
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