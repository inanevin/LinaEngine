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
	std::map<entt::id_type, std::function<void(LinaEngine::ECS::ECSRegistry*, LinaEngine::ECS::ECSEntity)>> ComponentDrawer::s_componentDrawFuncMap;
	int ComponentDrawer::s_currentCollisionShape = 0;

	void ComponentDrawer::RegisterComponentFunctions()
	{
		s_componentDrawFuncMap[entt::type_info<TransformComponent>::id()] = std::bind(&TransformComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		s_componentDrawFuncMap[entt::type_info<RigidbodyComponent>::id()] = std::bind(&RigidbodyComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		s_componentDrawFuncMap[entt::type_info<CameraComponent>::id()] = std::bind(&CameraComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		s_componentDrawFuncMap[entt::type_info<DirectionalLightComponent>::id()] = std::bind(&DirectionalLightComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		s_componentDrawFuncMap[entt::type_info<SpotLightComponent>::id()] = std::bind(&SpotLightComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		s_componentDrawFuncMap[entt::type_info<PointLightComponent>::id()] = std::bind(&PointLightComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		s_componentDrawFuncMap[entt::type_info<FreeLookComponent>::id()] = std::bind(&FreeLookComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		s_componentDrawFuncMap[entt::type_info<MeshRendererComponent>::id()] = std::bind(&MeshRendererComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
		s_componentDrawFuncMap[entt::type_info<SpriteRendererComponent>::id()] = std::bind(&SpriteRendererComponent::COMPONENT_DRAWFUNC, std::placeholders::_1, std::placeholders::_2);
	}

	// Use reflection for gods sake later on.
	std::vector<std::string> ComponentDrawer::GetEligibleComponents(LinaEngine::ECS::ECSRegistry* reg, LinaEngine::ECS::ECSEntity entity)
	{
		std::vector<std::string> eligibleTypes;

		if (!reg->has<TransformComponent>(entity))
			eligibleTypes.push_back("Transformation");
		if (!reg->has<MeshRendererComponent>(entity))
			eligibleTypes.push_back("MeshRenderer");
		if (!reg->has<CameraComponent>(entity))
			eligibleTypes.push_back("Camera");
		if (!reg->has<FreeLookComponent>(entity))
			eligibleTypes.push_back("FreeLook");
		if (!reg->has<PointLightComponent>(entity))
			eligibleTypes.push_back("PointLight");
		if (!reg->has<SpotLightComponent>(entity))
			eligibleTypes.push_back("SpotLight");
		if (!reg->has<DirectionalLightComponent>(entity))
			eligibleTypes.push_back("DirectionalLight");
		if (!reg->has<RigidbodyComponent>(entity))
			eligibleTypes.push_back("Rigidbody");

		return eligibleTypes;
	}

	void ComponentDrawer::AddComponentToEntity(ECSRegistry* ecs, ECSEntity entity, const std::string& comp)
	{

		LINA_CLIENT_TRACE(comp);
		if (comp.compare("Transformation") == 0)
			ecs->emplace<TransformComponent>(entity, TransformComponent());
		else if (comp.compare("MeshRenderer") == 0)
			ecs->emplace<MeshRendererComponent>(entity, MeshRendererComponent());
		else if (comp.compare("Camera") == 0)
			ecs->emplace<CameraComponent>(entity, CameraComponent());
		else if (comp.compare("FreeLook") == 0)
			ecs->emplace<FreeLookComponent>(entity, FreeLookComponent());
		else if (comp.compare("PointLight") == 0)
			ecs->emplace<PointLightComponent>(entity, PointLightComponent());
		else if (comp.compare("SpotLight") == 0)
			ecs->emplace<SpotLightComponent>(entity, SpotLightComponent());
		else if (comp.compare("DirectionalLight") == 0)
			ecs->emplace<DirectionalLightComponent>(entity, DirectionalLightComponent());
		else if (comp.compare("Rigidbody") == 0)
			ecs->emplace<RigidbodyComponent>(entity, RigidbodyComponent());

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