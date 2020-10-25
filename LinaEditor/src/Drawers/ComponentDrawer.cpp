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
#include "Rendering/RenderEngine.hpp"
#include "Rendering/Mesh.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Components/RigidbodyComponent.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Modals/SelectMeshModal.hpp"
#include "Modals/SelectMaterialModal.hpp"
#include "PackageManager/PAMMemory.hpp"
#include "IconsFontAwesome5.h"
#include "IconsMaterialDesign.h"

using namespace LinaEngine::ECS;
using namespace LinaEditor;

namespace LinaEditor
{
	ComponentDrawer* ComponentDrawer::s_activeInstance = nullptr;

	ComponentDrawer::ComponentDrawer()
	{
		// Display names.
		std::get<0>(m_componentFunctionsMap[GetTypeID<TransformComponent>()]) = "Transformation";
		std::get<0>(m_componentFunctionsMap[GetTypeID<RigidbodyComponent>()]) = "Rigidbody";
		std::get<0>(m_componentFunctionsMap[GetTypeID<CameraComponent>()]) = "Camera";
		std::get<0>(m_componentFunctionsMap[GetTypeID<DirectionalLightComponent>()]) = "Directional Light";
		std::get<0>(m_componentFunctionsMap[GetTypeID<SpotLightComponent>()]) = "Spot Light";
		std::get<0>(m_componentFunctionsMap[GetTypeID<PointLightComponent>()]) = "Point Light";
		std::get<0>(m_componentFunctionsMap[GetTypeID<FreeLookComponent>()]) = "Free Look";
		std::get<0>(m_componentFunctionsMap[GetTypeID<MeshRendererComponent>()]) = "Mesh Renderer";
		std::get<0>(m_componentFunctionsMap[GetTypeID<SpriteRendererComponent>()]) = "Sprite Renderer";

		// Add functions.
		std::get<1>(m_componentFunctionsMap[GetTypeID<TransformComponent>()]) = std::bind(&TransformComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(m_componentFunctionsMap[GetTypeID<RigidbodyComponent>()]) = std::bind(&RigidbodyComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(m_componentFunctionsMap[GetTypeID<CameraComponent>()]) = std::bind(&CameraComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(m_componentFunctionsMap[GetTypeID<DirectionalLightComponent>()]) = std::bind(&DirectionalLightComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(m_componentFunctionsMap[GetTypeID<SpotLightComponent>()]) = std::bind(&SpotLightComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(m_componentFunctionsMap[GetTypeID<PointLightComponent>()]) = std::bind(&PointLightComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(m_componentFunctionsMap[GetTypeID<FreeLookComponent>()]) = std::bind(&FreeLookComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(m_componentFunctionsMap[GetTypeID<MeshRendererComponent>()]) = std::bind(&MeshRendererComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(m_componentFunctionsMap[GetTypeID<SpriteRendererComponent>()]) = std::bind(&SpriteRendererComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);

		// Draw functions.
		std::get<2>(m_componentFunctionsMap[GetTypeID<TransformComponent>()]) = std::bind(&TransformComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(m_componentFunctionsMap[GetTypeID<RigidbodyComponent>()]) = std::bind(&RigidbodyComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(m_componentFunctionsMap[GetTypeID<CameraComponent>()]) = std::bind(&CameraComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(m_componentFunctionsMap[GetTypeID<DirectionalLightComponent>()]) = std::bind(&DirectionalLightComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(m_componentFunctionsMap[GetTypeID<SpotLightComponent>()]) = std::bind(&SpotLightComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(m_componentFunctionsMap[GetTypeID<PointLightComponent>()]) = std::bind(&PointLightComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(m_componentFunctionsMap[GetTypeID<FreeLookComponent>()]) = std::bind(&FreeLookComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(m_componentFunctionsMap[GetTypeID<MeshRendererComponent>()]) = std::bind(&MeshRendererComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(m_componentFunctionsMap[GetTypeID<SpriteRendererComponent>()]) = std::bind(&SpriteRendererComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);

		LinaEngine::ECS::ECSRegistry& reg = LinaEngine::Application::GetECSRegistry();
		reg.on_destroy<TransformComponent>().connect<&ComponentDrawer::ComponentRemoved>(this);
		reg.on_destroy<RigidbodyComponent>().connect<&ComponentDrawer::ComponentRemoved>(this);
		reg.on_destroy<CameraComponent>().connect<&ComponentDrawer::ComponentRemoved>(this);
		reg.on_destroy<DirectionalLightComponent>().connect<&ComponentDrawer::ComponentRemoved>(this);
		reg.on_destroy<SpotLightComponent>().connect<&ComponentDrawer::ComponentRemoved>(this);
		reg.on_destroy<PointLightComponent>().connect<&ComponentDrawer::ComponentRemoved>(this);
		reg.on_destroy<FreeLookComponent>().connect<&ComponentDrawer::ComponentRemoved>(this);
		reg.on_destroy<MeshRendererComponent>().connect<&ComponentDrawer::ComponentRemoved>(this);
		reg.on_destroy<SpriteRendererComponent>().connect<&ComponentDrawer::ComponentRemoved>(this);
	}

	// Use reflection for gods sake later on.
	std::vector<std::string> ComponentDrawer::GetEligibleComponents(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity)
	{
		std::vector<std::string> eligibleTypes;
		std::vector<ECSTypeID> typeIDs;

		// Store all components of the entity.
		ecs.visit(entity, [&typeIDs](const auto component)
			{
				ECSTypeID id = component;
				typeIDs.push_back(id);
			});

		// Iterate registered types & add as eligible if entity does not contain the type.
		for (std::map<ECSTypeID, ComponentValueTuple>::iterator it = m_componentFunctionsMap.begin(); it != m_componentFunctionsMap.end(); ++it)
		{
			if (std::find(typeIDs.begin(), typeIDs.end(), it->first) == typeIDs.end())
				eligibleTypes.push_back(std::get<0>(it->second));
		}

		return eligibleTypes;
	}

	void ComponentDrawer::AddComponentToEntity(ECSRegistry& ecs, ECSEntity entity, const std::string& comp)
	{
		// Call the add function of the type when the requested strings match.
		for (std::map<ECSTypeID, ComponentValueTuple>::iterator it = m_componentFunctionsMap.begin(); it != m_componentFunctionsMap.end(); ++it)
		{
			if (std::get<0>(it->second).compare(comp) == 0)
				std::get<1>(it->second)(ecs, entity);
		}
	}

	void ComponentDrawer::SwapComponentOrder(LinaEngine::ECS::ECSTypeID id1, LinaEngine::ECS::ECSTypeID id2)
	{
		// Swap iterators.
		std::vector<ECSTypeID>::iterator it1 = std::find(m_componentDrawList.begin(), m_componentDrawList.end(), id1);
		std::vector<ECSTypeID>::iterator it2 = std::find(m_componentDrawList.begin(), m_componentDrawList.end(), id2);
		std::iter_swap(it1, it2);
	}

	void ComponentDrawer::AddIDToDrawList(LinaEngine::ECS::ECSTypeID id)
	{
		// Add only if it doesn't exists.
		if (std::find(m_componentDrawList.begin(), m_componentDrawList.end(), id) == m_componentDrawList.end())
			m_componentDrawList.push_back(id);
	}

	void ComponentDrawer::ClearDrawList()
	{
		m_componentDrawList.clear();
	}

	void ComponentDrawer::DrawComponents(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity)
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

	bool ComponentDrawer::DrawComponentTitle(LinaEngine::ECS::ECSTypeID typeID, const char* title, const char* icon, bool* refreshPressed, bool* enabled, bool* foldoutOpen, const ImVec4& iconColor, const ImVec2& iconOffset)
	{
		// Caret button.
		const char* caret = *foldoutOpen ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT;
		if (WidgetsUtility::IconButtonNoDecoration(caret, 30, 0.8f))
			*foldoutOpen = !*foldoutOpen;

		// Title.
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		WidgetsUtility::IncrementCursorPosY(-5);
		ImGui::Text(title);
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
				IM_ASSERT(payload->DataSize == sizeof(LinaEngine::ECS::ECSTypeID));
				LinaEngine::ECS::ECSTypeID payloadID = *(const LinaEngine::ECS::ECSTypeID*)payload->m_data;
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
		buf.append("t");
		ImVec4 toggleColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 88);
		WidgetsUtility::IncrementCursorPosY(-4);
		WidgetsUtility::ToggleButton(buf.c_str(), enabled, 0.8f, 1.4f, toggleColor, ImVec4(toggleColor.x, toggleColor.y, toggleColor.z, 0.7f));

		// Refresh button
		buf.append("r");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 43);
		WidgetsUtility::IncrementCursorPosY(4);
		*refreshPressed = WidgetsUtility::IconButton(buf.c_str(), ICON_FA_SYNC_ALT, 0.0f, 0.6f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header));

		// Close button
		buf.append("c");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 20);
		return WidgetsUtility::IconButton(buf.c_str(), ICON_FA_TIMES, 0.0f, 0.6f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header));
	}

	void ComponentDrawer::ComponentRemoved(entt::registry&, entt::entity)
	{
		ClearDrawList();
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


void LinaEngine::ECS::TransformComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	TransformComponent& transform = ecs.get<TransformComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	bool refreshPressed = false;
	bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<TransformComponent>(), "Transformation", ICON_FA_ARROWS_ALT, &refreshPressed, &transform.m_isEnabled, &transform.m_foldoutOpen, ImGui::GetStyleColorVec4(ImGuiCol_Header));

	// Remove if requested.
	if (removeComponent)
	{
		ecs.remove<TransformComponent>(entity);
		return;
	}

	// Refresh
	if (refreshPressed)
		ecs.replace<TransformComponent>(entity, TransformComponent());

	// Draw component
	if (transform.m_foldoutOpen)
	{
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;

		ImGui::SetCursorPosX(cursorPosLabels);
		WidgetsUtility::AlignedText("Location");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		Vector3 location = transform.transform.m_location;
		ImGui::DragFloat3("##loc", &location.x);
		transform.transform.SetGlobalLocation(location);

		ImGui::SetCursorPosX(cursorPosLabels);
		WidgetsUtility::AlignedText("Rotation");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		Quaternion rotation = transform.transform.m_rotation;
		WidgetsUtility::DragQuaternion("##rot", rotation);
		transform.transform.SetGlobalRotation(rotation);

		ImGui::SetCursorPosX(cursorPosLabels);
		WidgetsUtility::AlignedText("Scale");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		Vector3 scale = transform.transform.m_scale;
		ImGui::DragFloat3("##scale", &scale.x);
		transform.transform.SetGlobalScale(scale);
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line
	WidgetsUtility::DrawBeveledLine();
}

void LinaEngine::ECS::RigidbodyComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	RigidbodyComponent& rb = ecs.get<RigidbodyComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	bool refreshPressed = false;
	bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<RigidbodyComponent>(), "Rigidbody", ICON_MD_ACCESSIBILITY, &refreshPressed, &rb.m_isEnabled, &rb.m_foldoutOpen, ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 3));

	// Remove if requested.
	if (removeComponent)
	{
		ecs.remove<RigidbodyComponent>(entity);
		return;
	}

	// Refresh
	if (refreshPressed)
		ecs.replace<RigidbodyComponent>(entity, RigidbodyComponent());

	// Draw component.
	if (rb.m_foldoutOpen)
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
			ecs.replace<LinaEngine::ECS::RigidbodyComponent>(entity, rb);

		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line
	WidgetsUtility::DrawBeveledLine();
}

void LinaEngine::ECS::CameraComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	CameraComponent& camera = ecs.get<CameraComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	bool refreshPressed = false;
	bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<CameraComponent>(), "Camera", ICON_FA_VIDEO, &refreshPressed, &camera.m_isEnabled, &camera.m_foldoutOpen, ImGui::GetStyleColorVec4(ImGuiCol_Header));

	// Remove if requested.
	if (removeComponent)
	{
		ecs.remove<CameraComponent>(entity);
		return;
	}

	// Refresh
	if (refreshPressed)
		ecs.replace<CameraComponent>(entity, CameraComponent());

	// Draw component.
	if (camera.m_foldoutOpen)
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

		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line.
	WidgetsUtility::DrawBeveledLine();
}

void LinaEngine::ECS::DirectionalLightComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	DirectionalLightComponent& dLight = ecs.get<DirectionalLightComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	bool refreshPressed = false;
	bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<DirectionalLightComponent>(), "DirectionalLight", ICON_FA_SUN, &refreshPressed, &dLight.m_isEnabled, &dLight.m_foldoutOpen, ImGui::GetStyleColorVec4(ImGuiCol_Header));

	// Remove if requested.
	if (removeComponent)
	{
		ecs.remove<DirectionalLightComponent>(entity);
		return;
	}

	// Refresh
	if (refreshPressed)
		ecs.replace<DirectionalLightComponent>(entity, DirectionalLightComponent());

	// Draw component.
	if (dLight.m_foldoutOpen)
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

		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line.
	WidgetsUtility::DrawBeveledLine();
}

void LinaEngine::ECS::PointLightComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	PointLightComponent& pLight = ecs.get<PointLightComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	bool refreshPressed = false;
	bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<PointLightComponent>(), "PointLight", ICON_FA_LIGHTBULB, &refreshPressed, &pLight.m_isEnabled, &pLight.m_foldoutOpen, ImGui::GetStyleColorVec4(ImGuiCol_Header));

	// Remove if requested.
	if (removeComponent)
	{
		ecs.remove<PointLightComponent>(entity);
		return;
	}

	// Refresh
	if (refreshPressed)
		ecs.replace<PointLightComponent>(entity, PointLightComponent());

	// Draw component.
	if (pLight.m_foldoutOpen)
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
		WidgetsUtility::AlignedText("Distance");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::DragFloat("##pldist", &pLight.m_distance);

		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line.
	WidgetsUtility::DrawBeveledLine();
}

void LinaEngine::ECS::SpotLightComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	SpotLightComponent& sLight = ecs.get<SpotLightComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	bool refreshPressed = false;
	bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<SpotLightComponent>(), "SpotLight", ICON_MD_HIGHLIGHT, &refreshPressed, &sLight.m_isEnabled, &sLight.m_foldoutOpen, ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 3.0f));

