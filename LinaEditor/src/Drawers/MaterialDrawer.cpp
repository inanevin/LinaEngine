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

#include "Drawers/MaterialDrawer.hpp"
#include "Rendering/Material.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imgui.h"
#include "IconsFontAwesome5.h"

namespace LinaEditor
{
#define CURSORPOS_X_LABELS 30
#define CURSORPOS_XPERC_VALUES 0.52f

	void MaterialDrawer::SetSelectedMaterial(LinaEngine::Graphics::Material& mat)
	{
		m_selectedMaterial = &mat;
	}

	void MaterialDrawer::DrawSelectedMaterial()
	{
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;

		WidgetsUtility::IncrementCursorPos(ImVec2(11, 11));

		static bool foldoutOpenGeneral = false;
		const char* caret = foldoutOpenGeneral ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT;
		if (WidgetsUtility::IconButtonNoDecoration(caret, 30, 0.8f))
			foldoutOpenGeneral = !foldoutOpenGeneral;

		// Title.
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::Text("General Settings");
		ImGui::AlignTextToFramePadding();
		ImGui::SameLine();

		if (foldoutOpenGeneral)
		{
			WidgetsUtility::IncrementCursorPosY(24);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Uses HDRI");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##useshdri", &m_selectedMaterial->m_usesHDRI);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Receives Lighting");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##receiveslighting", &m_selectedMaterial->m_receivesLighting);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Is Shadow Mapped");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##isshadowmapped", &m_selectedMaterial->m_isShadowMapped);
		}
	}
}