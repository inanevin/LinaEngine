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

Class: ComponentDrawer
Timestamp: 10/13/2020 2:34:33 PM

*/


#include "Drawers/ComponentDrawer.hpp"
#include "ECS/ECSComponent.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Components/RigidbodyComponent.hpp"
#include "imgui/imgui.h"
#include "IconsFontAwesome5.h"
#include "IconsMaterialDesign.h"
#include "Widgets/WidgetsUtility.hpp"

using namespace LinaEngine::ECS;
using namespace LinaEditor;

namespace LinaEditor
{
	std::map<LinaEngine::ECS::ECSTypeID, ComponentValueTuple> ComponentDrawer::s_componentDrawFuncMap;

	int ComponentDrawer::s_currentCollisionShape = 0;

	void ComponentDrawer::RegisterComponentFunctions()
	{

		// Display names.
		std::get<0>(s_componentDrawFuncMap[GetTypeID<TransformComponent>()]) = "Transformation";
		std::get<0>(s_componentDrawFuncMap[GetTypeID<RigidbodyComponent>()]) = "Rigidbody";
		std::get<0>(s_componentDrawFuncMap[GetTypeID<CameraComponent>()]) = "Camera";
		std::get<0>(s_componentDrawFuncMap[GetTypeID<DirectionalLightComponent>()]) = "Directional Light";
		std::get<0>(s_componentDrawFuncMap[GetTypeID<SpotLightComponent>()]) = "Spot Light";
		std::get<0>(s_componentDrawFuncMap[GetTypeID<PointLightComponent>()]) = "Point Light";
		std::get<0>(s_componentDrawFuncMap[GetTypeID<FreeLookComponent>()]) = "Free Look";
		std::get<0>(s_componentDrawFuncMap[GetTypeID<MeshRendererComponent>()]) = "Mesh Renderer";
		std::get<0>(s_componentDrawFuncMap[GetTypeID<SpriteRendererComponent>()]) = "Sprite Renderer";

		// Add functions.
		std::get<1>(s_componentDrawFuncMap[GetTypeID<TransformComponent>()]) = std::bind(&TransformComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(s_componentDrawFuncMap[GetTypeID<RigidbodyComponent>()]) = std::bind(&RigidbodyComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(s_componentDrawFuncMap[GetTypeID<CameraComponent>()]) = std::bind(&CameraComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(s_componentDrawFuncMap[GetTypeID<DirectionalLightComponent>()]) = std::bind(&DirectionalLightComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(s_componentDrawFuncMap[GetTypeID<SpotLightComponent>()]) = std::bind(&SpotLightComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(s_componentDrawFuncMap[GetTypeID<PointLightComponent>()]) = std::bind(&PointLightComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(s_componentDrawFuncMap[GetTypeID<FreeLookComponent>()]) = std::bind(&FreeLookComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(s_componentDrawFuncMap[GetTypeID<MeshRendererComponent>()]) = std::bind(&MeshRendererComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<1>(s_componentDrawFuncMap[GetTypeID<SpriteRendererComponent>()]) = std::bind(&SpriteRendererComponent::COMPONENT_ADDFUNC, std::placeholders::_1, std::placeholders::_2);

		// Draw functions.
		std::get<2>(s_componentDrawFuncMap[GetTypeID<TransformComponent>()]) = std::bind(&TransformComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(s_componentDrawFuncMap[GetTypeID<RigidbodyComponent>()]) = std::bind(&RigidbodyComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(s_componentDrawFuncMap[GetTypeID<CameraComponent>()]) = std::bind(&CameraComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(s_componentDrawFuncMap[GetTypeID<DirectionalLightComponent>()]) = std::bind(&DirectionalLightComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(s_componentDrawFuncMap[GetTypeID<SpotLightComponent>()]) = std::bind(&SpotLightComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(s_componentDrawFuncMap[GetTypeID<PointLightComponent>()]) = std::bind(&PointLightComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(s_componentDrawFuncMap[GetTypeID<FreeLookComponent>()]) = std::bind(&FreeLookComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(s_componentDrawFuncMap[GetTypeID<MeshRendererComponent>()]) = std::bind(&MeshRendererComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		std::get<2>(s_componentDrawFuncMap[GetTypeID<SpriteRendererComponent>()]) = std::bind(&SpriteRendererComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
	}

	// Use reflection for gods sake later on.
	std::vector<std::string> ComponentDrawer::GetEligibleComponents(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity)
	{
		std::vector<std::string> eligibleTypes;
		std::vector<ECSTypeID> typeIDs;
		
		// Store all components of the entity.
		ecs->visit(entity, [&typeIDs](const auto component)
			{
				ECSTypeID id = component;
				typeIDs.push_back(id);
			});

		// Iterate registered types & add as eligible if entity does not contain the type.
		for (std::map<ECSTypeID, ComponentValueTuple>::iterator it = ComponentDrawer::s_componentDrawFuncMap.begin(); it != ComponentDrawer::s_componentDrawFuncMap.end(); ++it)
		{
			if (std::find(typeIDs.begin(), typeIDs.end(), it->first) == typeIDs.end())
				eligibleTypes.push_back(std::get<0>(it->second));
		}

		return eligibleTypes;
	}

	void ComponentDrawer::AddComponentToEntity(ECSRegistry* ecs, ECSEntity entity, const std::string& comp)
	{
		// Call the add function of the type when the requested strings match.
		for (std::map<ECSTypeID, ComponentValueTuple>::iterator it = ComponentDrawer::s_componentDrawFuncMap.begin(); it != ComponentDrawer::s_componentDrawFuncMap.end(); ++it)
		{
			if (std::get<0>(it->second).compare(comp) == 0)
				std::get<1>(it->second)(ecs, entity);
		}
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


void LinaEngine::ECS::TransformComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	TransformComponent& transform = ecs->get<TransformComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	static bool open = false;
	bool refreshPressed = false;
	bool removeComponent = WidgetsUtility::DrawComponentTitle("Transformation", ICON_FA_ARROWS_ALT, &refreshPressed, &transform.m_isEnabled, &open, ImGui::GetStyleColorVec4(ImGuiCol_Header));

	// Remove if requested.
	if (removeComponent)
	{
		ecs->remove<TransformComponent>(entity);
		return;
	}

	// Refresh
	if (refreshPressed)
		ecs->replace<TransformComponent>(entity, TransformComponent());

	// Draw component
	if (open)
	{
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Location"); ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat3("##loc", &transform.transform.m_location.x);
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Rotation"); ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); WidgetsUtility::DragQuaternion("##rot", transform.transform.m_rotation);
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Scale");	 ImGui::SameLine();	ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat3("##scale", &transform.transform.m_scale.x);
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);

	}

	// Draw bevel line
	WidgetsUtility::DrawBeveledLine();
}


void LinaEngine::ECS::RigidbodyComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	RigidbodyComponent& rb = ecs->get<RigidbodyComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	static bool open = false;
	bool refreshPressed = false;
	bool removeComponent = WidgetsUtility::DrawComponentTitle("Rigidbody", ICON_MD_ACCESSIBILITY, &refreshPressed, &rb.m_isEnabled, &open, ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0,3));

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
	if (open)
	{
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);

		ComponentDrawer::s_currentCollisionShape = (int)rb.m_collisionShape;

		// Draw collision shape.
		static ImGuiComboFlags flags = 0;
		static ECS::CollisionShape selectedCollisionShape = rb.m_collisionShape;
		const char* collisionShapeLabel = rigidbodyShapes[ComponentDrawer::s_currentCollisionShape];

		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Shape"); ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues);
		if (ImGui::BeginCombo("##collshape", collisionShapeLabel, flags))
		{
			for (int i = 0; i < IM_ARRAYSIZE(rigidbodyShapes); i++)
			{
				const bool is_selected = (ComponentDrawer::s_currentCollisionShape == i);
				if (ImGui::Selectable(rigidbodyShapes[i], is_selected))
				{
					selectedCollisionShape = (ECS::CollisionShape)i;
					ComponentDrawer::s_currentCollisionShape = i;
					rb.m_collisionShape = selectedCollisionShape;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();

		}

		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Mass"); ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues);  ImGui::DragFloat("##mass", &rb.m_mass);

		if (rb.m_collisionShape == ECS::CollisionShape::BOX || rb.m_collisionShape == ECS::CollisionShape::CYLINDER)
		{
			ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Half Extents"); ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat3("##halfextents", &rb.m_halfExtents.x);
		}
		else if (rb.m_collisionShape == ECS::CollisionShape::SPHERE)
		{
			ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Radius"); ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat("##radius", &rb.m_radius);
		}
		else if (rb.m_collisionShape == ECS::CollisionShape::CAPSULE)
		{
			ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Radius"); ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat("##radius", &rb.m_radius);
			ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Height"); ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat("##height", &rb.m_capsuleHeight);
		}

		ImGui::SetCursorPosX(cursorPosLabels);
		if (ImGui::Button("Apply"))
			ecs->replace<LinaEngine::ECS::RigidbodyComponent>(entity, rb);

		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line
	WidgetsUtility::DrawBeveledLine();
}

void LinaEngine::ECS::CameraComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	CameraComponent& camera = ecs->get<CameraComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	static bool open = false;
	bool refreshPressed = false;
	bool removeComponent = WidgetsUtility::DrawComponentTitle("Camera", ICON_FA_VIDEO, &refreshPressed, &camera.m_isEnabled, &open, ImGui::GetStyleColorVec4(ImGuiCol_Header));

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
	if (open)
	{
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Clear Color");	 ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); WidgetsUtility::ColorButton("##clrclr", &camera.clearColor.r);
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Field of View"); ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat("##fov", &camera.fieldOfView);
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Near Plane");	 ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat("##zNear", &camera.zNear);
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Far Plane");	 ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat("##zFar", &camera.zFar);
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line.
	WidgetsUtility::DrawBeveledLine();
}

void LinaEngine::ECS::DirectionalLightComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	DirectionalLightComponent& dLight = ecs->get<DirectionalLightComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	static bool open = false;
	bool refreshPressed = false;
	bool removeComponent = WidgetsUtility::DrawComponentTitle("DirectionalLight", ICON_FA_SUN, &refreshPressed, &dLight.m_isEnabled, &open, ImGui::GetStyleColorVec4(ImGuiCol_Header));

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
	if (open)
	{
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Color");				ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); WidgetsUtility::ColorButton("##dclr", &dLight.color.r);
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Shadow Near Plane");	ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat("##szNear", &dLight.shadowNearPlane);
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Shadow Far Plane");		ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat("##szFar", &dLight.shadowFarPlane);
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Shadow Projection");	ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat4("##sproj", &dLight.shadowProjectionSettings.x);
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line.
	WidgetsUtility::DrawBeveledLine();
}

void LinaEngine::ECS::PointLightComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	PointLightComponent& pLight = ecs->get<PointLightComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	static bool open = false;
	bool refreshPressed = false;
	bool removeComponent = WidgetsUtility::DrawComponentTitle("PointLight", ICON_FA_LIGHTBULB, &refreshPressed, &pLight.m_isEnabled, &open, ImGui::GetStyleColorVec4(ImGuiCol_Header));

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
	if (open)
	{
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Color");		ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); WidgetsUtility::ColorButton("##pclr", &pLight.color.r);
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Distance");	ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat("##pldist", &pLight.distance);
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line.
	WidgetsUtility::DrawBeveledLine();
}

void LinaEngine::ECS::SpotLightComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	SpotLightComponent& sLight = ecs->get<SpotLightComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	static bool open = false;
	bool refreshPressed = false;
	bool removeComponent = WidgetsUtility::DrawComponentTitle("SpotLight", ICON_MD_HIGHLIGHT, &refreshPressed, &sLight.m_isEnabled, &open, ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 3.0f));

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
	if (open)
	{
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Color");		ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); WidgetsUtility::ColorButton("##sclr", &sLight.color.r);
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Distance");		ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat("##sldist", &sLight.distance);
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Cutoff");		ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat("##cutOff", &sLight.cutOff);
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Outer Cutoff");	ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat("##outerCutOff", &sLight.outerCutOff);
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line.
	WidgetsUtility::DrawBeveledLine();
}


void LinaEngine::ECS::FreeLookComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	FreeLookComponent& freeLook = ecs->get<FreeLookComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	static bool open = false;
	bool refreshPressed = false;
	bool removeComponent = WidgetsUtility::DrawComponentTitle("FreeLook", ICON_MD_3D_ROTATION, &refreshPressed, &freeLook.m_isEnabled, &open, ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0,3));

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
	if (open)
	{
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Movement Speeds");	ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat2("##ms", &freeLook.m_movementSpeeds.x);
		ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Rotation Speeds");	ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat2("##rs", &freeLook.m_rotationSpeeds.x);
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line.
	WidgetsUtility::DrawBeveledLine();
}

void LinaEngine::ECS::MeshRendererComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	MeshRendererComponent& renderer = ecs->get<MeshRendererComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	static bool open = false;
	bool refreshPressed = false;
	bool removeComponent = WidgetsUtility::DrawComponentTitle("MeshRenderer", ICON_MD_GRID_ON, &refreshPressed, &renderer.m_isEnabled, &open, ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0,3));

	// Remove if requested.
	if (removeComponent)
	{
		ecs->remove<MeshRendererComponent>(entity);
		return;
	}

	// Refresh
	if (refreshPressed)
		ecs->replace<MeshRendererComponent>(entity, MeshRendererComponent());

	// Draw component.
	if (open)
	{
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;
		//ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Movement Speeds");	ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat2("##ms", &renderer.m_movementSpeeds.x);
		//ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Rotation Speeds");	ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat2("##rs", &renderer.m_rotationSpeeds.x);
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line.
	WidgetsUtility::DrawBeveledLine();
}

void LinaEngine::ECS::SpriteRendererComponent::COMPONENT_DRAWFUNC(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity)
{
	// Get component
	SpriteRendererComponent& renderer = ecs->get<SpriteRendererComponent>(entity);

	// Align.
	WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
	WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

	// Draw title.
	static bool open = false;
	bool refreshPressed = false;
	bool removeComponent = WidgetsUtility::DrawComponentTitle("SpriteRenderer", ICON_FA_LIGHTBULB, &refreshPressed, &renderer.m_isEnabled, &open, ImGui::GetStyleColorVec4(ImGuiCol_Header));

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
	if (open)
	{
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
		float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
		float cursorPosLabels = CURSORPOS_X_LABELS;
		//ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Movement Speeds");	ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat2("##ms", &renderer.m_movementSpeeds.x);
		//ImGui::SetCursorPosX(cursorPosLabels); WidgetsUtility::AlignedText("Rotation Speeds");	ImGui::SameLine(); ImGui::SetCursorPosX(cursorPosValues); ImGui::DragFloat2("##rs", &renderer.m_rotationSpeeds.x);
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
	}

	// Draw bevel line.
	WidgetsUtility::DrawBeveledLine();
}