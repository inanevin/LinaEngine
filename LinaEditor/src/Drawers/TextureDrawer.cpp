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

Class: TextureDrawer
Timestamp: 10/16/2020 6:27:33 PM

*/


#include "Drawers/TextureDrawer.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Rendering/Texture.hpp"

using namespace LinaEngine::Graphics;

namespace LinaEditor
{
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

	static PixelFormat selectedInternalPF;
	static PixelFormat selectedPF;
	static SamplerFilter selectedMinFilter;
	static SamplerFilter selectedMagFilter;
	static SamplerWrapMode selectedWrapS;
	static SamplerWrapMode selectedWrapR;
	static SamplerWrapMode selectedWrapT;

	const int TEXTUREPREVIEW_MAX_WIDTH = 250;


	void TextureDrawer::SetSelectedTexture(LinaEngine::Graphics::Texture* texture)
	{
		m_selectedTexture = texture;

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

	void TextureDrawer::DrawTextureProperties()
	{
		SamplerParameters& params = m_selectedTexture->GetSampler().GetSamplerParameters();
		static ImGuiComboFlags flags = 0;
		static PixelFormat selectedInternalPF = params.textureParams.internalPixelFormat;
		static PixelFormat selectedPF = params.textureParams.pixelFormat;
		static SamplerFilter selectedMinFilter = params.textureParams.minFilter;
		static SamplerFilter selectedMagFilter = params.textureParams.magFilter;
		static SamplerWrapMode selectedWrapS = params.textureParams.wrapS;
		static SamplerWrapMode selectedWrapR = params.textureParams.wrapR;
		static SamplerWrapMode selectedWrapT = params.textureParams.wrapT;
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
					selectedInternalPF = (PixelFormat)n;
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
					selectedPF = (PixelFormat)n;
					m_currentPixelFormat = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		// Mýn Filter ComboBox
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

		// Mýn Filter ComboBox
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
			SamplerParameters newParams = params;
			std::string path = m_selectedTexture->GetPath();
			m_renderEngine->UnloadTextureResource(m_selectedTexture->GetID());
			m_selectedTexture = &m_renderEngine->CreateTexture2D(path, newParams);
		}

		// Setup data for drawing texture.
		float currentWindowX = ImGui::GetCurrentWindow()->Size.x;

		if (currentWindowX > TEXTUREPREVIEW_MAX_WIDTH)
			currentWindowX = TEXTUREPREVIEW_MAX_WIDTH;

		float currentWindowY = ImGui::GetCurrentWindow()->Size.y;
		Vector2 textureSize = m_selectedTexture->GetSize();
		float textureAspect = textureSize.x / textureSize.y;
		float desiredH = currentWindowX / textureAspect;
		ImVec2 pMin = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
		ImVec2 pMax = ImVec2(ImGui::GetCursorScreenPos().x + currentWindowX, ImGui::GetCursorScreenPos().y + desiredH);
		ImVec2 size = ImGui::GetCurrentWindow()->Size;

		// Draw texture
		ImGui::GetWindowDrawList()->AddImage((void*)m_selectedTexture->GetID(), pMin, pMax, ImVec2(0, 1), ImVec2(1, 0));

	}

	int TextureDrawer::GetSamplerFilterID(SamplerFilter filter)
	{
		if (filter == SamplerFilter::FILTER_NEAREST)
			return 0;
		else if (filter == SamplerFilter::FILTER_LINEAR)
			return 1;
		else if (filter == SamplerFilter::FILTER_NEAREST_MIPMAP_NEAREST)
			return 2;
		else if (filter == SamplerFilter::FILTER_LINEAR_MIPMAP_NEAREST)
			return 3;
		else if (filter == SamplerFilter::FILTER_NEAREST_MIPMAP_LINEAR)
			return 4;
		else if (filter == SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR)
			return 5;
	}

	int TextureDrawer::GetWrapModeID(SamplerWrapMode wrapMode)
	{
		if (wrapMode == SamplerWrapMode::WRAP_CLAMP_EDGE)
			return 0;
		else if (wrapMode == SamplerWrapMode::WRAP_CLAMP_MIRROR)
			return 1;
		else if (wrapMode == SamplerWrapMode::WRAP_CLAMP_BORDER)
			return 2;
		else if (wrapMode == SamplerWrapMode::WRAP_REPEAT)
			return 3;
		else if (wrapMode == SamplerWrapMode::WRAP_REPEAT_MIRROR)
			return 4;
	}

	SamplerFilter TextureDrawer::GetSamplerFilterFromID(int id)
	{
		if (id == 0)
			return SamplerFilter::FILTER_NEAREST;
		else if (id == 1)
			return SamplerFilter::FILTER_LINEAR;
		else if (id == 2)
			return SamplerFilter::FILTER_NEAREST_MIPMAP_NEAREST;
		else if (id == 3)
			return SamplerFilter::FILTER_LINEAR_MIPMAP_NEAREST;
		else if (id == 4)
			return SamplerFilter::FILTER_NEAREST_MIPMAP_LINEAR;
		else if (id == 5)
			return SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
	}

	SamplerWrapMode TextureDrawer::GetWrapModeFromID(int id)
	{
		if (id == 0)
			return SamplerWrapMode::WRAP_CLAMP_EDGE;
		else if (id == 1)
			return SamplerWrapMode::WRAP_CLAMP_MIRROR;
		else if (id == 2)
			return SamplerWrapMode::WRAP_CLAMP_BORDER;
		else if (id == 2)
			return SamplerWrapMode::WRAP_REPEAT;
		else if (id == 4)
			return SamplerWrapMode::WRAP_REPEAT_MIRROR;
	}
}