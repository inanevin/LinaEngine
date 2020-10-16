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
#include "Widgets/WidgetsUtility.hpp"
#include "IconsFontAwesome5.h"
#include "Drawers/EntityDrawer.hpp"
#include "Drawers/ComponentDrawer.hpp"

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

	static Graphics::PixelFormat selectedInternalPF;
	static Graphics::PixelFormat selectedPF;
	static Graphics::SamplerFilter selectedMinFilter;
	static Graphics::SamplerFilter selectedMagFilter;
	static Graphics::SamplerWrapMode selectedWrapS;
	static Graphics::SamplerWrapMode selectedWrapR;
	static Graphics::SamplerWrapMode selectedWrapT;

	const int TEXTUREPREVIEW_MAX_WIDTH = 250;

	void PropertiesPanel::Setup()
	{
		m_ecs = m_guiLayer->GetECS();
		m_renderEngine = m_guiLayer->GetRenderEngine();

		// Register component draw functions
		ComponentDrawer::RegisterComponentFunctions();
	}

	void PropertiesPanel::EntitySelected(LinaEngine::ECS::ECSEntity selectedEntity)
	{
		m_selectedEntity = selectedEntity;
		m_currentDrawType = DrawType::ENTITIES;
		m_copySelectedEntityName = true;
		ComponentDrawer::ClearDrawList();
	}

	void PropertiesPanel::Texture2DSelected(LinaEngine::Graphics::Texture* texture, int id, std::string& path)
	{

		m_SelectedTexture = texture;
		m_currentDrawType = DrawType::TEXTURE2D;
		m_SelectedTexturePath = path;

		// Reset selected sampler params.
		Graphics::SamplerParameters& params = texture->GetSampler().GetSamplerParameters();
		m_currentInternalPixelFormat = (int)params.textureParams.internalPixelFormat;
		m_currentPixelFormat = (int)params.textureParams.pixelFormat;
		m_currentMinFilter = GetSamplerFilterID(params.textureParams.minFilter);
		m_currentMagFilter = GetSamplerFilterID(params.textureParams.magFilter);
		m_currentWrapS = GetWrapModeID(params.textureParams.wrapS);
		m_currentWrapR = GetWrapModeID(params.textureParams.wrapR);
		m_currentWrapT = GetWrapModeID(params.textureParams.wrapT);
		m_currentAnisotropy = params.anisotropy;
		m_currentGenerateMips = params.textureParams.generateMipMaps;
	}

	void PropertiesPanel::Draw(float frameTime)
	{
		// Make sure we draw nothing if nothing is selected.
		if (m_currentDrawType != DrawType::NONE)
		{
			if (m_selectedEntity == entt::null && m_SelectedTexture == nullptr && m_selectedMesh == nullptr && m_selectedMaterial == nullptr)
				m_currentDrawType = DrawType::NONE;
		}

		if (m_show)
		{
			
			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImVec2 panelSize = ImVec2(m_Size.x, m_Size.y);
			ImGui::SetNextWindowSize(panelSize, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;


			// window.
			ImGui::Begin("Properties", &m_show, flags);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 0));

			// Shadow.
			WidgetsUtility::DrawShadowedLine(5);

			// Draw the selected item.
			if (m_currentDrawType == DrawType::ENTITIES)
			{
				if (m_ecs->valid(m_selectedEntity))
					EntityDrawer::DrawEntity(m_ecs, m_selectedEntity, &m_copySelectedEntityName);
			}
			else if (m_currentDrawType == DrawType::TEXTURE2D)
				DrawTextureProperties();
			else if (m_currentDrawType == DrawType::MESH)
				DrawMeshProperties();
			else if (m_currentDrawType == DrawType::MATERIAL)
				DrawMaterialProperties();

			ImGui::PopStyleVar();
			ImGui::End();

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
		const char* internalPFLabel = pixelFormats[m_currentInternalPixelFormat];
		const char* pfLabel = pixelFormats[m_currentPixelFormat];
		const char* minFilterLabel = samplerFilters[m_currentMinFilter];
		const char* magFilterLabel = samplerFilters[m_currentMagFilter];
		const char* wrapSLabel = wrapModes[m_currentWrapS];
		const char* wrapRLabel = wrapModes[m_currentWrapR];
		const char* wrapTLabel = wrapModes[m_currentWrapT];


		ImGui::Checkbox("Generate Mipmaps?", &m_currentGenerateMips);
		ImGui::DragInt("Anisotropy", &m_currentAnisotropy, 0.05f, 0, 8);

		// Internal Pixel Format ComboBox
		if (ImGui::BeginCombo("Internal Pixel Format", internalPFLabel, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(pixelFormats); n++)
			{
				const bool is_selected = (m_currentInternalPixelFormat == n);
				if (ImGui::Selectable(pixelFormats[n], is_selected))
				{
					selectedInternalPF = (Graphics::PixelFormat)n;
					m_currentInternalPixelFormat = n;
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
				const bool is_selected = (m_currentPixelFormat == n);
				if (ImGui::Selectable(pixelFormats[n], is_selected))
				{
					selectedPF = (Graphics::PixelFormat)n;
					m_currentPixelFormat = n;
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
				const bool is_selected = (m_currentMinFilter == n);
				if (ImGui::Selectable(samplerFilters[n], is_selected))
				{
					selectedMinFilter = GetSamplerFilterFromID(n);
					m_currentMinFilter = n;
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
				const bool is_selected = (m_currentMagFilter == n);
				if (ImGui::Selectable(samplerFilters[n], is_selected))
				{
					m_currentMagFilter = n;
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
				const bool is_selected = (m_currentWrapS == n);
				if (ImGui::Selectable(wrapModes[n], is_selected))
				{
					selectedWrapS = GetWrapModeFromID(n);
					m_currentWrapS = n;
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
				const bool is_selected = (m_currentWrapR == n);
				if (ImGui::Selectable(wrapModes[n], is_selected))
				{
					selectedWrapS = GetWrapModeFromID(n);
					m_currentWrapR = n;
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
				const bool is_selected = (m_currentWrapT == n);
				if (ImGui::Selectable(wrapModes[n], is_selected))
				{
					selectedWrapS = GetWrapModeFromID(n);
					m_currentWrapT = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		// Apply button
		if (ImGui::Button("Apply"))
		{
			params.anisotropy = m_currentAnisotropy;
			params.textureParams.generateMipMaps = m_currentGenerateMips;
			params.textureParams.internalPixelFormat = selectedInternalPF;
			params.textureParams.pixelFormat = selectedPF;
			params.textureParams.minFilter = selectedMinFilter;
			params.textureParams.magFilter = selectedMagFilter;
			params.textureParams.wrapR = selectedWrapR;
			params.textureParams.wrapS = selectedWrapS;
			params.textureParams.wrapT = selectedWrapT;
			Graphics::SamplerParameters newParams = params;
			m_renderEngine->UnloadTextureResource(m_SelectedTexture->GetID());
			m_SelectedTexture = &m_renderEngine->CreateTexture2D(m_SelectedTexturePath, newParams);
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
		Graphics::MeshParameters params = m_selectedMesh->GetParameters();

		ImGui::Checkbox("Triangulate", &m_currentMeshParams.triangulate);
		ImGui::Checkbox("Generate Smooth Normals", &m_currentMeshParams.smoothNormals);
		ImGui::Checkbox("Calculate Tangent Space", &m_currentMeshParams.calculateTangentSpace);

		if (ImGui::Button("Apply"))
		{
			Graphics::MeshParameters params = m_currentMeshParams;
			m_renderEngine->UnloadMeshResource(m_selectedMeshID);
			m_selectedMesh = &m_renderEngine->CreateMesh(m_selectedMeshID, m_selectedMeshPath, params);
		}
	}
	void PropertiesPanel::DrawMaterialProperties()
	{
		ImGui::Text("test");
	}
}