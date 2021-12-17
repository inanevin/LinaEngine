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
#include "Log/Log.hpp"
#include "Panels/ScenePanel.hpp"
#include "Core/GUILayer.hpp"
#include "Core/InputMappings.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/InputBackend.hpp"
#include "Core/Application.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "imgui/imgui.h"
#include <imgui/imguizmo/ImGuizmo.h>

static ImGuizmo::OPERATION currentTransformGizmoOP = ImGuizmo::OPERATION::TRANSLATE;
static ImGuizmo::MODE currentTransformGizmoMode = ImGuizmo::MODE::WORLD;
static Matrix gridLineMatrix = Matrix::Identity();
static Matrix modelMatrix = Matrix::Identity();
static ImVec2 previousWindowSize;
#define GRID_SIZE 1000

namespace Lina::Editor
{
	bool scenePanelFirstRun = false;

	void ScenePanel::Initialize()
	{
		Lina::Event::EventSystem::Get()->Connect<EEntityUnselected, &ScenePanel::Unselected>(this);
		Lina::Event::EventSystem::Get()->Connect<EEntitySelected, &ScenePanel::EntitySelected>(this);
		Lina::Event::EventSystem::Get()->Connect<Event::ELevelUninstalled, &ScenePanel::LevelUninstalled>(this);
		Lina::Event::EventSystem::Get()->Connect<ETransformGizmoChanged, &ScenePanel::OnTransformGizmoChanged>(this);
		Lina::Event::EventSystem::Get()->Connect<ETransformPivotChanged, &ScenePanel::OnTransformPivotChanged>(this);
	}