	// Remove if requested.
	if (removeComponent)
	{
		ecs.remove<SpotLightComponent>(entity);
		return;
	}

	// Refresh
	if (refreshPressed)
		ecs.replace<SpotLightComponent>(entity, SpotLightComponent());

	// Draw component.
	if (sLight.m_foldoutOpen)
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
		WidgetsUtility::AlignedText("Cutoff");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::DragFloat("##cutOff", &sLight.m_cutoff);

		ImGui::SetCursorPosX(cursorPosLabels);
		WidgetsUtility::AlignedText("Outer Cutoff");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::DragFloat("##outerCutOff", &sLight.m_outerCutoff);

		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line.
	WidgetsUtility::DrawBeveledLine();
}

void LinaEngine::ECS::FreeLookComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	FreeLookComponent& freeLook = ecs.get<FreeLookComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	bool refreshPressed = false;
	bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<FreeLookComponent>(), "FreeLook", ICON_MD_3D_ROTATION, &refreshPressed, &freeLook.m_isEnabled, &freeLook.m_foldoutOpen, ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 3));

	// Remove if requested.
	if (removeComponent)
	{
		ecs.remove<FreeLookComponent>(entity);
		return;
	}

	// Refresh
	if (refreshPressed)
		ecs.replace<FreeLookComponent>(entity, FreeLookComponent());

	// Draw component.
	if (freeLook.m_foldoutOpen)
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

