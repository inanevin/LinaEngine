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

#include "Drawers/ComponentDrawer.hpp"
#include "Core/Application.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/Material.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Components/RigidbodyComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Modals/SelectMeshModal.hpp"
#include "Modals/SelectMaterialModal.hpp"
#include "Memory/Memory.hpp"
#include "Core/EditorCommon.hpp"
#include "IconsFontAwesome5.h"
#include "IconsMaterialDesign.h"
#include "imgui/imguizmo/ImGuizmo.h"

using namespace Lina::ECS;
using namespace Lina::Editor;

namespace Lina::Editor
{
	ComponentDrawer* ComponentDrawer::s_activeInstance = nullptr;

	ComponentDrawer::ComponentDrawer()
	{

#ifdef LINA_EDITOR

		RegisterComponentToDraw<EntityDataComponent>(GetTypeID<EntityDataComponent>(), "Transformation", std::bind(&ComponentDrawer::DrawEntityDataComponent, this, std::placeholders::_1, std::placeholders::_2));
		RegisterComponentToDraw<RigidbodyComponent>(GetTypeID<RigidbodyComponent>(), "Rigidbody", std::bind(&ComponentDrawer::DrawRigidbodyComponent, this, std::placeholders::_1, std::placeholders::_2));
		RegisterComponentToDraw<CameraComponent>(GetTypeID<CameraComponent>(), "Camera", std::bind(&ComponentDrawer::DrawCameraComponent, this, std::placeholders::_1, std::placeholders::_2));
		RegisterComponentToDraw<DirectionalLightComponent>(GetTypeID<DirectionalLightComponent>(), "Directional Light", std::bind(&ComponentDrawer::DrawDirectionalLightComponent, this, std::placeholders::_1, std::placeholders::_2));
		RegisterComponentToDraw<SpotLightComponent>(GetTypeID<SpotLightComponent>(), "Spot Light", std::bind(&ComponentDrawer::DrawSpotLightComponent, this, std::placeholders::_1, std::placeholders::_2));
		RegisterComponentToDraw<PointLightComponent>(GetTypeID<PointLightComponent>(), "Point Light", std::bind(&ComponentDrawer::DrawPointLightComponent, this, std::placeholders::_1, std::placeholders::_2));
		RegisterComponentToDraw<FreeLookComponent>(GetTypeID<FreeLookComponent>(), "Free Look", std::bind(&ComponentDrawer::DrawFreeLookComponent, this, std::placeholders::_1, std::placeholders::_2));
		RegisterComponentToDraw<MeshRendererComponent>(GetTypeID<MeshRendererComponent>(), "Mesh Renderer", std::bind(&ComponentDrawer::DrawMeshRendererComponent, this, std::placeholders::_1, std::placeholders::_2));
		RegisterComponentToDraw<ModelRendererComponent>(GetTypeID<ModelRendererComponent>(), "Model Renderer", std::bind(&ComponentDrawer::DrawModelRendererComponent, this, std::placeholders::_1, std::placeholders::_2));
		RegisterComponentToDraw<SpriteRendererComponent>(GetTypeID<SpriteRendererComponent>(), "Sprite Renderer", std::bind(&ComponentDrawer::DrawSpriteRendererComponent, this, std::placeholders::_1, std::placeholders::_2));

#endif

	}


	void ComponentDrawer::Setup()
	{
		s_activeInstance = this;

	}

	// Use reflection for gods sake later on.
	std::vector<std::string> ComponentDrawer::GetEligibleComponents(Lina::ECS::Registry* ecs, Lina::ECS::Entity entity)
	{
		std::vector<std::string> eligibleTypes;
		std::vector<TypeID> typeIDs;

		// Store all components of the entity.
		ecs->visit(entity, [&typeIDs](const auto component)
			{
				TypeID id = component.hash();
				typeIDs.push_back(id);
			});

		// Iterate registered types & add as eligible if entity does not contain the type.
		for (std::map<TypeID, ComponentValueTuple>::iterator it = m_componentFunctionsMap.begin(); it != m_componentFunctionsMap.end(); ++it)
		{
			// Exclusion
			if (std::get<0>(it->second).compare("Mesh Renderer") == 0) continue;

			if (std::find(typeIDs.begin(), typeIDs.end(), it->first) == typeIDs.end())
			{
				eligibleTypes.push_back(std::get<0>(it->second));
			}
		}

		return eligibleTypes;
	}

