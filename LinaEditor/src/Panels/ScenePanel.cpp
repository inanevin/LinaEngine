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

Class: ScenePanel
Timestamp: 6/5/2020 6:51:39 PM

*/


#include "Panels/ScenePanel.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "Rendering/RenderEngine.hpp"

namespace LinaEditor
{
	void ScenePanel::Draw()
	{
		if (m_Show)
		{
			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImVec2 panelSize = ImVec2(m_Size.x, m_Size.y);
			ImGui::SetNextWindowSize(panelSize, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(0.2f);

			if (ImGui::Begin("Resources Panel", &m_Show))
			{
				ImGui::GetWindowDrawList()->AddImage((void*)m_RenderEngine->GetFinalImage(), ImVec2(ImGui::GetCursorScreenPos()), ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetCurrentWindow()->Size.x, ImGui::GetCursorScreenPos().y + ImGui::GetCurrentWindow()->Size.y), ImVec2(0, 1), ImVec2(1, 0));

			}

			ImGui::End();
		}
	}

	void ScenePanel::Setup(LinaEngine::Graphics::RenderEngine& renderEngine)
	{
		m_RenderEngine = &renderEngine;
	}


}