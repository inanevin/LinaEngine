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

namespace LinaEditor
{
	void SelectComponentModal::Draw(const std::vector<EntityDrawer::ComponentTypes>& types)
	{
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
      
        WidgetsUtility::FramePaddingX(3);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("ChildR", ImVec2(0, 400), true, window_flags);

        ImGui::Columns(2);
        for (int i = 0; i < 100; i++)
        {
            char buf[32];
            sprintf(buf, "%03d", i);
            ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
            ImGui::NextColumn();
        }
        ImGui::EndChild();
        WidgetsUtility::PopStyleVar();
        ImGui::PopStyleVar();
	}
}