	void ComponentDrawer::AddComponentToEntity(Registry* ecs, Entity entity, const std::string& comp)
	{
		// Call the add function of the type when the requested strings match.
		for (std::map<TypeID, ComponentValueTuple>::iterator it = m_componentFunctionsMap.begin(); it != m_componentFunctionsMap.end(); ++it)
		{
			if (std::get<0>(it->second).compare(comp) == 0)
				std::get<1>(it->second)(ecs, entity);
		}
	}

	void ComponentDrawer::SwapComponentOrder(Lina::ECS::TypeID id1, Lina::ECS::TypeID id2)
	{
		// Swap iterators.
		std::vector<TypeID>::iterator it1 = std::find(m_componentDrawList.begin(), m_componentDrawList.end(), id1);
		std::vector<TypeID>::iterator it2 = std::find(m_componentDrawList.begin(), m_componentDrawList.end(), id2);
		std::iter_swap(it1, it2);
	}

	void ComponentDrawer::AddIDToDrawList(Lina::ECS::TypeID id)
	{
		// Add only if it doesn't exists.
		if (std::find(m_componentDrawList.begin(), m_componentDrawList.end(), id) == m_componentDrawList.end())
			m_componentDrawList.push_back(id);
	}

	void ComponentDrawer::ClearDrawList()
	{
		m_componentDrawList.clear();
	}

	void ComponentDrawer::DrawComponents(Lina::ECS::Registry* ecs, Lina::ECS::Entity entity)
	{
		s_activeInstance = this;

		// Draw components.
		for (int i = 0; i < m_componentDrawList.size(); i++)
		{
			auto func = std::get<2>(m_componentFunctionsMap[m_componentDrawList[i]]);
			if (func)
				func(ecs, entity);
		}
	}

	bool ComponentDrawer::DrawComponentTitle(Lina::ECS::TypeID typeID, const char* title, const char* icon, bool* refreshPressed, bool* enabled, bool* foldoutOpen, const ImVec4& iconColor, const ImVec2& iconOffset, bool cantDelete, bool noRefresh)
	{
	
		const char* caret = *foldoutOpen ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT;
		if (WidgetsUtility::IconButtonNoDecoration(caret, 30, 0.8f))
			*foldoutOpen = !*foldoutOpen;

		// Title.
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::Text(title);
		if(ImGui::IsItemClicked())
			*foldoutOpen = !*foldoutOpen;
		ImGui::AlignTextToFramePadding();
		ImGui::SameLine();

		// Title is the drag and drop target.
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			// Set payload to carry the type id.
			ImGui::SetDragDropPayload("COMP_MOVE_PAYLOAD", &typeID, sizeof(int));

			// Display preview 
			ImGui::Text("Move ");
			ImGui::EndDragDropSource();
		}

