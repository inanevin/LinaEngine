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
#include "Panels/LevelPanel.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/Material.hpp"
#include "Core/GUILayer.hpp"
#include "Log/Log.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/Application.hpp"
#include "Core/InputBackend.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace Lina::Editor
{
	using namespace Lina::ECS;
	using namespace Lina;

	void ECSPanel::Initialize(const char* id)
	{
		EditorPanel::Initialize(id);
		Lina::Event::EventSystem::Get()->Connect<Event::ELevelInstalled, &ECSPanel::OnLevelInstall>(this);
	}

	void ECSPanel::Refresh()
	{
		m_selectedEntity = entt::null;
		Lina::Event::EventSystem::Get()->Trigger<EEntityUnselected>(EEntityUnselected{});
	}

	void ECSPanel::DrawEntityNode(int id, Lina::ECS::Entity entity)
	{
		Lina::ECS::Registry* ecs = Lina::ECS::Registry::Get();
		Lina::ECS::EntityDataComponent& data = ecs->get<Lina::ECS::EntityDataComponent>(entity);
		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
		ImGuiTreeNodeFlags flags = data.m_children.size() == 0 ? leaf_flags : base_flags;

		if (entity == m_selectedEntity)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, data.m_name.c_str());


		if (ImGui::IsItemClicked())
		{
			m_selectedEntity = entity;
			Lina::Event::EventSystem::Get()->Trigger<EEntitySelected>(EEntitySelected{ m_selectedEntity });
		}


		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload(ECS_MOVEENTITY, &entity, sizeof(Entity));

			// Display preview 
			ImGui::Text(data.m_name.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ECS_MOVEENTITY))
			{
				IM_ASSERT(payload->DataSize == sizeof(Entity));
				ecs->AddChildToEntity(entity, *(Entity*)payload->Data);
			}
			ImGui::EndDragDropTarget();
		}

		if (nodeOpen)
		{
			int counter = 0;
			for (Entity child : data.m_children)
			{
				DrawEntityNode(counter, child);
				counter++;
			}
			ImGui::TreePop();
		}
	}

	void ECSPanel::OnLevelInstall(Event::ELevelInstalled ev)
	{
		Lina::Event::EventSystem::Get()->Trigger<EEntityUnselected>(EEntityUnselected{});
		m_selectedEntity = entt::null;
	}

	void ECSPanel::Draw()
	{
		if (m_show)
		{
			Lina::ECS::Registry* ecs = Lina::ECS::Registry::Get();
			
			Begin();

			// Statics.
			static char entityName[256] = "Entity";
			WidgetsUtility::WindowPadding(ImVec2(3, 4));

			// Handle Right Click.
			if (Lina::Application::Get().GetActiveLevelExists() && ImGui::BeginPopupContextWindow())
			{
				if (ImGui::BeginMenu("Create"))
				{
					if (ImGui::MenuItem("Entity"))
					{
						m_selectedEntity = ecs->CreateEntity("Entity");
						Lina::Event::EventSystem::Get()->Trigger<EEntitySelected>(EEntitySelected{ m_selectedEntity });
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
			auto singleView = ecs->view<Lina::ECS::EntityDataComponent>();

			for (auto entity : singleView)
			{
				Lina::ECS::EntityDataComponent& data = ecs->get<Lina::ECS::EntityDataComponent>(entity);

				if (data.m_parent == entt::null)
					DrawEntityNode(entityCounter, entity);

				// Deselect.
				if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					Lina::Event::EventSystem::Get()->Trigger<EEntityUnselected>(EEntityUnselected{});
					m_selectedEntity = entt::null;
				}

				entityCounter++;
			}

			WidgetsUtility::PopStyleVar();
			WidgetsUtility::PopStyleVar();



			if (m_selectedEntity != entt::null)
			{
				// Duplicate
				if (ImGui::IsKeyDown(Lina::Input::InputCode::Key::LCTRL) && ImGui::IsKeyReleased(Lina::Input::InputCode::D))
				{
					m_selectedEntity = ecs->CreateEntity(m_selectedEntity);
					Lina::Event::EventSystem::Get()->Trigger<EEntitySelected>(EEntitySelected{ m_selectedEntity });
				}

				// Delete
				if (ImGui::IsKeyReleased(Lina::Input::InputCode::Key::Delete) && ImGui::IsWindowFocused())
				{
					Lina::Event::EventSystem::Get()->Trigger<EEntityUnselected>(EEntityUnselected{});
					ecs->DestroyEntity(m_selectedEntity);
					m_selectedEntity = entt::null;
				}

			}
			ImGuiID id = ImGui::GetWindowDockID();
			ImVec2 min = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPos().x, ImGui::GetWindowPos().y + ImGui::GetCursorPosY());
			ImVec2 max = ImVec2(min.x + ImGui::GetWindowSize().x, min.y + ImGui::GetWindowSize().y);


			/*if (ImGui::BeginDragDropTargetCustom(ImRect(min, max), id))
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ECS_MOVEENTITY))
				{
					IM_ASSERT(payload->DataSize == sizeof(Entity));
					Entity entity = *(Entity*)payload->Data;
					ecs->RemoveFromParent(entity);
				}

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMESH_ID))
				{
					IM_ASSERT(payload->DataSize == sizeof(uint32));

					auto* ecs = Lina::ECS::Registry::Get();
					auto& model = Lina::Graphics::Model::GetModel(*(uint32*)payload->Data);
					auto entity = ecs->CreateEntity(Utility::GetFileNameOnly(model.GetPath()));
					auto& mr = ecs->emplace<ECS::ModelRendererComponent>(entity);
					mr.SetModel(entity, model);

					auto& mat = Graphics::Material::GetMaterial("Resources/Engine/Materials/DefaultLit.mat");

					for (int i = 0; i < model.GetImportedMaterials().size(); i++)
						mr.SetMaterial(entity, i, mat);
				}

				ImGui::EndDragDropTarget();
			}*/

			End();

		}
	}



}