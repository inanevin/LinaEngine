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

#include "Drawers/TextureDrawer.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Core/Application.hpp"
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

#define CURSORPOS_X_LABELS 12
#define CURSORPOS_XPERC_VALUES 0.42f
#define CURSORPOS_XPERC_VALUES2 

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

	void TextureDrawer::DrawSelectedTexture()
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

		float cursorPosValues = ImGui::GetWindowSize().x * 0.5f;
		float targetWidth = ImGui::GetWindowWidth() - cursorPosValues - 20;

		WidgetsUtility::IncrementCursorPos(ImVec2(11, 11));
		WidgetsUtility::AlignedText("Generate Mipmaps?");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::Checkbox("##generateMipmaps", &m_currentGenerateMips);

		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);
		WidgetsUtility::AlignedText("Anisotropy");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::SetNextItemWidth(targetWidth);
		ImGui::DragInt("##anisotropy", &m_currentAnisotropy, 0.05f, 0, 8);

		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);
		WidgetsUtility::AlignedText("Internal Pixel Format");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::SetNextItemWidth(targetWidth);

		// Internal Pixel Format ComboBox
		if (ImGui::BeginCombo("##internalpixelformat", internalPFLabel, flags))
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

		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::AlignedText("Pixel Format");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::SetNextItemWidth(targetWidth);

		// Pixel Format ComboBox
		if (ImGui::BeginCombo("##pixelformat", pfLabel, flags))
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

		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::AlignedText("Min Filter");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::SetNextItemWidth(targetWidth);

		// Mýn Filter ComboBox
		if (ImGui::BeginCombo("##minfilter", minFilterLabel, flags))
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

		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::AlignedText("Mag Filter");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::SetNextItemWidth(targetWidth);

		// Mýn Filter ComboBox
		if (ImGui::BeginCombo("##magfilter", magFilterLabel, flags))
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


		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::AlignedText("Wrap S");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::SetNextItemWidth(targetWidth);

		// Wrap S ComboBox
		if (ImGui::BeginCombo("##wrapS", wrapSLabel, flags))
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

		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::AlignedText("Wrap R");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::SetNextItemWidth(targetWidth);

		// Wrap T ComboBox
		if (ImGui::BeginCombo("##wrapR", wrapRLabel, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(wrapModes); n++)
			{
				const bool is_selected = (m_currentWrapR == n);
				if (ImGui::Selectable(wrapModes[n], is_selected))
				{
					selectedWrapR = GetWrapModeFromID(n);
					m_currentWrapR = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		WidgetsUtility::IncrementCursorPosX(11);
		WidgetsUtility::AlignedText("Wrap T");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::SetNextItemWidth(targetWidth);

		// Wrap T ComboBox
		if (ImGui::BeginCombo("##wrapT", wrapTLabel, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(wrapModes); n++)
			{
				const bool is_selected = (m_currentWrapT == n);
				if (ImGui::Selectable(wrapModes[n], is_selected))
				{
					selectedWrapT = GetWrapModeFromID(n);
					m_currentWrapT = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		WidgetsUtility::IncrementCursorPosX(11);

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
			LinaEngine::Application::GetRenderEngine().UnloadTextureResource(m_selectedTexture->GetID());
			m_selectedTexture = &LinaEngine::Application::GetRenderEngine().CreateTexture2D(path, newParams);
		}

		
		WidgetsUtility::IncrementCursorPosY(12);
		WidgetsUtility::DrawBeveledLine();

		Vector2 textureSize = m_selectedTexture->GetSize();

		WidgetsUtility::IncrementCursorPosY(12);
		WidgetsUtility::IncrementCursorPosX(12);
		ImGui::Text("Preview");
		WidgetsUtility::IncrementCursorPosX(12);
		ImGui::Text("Size: ");
		ImGui::SameLine();
		ImGui::Text(textureSize.ToString().c_str());	
		WidgetsUtility::IncrementCursorPosY(25);

		// Setup data for drawing texture.
		float currentWindowX = ImGui::GetWindowWidth();

		if (currentWindowX > TEXTUREPREVIEW_MAX_WIDTH)
			currentWindowX = TEXTUREPREVIEW_MAX_WIDTH;

		float currentWindowY = ImGui::GetWindowHeight();
		float textureAspect = textureSize.x / textureSize.y;
		float desiredH = currentWindowX / textureAspect;
		ImVec2 pMin = ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() / 2.0f - currentWindowX / 2.0f, ImGui::GetCursorScreenPos().y);
		ImVec2 pMax = ImVec2(pMin.x + currentWindowX, ImGui::GetCursorScreenPos().y + desiredH);

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