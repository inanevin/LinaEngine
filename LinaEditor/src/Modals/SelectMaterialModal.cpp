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

#include "Modals/SelectMaterialModal.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imgui.h"

namespace LinaEditor
{
	void SelectMaterialModal::Draw(const std::map<int, LinaEngine::Graphics::Material>& map, int* selectedMatID, std::string& selectedMatPath)
	{
		ImGui::BeginChild("SelectMeshModalChild", ImVec2(0, 300), true);

		static int selected = -1;
		static std::string selectedPath = "";
		for (std::map<int, LinaEngine::Graphics::Material>::const_iterator it = map.begin(); it != map.end(); it++)
		{
			const std::string& path = it->second.GetPath();

			if (path.compare(INTERNAL_MAT_PATH) == 0) continue;

			WidgetsUtility::IncrementCursorPosY(5);
			WidgetsUtility::IncrementCursorPosX(5);

			if (ImGui::Selectable(path.c_str(), selected == it->second.GetID()))
			{
				selected = it->second.GetID();
				selectedPath = it->second.GetPath();
			}
		}

		ImGui::EndChild();
		WidgetsUtility::IncrementCursorPosY(15);
		WidgetsUtility::IncrementCursorPosX(10);

		if (WidgetsUtility::Button("Add Selected", ImVec2(ImGui::GetWindowSize().x * 0.5f - 20, 0.0f)))
		{
			*selectedMatID = selected;
			selectedMatPath = selectedPath;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		WidgetsUtility::IncrementCursorPosX(10);
		if (WidgetsUtility::Button("Close", ImVec2(ImGui::GetWindowSize().x * 0.5f - 20, 0.0f)))
		{

			ImGui::CloseCurrentPopup();
		}
	}

}