	void ScenePanel::Draw()
	{
		ImGuizmo::BeginFrame();

		if (m_show)
		{

			Lina::Graphics::RenderEngineBackend* renderEngine = Lina::Graphics::RenderEngineBackend::Get();
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
			ImGui::SetNextWindowBgAlpha(1.0f);

			if (ImGui::Begin(SCENE_ID, NULL, flags))
			{
				WidgetsUtility::CloseWindowTabPopup(&m_show);
				ImVec2 sceneWindowPos = WidgetsUtility::GetWindowPosWithContentRegion();
				ImVec2 sceneWindowSize = WidgetsUtility::GetWindowSizeWithContentRegion();

				if (!scenePanelFirstRun)
				{
					scenePanelFirstRun = true;
					Lina::Graphics::RenderEngineBackend::Get()->SetViewportDisplay(Vector2(0, 0), Vector2((int)(sceneWindowSize.x), (int)(sceneWindowSize.y)));
				}

				// Set Focus
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImVec2 min = ImVec2(sceneWindowPos.x, sceneWindowPos.y);
					ImVec2 max = ImVec2(min.x + sceneWindowSize.x, min.y + sceneWindowSize.y);

					if (ImGui::IsMouseHoveringRect(min, max))
						m_isFocused = true;
					else
						m_isFocused = false;
				}

				ImGui::BeginChild("finalImage");

				// Get game viewport aspect.
				Vector2 vpSize = renderEngine->GetViewportSize();
				float aspect = (float)vpSize.x / (float)vpSize.y;

				// Resize scene panel.
				if ((sceneWindowSize.x != previousWindowSize.x || sceneWindowSize.y != previousWindowSize.y))
				{
					Lina::Graphics::RenderEngineBackend::Get()->SetViewportDisplay(Vector2(0, 0), Vector2((int)(sceneWindowSize.x), (int)(sceneWindowSize.y)));
					previousWindowSize = sceneWindowSize;
				}

				// Desired window height.
				// Mins & max for scene panel area.
				// float currentWindowX = ImGui::GetWindowSize().x;
				// float currentWindowY = ImGui::GetWindowSize().y;
				// ImVec2 pMin = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
				// ImVec2 pMax = ImVec2(ImGui::GetCursorScreenPos().x + currentWindowX, ImGui::GetCursorScreenPos().y + currentWindowY);
				// ImVec2 currentPanelSize = ImVec2(pMax.x - pMin.x, pMax.y - pMin.y);
				// float desiredHeight = currentPanelSize.x / aspect;
				//ImVec2 imageRectMin = ImVec2(pMin.x, pMin.y + (currentPanelSize.y - desiredHeight) / 2.0f);
				//ImVec2 imageRectMax = ImVec2(pMax.x, pMax.y - (currentPanelSize.y - desiredHeight) / 2.0f);
				ImVec2 imageRectMin = sceneWindowPos;
				ImVec2 imageRectMax = ImVec2(sceneWindowPos.x + sceneWindowSize.x, sceneWindowPos.y + sceneWindowSize.y);

				if (m_drawMode == DrawMode::FinalImage)
					ImGui::GetWindowDrawList()->AddImage((void*)renderEngine->GetFinalImage(), imageRectMin, imageRectMax, ImVec2(0, 1), ImVec2(1, 0));
				else if (m_drawMode == DrawMode::ShadowMap)
					ImGui::GetWindowDrawList()->AddImage((void*)renderEngine->GetShadowMapImage(), imageRectMin, imageRectMax, ImVec2(0, 1), ImVec2(1, 0));

				if (Lina::Engine::Get()->GetPlayMode())
				{
					if (m_borderAlpha < 1.0f)
						m_borderAlpha = Math::Lerp(m_borderAlpha, 1.1f, Lina::Engine::Get()->GetRawDelta() * 1.2f);

					ImGui::GetWindowDrawList()->AddRect(imageRectMin, ImVec2(imageRectMax.x, imageRectMax.y - 4), ImGui::ColorConvertFloat4ToU32(ImVec4(0.33f, 0.54f, 0.78f, m_borderAlpha)), 0, 0, 5);
				}
				else
				{
					if (m_borderAlpha != 0.0f)
						m_borderAlpha = 0.0f;
				}


				if (renderEngine->GetCameraSystem()->GetActiveCameraComponent() == nullptr)
				{
					ImVec2 size = ImVec2(240, 100);
					ImVec2 pos = ImVec2(sceneWindowPos.x + sceneWindowSize.x / 2.0f - (size.x / 2.0f), sceneWindowPos.y + sceneWindowSize.y / 2.0f - (size.y / 2.0f));
					ImGuiWindowFlags noCamFlags = 0
						| ImGuiWindowFlags_NoDocking
						| ImGuiWindowFlags_NoTitleBar
						| ImGuiWindowFlags_NoResize
						| ImGuiWindowFlags_NoMove
						| ImGuiWindowFlags_NoScrollbar
						| ImGuiWindowFlags_NoSavedSettings;
				
				
					ImGui::SetNextWindowPos(pos);
					ImGui::SetNextWindowBgAlpha(0.8f);
					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4);
					ImGui::PushFont(GUILayer::GetBigFont());
					ImGui::BeginChild("##scenepanel_nocam", size, false, noCamFlags);
					const char* noCamAvab = "No camera available!";
					const char* latestViewMatrix = "(Using latest view matrix)";
					ImVec2 windowSize = ImGui::GetWindowSize();
					ImVec2 textSize1 = ImGui::CalcTextSize(noCamAvab);
					ImVec2 textSize2 = ImGui::CalcTextSize(latestViewMatrix);
					ImGui::SetCursorPosX((windowSize.x - textSize1.x) * 0.5f);
					ImGui::SetCursorPosY((windowSize.y - textSize1.y) * 0.5f - 10);
					ImGui::Text("No camera available!");
					ImGui::SetCursorPosX((windowSize.x - textSize2.x) * 0.5f);
					ImGui::SetCursorPosY((windowSize.y - textSize2.y) * 0.5f + 10);
					ImGui::Text(latestViewMatrix);
					ImGui::PopFont();
					ImGui::PopStyleVar();
					ImGui::EndChild();
				}

				ImGuiIO& io = ImGui::GetIO();
				ImGuizmo::Enable(true);
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(imageRectMin.x, imageRectMin.y, imageRectMax.x - imageRectMin.x, imageRectMax.y - imageRectMin.y);
				ImGui::PushClipRect(imageRectMin, imageRectMax, false);

				ProcessInput();
				DrawGizmos();
				ImGui::EndChild();

			}