void LinaEngine::ECS::MeshRendererComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	MeshRendererComponent& renderer = ecs.get<MeshRendererComponent>(entity);
	LinaEngine::Graphics::RenderEngine& renderEngine = LinaEngine::Application::GetRenderEngine();

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	bool refreshPressed = false;
	bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<MeshRendererComponent>(), "MeshRenderer", ICON_MD_GRID_ON, &refreshPressed, &renderer.m_isEnabled, &renderer.m_foldoutOpen, ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 3));

	// Remove if requested.
	if (removeComponent)
	{
		ecs.remove<MeshRendererComponent>(entity);
		return;
	}

	// Refresh
	if (refreshPressed)
		ecs.replace<MeshRendererComponent>(entity, MeshRendererComponent());

	// Draw component.
	if (renderer.m_foldoutOpen)
	{
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;

		// Mesh selection
		if (renderEngine.MeshExists(renderer.m_meshID))
		{
			renderer.m_selectedMeshPath = renderer.m_meshPath;
			renderer.m_selectedMeshID = renderer.m_meshID;
		}

		char meshPathC[128] = "";
		strcpy(meshPathC, renderer.m_selectedMeshPath.c_str());

		ImGui::SetCursorPosX(cursorPosLabels);
		WidgetsUtility::AlignedText("Mesh");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 35 - ImGui::GetCursorPosX());
		ImGui::InputText("##selectedMesh", meshPathC, IM_ARRAYSIZE(meshPathC), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		WidgetsUtility::IncrementCursorPosY(5);

		if (WidgetsUtility::IconButton("##selectmesh", ICON_FA_PLUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
			ImGui::OpenPopup("Select Mesh");

		bool meshPopupOpen = true;
		WidgetsUtility::FramePaddingY(8);
		WidgetsUtility::FramePaddingX(4);
		ImGui::SetNextWindowSize(ImVec2(280, 400));
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Size.x / 2.0f - 140, ImGui::GetMainViewport()->Size.y / 2.0f - 200));
		if (ImGui::BeginPopupModal("Select Mesh", &meshPopupOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			SelectMeshModal::Draw(Application::GetRenderEngine().GetLoadedMeshes(), &renderer.m_selectedMeshID, renderer.m_selectedMeshPath);
			ImGui::EndPopup();
		}
		WidgetsUtility::PopStyleVar(); WidgetsUtility::PopStyleVar();

		renderer.m_meshID = renderer.m_selectedMeshID;
		renderer.m_meshPath = renderer.m_selectedMeshPath;

		// Material selection
		if (renderEngine.MaterialExists(renderer.m_materialID))
		{
			renderer.m_selectedMatID = renderer.m_materialID;
			renderer.m_selectedMatPath = renderer.m_materialPath;
		}

		char matPathC[128] = "";
		strcpy(matPathC, renderer.m_selectedMatPath.c_str());

		ImGui::SetCursorPosX(cursorPosLabels);
		WidgetsUtility::AlignedText("Material");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 35 - ImGui::GetCursorPosX());
		ImGui::InputText("##selectedMat", matPathC, IM_ARRAYSIZE(matPathC), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		WidgetsUtility::IncrementCursorPosY(5);

		if (WidgetsUtility::IconButton("##selectmat", ICON_FA_PLUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
			ImGui::OpenPopup("Select Material");

		bool materialPopupOpen = true;
		WidgetsUtility::FramePaddingY(8);
		WidgetsUtility::FramePaddingX(4);
		ImGui::SetNextWindowSize(ImVec2(280, 400));
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Size.x / 2.0f - 140, ImGui::GetMainViewport()->Size.y / 2.0f - 200));
		if (ImGui::BeginPopupModal("Select Material", &materialPopupOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			SelectMaterialModal::Draw(Application::GetRenderEngine().GetLoadedMaterials(), &renderer.m_selectedMatID, renderer.m_selectedMatPath);
			ImGui::EndPopup();
		}
		WidgetsUtility::PopStyleVar(); WidgetsUtility::PopStyleVar();

		renderer.m_materialID = renderer.m_selectedMatID;
		renderer.m_materialPath = renderer.m_selectedMatPath;

		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line.
	WidgetsUtility::DrawBeveledLine();
}

void LinaEngine::ECS::SpriteRendererComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	SpriteRendererComponent& renderer = ecs.get<SpriteRendererComponent>(entity);
	LinaEngine::Graphics::RenderEngine& renderEngine = LinaEngine::Application::GetRenderEngine();

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	bool refreshPressed = false;
	bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<SpriteRendererComponent>(), "Sprite Renderer", ICON_MD_GRID_ON, &refreshPressed, &renderer.m_isEnabled, &renderer.m_foldoutOpen, ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 3));

	// Remove if requested.
	if (removeComponent)
	{
		ecs.remove<SpriteRendererComponent>(entity);
		return;
	}

	// Refresh
	if (refreshPressed)
		ecs.replace<SpriteRendererComponent>(entity, SpriteRendererComponent());

	// Draw component.
	if (renderer.m_foldoutOpen)
	{
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;

		// Material selection
		if (renderEngine.MaterialExists(renderer.m_materialID))
		{
			renderer.m_selectedMatID = renderer.m_materialID;
			renderer.m_selectedMatPath = renderer.m_materialPath;
		}

		char spriteMaterialPath[128] = "heyaa";
		strcpy(spriteMaterialPath, renderer.m_selectedMatPath.c_str());

		ImGui::SetCursorPosX(cursorPosLabels);
		WidgetsUtility::AlignedText("Material");
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosValues);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 35 - ImGui::GetCursorPosX());
		ImGui::InputText("##aqqq", spriteMaterialPath, IM_ARRAYSIZE(spriteMaterialPath), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		WidgetsUtility::IncrementCursorPosY(5);

		if (WidgetsUtility::IconButton("##selectspritemat", ICON_FA_PLUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
			ImGui::OpenPopup("Select Sprite Material");

		bool materialPopupOpen = true;
		WidgetsUtility::FramePaddingY(8);
		WidgetsUtility::FramePaddingX(4);
		ImGui::SetNextWindowSize(ImVec2(280, 400));
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Size.x / 2.0f - 140, ImGui::GetMainViewport()->Size.y / 2.0f - 200));
		if (ImGui::BeginPopupModal("Select Sprite Material", &materialPopupOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			SelectMaterialModal::Draw(Application::GetRenderEngine().GetLoadedMaterials(), &renderer.m_selectedMatID, renderer.m_selectedMatPath);
			ImGui::EndPopup();
		}
		WidgetsUtility::PopStyleVar(); WidgetsUtility::PopStyleVar();

		renderer.m_materialID = renderer.m_selectedMatID;
		renderer.m_materialPath = renderer.m_selectedMatPath;

		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line.
	WidgetsUtility::DrawBeveledLine();
}