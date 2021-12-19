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
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>
#include <entt/meta/node.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/node.hpp>
#include <entt/meta/resolve.hpp>
using namespace Lina::ECS;
using namespace Lina::Editor;


namespace Lina::Editor
{

	std::map<entt::id_type, std::string> m_variableTypeIdentifiers;
	std::map<entt::id_type, bool> m_componentFoldoutState;

	ComponentDrawer* ComponentDrawer::s_activeInstance = nullptr;
	using namespace entt::literals;

	template<typename Type>
	Type& Drawer_Get(Lina::ECS::Entity entity) {
		return Lina::ECS::Registry::Get()->template get<Type>(entity);
	}

	template<typename Type>
	void Drawer_Reset(Lina::ECS::Entity entity) {
		Lina::ECS::Registry::Get()->template replace<Type>(entity, Type());
	}

	template<typename Type>
	void Drawer_Remove(Lina::ECS::Entity entity) {
		Lina::ECS::Registry::Get()->template remove<Type>(entity);
	}

	template<typename Type>
	void Drawer_Copy(entt::entity entity) {

	}

	template<typename Type>
	void Drawer_Paste(entt::entity entity) {

	}

	template<typename Type>
	void Drawer_SetEnabled(bool enabled, Lina::ECS::Entity ent)
	{
		Lina::ECS::Registry::Get()->template get<Type>(ent).m_isEnabled = enabled;

		// If we are toggling model renderer component, toggle every mesh renderer component below it.
		if (entt::type_id<Type>().hash() == entt::type_id<ModelRendererComponent>().hash())
		{
			auto& data = Lina::ECS::Registry::Get()->get<EntityDataComponent>(ent);
			for (auto child : data.m_children)
			{
				auto* meshRenderer = Lina::ECS::Registry::Get()->try_get<MeshRendererComponent>(child);
				meshRenderer->m_isEnabled = enabled;
			}
		}
	}

	template<typename Type>
	void Drawer_SetModel(Lina::ECS::Entity ent, Lina::Graphics::Model* model)
	{
		Lina::ECS::Registry::Get()->get<Type>(ent).SetModel(ent, *model);
	}

	template<typename Type>
	void Drawer_RemoveModel(Lina::ECS::Entity ent)
	{
		Type& comp = Lina::ECS::Registry::Get()->template get<Type>(ent);
		comp.RemoveModel(ent);
	}

	template<typename Type>
	void Drawer_SetMaterial(Lina::ECS::Entity ent, int materialIndex, const Graphics::Material& material)
	{
		Type& comp = Lina::ECS::Registry::Get()->template get<Type>(ent);
		comp.SetMaterial(ent, materialIndex, material);
	}

	template<typename Type>
	void Drawer_RemoveMaterial(Lina::ECS::Entity ent, int materialIndex)
	{
		Type& comp = Lina::ECS::Registry::Get()->template get<Type>(ent);
		comp.RemoveMaterial(ent, materialIndex);
	}

	template<typename Type>
	std::string Drawer_GetMaterialName(Lina::ECS::Entity ent, int index)
	{
		std::string modelPath = Lina::ECS::Registry::Get()->template get<Type>(ent).GetModelPath();
		return Lina::Graphics::Model::GetModel(modelPath).GetMaterialSpecs()[index].m_name;;
	}