			// Mesh drag & drop.
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMESH_ID))
				{
					IM_ASSERT(payload->DataSize == sizeof(uint32));

					auto* ecs = Lina::ECS::Registry::Get();
					auto& model = Lina::Graphics::Model::GetModel(*(uint32*)payload->Data);
					auto entity = ecs->CreateEntity(Utility::GetFileNameOnly(model.GetPath()));
					auto& mr = ecs->emplace<ECS::ModelRendererComponent>(entity);
					mr.SetModel(ecs, entity, model);

					auto& mat = Graphics::Material::GetMaterial("resources/engine/materials/DefaultLit.mat");

					for (int i = 0; i < model.GetMaterialSpecs().size(); i++)
						mr.SetMaterial(ecs, entity, i, mat);
				}
				ImGui::EndDragDropTarget();
			}


			ImGui::End();

		}
	}


	void ScenePanel::EntitySelected(EEntitySelected ev)
	{
		m_selectedTransform = ev.m_entity;
	}

	void ScenePanel::Unselected(EEntityUnselected ev)
	{
		m_selectedTransform = entt::null;
	}

	void ScenePanel::LevelUninstalled(Event::ELevelUninstalled ev)
	{
		Unselected(EEntityUnselected());
	}

	void ScenePanel::ProcessInput()
	{
		if (ImGui::IsWindowFocused())
		{
			if (ImGui::IsKeyPressed(LINA_KEY_Q))
				Event::EventSystem::Get()->Trigger<ETransformGizmoChanged>(ETransformGizmoChanged{ 0 });
			if (ImGui::IsKeyPressed(LINA_KEY_E))
				Event::EventSystem::Get()->Trigger<ETransformGizmoChanged>(ETransformGizmoChanged{ 1 });
			if (ImGui::IsKeyPressed(LINA_KEY_R))
				Event::EventSystem::Get()->Trigger<ETransformGizmoChanged>(ETransformGizmoChanged{ 2 });
			if (ImGui::IsKeyPressed(LINA_KEY_T))
				Event::EventSystem::Get()->Trigger<ETransformPivotChanged>(ETransformPivotChanged{ currentTransformGizmoMode == ImGuizmo::LOCAL });

			auto* inputEngine = Lina::Input::InputEngineBackend::Get();
			bool isInPlay = Lina::Engine::Get()->GetPlayMode();

			if (inputEngine->GetKey(Lina::Input::InputCode::LCTRL) && inputEngine->GetKeyDown(Lina::Input::InputCode::Space))
			{

				if (isInPlay)
				{
					Lina::Engine::Get()->SetPlayMode(false);
					Lina::Input::InputEngineBackend::Get()->SetCursorMode(Lina::Input::CursorMode::Visible);
					// unconfine mouse
				}
				else
				{
					Lina::Engine::Get()->SetPlayMode(true);
					Lina::Input::InputEngineBackend::Get()->SetCursorMode(Lina::Input::CursorMode::Disabled);
					// confine mouse
				}
			}

			if (isInPlay)
			{
				if (Lina::Input::InputEngineBackend::Get()->GetKeyDown(Lina::Input::InputCode::Escape))
				{
					Lina::Input::CursorMode currentMode = Lina::Input::InputEngineBackend::Get()->GetCursorMode();

					if (currentMode == Lina::Input::CursorMode::Visible)
					{
						Lina::Engine::Get()->SetPlayMode(false);
					}
					else
					{
						Lina::Input::InputEngineBackend::Get()->SetCursorMode(Lina::Input::CursorMode::Visible);
						// unconfine
					}
				}


				if (m_isFocused && inputEngine->GetMouseButtonDown(Lina::Input::InputCode::Mouse1))
				{
					Lina::Input::InputEngineBackend::Get()->SetCursorMode(Lina::Input::CursorMode::Disabled);
					// confine
				}
			}

		}

	}

	void ScenePanel::DrawGizmos()
	{
		Lina::Graphics::RenderEngineBackend* renderEngine = Lina::Graphics::RenderEngineBackend::Get();

		Matrix& view = renderEngine->GetCameraSystem()->GetViewMatrix();
		Matrix& projection = renderEngine->GetCameraSystem()->GetProjectionMatrix();

		//ImGui::GetWindowDrawList()->AddLine(ImVec2(coord.x, coord.y), ImVec2(coord2.x, coord2.y), col, 2);
		if (m_selectedTransform != entt::null)
		{
			ECS::EntityDataComponent& data = Lina::ECS::Registry::Get()->get<ECS::EntityDataComponent>(m_selectedTransform);
			// Get required matrices.
			glm::mat4 object = data.ToMatrix();


			// Draw transformation handle.
			ImGuizmo::Manipulate(&view[0][0], &projection[0][0], currentTransformGizmoOP, currentTransformGizmoMode, &object[0][0]);

			float matrixTranslation[3], matrixRotation[3], matrixScale[3];
			ImGuizmo::DecomposeMatrixToComponents(&object[0][0], matrixTranslation, matrixRotation, matrixScale);

			if (ImGuizmo::IsUsing())
			{

				glm::vec3 rot = data.GetRotationAngles();
				glm::vec3 deltaRotation = glm::vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]) - rot;
				data.SetRotationAngles(rot + deltaRotation);
			}

			data.SetLocation(Vector3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]));
			data.SetScale(Vector3(matrixScale[0], matrixScale[1], matrixScale[2]));

		}


		// ImGuizmo::DrawGrid(&view[0][0], &projection[0][0], &gridLineMatrix[0][0], GRID_SIZE);

	}

	void ScenePanel::OnTransformGizmoChanged(ETransformGizmoChanged ev)
	{
		if (ev.m_currentGizmo == 0)
			currentTransformGizmoOP = ImGuizmo::TRANSLATE;
		else if (ev.m_currentGizmo == 1)
			currentTransformGizmoOP = ImGuizmo::ROTATE;
		else if (ev.m_currentGizmo == 2)
			currentTransformGizmoOP = ImGuizmo::SCALE;
	}

	void ScenePanel::OnTransformPivotChanged(ETransformPivotChanged ev)
	{
		if (ev.m_isGlobal)
			currentTransformGizmoMode = ImGuizmo::MODE::WORLD;
		else
			currentTransformGizmoMode = ImGuizmo::MODE::LOCAL;
	}

}