		// Dropped on another title, swap component orders.
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("COMP_MOVE_PAYLOAD"))
			{
				IM_ASSERT(payload->DataSize == sizeof(Lina::ECS::TypeID));
				Lina::ECS::TypeID payloadID = *(const Lina::ECS::TypeID*)payload->m_data;
				SwapComponentOrder(payloadID, typeID);
			}
			ImGui::EndDragDropTarget();
		}

		// Icon
		WidgetsUtility::IncrementCursorPosY(6);
		WidgetsUtility::IncrementCursorPos(ImVec2(iconOffset.x, iconOffset.y));
		WidgetsUtility::Icon(icon, 0.6f, iconColor);
		WidgetsUtility::IncrementCursorPos(ImVec2(-iconOffset.x, -iconOffset.y));

		// Enabled toggle
		std::string buf(title);
		if (!cantDelete && enabled != nullptr)
		{
			buf.append("t");
			ImVec4 toggleColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 88);
			WidgetsUtility::IncrementCursorPosY(-4);
			WidgetsUtility::ToggleButton(buf.c_str(), enabled, 0.8f, 1.4f, toggleColor, ImVec4(toggleColor.x, toggleColor.y, toggleColor.z, 0.7f));
		}

		// Refresh button
		if (!noRefresh)
		{
			buf.append("r");
			ImGui::SameLine();
			if (cantDelete)
				ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 23);
			else
				ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 43);
			WidgetsUtility::IncrementCursorPosY(4);
			*refreshPressed = WidgetsUtility::IconButton(buf.c_str(), ICON_FA_SYNC_ALT, 0.0f, 0.6f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header));
		}

		// Close button
		if (!cantDelete)
		{
			buf.append("c");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 20);
			bool removed = WidgetsUtility::IconButton(buf.c_str(), ICON_FA_TIMES, 0.0f, 0.6f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header));

			if (removed)
				ClearDrawList();

			return removed;
		}
	}

