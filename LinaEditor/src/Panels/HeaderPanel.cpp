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
#include "IconsFontAwesome5.h"

ImVec4 headerBGColor = ImVec4(0, 0, 0, 1);
ImVec4 headerButtonsColor = ImVec4(1, 1, 1, 1); // ImVec4(113.f / 255.f, 36.f / 255.f, 78.f / 255.f, 1);
ImVec2 resizeStartPos;
ImVec2 headerClickPos;
ImVec2 linaLogoSize = ImVec2(160, 18);
LinaEngine::Vector2 resizeStartSize;
LinaEngine::Graphics::Texture* windowLogo;
LinaEngine::Graphics::Texture* windowIcon;
bool appResizeActive;

#define RESIZE_THRESHOLD 10

namespace LinaEditor
{
	void HeaderPanel::Setup()
	{
		m_renderEngine = m_guiLayer->GetRenderEngine();
		m_appWindow = m_guiLayer->GetAppWindow();

		// Logo texture
		windowLogo = &m_renderEngine->CreateTexture2D(LinaEngine::Utility::GetUniqueID(), "resources/textures/linaEngineText.png");
		windowIcon = &m_renderEngine->CreateTexture2D(LinaEngine::Utility::GetUniqueID(), "resources/textures/linaEngineIcon.png");
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

			// Start drawing window.
			ImGui::SetNextWindowPos(ImVec2(viewport->GetWorkPos().x, viewport->GetWorkPos().y));
			ImGui::SetNextWindowSize(ImVec2(viewport->GetWorkSize().x, 80));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, headerBGColor);
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

			// Icon
			ImGui::Image((void*)windowIcon->GetID(), ImVec2(16, 16), ImVec2(0, 1), ImVec2(1, 0));

			// Title
			ImGui::SameLine();
			ImGui::Text(m_title.c_str());

			// Minimize, maximize, exit buttons.
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 105);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - linaLogoSize.y / 2.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, headerBGColor);
			ImGui::PushStyleColor(ImGuiCol_Text, headerButtonsColor);
		
			if (ImGui::Button(ICON_FA_WINDOW_MINIMIZE))
			{
				m_appWindow->Iconify();
			}

			ImGui::SameLine();

			if (ImGui::Button(ICON_FA_WINDOW_MAXIMIZE))
			{
				m_appWindow->Maximize();
			}

			ImGui::SameLine();

			if (ImGui::Button(ICON_FA_WINDOW_CLOSE))
			{
				m_appWindow->Close();
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

			// Logo
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - linaLogoSize.x / 2.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPos().y + linaLogoSize.y / 2.0f + 15);
			ImGui::Image((void*)windowLogo->GetID(), linaLogoSize, ImVec2(0, 1), ImVec2(1, 0));

		
			ImGui::End();
			ImGui::PopStyleColor();
		}
	}

}