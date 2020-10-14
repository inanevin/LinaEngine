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

Class: SelectComponentModal
Timestamp: 10/12/2020 12:45:51 AM

*/


#include "Modals/SelectComponentModal.hpp"
#include "imgui/imgui.h"
#include "IconsFontAwesome5.h"
#include "IconsForkAwesome.h"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"

#define MAX_SELECTABLE_COMPONENTS 100

namespace LinaEditor
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