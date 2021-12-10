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

#include "Modals/SelectComponentModal.hpp"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imgui.h"
#include "IconsFontAwesome5.h"
#include "IconsForkAwesome.h"

#define MAX_SELECTABLE_COMPONENTS 100

namespace Lina::Editor
{
	std::vector<std::string> SelectComponentModal::Draw(const std::vector<std::string>& types)
	{
		std::vector<std::string> v;

		ImGui::BeginChild("SelectComponentModalChild", ImVec2(0, 300), true);
		WidgetsUtility::IncrementCursorPosY(5);

		ImGui::Columns(2, NULL, false);

		static bool selection[MAX_SELECTABLE_COMPONENTS] = { false };

		for (int n = 0; n < types.size(); n++)
		{
			char buf[32];
			sprintf(buf, types[n].c_str(), n);
			if (ImGui::Selectable(buf, selection[n]))
			{
				if (!ImGui::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
					memset(selection, 0, sizeof(selection));
				selection[n] ^= 1;
			}

			ImGui::NextColumn();
		}

		ImGui::EndChild();
		WidgetsUtility::IncrementCursorPosY(15);
		WidgetsUtility::IncrementCursorPosX(10);

		if (WidgetsUtility::Button("Add Selected", ImVec2(ImGui::GetWindowSize().x * 0.5f - 20, 0.0f)))
		{
			for (int i = 0; i < types.size(); i++)
			{
				if (selection[i])
					v.push_back(types[i]);
			}

			// Clear selection
			memset(selection, 0, sizeof(selection));

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		WidgetsUtility::IncrementCursorPosX(10);
		if (WidgetsUtility::Button("Close", ImVec2(ImGui::GetWindowSize().x * 0.5f - 20, 0.0f)))
		{
			// Clear selection
			memset(selection, 0, sizeof(selection));

			ImGui::CloseCurrentPopup();
		}

		return v;
	}
}