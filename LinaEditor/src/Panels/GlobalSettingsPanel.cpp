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

#include "Panels/GlobalSettingsPanel.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/EditorCommon.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Core/Application.hpp"
#include "Rendering/RenderSettings.hpp"
#include "imgui/imgui.h"
#include "IconsFontAwesome5.h"

namespace LinaEditor
{
#define CURSORPOS_X_LABELS 12
#define CURSORPOS_XPERC_VALUES 0.20f

    void GlobalSettingsPanel::Setup()
    {

    }

    void GlobalSettingsPanel::Draw()
    {
		if (m_show)
		{
			float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
			float cursorPosLabels = CURSORPOS_X_LABELS;

			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowBgAlpha(1.0f);

			ImGui::Begin(GLOBALSETTINGS_ID, &m_show, flags);

			LinaEngine::Graphics::RenderSettings& renderSettings = LinaEngine::Application::GetRenderEngine().GetRenderSettings();

			// Shadow.
			WidgetsUtility::DrawShadowedLine(5);
			WidgetsUtility::FramePaddingY(0);
			WidgetsUtility::IncrementCursorPosY(12);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Lighting");

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("FXAA");

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Enabled");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##fxaaEnabled", &renderSettings.m_fxaaEnabled);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Reduce Min");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##fxaaReduceMin", &renderSettings.m_fxaaReduceMin, 0.05f);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Reduce Mul");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##fxaaReduceMul", &renderSettings.m_fxaaReduceMul, 0.05f);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Span Max");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##fxaaSpanMax", &renderSettings.m_fxaaSpanMax, 0.05f);

			WidgetsUtility::IncrementCursorPosY(6);

			WidgetsUtility::DrawBeveledLine();
			WidgetsUtility::IncrementCursorPosY(6);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Bloom");

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Enabled");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##bloomEnabled", &renderSettings.m_bloomEnabled);

			WidgetsUtility::IncrementCursorPosY(6);

			WidgetsUtility::DrawBeveledLine();
			WidgetsUtility::IncrementCursorPosY(6);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Post FX General");

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Gamma");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##gamma", &renderSettings.m_gamma, 0.05f);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Exposure");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##exposure", &renderSettings.m_exposure, 0.05f);

			WidgetsUtility::IncrementCursorPosY(6);
			WidgetsUtility::DrawBeveledLine();
			WidgetsUtility::IncrementCursorPosY(6);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Vignette Enabled");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##vigEnabled", &renderSettings.m_vignetteEnabled);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Vignette Amount");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##vigmat", &renderSettings.m_vignetteAmount, 0.05f);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Vignette Pow");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##vigpow", &renderSettings.m_vignettePow, 0.05f);

			WidgetsUtility::IncrementCursorPosY(6);
			WidgetsUtility::DrawBeveledLine();
			WidgetsUtility::IncrementCursorPosY(6);

			LinaEngine::Application::GetRenderEngine().UpdateRenderSettings();
			ImGui::SetCursorPosX(cursorPosLabels);

			if (ImGui::Button("Save Settings"))
				LinaEngine::Graphics::RenderSettings::SerializeRenderSettings(renderSettings, RENDERSETTINGS_FOLDERPATH, RENDERSETTINGS_FILE);

			ImGui::PopStyleVar();
			ImGui::End();


		}
    }
}