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

#include "Panels/ECSPanel.hpp"
#include "Panels/ScenePanel.hpp"
#include "Core/GUILayer.hpp"
#include "Utility/Log.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/Application.hpp"
#include "Input/InputEngine.hpp"

namespace LinaEditor
{
	using namespace LinaEngine::ECS;
	using namespace LinaEngine;

	void ECSPanel::Setup()
	{

	}

	void ECSPanel::Refresh()
	{
		m_selectedEntity = entt::null;
		EditorApplication::GetEditorDispatcher().DispatchAction<void*>(LinaEngine::Action::ActionType::Unselect, 0);
	}

	void ECSPanel::DrawEntityNode(int id, LinaEngine::ECS::ECSEntity entity)
	{
		LinaEngine::ECS::ECSRegistry& ecs = LinaEngine::Application::GetECSRegistry();
		LinaEngine::ECS::ECSEntityData& data = ecs.get<LinaEngine::ECS::ECSEntityData>(entity);
		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
		ImGuiTreeNodeFlags flags = data.m_children.size() == 0 ? leaf_flags : base_flags;

		if (entity == m_selectedEntity)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, data.m_name.c_str());


		if (ImGui::IsItemClicked())
		{
			m_selectedEntity = entity;
			EditorApplication::GetEditorDispatcher().DispatchAction<ECSEntity>(LinaEngine::Action::ActionType::EntitySelected, m_selectedEntity);
		}


		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload(ECS_MOVEENTITY, &entity, sizeof(ECSEntity));

			// Display preview 
			ImGui::Text(data.m_name.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ECS_MOVEENTITY))
			{
				IM_ASSERT(payload->DataSize == sizeof(ECSEntity));
				ecs.AddChildToEntity(entity, *(ECSEntity*)payload->m_data);
			}
			ImGui::EndDragDropTarget();
		}

		if (nodeOpen)
		{
			int counter = 0;
			for (ECSEntity child : data.m_children)
			{
				DrawEntityNode(counter, child);
					counter++;
			}
			ImGui::TreePop();
		}
	}

	void ECSPanel::Draw()
	{
		if (m_show)
		{
			LinaEngine::ECS::ECSRegistry& ecs = LinaEngine::Application::GetECSRegistry();

			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowBgAlpha(1.0f);

			if (ImGui::Begin(ECS_ID, &m_show, flags))
			{

				WidgetsUtility::DrawShadowedLine(5);

				// Statics.
				static char entityName[256] = "Entity";
				WidgetsUtility::WindowPadding(ImVec2(3, 4));

				// Handle Right Click.
				if (LinaEngine::Application::GetApp().GetActiveLevelExists() && ImGui::BeginPopupContextWindow())
				{
					if (ImGui::BeginMenu("Create"))
					{
						if (ImGui::MenuItem("Entity"))
						{
							m_selectedEntity = ecs.CreateEntity("Entity");
							EditorApplication::GetEditorDispatcher().DispatchAction<ECSEntity>(LinaEngine::Action::ActionType::EntitySelected, m_selectedEntity);
						}

						ImGui::EndMenu();
					}
					ImGui::EndPopup();
				}

				WidgetsUtility::PopStyleVar();
				WidgetsUtility::WindowPadding(ImVec2(0, 0));
				WidgetsUtility::FramePadding(ImVec2(0, 0));
				WidgetsUtility::IncrementCursorPosY(7);
				int entityCounter = 0;
				auto singleView = ecs.view<LinaEngine::ECS::ECSEntityData>();

				for (auto entity : singleView)
				{
					LinaEngine::ECS::ECSEntityData& data = ecs.get<LinaEngine::ECS::ECSEntityData>(entity);

					if (data.m_parent == entt::null)
						DrawEntityNode(entityCounter, entity);

					// Deselect.
					if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						EditorApplication::GetEditorDispatcher().DispatchAction<void*>(LinaEngine::Action::ActionType::Unselect, 0);
						m_selectedEntity = entt::null;
					}
					//WidgetsUtility::IncrementCursorPosY(2);

					entityCounter++;
				}

				WidgetsUtility::PopStyleVar();
				WidgetsUtility::PopStyleVar();
			}

			ImGuiID id = ImGui::GetWindowDockID();
			ImVec2 min = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPos().x, ImGui::GetWindowPos().y + ImGui::GetCursorPosY());
			ImVec2 max = ImVec2(min.x + ImGui::GetWindowSize().x, min.y + ImGui::GetWindowSize().y);
		

			if (ImGui::BeginDragDropTargetCustom(ImRect(min,max), id))
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ECS_MOVEENTITY))
				{
					IM_ASSERT(payload->DataSize == sizeof(ECSEntity));
					ECSEntity entity = *(ECSEntity*)payload->m_data;
					ecs.RemoveFromParent(entity);
				}
				ImGui::EndDragDropTarget();
			}


			if (m_selectedEntity != entt::null)
			{
				if (ImGui::IsKeyDown(LinaEngine::Input::InputCode::Key::LCTRL) && ImGui::IsKeyReleased(LinaEngine::Input::InputCode::D))
				{
					m_selectedEntity = ecs.CreateEntity(m_selectedEntity);
					EditorApplication::GetEditorDispatcher().DispatchAction<ECSEntity>(LinaEngine::Action::ActionType::EntitySelected, m_selectedEntity);
				}
			}
			ImGui::End();

		}
	}



}