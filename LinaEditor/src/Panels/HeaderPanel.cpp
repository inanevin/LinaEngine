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

Class: HeaderPanel
Timestamp: 10/8/2020 1:39:19 PM

*/


#include "Panels/HeaderPanel.hpp"
#include "Utility/Math/Vector.hpp"
#include "Utility/Math/Math.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Rendering/Window.hpp"
#include "Core/GUILayer.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "imgui.h"

ImVec4 menuBarBackground = ImVec4(0, 0, 0, 1);
ImVec2 resizeStartPos;
ImVec2 headerClickPos;
LinaEngine::Vector2 resizeStartSize;
LinaEngine::Graphics::Texture* logo;
bool appResizeActive;

#define RESIZE_THRESHOLD 10

namespace LinaEditor
{
	void HeaderPanel::Setup()
	{
		m_renderEngine = m_guiLayer->GetRenderEngine();
		m_appWindow = m_guiLayer->GetAppWindow();

		// Logo texture
		logo = &m_renderEngine->CreateTexture2D(LinaEngine::Utility::GetUniqueID(), "resources/textures/linaEngineText.png");

	}

	void HeaderPanel::Draw()
	{
		if (m_show)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();

			// Handle app window resize.
			bool horizontalResize = Math::Abs(ImGui::GetMousePos().x - viewport->Size.x) < RESIZE_THRESHOLD;
			bool verticalResize = Math::Abs(ImGui::GetMousePos().y - viewport->Size.y) < RESIZE_THRESHOLD;

			if (horizontalResize && !verticalResize)
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
			}
			else if (verticalResize && !horizontalResize)
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
			}
			else if (verticalResize && horizontalResize)
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
			}

			if (horizontalResize || verticalResize || appResizeActive)
			{
				if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
				{
					appResizeActive = true;
					ImVec2 delta = ImVec2(ImGui::GetMousePos().x - resizeStartPos.x, ImGui::GetMousePos().y - resizeStartPos.y);

					m_appWindow->SetSize(Vector2(resizeStartSize.x + delta.x, resizeStartSize.y + delta.y));
				}
				else
				{
					resizeStartSize = m_appWindow->GetSize();
					resizeStartPos = ImGui::GetMousePos();
					appResizeActive = false;
				}
			}
			ImGui::SetNextWindowPos(ImVec2(viewport->GetWorkPos().x, viewport->GetWorkPos().y));
			ImGui::SetNextWindowSize(ImVec2(viewport->GetWorkSize().x, 80));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, menuBarBackground);
			ImGui::Begin("Header", NULL, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);


			// Handle window movement.
			if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				headerClickPos = ImGui::GetMousePos();

				ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
				Vector2 windowPos = m_appWindow->GetPos();
				Vector2 newPos = Vector2(windowPos.x + delta.x, windowPos.y + delta.y);

				if (newPos.x < 0.0f)
					newPos.x = 0.0f;

				if (newPos.y < 0.0f)
					newPos.y = 0.0f;

				m_appWindow->SetPos(newPos);


			}

			ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - 115);
			ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 15);
			ImGui::Image((void*)logo->GetID(), ImVec2(230, 27), ImVec2(0, 1), ImVec2(1, 0));

			ImGui::End();
			ImGui::PopStyleColor();
		}
	}

}