#define CURSORPOS_X_LABELS 12
#define CURSORPOS_Y_INCREMENT_BEFORE 15
#define CURSORPOS_Y_INCREMENT_BEFOREVAL 2.5f
#define CURSORPOS_Y_INCREMENT_AFTER 6.5f
#define CURSORPOS_XPERC_VALUES 0.32f
#define CURSORPOS_XPERC_VALUES2 0.545f

	const char* rigidbodyShapes[]
	{
		"SPHERE",
		"BOX",
		"CYLINDER",
		"CAPSULE"
	};

	void ComponentDrawer::DrawEntityDataComponent(Lina::ECS::Registry* ecs, Lina::ECS::Entity entity)
	{
		// Get component
		EntityDataComponent& data = ecs->get<EntityDataComponent>(entity);
		TypeID id = GetTypeID<EntityDataComponent>();

		// Align.
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

		// Draw title.
		bool refreshPressed = false;
		ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<EntityDataComponent>(), "Entity Data", ICON_FA_ARROWS_ALT, &refreshPressed, nullptr, &m_foldoutStateMap[entity][id], ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 0), true);

		// Refresh
		if (refreshPressed)
		{
			data.SetLocalLocation(Vector3::Zero);
			data.SetLocalRotation(Quaternion());
			data.SetLocalScale(Vector3::One);
		}

		// Draw component
		if (m_foldoutStateMap[entity][id])
		{
			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
			float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
			float cursorPosLabels = CURSORPOS_X_LABELS;

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Location");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			Vector3 location = data.GetLocalLocation();
			ImGui::DragFloat3("##loc", &location.x);
			// data.SetLocalLocation(location);
			data.SetLocalLocation(location);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Rotation");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			glm::vec3 rot = data.GetLocalRotationAngles();
			ImGui::DragFloat3("##rot", &rot.x);
			data.SetLocalRotationAngles(rot);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Scale");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			Vector3 scale = data.GetLocalScale();
			ImGui::DragFloat3("##scale", &scale.x);
			data.SetLocalScale(scale);
			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
			ImGui::SetCursorPosX(cursorPosLabels);

			bool caretGlobal = WidgetsUtility::Caret("##transform_globalValues");
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			WidgetsUtility::IncrementCursorPosY(-5);
			ImGui::Text("Global Values");

			if (caretGlobal)
			{
				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText("Location");
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				Vector3 globalLocation = data.GetLocation();
				ImGui::InputFloat3("##dbg_loc", &globalLocation.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText("Rotation");
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				Quaternion globalRotation = data.GetRotation();
				Vector3 angles = data.GetRotationAngles();
				ImGui::InputFloat3("##dbg_rot", &angles.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText("Scale");
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				Vector3 globalScale = data.GetScale();
				ImGui::InputFloat3("##dbg_scl", &globalScale.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			}


		}

		// Draw bevel line
		WidgetsUtility::DrawBeveledLine();
	}

	void ComponentDrawer::DrawCameraComponent(Lina::ECS::Registry* ecs, Lina::ECS::Entity entity)
	{
		// Get component
		CameraComponent& camera = ecs->get<CameraComponent>(entity);
		TypeID id = GetTypeID<CameraComponent>();

		// Align.
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

		// Draw title.
		bool refreshPressed = false;
		bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<CameraComponent>(), "Camera", ICON_FA_VIDEO, &refreshPressed, nullptr, &m_foldoutStateMap[entity][id], ImGui::GetStyleColorVec4(ImGuiCol_Header));

		// Remove if requested.
		if (removeComponent)
		{
			ecs->remove<CameraComponent>(entity);
			return;
		}

		// Refresh
		if (refreshPressed)
			ecs->replace<CameraComponent>(entity, CameraComponent());

		// Draw component.
		if (m_foldoutStateMap[entity][id])
		{
			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
			float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
			float cursorPosLabels = CURSORPOS_X_LABELS;
			ImGui::SetCursorPosX(cursorPosLabels);

			WidgetsUtility::AlignedText("Clear Color");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			WidgetsUtility::ColorButton("##clrclr", &camera.m_clearColor.r);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Field of View");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##fov", &camera.m_fieldOfView);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Near Plane");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##zNear", &camera.m_zNear);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Far Plane");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##zFar", &camera.m_zFar);

			ImGui::SetCursorPosX(cursorPosValues);
			if (ImGui::Button("Set Active Camera", ImVec2(110, 30)))
				Lina::Graphics::RenderEngineBackend::Get()->GetCameraSystem()->SetActiveCamera(entity);

			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
		}

		// Draw bevel line.
		WidgetsUtility::DrawBeveledLine();
	}

	void ComponentDrawer::DrawFreeLookComponent(Lina::ECS::Registry* ecs, Lina::ECS::Entity entity)
	{
		// Get component
		FreeLookComponent& freeLook = ecs->get<FreeLookComponent>(entity);
		TypeID id = GetTypeID<FreeLookComponent>();

		// Align.
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

		// Draw title.
		bool refreshPressed = false;
		bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<FreeLookComponent>(), "FreeLook", ICON_MD_3D_ROTATION, &refreshPressed, &freeLook.m_isEnabled, &m_foldoutStateMap[entity][id], ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 3));

		// Remove if requested.
		if (removeComponent)
		{
			ecs->remove<FreeLookComponent>(entity);
			return;
		}

		// Refresh
		if (refreshPressed)
			ecs->replace<FreeLookComponent>(entity, FreeLookComponent());

		// Draw component.
		if (m_foldoutStateMap[entity][id])
		{
			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
			float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
			float cursorPosLabels = CURSORPOS_X_LABELS;
			ImGui::SetCursorPosX(cursorPosLabels);

			WidgetsUtility::AlignedText("Movement Speeds");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat2("##ms", &freeLook.m_movementSpeeds.x);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Rotation Speeds");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat2("##rs", &freeLook.m_rotationSpeeds.x);

			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
		}

		// Draw bevel line.
		WidgetsUtility::DrawBeveledLine();
	}

	void ComponentDrawer::DrawRigidbodyComponent(Lina::ECS::Registry* ecs, Lina::ECS::Entity entity)
	{
		// Get component
		RigidbodyComponent& rb = ecs->get<RigidbodyComponent>(entity);
		TypeID id = GetTypeID<RigidbodyComponent>();

		// Align.
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

		// Draw title.
		bool refreshPressed = false;
		bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<RigidbodyComponent>(), "Rigidbody", ICON_MD_ACCESSIBILITY, &refreshPressed, &rb.m_isEnabled, &m_foldoutStateMap[entity][id], ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 3));

		// Remove if requested.
		if (removeComponent)
		{
			ecs->remove<RigidbodyComponent>(entity);
			return;
		}

		// Refresh
		if (refreshPressed)
			ecs->replace<RigidbodyComponent>(entity, RigidbodyComponent());

		// Draw component.
		if (m_foldoutStateMap[entity][id])
		{
			float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
			float cursorPosLabels = CURSORPOS_X_LABELS;
			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);

			ComponentDrawer::s_activeInstance->m_currentCollisionShape = (int)rb.m_collisionShape;

			// Draw collision shape.
			static ImGuiComboFlags flags = 0;
			static ECS::CollisionShape selectedCollisionShape = rb.m_collisionShape;
			const char* collisionShapeLabel = rigidbodyShapes[ComponentDrawer::s_activeInstance->m_currentCollisionShape];

			ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Shape"); ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues);
			if (ImGui::BeginCombo("##collshape", collisionShapeLabel, flags))
			{
				for (int i = 0; i < IM_ARRAYSIZE(rigidbodyShapes); i++)
				{
					const bool is_selected = (ComponentDrawer::s_activeInstance->m_currentCollisionShape == i);
					if (ImGui::Selectable(rigidbodyShapes[i], is_selected))
					{
						selectedCollisionShape = (ECS::CollisionShape)i;
						ComponentDrawer::s_activeInstance->m_currentCollisionShape = i;
						rb.m_collisionShape = selectedCollisionShape;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();

			}

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Mass");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##mass", &rb.m_mass);

			if (rb.m_collisionShape == ECS::CollisionShape::BOX || rb.m_collisionShape == ECS::CollisionShape::Cylinder)
			{
				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText("Half Extents");
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::DragFloat3("##halfextents", &rb.m_halfExtents.x);
			}
			else if (rb.m_collisionShape == ECS::CollisionShape::Sphere)
			{
				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText("Radius");
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::DragFloat("##radius", &rb.m_radius);
			}
			else if (rb.m_collisionShape == ECS::CollisionShape::CAPSULE)
			{
				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText("Radius");
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::DragFloat("##radius", &rb.m_radius);

				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText("Height");
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::DragFloat("##height", &rb.m_capsuleHeight);
			}

			ImGui::SetCursorPosX(cursorPosLabels);
			if (ImGui::Button("Apply"))
				ecs->replace<Lina::ECS::RigidbodyComponent>(entity, rb);

			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
		}

		// Draw bevel line
		WidgetsUtility::DrawBeveledLine();
	}

	void ComponentDrawer::DrawPointLightComponent(Lina::ECS::Registry* ecs, Lina::ECS::Entity entity)
	{
		// Get component
		PointLightComponent& pLight = ecs->get<PointLightComponent>(entity);
		ECS::EntityDataComponent& data = ecs->get<ECS::EntityDataComponent>(entity);
		TypeID id = GetTypeID<PointLightComponent>();

		// Align.
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

		// Draw title.
		bool refreshPressed = false;
		bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<PointLightComponent>(), "PointLight", ICON_FA_LIGHTBULB, &refreshPressed, &pLight.m_isEnabled, &m_foldoutStateMap[entity][id], ImGui::GetStyleColorVec4(ImGuiCol_Header));

		// Remove if requested.
		if (removeComponent)
		{
			ecs->remove<PointLightComponent>(entity);
			return;
		}

		// Refresh
		if (refreshPressed)
			ecs->replace<PointLightComponent>(entity, PointLightComponent());

		

		// Draw component.
		if (m_foldoutStateMap[entity][id])
		{
			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
			float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
			float cursorPosLabels = CURSORPOS_X_LABELS;

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Color");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			WidgetsUtility::ColorButton("##pclr", &pLight.m_color.r);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Intensity");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##plint", &pLight.m_intensity);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Distance");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##pldist", &pLight.m_distance);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Casts Shadows");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##castsShadows", &pLight.m_castsShadows);

			if (pLight.m_castsShadows)
			{
				ImGui::Indent();
				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText("Bias");
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::DragFloat("##bias", &pLight.m_bias);

				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText("Shadow Near");
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::DragFloat("##shadowNear", &pLight.m_shadowNear);

				ImGui::SetCursorPosX(cursorPosLabels);
				WidgetsUtility::AlignedText("Shadow Far");
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::DragFloat("##shadowFar", &pLight.m_shadowFar);
				ImGui::Unindent();
			}


			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Draw Debug");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##drawDebug", &pLight.m_drawDebug);


			if (pLight.m_drawDebug)
			{
				Vector3 end1 = data.GetLocation() + (pLight.m_distance * data.GetRotation().GetRight());
				Vector3 end2 = data.GetLocation() + (-pLight.m_distance * data.GetRotation().GetRight());
				Vector3 end3 = data.GetLocation() + (pLight.m_distance * data.GetRotation().GetForward());
				Vector3 end4 = data.GetLocation() + (-pLight.m_distance * data.GetRotation().GetForward());
				Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end1, Lina::Color::Red, 1.4f);
				Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end2, Lina::Color::Red, 1.4f);
				Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end3, Lina::Color::Red, 1.4f);
				Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end4, Lina::Color::Red, 1.4f);
			}

			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
		}

		// Draw bevel line.
		WidgetsUtility::DrawBeveledLine();
	}

	void ComponentDrawer::DrawSpotLightComponent(Lina::ECS::Registry* ecs, Lina::ECS::Entity entity)
	{
		// Get component
		SpotLightComponent& sLight = ecs->get<SpotLightComponent>(entity);
		ECS::EntityDataComponent& data = ecs->get<ECS::EntityDataComponent>(entity);
		TypeID id = GetTypeID<SpotLightComponent>();

		// Align.
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

		// Draw title.
		bool refreshPressed = false;
		bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<SpotLightComponent>(), "SpotLight", ICON_MD_HIGHLIGHT, &refreshPressed, &sLight.m_isEnabled, &m_foldoutStateMap[entity][id], ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 3.0f));

		// Remove if requested.
		if (removeComponent)
		{
			ecs->remove<SpotLightComponent>(entity);
			return;
		}

		// Refresh
		if (refreshPressed)
			ecs->replace<SpotLightComponent>(entity, SpotLightComponent());

		// Draw component.
		if (m_foldoutStateMap[entity][id])
		{
			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
			float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
			float cursorPosLabels = CURSORPOS_X_LABELS;

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Color");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			WidgetsUtility::ColorButton("##sclr", &sLight.m_color.r);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Distance");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##sldist", &sLight.m_distance);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Intensity");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##slint", &sLight.m_intensity);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Cutoff");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##cutOff", &sLight.m_cutoff);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Outer Cutoff");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##outerCutOff", &sLight.m_outerCutoff);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Draw Debug");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##drawDebug", &sLight.m_drawDebug);

			if (sLight.m_drawDebug)
			{
				Vector3 end1 = data.GetLocation() + (sLight.m_distance * data.GetRotation().GetForward());
				Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end1, Lina::Color::Red, 1.4f);
			}

			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
		}

		// Draw bevel line.
		WidgetsUtility::DrawBeveledLine();
	}

	void ComponentDrawer::DrawDirectionalLightComponent(Lina::ECS::Registry* ecs, Lina::ECS::Entity entity)
	{
		// Get component
		DirectionalLightComponent& dLight = ecs->get<DirectionalLightComponent>(entity);
		ECS::EntityDataComponent& data = ecs->get<ECS::EntityDataComponent>(entity);
		TypeID id = GetTypeID<DirectionalLightComponent>();

		// Align.
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

		// Draw title.
		bool refreshPressed = false;
		bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<DirectionalLightComponent>(), "DirectionalLight", ICON_FA_SUN, &refreshPressed, &dLight.m_isEnabled, &m_foldoutStateMap[entity][id], ImGui::GetStyleColorVec4(ImGuiCol_Header));

		// Remove if requested.
		if (removeComponent)
		{
			ecs->remove<DirectionalLightComponent>(entity);
			return;
		}

		// Refresh
		if (refreshPressed)
			ecs->replace<DirectionalLightComponent>(entity, DirectionalLightComponent());

		// Draw component.
		if (m_foldoutStateMap[entity][id])
		{
			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
			float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
			float cursorPosLabels = CURSORPOS_X_LABELS;
			ImGui::SetCursorPosX(cursorPosLabels);

			WidgetsUtility::AlignedText("Color");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			WidgetsUtility::ColorButton("##dclr", &dLight.m_color.r);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Intensity");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##dlint", &dLight.m_intensity);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Shadow Near Plane");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##szNear", &dLight.m_shadowZNear);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Shadow Far Plane");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat("##szFar", &dLight.m_shadowZFar);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Shadow Projection");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::DragFloat4("##sproj", &dLight.m_shadowOrthoProjection.x);

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Draw Debug");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::Checkbox("##drawDebug", &dLight.m_drawDebug);

			if (dLight.m_drawDebug)
			{
				Vector3 dir = Vector3::Zero - data.GetLocation();
				Vector3 end1 = data.GetLocation() + dir;
				Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end1, Lina::Color::Red, 1.4f);
			}

			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
		}

		// Draw bevel line.
		WidgetsUtility::DrawBeveledLine();
	}

	void ComponentDrawer::DrawMeshRendererComponent(Lina::ECS::Registry* ecs, Lina::ECS::Entity entity)
	{
		// Get component
		MeshRendererComponent& renderer = ecs->get<MeshRendererComponent>(entity);
		TypeID id = GetTypeID<MeshRendererComponent>();
		
		// Align.
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

		bool refreshPressed = false;
		ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<ModelRendererComponent>(), "MeshRenderer", ICON_MD_GRID_ON, &refreshPressed, &renderer.m_isEnabled, &m_foldoutStateMap[entity][id], ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 3), true, true);

		// Draw bevel line.
		WidgetsUtility::DrawBeveledLine();
	}

	void ComponentDrawer::DrawModelRendererComponent(Lina::ECS::Registry* ecs, Lina::ECS::Entity entity)
	{
		// Get component
		ModelRendererComponent& renderer = ecs->get<ModelRendererComponent>(entity);
		TypeID id = GetTypeID<ModelRendererComponent>();

		// Align.
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

		// Draw title.
		bool refreshPressed = false;
		bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<ModelRendererComponent>(), "ModelRenderer", ICON_MD_TOYS, &refreshPressed, &renderer.m_isEnabled, &m_foldoutStateMap[entity][id], ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 3));

		// Remove if requested.
		if (removeComponent)
		{
			ecs->remove<ModelRendererComponent>(entity);
			return;
		}

		// Refresh
		if (refreshPressed)
			ecs->replace<ModelRendererComponent>(entity, ModelRendererComponent());

		// Draw component.
		if (m_foldoutStateMap[entity][id])
		{
			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
			float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
			float cursorPosLabels = CURSORPOS_X_LABELS;

			// Model selection
			char meshPathC[128] = "";
			strcpy(meshPathC, renderer.m_modelPath.c_str());

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Model");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 35 - ImGui::GetCursorPosX());

			Lina::Graphics::Model* selected = WidgetsUtility::ModelComboBox("##modelrend_model", renderer.m_modelID);

			if (selected)
				renderer.SetModel(ecs, entity, *selected);

			// Mesh drag & drop.
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMESH_ID))
				{
					IM_ASSERT(payload->DataSize == sizeof(uint32));

					auto& model = Lina::Graphics::Model::GetModel(*(uint32*)payload->m_data);
					renderer.SetModel(ecs, entity, model);
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			WidgetsUtility::IncrementCursorPosY(5);
			// Remove Model
			if (WidgetsUtility::IconButton("##selectmesh", ICON_FA_MINUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
			{
				renderer.RemoveModel(ecs, entity);
			}

			for (int i = 0; i < renderer.m_materialPaths.size(); i++)
			{
				// Material selection.
				char matPathC[128] = "";
				strcpy(matPathC, renderer.m_materialPaths[i].c_str());

				// Draw material name
				ImGui::SetCursorPosX(cursorPosLabels);
				std::string materialName = Lina::Graphics::Model::GetModel(renderer.m_modelPath).GetMaterialSpecs()[i].m_name;
				WidgetsUtility::AlignedText(materialName.c_str());
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursorPosValues);
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 35 - ImGui::GetCursorPosX());

				const std::string cboxID = "#modelrend_material " + i;
				Lina::Graphics::Material* selectedMaterial = WidgetsUtility::MaterialComboBox(cboxID.c_str(), renderer.m_materialPaths[i]);

				if (selectedMaterial != nullptr)
					renderer.SetMaterial(ecs, entity, i, *selectedMaterial);

				
				
				ImGui::SameLine();
				WidgetsUtility::IncrementCursorPosY(5);

				// Material drag & drop.
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMATERIAL_ID))
					{
						IM_ASSERT(payload->DataSize == sizeof(uint32));

						auto& mat = Lina::Graphics::Material::GetMaterial(*(uint32*)payload->m_data);
						renderer.SetMaterial(ecs, entity, i, mat);
					}
					ImGui::EndDragDropTarget();
				}

				// Remove Material
				std::string icnBtn = "##selectmat" + std::to_string(i);
				if (WidgetsUtility::IconButton(icnBtn.c_str(), ICON_FA_MINUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
				{
					renderer.RemoveMaterial(ecs, entity, i);
				}
			}

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
		}



		// Draw bevel line.
		WidgetsUtility::DrawBeveledLine();
	}

	void ComponentDrawer::DrawSpriteRendererComponent(Lina::ECS::Registry* ecs, Lina::ECS::Entity entity)
	{
		// Get component
		SpriteRendererComponent& renderer = ecs->get<SpriteRendererComponent>(entity);
		TypeID id = GetTypeID<SpriteRendererComponent>();

		// Align.
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

		// Draw title.
		bool refreshPressed = false;
		bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<SpriteRendererComponent>(), "Sprite Renderer", ICON_MD_GRID_ON, &refreshPressed, &renderer.m_isEnabled, &m_foldoutStateMap[entity][id], ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 3));

		// Remove if requested.
		if (removeComponent)
		{
			ecs->remove<SpriteRendererComponent>(entity);
			return;
		}

		// Refresh
		if (refreshPressed)
			ecs->replace<SpriteRendererComponent>(entity, SpriteRendererComponent());

		// Draw component.
		if (m_foldoutStateMap[entity][id])
		{
			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
			float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
			float cursorPosLabels = CURSORPOS_X_LABELS;

			// Material selection
			if (Lina::Graphics::Material::MaterialExists(renderer.m_materialID))
			{
				renderer.m_selectedMatID = renderer.m_materialID;
				renderer.m_selectedMatPath = renderer.m_materialPaths;
			}


			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::AlignedText("Material");
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosValues);

			Graphics::Material* selected = WidgetsUtility::MaterialComboBox("##spr_material", renderer.m_materialPaths);

			if (selected != nullptr)
			{
				renderer.m_materialID = selected->GetID();
				renderer.m_materialPaths = selected->GetPath();
			}
			
		
			// Material drag & drop.
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMATERIAL_ID))
				{
					IM_ASSERT(payload->DataSize == sizeof(uint32));
					renderer.m_materialID = Lina::Graphics::Material::GetMaterial(*(uint32*)payload->m_data).GetID();
					renderer.m_materialPaths = Lina::Graphics::Material::GetMaterial(*(uint32*)payload->m_data).GetPath();

				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			WidgetsUtility::IncrementCursorPosY(5);


			if (WidgetsUtility::IconButton("##selectspritemat", ICON_FA_MINUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
			{
				renderer.m_materialID = -1;
				renderer.m_materialPaths = "";
			}

			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
		}

		// Draw bevel line.
		WidgetsUtility::DrawBeveledLine();
	}

}
