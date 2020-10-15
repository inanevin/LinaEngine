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
#include "Core/GUILayer.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"
#include <imgui/imguizmo/ImGuizmo.h>
#include "Input/InputMappings.hpp"
#include "Rendering/RenderEngine.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "Widgets/WidgetsUtility.hpp"

static ImGuizmo::OPERATION currentTransformGizmoOP = ImGuizmo::OPERATION::TRANSLATE;
static ImGuizmo::MODE currentTransformGizmoMode = ImGuizmo::MODE::LOCAL;
static Matrix gridLineMatrix = Matrix::Identity();
static Matrix modelMatrix = Matrix::Identity();
static ImVec2 previousWindowSize;
#define GRID_SIZE 1000

namespace LinaEditor
{

	void ScenePanel::Draw(float frameTime)
	{
		ImGuizmo::BeginFrame();

		if (m_show)
		{

			// Set window properties.

			ImVec2 panelSize = ImVec2(m_Size.x, m_Size.y);
			ImGui::SetNextWindowSize(panelSize, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

			if (ImGui::Begin("Scene", &m_show, flags))
			{
				if (m_renderEngine->GetCameraSystem()->GetCurrentCameraComponent() == nullptr)
				{
					ImGui::Text("NO CAMERA AVAILABLE");
				}

				ImGui::BeginChild("finalImage");
				WidgetsUtility::DrawShadowedLine(5);

				
				// Get game viewport aspect.
				Vector2 vpSize = m_renderEngine->GetViewportSize();
				float aspect = (float)vpSize.x / (float)vpSize.y;

				// Mins & max for scene panel area.
				float currentWindowX = ImGui::GetWindowSize().x;
				float currentWindowY = ImGui::GetWindowSize().y;
				ImVec2 pMin = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
				ImVec2 pMax = ImVec2(ImGui::GetCursorScreenPos().x + currentWindowX - 12, ImGui::GetCursorScreenPos().y + currentWindowY - 42);
				ImVec2 size = ImGui::GetCurrentWindow()->Size;
				ImVec2 currentPanelSize = ImVec2(pMax.x - pMin.x, pMax.y - pMin.y);

				// Resize scene panel.
				if ((size.x != previousWindowSize.x || size.y != previousWindowSize.y))
				{
					//m_RenderEngine->SetViewportDisplay(Vector2(0,0), Vector2((int)(512), (int)(512)));
					//m_RenderEngine->OnWindowResized((uint32)ImGui::GetCurrentWindow()->Size.x, (uint32)ImGui::GetCurrentWindow()->Size.y);
					previousWindowSize = size;
				}

				// Desired window height.
				float desiredHeight = currentPanelSize.x / aspect;


				// Calculate desired drawing rect for the image.
				ImVec2 imageRectMin = ImVec2(pMin.x, pMin.y + (currentPanelSize.y - desiredHeight) / 2.0f);
				ImVec2 imageRectMax = ImVec2(pMax.x, pMax.y - (currentPanelSize.y - desiredHeight) / 2.0f);


				if (m_drawMode == DrawMode::FinalImage)
					ImGui::GetWindowDrawList()->AddImage((void*)m_renderEngine->GetFinalImage(), imageRectMin, imageRectMax, ImVec2(0, 1), ImVec2(1, 0));
				else if (m_drawMode == DrawMode::ShadowMap)
					ImGui::GetWindowDrawList()->AddImage((void*)m_renderEngine->GetShadowMapImage(), imageRectMin, imageRectMax, ImVec2(0, 1), ImVec2(1, 0));


				ImGuiIO& io = ImGui::GetIO();
				ImGuizmo::Enable(true);
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(imageRectMin.x, imageRectMin.y, imageRectMax.x - imageRectMin.x, imageRectMax.y - imageRectMin.y);
				ImGui::PushClipRect(imageRectMin, imageRectMax, false);

				// Handle inputs.
				ProcessInput();

				// Draw Gizmos
				DrawGizmos();

				ImGui::EndChild();

			}

			ImGui::End();
		}
	}

	void ScenePanel::Setup()
	{
		m_renderEngine = m_guiLayer->GetRenderEngine();

	}


	void ScenePanel::ProcessInput()
	{

		if (ImGui::IsKeyPressed(LINA_KEY_Q))
			currentTransformGizmoOP = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(LINA_KEY_E))
			currentTransformGizmoOP = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(LINA_KEY_R))
			currentTransformGizmoOP = ImGuizmo::SCALE;
		if (ImGui::IsKeyPressed(LINA_KEY_T))
			currentTransformGizmoMode = currentTransformGizmoMode == ImGuizmo::MODE::WORLD ? ImGuizmo::MODE::LOCAL : ImGuizmo::MODE::WORLD;
	}

	void ScenePanel::DrawGizmos()
	{
		Matrix& view = m_renderEngine->GetCameraSystem()->GetViewMatrix();
		Matrix& projection = m_renderEngine->GetCameraSystem()->GetProjectionMatrix();

		//ImGui::GetWindowDrawList()->AddLine(ImVec2(coord.x, coord.y), ImVec2(coord2.x, coord2.y), col, 2);
		if (m_SelectedTransform != nullptr)
		{
			// Get required matrices.
			Matrix object = m_SelectedTransform->transform.ToMatrix();

			// Draw transformation handle.
			ImGuizmo::Manipulate(&view[0][0], &projection[0][0], currentTransformGizmoOP, currentTransformGizmoMode, &object[0][0], NULL, NULL, NULL, NULL);
			float matrixTranslation[3], matrixRotation[3], matrixScale[3];
			ImGuizmo::DecomposeMatrixToComponents(&object[0][0], matrixTranslation, matrixRotation, matrixScale);

			// Set object transformation back.
			m_SelectedTransform->transform.m_location = Vector3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
			m_SelectedTransform->transform.m_scale = Vector3(matrixScale[0], matrixScale[1], matrixScale[2]);
			m_SelectedTransform->transform.m_rotation = Quaternion::Euler(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
		}

		// ImGuizmo::DrawGrid(&view[0][0], &projection[0][0], &gridLineMatrix[0][0], GRID_SIZE);

	}

}