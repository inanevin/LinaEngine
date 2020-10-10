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

Class: EditorPanel
Timestamp: 5/23/2020 4:16:19 PM

*/


#include "Panels/EditorPanel.hpp"
#include "Core/EditorCommon.hpp"
#include "Utility/Math/Color.hpp"
#include "imgui/imgui.h"

#define OFFSET 4

namespace LinaEditor
{
    void EditorPanel::DrawWindowRect()
    {
		ImVec2 min = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		ImVec2 max = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(), ImGui::GetWindowPos().y + ImGui::GetWindowHeight());
		ImGui::BeginChild("##ch");
		ImGui::PushClipRect(min, max, false);
		ImGui::GetWindowDrawList()->AddRect(min, max, ImGui::ColorConvertFloat4ToU32(ImVec4(HEADER_BG_COLOR.r, HEADER_BG_COLOR.g, HEADER_BG_COLOR.b, HEADER_BG_COLOR.a)), 0, 15, OFFSET);
		ImGui::PopClipRect();
		ImGui::EndChild();
    }
}