	void Drawer_DebugPointLight(Lina::ECS::Entity ent)
	{
		ECS::EntityDataComponent& data = Lina::ECS::Registry::Get()->get<ECS::EntityDataComponent>(ent);
		ECS::PointLightComponent& pLight = Lina::ECS::Registry::Get()->get<ECS::PointLightComponent>(ent);
		Vector3 end1 = data.GetLocation() + (pLight.m_distance * data.GetRotation().GetRight());
		Vector3 end2 = data.GetLocation() + (-pLight.m_distance * data.GetRotation().GetRight());
		Vector3 end3 = data.GetLocation() + (pLight.m_distance * data.GetRotation().GetForward());
		Vector3 end4 = data.GetLocation() + (-pLight.m_distance * data.GetRotation().GetForward());
		Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end1, Lina::Color::Red, 1.4f);
		Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end2, Lina::Color::Red, 1.4f);
		Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end3, Lina::Color::Red, 1.4f);
		Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end4, Lina::Color::Red, 1.4f);
	}

	void Drawer_DebugSpotLight(Lina::ECS::Entity ent)
	{
		ECS::EntityDataComponent& data = Lina::ECS::Registry::Get()->get<ECS::EntityDataComponent>(ent);
		ECS::SpotLightComponent& sLight = Lina::ECS::Registry::Get()->get<ECS::SpotLightComponent>(ent);
		Vector3 end1 = data.GetLocation() + (sLight.m_distance * data.GetRotation().GetForward());
		Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end1, Lina::Color::Red, 1.4f);
	}

	void Drawer_DebugDirectionalLight(Lina::ECS::Entity ent)
	{
		ECS::EntityDataComponent& data = Lina::ECS::Registry::Get()->get<ECS::EntityDataComponent>(ent);
		Vector3 dir = Vector3::Zero - data.GetLocation();
		Vector3 end1 = data.GetLocation() + dir;
		Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end1, Lina::Color::Red, 1.4f);
	}


	template<typename Type>
	void RegisterComponentForEditor(char* title, char* icon, uint8 drawFlags)
	{
		entt::meta<Type>().type().props(std::make_pair("Foldout"_hs, true), std::make_pair("Title"_hs, title), std::make_pair("Icon"_hs, icon), std::make_pair("DrawFlags"_hs, drawFlags));
		entt::meta<Type>().func<&Drawer_SetEnabled<Type>, entt::as_ref_t>("setEnabled"_hs);
		entt::meta<Type>().func<&Drawer_Get<Type>, entt::as_ref_t>("get"_hs);
		entt::meta<Type>().func<&Drawer_Reset<Type>, entt::as_ref_t>("reset"_hs);
		entt::meta<Type>().func<&Drawer_Remove<Type>, entt::as_ref_t>("remove"_hs);
		entt::meta<Type>().func<&Drawer_Copy<Type>, entt::as_ref_t>("copy"_hs);
		entt::meta<Type>().func<&Drawer_Paste<Type>, entt::as_ref_t>("paste"_hs);
	}

	void Drawer_DebugPLight(Lina::ECS::Entity ent) {
		LINA_TRACE("Debug PLIGHT");
	}
	ComponentDrawer::ComponentDrawer()
	{
		RegisterComponentToDraw<CameraComponent>(GetTypeID<CameraComponent>(), "Camera", std::bind(&ComponentDrawer::DrawCameraComponent, this, std::placeholders::_1));
		RegisterComponentToDraw<DirectionalLightComponent>(GetTypeID<DirectionalLightComponent>(), "Directional Light", std::bind(&ComponentDrawer::DrawDirectionalLightComponent, this, std::placeholders::_1));
		RegisterComponentToDraw<SpotLightComponent>(GetTypeID<SpotLightComponent>(), "Spot Light", std::bind(&ComponentDrawer::DrawSpotLightComponent, this, std::placeholders::_1));
		RegisterComponentToDraw<PointLightComponent>(GetTypeID<PointLightComponent>(), "Point Light", std::bind(&ComponentDrawer::DrawPointLightComponent, this, std::placeholders::_1));
		RegisterComponentToDraw<FreeLookComponent>(GetTypeID<FreeLookComponent>(), "Free Look", std::bind(&ComponentDrawer::DrawFreeLookComponent, this, std::placeholders::_1));
		RegisterComponentToDraw<MeshRendererComponent>(GetTypeID<MeshRendererComponent>(), "Mesh Renderer", std::bind(&ComponentDrawer::DrawMeshRendererComponent, this, std::placeholders::_1));
		RegisterComponentToDraw<ModelRendererComponent>(GetTypeID<ModelRendererComponent>(), "Model Renderer", std::bind(&ComponentDrawer::DrawModelRendererComponent, this, std::placeholders::_1));
		RegisterComponentToDraw<SpriteRendererComponent>(GetTypeID<SpriteRendererComponent>(), "Sprite Renderer", std::bind(&ComponentDrawer::DrawSpriteRendererComponent, this, std::placeholders::_1));

		uint8 defaultDrawFlags = ComponentDrawFlags_None;

#define PROPS(LABEL, TYPE) std::make_pair("Label"_hs, LABEL), std::make_pair("Type"_hs, TYPE)
#define PROPS_DEP(LABEL,TYPE, DISPLAYDEPENDENCY) std::make_pair("Label"_hs, LABEL), std::make_pair("Type"_hs, TYPE), std::make_pair("DisplayDependency"_hs, DISPLAYDEPENDENCY)

		// Camera component
		entt::meta<CameraComponent>().data<&CameraComponent::m_isEnabled>("enabled"_hs);
		entt::meta<CameraComponent>().data<&CameraComponent::m_zFar>("zf"_hs).props(PROPS("Far", ComponentVariableType::DragFloat));
		entt::meta<CameraComponent>().data<&CameraComponent::m_zNear>("zn"_hs).props(PROPS("Near", ComponentVariableType::DragFloat));
		entt::meta<CameraComponent>().data<&CameraComponent::m_fieldOfView>("fov"_hs).props(PROPS("Fov", ComponentVariableType::DragFloat));
		entt::meta<CameraComponent>().data<&CameraComponent::m_clearColor>("cc"_hs).props(PROPS("Clear Color", ComponentVariableType::Color));
		RegisterComponentForEditor<CameraComponent>("Camera", ICON_FA_CAMERA, defaultDrawFlags);

		// Dirlight
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_isEnabled>("enabled"_hs);
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_drawDebug>("debug"_hs).props(PROPS("Enable Debug", ComponentVariableType::Checkmark));
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_shadowZFar>("szf"_hs).props(PROPS_DEP("Shadow Far", ComponentVariableType::DragFloat, "castShadows"_hs));
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_shadowZNear>("szn"_hs).props(PROPS_DEP("Shadow Near", ComponentVariableType::DragFloat, "castShadows"_hs));
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_shadowOrthoProjection>("so"_hs).props(PROPS_DEP("Shadow Projection", ComponentVariableType::Vector4, "castShadows"_hs));
		//entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_castsShadows>("castShadows"_hs).props(PROPS("Cast Shadows", ComponentVariableType::Checkmark));
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_intensity>("i"_hs).props(PROPS("Intensity", ComponentVariableType::DragFloat));
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_color>("cc"_hs).props(PROPS("Color", ComponentVariableType::Color));
		entt::meta<DirectionalLightComponent>().func<&Drawer_DebugDirectionalLight, entt::as_ref_t>("drawDebug"_hs);
		RegisterComponentForEditor<DirectionalLightComponent>("Directional Light", ICON_FA_SUN, defaultDrawFlags);

		// Spotlight
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_isEnabled>("enabled"_hs);
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_drawDebug>("debug"_hs).props(PROPS("Enable Debug", ComponentVariableType::Checkmark));
		// entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_castsShadows>("castShadows"_hs).props(PROPS("Cast Shadows", ComponentVariableType::Checkmark));
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_outerCutoff>("oc"_hs).props(PROPS("Outer Cutoff", ComponentVariableType::DragFloat));
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_cutoff>("cof"_hs).props(PROPS("Cutoff", ComponentVariableType::DragFloat));
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_distance>("dist"_hs).props(PROPS("Distance", ComponentVariableType::DragFloat));
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_intensity>("int"_hs).props(PROPS("Intensity", ComponentVariableType::DragFloat));
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_color>("c"_hs).props(PROPS("Color", ComponentVariableType::Color));
		entt::meta<SpotLightComponent>().func<&Drawer_DebugSpotLight, entt::as_ref_t>("drawDebug"_hs);
		RegisterComponentForEditor<SpotLightComponent>("Spot Light", ICON_MD_FLASH_ON, defaultDrawFlags);

		// Pointlight
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_isEnabled>("enabled"_hs);
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_drawDebug>("debug"_hs).props(PROPS("Enable Debug", ComponentVariableType::Checkmark));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_shadowFar>("sf"_hs).props(PROPS_DEP("Shadow Far", ComponentVariableType::DragFloat, "castShadows"_hs));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_shadowNear>("sn"_hs).props(PROPS_DEP("Shadow Near", ComponentVariableType::DragFloat, "castShadows"_hs));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_bias>("b"_hs).props(PROPS_DEP("Bias", ComponentVariableType::DragFloat, "castShadows"_hs));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_castsShadows>("castShadows"_hs).props(PROPS("Cast Shadows", ComponentVariableType::Checkmark));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_distance>("ds"_hs).props(PROPS("Distance", ComponentVariableType::DragFloat));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_intensity>("i"_hs).props(PROPS("Intensity", ComponentVariableType::DragFloat));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_color>("c"_hs).props(PROPS("Color", ComponentVariableType::Color));
		entt::meta<PointLightComponent>().func<&Drawer_DebugPointLight, entt::as_ref_t>("drawDebug"_hs);
		RegisterComponentForEditor<PointLightComponent>("Point Light", ICON_FA_LIGHTBULB, defaultDrawFlags);

		// Freelook
		entt::meta<FreeLookComponent>().data<&FreeLookComponent::m_isEnabled>("enabled"_hs);
		entt::meta<FreeLookComponent>().data<&FreeLookComponent::m_rotationSpeeds>("rs"_hs).props(PROPS("Rotation Speed", ComponentVariableType::Vector2));
		entt::meta<FreeLookComponent>().data<&FreeLookComponent::m_movementSpeeds>("ms"_hs).props(PROPS("Movement Speed", ComponentVariableType::Vector2));
		RegisterComponentForEditor<FreeLookComponent>("Freelook", ICON_FA_EYE, defaultDrawFlags);

		// Model Renderer
		auto idt = "matpath_arr"_hs;
		entt::meta<ModelRendererComponent>().data<&ModelRendererComponent::m_isEnabled>("enabled"_hs);
		entt::meta<ModelRendererComponent>().data<&ModelRendererComponent::m_materialPaths>(idt).props(PROPS("Materials", ComponentVariableType::MaterialPathArray));
		entt::meta<ModelRendererComponent>().data<&ModelRendererComponent::m_modelPath>("modpath"_hs).props(PROPS("Model", ComponentVariableType::ModelPath));
		entt::meta<ModelRendererComponent>().func<&Drawer_SetModel<ModelRendererComponent>, entt::as_ref_t>("setModel"_hs);
		entt::meta<ModelRendererComponent>().func<&Drawer_GetMaterialName<ModelRendererComponent>, entt::as_ref_t>("getMaterialName"_hs);
		entt::meta<ModelRendererComponent>().func<&Drawer_RemoveModel<ModelRendererComponent>, entt::as_ref_t>("removeModel"_hs);
		entt::meta<ModelRendererComponent>().func<&Drawer_SetMaterial<ModelRendererComponent>, entt::as_ref_t>("setMaterial"_hs);
		entt::meta<ModelRendererComponent>().func<&Drawer_RemoveMaterial<ModelRendererComponent>, entt::as_ref_t>("removeMaterial"_hs);
		RegisterComponentForEditor<ModelRendererComponent>("Model Renderer", ICON_FA_CUBE, defaultDrawFlags);

		// Sprite renderer
		entt::meta<SpriteRendererComponent>().data<&SpriteRendererComponent::m_isEnabled>("enabled"_hs);
		entt::meta<SpriteRendererComponent>().data<&SpriteRendererComponent::m_materialPaths>("matpath"_hs).props(PROPS("Material", ComponentVariableType::MaterialPath));;
		RegisterComponentForEditor<SpriteRendererComponent>("Sprite", ICON_MD_TOYS, defaultDrawFlags);





		//	ECS::TypeID tid;
		//	auto x = entt::resolve(tid).data();
		//
		//	for (auto data : entt::resolve<ModelRendererComponent>().data()) {
		//		auto t = data.type();
		//		LINA_TRACE("xdxdxaaad {0}", m_variableTypeIdentifiers[data.id()]);
		//	}
		//	auto factory = entt::meta<ModelRendererComponent>().type();

	}

	void ComponentDrawer::Initialize()
	{
		s_activeInstance = this;

		auto* ecs = Lina::ECS::Registry::Get();
		Lina::ECS::Entity ent = ecs->CreateEntity("ent");
		auto& pl = ecs->emplace<PointLightComponent>(ent);

		Lina::ECS::TypeID tid = entt::type_id<PointLightComponent>().hash();

		auto resolved = entt::resolve(tid);

		entt::meta_any& inst = resolved.func("get"_hs).invoke({}, ent);


		bool enabledData = resolved.data("enabled"_hs).get(inst).cast<bool>();
		float intensity = resolved.data("i"_hs).get(inst).cast<float>();
		resolved.data("i"_hs).set(inst, 17.75f);
		float intensity2 = resolved.data("i"_hs).get(inst).cast<float>();

		for (auto data : resolved.data())
		{
			if (data.type().is_floating_point())
			{
				data.set(inst, 29.87f);
			}
		}

		float intensity3 = resolved.data("i"_hs).get(inst).cast<float>();
		auto& pl2 = ecs->get<PointLightComponent>(ent);

		LINA_TRACE("a");
	}

	// Use reflection for gods sake later on.
	std::vector<std::string> ComponentDrawer::GetEligibleComponents(Lina::ECS::Entity entity)
	{
		std::vector<std::string> eligibleTypes;
		std::vector<TypeID> typeIDs;
		auto* ecs = Lina::ECS::Registry::Get();

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

	void ComponentDrawer::AddComponentToEntity(Entity entity, const std::string& comp)
	{
		// Call the add function of the type when the requested strings match.
		for (std::map<TypeID, ComponentValueTuple>::iterator it = m_componentFunctionsMap.begin(); it != m_componentFunctionsMap.end(); ++it)
		{
			if (std::get<0>(it->second).compare(comp) == 0)
				std::get<1>(it->second)(entity);
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

	void ComponentDrawer::DrawComponents(Lina::ECS::Entity entity)
	{
		s_activeInstance = this;

		// Draw components.
		for (int i = 0; i < m_componentDrawList.size(); i++)
		{
			auto func = std::get<2>(m_componentFunctionsMap[m_componentDrawList[i]]);
			if (func)
				func(entity);
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
		if (ImGui::IsItemClicked())
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
				Lina::ECS::TypeID payloadID = *(const Lina::ECS::TypeID*)payload->Data;
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


	void ComponentDrawer::DrawComponent(Lina::ECS::TypeID tid, Lina::ECS::Entity ent)
	{
		auto* ecs = ECS::Registry::Get();
		auto resolvedData = entt::resolve(tid);

		if (resolvedData)
		{
			auto title = resolvedData.prop("Title"_hs).value().cast<char*>();
			uint8 drawFlags = resolvedData.prop("DrawFlags"_hs).value().cast<uint8>();
			auto icon = resolvedData.prop("Icon"_hs).value().cast<char*>();
			auto foldout = resolvedData.prop("Foldout"_hs).value().cast<bool>();
			entt::meta_any& instance = resolvedData.func("get"_hs).invoke({}, ent);

			bool drawToggle = !(drawFlags & ComponentDrawFlags_NoToggle);
			bool drawRemove = !(drawFlags & ComponentDrawFlags_NoRemove);
			bool drawCopy = !(drawFlags & ComponentDrawFlags_NoCopy);
			bool drawPaste = !(drawFlags & ComponentDrawFlags_NoPaste);
			bool drawReset = !(drawFlags & ComponentDrawFlags_NoReset);
			bool remove = false;
			bool copy = false;
			bool paste = false;
			bool reset = false;
			bool enabled = resolvedData.data("enabled"_hs).get(instance).cast<bool>();
			WidgetsUtility::ComponentHeader(&m_componentFoldoutState[tid], title, icon, drawToggle ? &enabled : nullptr, drawRemove ? &remove : nullptr, drawCopy ? &copy : nullptr, drawPaste ? &paste : nullptr, drawReset ? &reset : nullptr);

			resolvedData.data("enabled"_hs).set(instance, enabled);

			if (remove)
			{
				resolvedData.func("remove"_hs).invoke({}, ent);
				return;
			}

			if (reset)
				resolvedData.func("reset"_hs).invoke({}, ent);

			if (copy)
				resolvedData.func("copy"_hs).invoke({}, ent);

			if (paste)
				resolvedData.func("paste"_hs).invoke({}, ent);

			if (m_componentFoldoutState[tid])
			{
			
				// Draw each reflected property in the component according to it's type.
				WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFOREVAL);
				float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
				float cursorPosLabels = CURSORPOS_X_LABELS;
				int varCounter = 0;
				std::string varLabelID = "";
				for (auto data : resolvedData.data())
				{

					auto labelProperty = data.prop("Label"_hs);
					auto typeProperty = data.prop("Type"_hs);
					if (!labelProperty || !typeProperty) continue;
					const char* label = labelProperty.value().cast<const char*>();
					ComponentVariableType type = typeProperty.value().cast<ComponentVariableType>();

					auto displayDependencyProperty = data.prop("DisplayDependency"_hs);

					if (displayDependencyProperty)
					{
						entt::hashed_string displayDependencyHash = displayDependencyProperty.value().cast<entt::hashed_string>();
						auto dependantMember = resolvedData.data(displayDependencyHash);

						if (dependantMember)
						{
							bool dependantMemberValue = dependantMember.get(instance).cast<bool>();
							if (!dependantMemberValue) continue;
						}
					}

					varLabelID = "##_" + std::string(title) + std::to_string(varCounter);
					ImGui::SetCursorPosX(cursorPosLabels);
					WidgetsUtility::AlignedText(label);
					ImGui::SameLine();
					ImGui::SetCursorPosX(cursorPosValues);

					if (type == ComponentVariableType::DragFloat)
					{
						float variable = data.get(instance).cast<float>();
						ImGui::DragFloat(varLabelID.c_str(), &variable);
						data.set(instance, variable);
					}
					else if (type == ComponentVariableType::DragInt)
					{
						int variable = data.get(instance).cast<int>();
						ImGui::DragInt(varLabelID.c_str(), &variable);
						data.set(instance, variable);
					}
					else if (type == ComponentVariableType::Vector2)
					{
						Vector2 variable = data.get(instance).cast<Vector2>();
						ImGui::DragFloat2(varLabelID.c_str(), &variable.x);
						data.set(instance, variable);
					}
					else if (type == ComponentVariableType::Vector3)
					{
						Vector3 variable = data.get(instance).cast<Vector3>();
						ImGui::DragFloat3(varLabelID.c_str(), &variable.x);
						data.set(instance, variable);
					}
					else if (type == ComponentVariableType::Vector4)
					{
						Vector4 variable = data.get(instance).cast<Vector4>();
						ImGui::DragFloat4(varLabelID.c_str(), &variable.x);
						data.set(instance, variable);
					}
					else if (type == ComponentVariableType::Color)
					{
						Color variable = data.get(instance).cast<Color>();
						WidgetsUtility::ColorButton(varLabelID.c_str(), &variable.r);
						data.set(instance, variable);
					}
					else if (type == ComponentVariableType::Checkmark)
					{
						bool variable = data.get(instance).cast<bool>();
						ImGui::Checkbox(varLabelID.c_str(), &variable);
						data.set(instance, variable);
					}
					else if (type == ComponentVariableType::ModelPath)
					{
						std::string modelPath = data.get(instance).cast<std::string>();
						bool removed = false;
						Graphics::Model* selectedModel = WidgetsUtility::ModelComboBox(varLabelID.c_str(), StringID(modelPath.c_str()).value(), &removed);

						// Select model.
						if (selectedModel)
							resolvedData.func("setModel"_hs).invoke({}, ent, selectedModel);

						// Remove Model
						if (removed || reset)
							resolvedData.func("removeModel"_hs).invoke({}, ent);


						// Mesh drag & drop.
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMESH_ID))
							{
								IM_ASSERT(payload->DataSize == sizeof(StringIDType));

								auto& model = Lina::Graphics::Model::GetModel(*(StringIDType*)payload->Data);
								resolvedData.func("setModel"_hs).invoke({}, ent, model);
							}
							ImGui::EndDragDropTarget();
						}
					}
					else if (type == ComponentVariableType::MaterialPathArray)
					{
						ImGui::NewLine();
						ImGui::SetCursorPosX(cursorPosLabels);

						std::vector<std::string> materials = data.get(instance).cast<std::vector<std::string>>();

						for (int i = 0; i < materials.size(); i++)
						{
							// Material selection.
							char matPathC[128] = "";
							strcpy(matPathC, materials[i].c_str());

							// Draw material name
							ImGui::SetCursorPosX(cursorPosLabels);
							std::string materialName = resolvedData.func("getMaterialName"_hs).invoke({}, ent, i).cast<std::string>();
							WidgetsUtility::AlignedText(materialName.c_str());
							ImGui::SameLine();
							ImGui::SetCursorPosX(cursorPosValues);

							const std::string cboxID = "##modRendMat " + std::to_string(i);
							bool removed = false;
							Lina::Graphics::Material* selectedMaterial = WidgetsUtility::MaterialComboBox(cboxID.c_str(), materials[i], &removed);

							if (selectedMaterial != nullptr)
								resolvedData.func("setMaterial"_hs).invoke({}, ent, i, *selectedMaterial);

							if (removed)
								resolvedData.func("removeMaterial"_hs).invoke({}, ent, i);


							// Material drag & drop.
							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMATERIAL_ID))
								{
									IM_ASSERT(payload->DataSize == sizeof(StringIDType));

									auto& mat = Lina::Graphics::Material::GetMaterial(*(StringIDType*)payload->Data);
									resolvedData.func("setMaterial"_hs).invoke({}, ent, i, mat);
								}
								ImGui::EndDragDropTarget();
							}

						}

					}



					varCounter++;
					varLabelID.clear();
				}

				// Check if the target data contains a debug property,
				// If so, and if the property is set to true,
				// Call the debug function if exists.
				auto debugProperty = resolvedData.data("debug"_hs);
				if (debugProperty)
				{
					bool isDebugEnabled = debugProperty.get(instance).cast<bool>();

					if (isDebugEnabled)
					{
						auto debugFunc = resolvedData.func("drawDebug"_hs);

						if (debugFunc)
							debugFunc.invoke({}, ent);
					}
				}
			}



		}

	}

	void ComponentDrawer::DrawEntityData(Lina::ECS::Entity entity)
	{
		// Get component
		auto* ecs = Lina::ECS::Registry::Get();
		EntityDataComponent& data = ecs->get<EntityDataComponent>(entity);
		TypeID id = GetTypeID<EntityDataComponent>();

		bool foldoutOpen = m_foldoutStateMap[entity][id];
		float sizeY = foldoutOpen ? 15 : 100;
		bool enabled = false;
		bool close, copy, reset = false;
		bool actv = false;
		//WidgetsUtility::ComponentHeader(&enabled, "Entity Data", ICON_FA_ADDRESS_BOOK, &actv, &close, &copy, &reset);
		//ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.05f, 1.0f));
		//ImGui::BeginChild("##entity_data", ImVec2(0, 0), ImGuiWindowFlags_NoDecoration);
		//
		//
		//ImGui::EndChild();
		//ImGui::PopStyleColor();
	}

	void ComponentDrawer::DrawEntityDataComponent(Lina::ECS::Entity entity)
	{
		// Get component
		auto* ecs = Lina::ECS::Registry::Get();
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

	void ComponentDrawer::DrawCameraComponent(Lina::ECS::Entity entity)
	{
		// Get component
		auto* ecs = Lina::ECS::Registry::Get();
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

	void ComponentDrawer::DrawFreeLookComponent(Lina::ECS::Entity entity)
	{
		// Get component
		auto* ecs = Lina::ECS::Registry::Get();
		FreeLookComponent& freeLook = ecs->get<FreeLookComponent>(entity);
		TypeID id = GetTypeID<FreeLookComponent>();

		// Align.
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

		bool removeComponent = false;
		bool refreshPressed = false;
		bool copyButton = false;
		//WidgetsUtility::ComponentHeader(&(m_foldoutStateMap[entity][id]), "Freelook", ICON_FA_EYE, &freeLook.m_isEnabled, &removeComponent, &copyButton, &refreshPressed);

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

	void ComponentDrawer::DrawRigidbodyComponent(Lina::ECS::Entity entity)
	{
		// Get component
		auto* ecs = Lina::ECS::Registry::Get();
		PhysicsComponent& rb = ecs->get<PhysicsComponent>(entity);
		TypeID id = GetTypeID<PhysicsComponent>();

		// Align.
		WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_BEFORE);
		WidgetsUtility::IncrementCursorPosX(CURSORPOS_X_LABELS);

		// Draw title.
		bool refreshPressed = false;
		bool removeComponent = ComponentDrawer::s_activeInstance->DrawComponentTitle(GetTypeID<PhysicsComponent>(), "Rigidbody", ICON_MD_ACCESSIBILITY, &refreshPressed, &rb.m_isEnabled, &m_foldoutStateMap[entity][id], ImGui::GetStyleColorVec4(ImGuiCol_Header), ImVec2(0, 3));

		// Remove if requested.
		if (removeComponent)
		{
			ecs->remove<PhysicsComponent>(entity);
			return;
		}

		// Refresh
		if (refreshPressed)
			ecs->replace<PhysicsComponent>(entity, PhysicsComponent());

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
				ecs->replace<Lina::ECS::PhysicsComponent>(entity, rb);

			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
		}

		// Draw bevel line
		WidgetsUtility::DrawBeveledLine();
	}

	void ComponentDrawer::DrawPointLightComponent(Lina::ECS::Entity entity)
	{
		// Get component
		auto* ecs = Lina::ECS::Registry::Get();
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

	void ComponentDrawer::DrawSpotLightComponent(Lina::ECS::Entity entity)
	{
		// Get component
		auto* ecs = Lina::ECS::Registry::Get();
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

	void ComponentDrawer::DrawDirectionalLightComponent(Lina::ECS::Entity entity)
	{
		// Get component
		auto* ecs = Lina::ECS::Registry::Get();
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

	void ComponentDrawer::DrawMeshRendererComponent(Lina::ECS::Entity entity)
	{
		// Get component
		auto* ecs = Lina::ECS::Registry::Get();
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

	void ComponentDrawer::DrawModelRendererComponent(Lina::ECS::Entity entity)
	{
		// Get component
		auto* ecs = Lina::ECS::Registry::Get();
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
				renderer.SetModel(entity, *selected);

			// Mesh drag & drop.
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMESH_ID))
				{
					IM_ASSERT(payload->DataSize == sizeof(StringIDType));

					auto& model = Lina::Graphics::Model::GetModel(*(StringIDType*)payload->Data);
					renderer.SetModel(entity, model);
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			WidgetsUtility::IncrementCursorPosY(5);
			// Remove Model
			if (WidgetsUtility::IconButton("##selectmesh", ICON_FA_MINUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
			{
				renderer.RemoveModel(entity);
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
					renderer.SetMaterial(entity, i, *selectedMaterial);

				ImGui::SameLine();
				WidgetsUtility::IncrementCursorPosY(5);

				// Material drag & drop.
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMATERIAL_ID))
					{
						IM_ASSERT(payload->DataSize == sizeof(StringIDType));

						auto& mat = Lina::Graphics::Material::GetMaterial(*(StringIDType*)payload->Data);
						renderer.SetMaterial(entity, i, mat);
					}
					ImGui::EndDragDropTarget();
				}

				// Remove Material
				std::string icnBtn = "##selectmat" + std::to_string(i);
				if (WidgetsUtility::IconButton(icnBtn.c_str(), ICON_FA_MINUS_SQUARE, 0.0f, .7f, ImVec4(1, 1, 1, 0.8f), ImVec4(1, 1, 1, 1), ImGui::GetStyleColorVec4(ImGuiCol_Header)))
				{
					renderer.RemoveMaterial(entity, i);
				}
			}

			ImGui::SetCursorPosX(cursorPosLabels);
			WidgetsUtility::IncrementCursorPosY(CURSORPOS_Y_INCREMENT_AFTER);
		}



		// Draw bevel line.
		WidgetsUtility::DrawBeveledLine();
	}

	void ComponentDrawer::DrawSpriteRendererComponent(Lina::ECS::Entity entity)
	{
		// Get component
		auto* ecs = Lina::ECS::Registry::Get();
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
					IM_ASSERT(payload->DataSize == sizeof(StringIDType));
					renderer.m_materialID = Lina::Graphics::Material::GetMaterial(*(StringIDType*)payload->Data).GetID();
					renderer.m_materialPaths = Lina::Graphics::Material::GetMaterial(*(uint32*)payload->Data).GetPath();

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
