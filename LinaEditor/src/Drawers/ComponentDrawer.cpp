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
#include "Memory/Memory.hpp"
#include "Core/EditorCommon.hpp"
#include "IconsFontAwesome5.h"
#include "IconsMaterialDesign.h"
#include "imgui/imguizmo/ImGuizmo.h"
#include "Core/CustomFontIcons.hpp"

using namespace Lina::ECS;
using namespace Lina::Editor;


namespace Lina::Editor
{


	std::map<Lina::ECS::TypeID, bool> m_componentFoldoutState;
	std::pair<Lina::ECS::Entity, Lina::ECS::TypeID> m_copyBuffer;

	template<typename Type>
	void Drawer_SetEnabled(Lina::ECS::Entity ent, bool enabled)
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
	Type& Drawer_Get(Lina::ECS::Entity entity)
	{
		return Lina::ECS::Registry::Get()->template get<Type>(entity);
	}

	template<typename Type>
	void Drawer_Add(Lina::ECS::Entity entity)
	{
		Lina::ECS::Registry::Get()->template emplace<Type>(entity);
	}

	template<typename Type>
	bool Drawer_Has(Lina::ECS::Entity entity)
	{
		return Lina::ECS::Registry::Get()->all_of<Type>(entity);
	}

	template<typename Type>
	void Drawer_Reset(Lina::ECS::Entity entity)
	{
		ECS::TypeID tid = GetTypeID<Type>();

		if (tid == GetTypeID<EntityDataComponent>())
		{
			EntityDataComponent& comp = Lina::ECS::Registry::Get()->get<EntityDataComponent>(entity);
			comp.SetLocalLocation(Vector3::Zero);
			comp.SetLocalRotation(Quaternion());
			comp.SetLocalScale(Vector3::One);
		}
		else
			Lina::ECS::Registry::Get()->template replace<Type>(entity, Type());
	}

	template<typename Type>
	void Drawer_Remove(Lina::ECS::Entity entity)
	{
		Lina::ECS::Registry::Get()->template remove<Type>(entity);
	}

	void Drawer_Copy(Lina::ECS::Entity entity, Lina::ECS::TypeID tid)
	{
		m_copyBuffer.first = entity;
		m_copyBuffer.second = tid;
	}

	template<typename Type>
	void Drawer_Paste(Lina::ECS::Entity entity)
	{
		Lina::ECS::TypeID pastedTid = Lina::ECS::GetTypeID<Type>();


		if (pastedTid == m_copyBuffer.second)
		{
			if (m_copyBuffer.first != entt::null)
			{
				if (pastedTid == GetTypeID<EntityDataComponent>())
				{
					EntityDataComponent& copy = Lina::ECS::Registry::Get()->get<EntityDataComponent>(m_copyBuffer.first);

					auto& data = Lina::ECS::Registry::Get()->get<EntityDataComponent>(entity);
					data.SetLocation(copy.GetLocation());
					data.SetRotation(copy.GetRotation());
					data.SetScale(copy.GetScale());
				}
				else
				{
					Type* copy = Lina::ECS::Registry::Get()->try_get<Type>(m_copyBuffer.first);

					if (copy != nullptr)
					{
						Lina::ECS::Registry::Get()->replace<Type>(entity, *copy);
						Drawer_SetEnabled<Type>(entity, copy->m_isEnabled);
					}
				}

			}
		}

		// If we pasted a model renderer, it's data will be copied but we still need to 
		// manually set the pasted model & materials on the renderer.
		if (pastedTid == GetTypeID<ModelRendererComponent>())
		{
			auto& mr = Lina::ECS::Registry::Get()->get<ModelRendererComponent>(entity);
			std::string modelPath = mr.GetModelPath();
			std::vector<std::string> materials = mr.GetMaterialPaths();
			if (Lina::Graphics::Model::ModelExists(modelPath))
			{
				mr.SetModel(entity, Lina::Graphics::Model::GetModel(modelPath));

				for (int i = 0; i < materials.size(); i++)
				{
					if (Lina::Graphics::Material::MaterialExists(materials[i]))
						mr.SetMaterial(entity, i, Lina::Graphics::Material::GetMaterial(materials[i]));
					else
						mr.SetMaterial(entity, i, Lina::Graphics::Material::GetMaterial("Resources/Engine/Materials/DefaultLit.mat"));
				}
			}
			else
				mr.RemoveModel(entity);


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

	void Drawer_DebugPLight(Lina::ECS::Entity ent) {
		LINA_TRACE("Debug PLIGHT");
	}

	ComponentDrawer::ComponentDrawer()
	{
		uint8 defaultDrawFlags = ComponentDrawFlags_None;
		// Camera component
		entt::meta<CameraComponent>().data<&CameraComponent::m_isEnabled>("enabled"_hs);
		entt::meta<CameraComponent>().data<&CameraComponent::m_zFar>("zf"_hs).props(PROPS("Far", ComponentVariableType::DragFloat, "Far Plane"));
		entt::meta<CameraComponent>().data<&CameraComponent::m_zNear>("zn"_hs).props(PROPS("Near", ComponentVariableType::DragFloat, "Near Plane"));
		entt::meta<CameraComponent>().data<&CameraComponent::m_fieldOfView>("fov"_hs).props(PROPS("Fov", ComponentVariableType::DragFloat, "Field of View Angles"));
		entt::meta<CameraComponent>().data<&CameraComponent::m_clearColor>("cc"_hs).props(PROPS("Clear Color", ComponentVariableType::Color, "Background Clear Color"));
		RegisterComponentForEditor<CameraComponent>("Camera", ICON_FA_CAMERA, defaultDrawFlags, "View");

		// Dirlight
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_isEnabled>("enabled"_hs);
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_drawDebug>("debug"_hs).props(PROPS("Enable Debug", ComponentVariableType::Checkmark, "Enables debug drawing of light's reach."));
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_shadowZFar>("szf"_hs).props(PROPS_DEP("Shadow Far", ComponentVariableType::DragFloat, "Far plane distance used in shadow rendering.", "castShadows"_hs, ));
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_shadowZNear>("szn"_hs).props(PROPS_DEP("Shadow Near", ComponentVariableType::DragFloat, "Near plane distance used in shadow rendering.", "castShadows"_hs));
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_shadowOrthoProjection>("so"_hs).props(PROPS_DEP("Shadow Projection", ComponentVariableType::Vector4,"Shadow projection matrix (ortho).", "castShadows"_hs));
		//entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_castsShadows>("castShadows"_hs).props(PROPS("Cast Shadows", ComponentVariableType::Checkmark));
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_intensity>("i"_hs).props(PROPS("Intensity", ComponentVariableType::DragFloat, ""));
		entt::meta<DirectionalLightComponent>().data<&DirectionalLightComponent::m_color>("cc"_hs).props(PROPS("Color", ComponentVariableType::Color, ""));
		entt::meta<DirectionalLightComponent>().func<&ComponentDrawer::DrawDebugDirectionalLight, entt::as_ref_t>("drawDebug"_hs);
		RegisterComponentForEditor<DirectionalLightComponent>("Directional Light", ICON_FA_SUN, defaultDrawFlags, "Lights");

		// Spotlight
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_isEnabled>("enabled"_hs);
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_drawDebug>("debug"_hs).props(PROPS("Enable Debug", ComponentVariableType::Checkmark, "Enables debug drawing of light's reach."));
		// entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_castsShadows>("castShadows"_hs).props(PROPS("Cast Shadows", ComponentVariableType::Checkmark));
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_outerCutoff>("oc"_hs).props(PROPS("Outer Cutoff", ComponentVariableType::DragFloat, "The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff."));
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_cutoff>("cof"_hs).props(PROPS("Cutoff", ComponentVariableType::DragFloat, "The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff."));
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_distance>("dist"_hs).props(PROPS("Distance", ComponentVariableType::DragFloat, ""));
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_intensity>("int"_hs).props(PROPS("Intensity", ComponentVariableType::DragFloat, ""));
		entt::meta<SpotLightComponent>().data<&SpotLightComponent::m_color>("c"_hs).props(PROPS("Color", ComponentVariableType::Color, ""));
		entt::meta<SpotLightComponent>().func<&ComponentDrawer::DrawDebugSpotLight, entt::as_ref_t>("drawDebug"_hs);
		RegisterComponentForEditor<SpotLightComponent>("Spot Light", ICON_CS_SPOTLIGHT, defaultDrawFlags, "Lights");

		// Pointlight
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_isEnabled>("enabled"_hs);
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_drawDebug>("debug"_hs).props(PROPS("Enable Debug", ComponentVariableType::Checkmark, "Enables debug drawing of light's reach."));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_shadowFar>("sf"_hs).props(PROPS_DEP("Shadow Far", ComponentVariableType::DragFloat, "Far plane distance used in shadow rendering.", "castShadows"_hs));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_shadowNear>("sn"_hs).props(PROPS_DEP("Shadow Near", ComponentVariableType::DragFloat, "Near plane distance used in shadow rendering.", "castShadows"_hs));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_bias>("b"_hs).props(PROPS_DEP("Bias", ComponentVariableType::DragFloat, "Controls the amount of darkening.", "castShadows"_hs));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_castsShadows>("castShadows"_hs).props(PROPS("Cast Shadows", ComponentVariableType::Checkmark, ""));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_distance>("ds"_hs).props(PROPS("Distance", ComponentVariableType::DragFloat, ""));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_intensity>("i"_hs).props(PROPS("Intensity", ComponentVariableType::DragFloat, ""));
		entt::meta<PointLightComponent>().data<&PointLightComponent::m_color>("c"_hs).props(PROPS("Color", ComponentVariableType::Color, ""));
		entt::meta<PointLightComponent>().func<&ComponentDrawer::DrawDebugPointLight, entt::as_ref_t>("drawDebug"_hs);
		RegisterComponentForEditor<PointLightComponent>("Point Light", ICON_FA_LIGHTBULB, defaultDrawFlags, "Lights");

		// Freelook
		entt::meta<FreeLookComponent>().data<&FreeLookComponent::m_isEnabled>("enabled"_hs);
		entt::meta<FreeLookComponent>().data<&FreeLookComponent::m_rotationSpeeds>("rs"_hs).props(PROPS("Rotation Speed", ComponentVariableType::Vector2, ""));
		entt::meta<FreeLookComponent>().data<&FreeLookComponent::m_movementSpeeds>("ms"_hs).props(PROPS("Movement Speed", ComponentVariableType::Vector2, ""));
		RegisterComponentForEditor<FreeLookComponent>("Freelook", ICON_FA_EYE, defaultDrawFlags, "View");

		// Model Renderer
		auto idt = "matpath_arr"_hs;
		entt::meta<ModelRendererComponent>().data<&ModelRendererComponent::m_isEnabled>("enabled"_hs);
		entt::meta<ModelRendererComponent>().data<&ModelRendererComponent::m_materialPaths>(idt).props(PROPS("Materials", ComponentVariableType::MaterialPathArray, ""));
		entt::meta<ModelRendererComponent>().data<&ModelRendererComponent::m_modelPath>("modpath"_hs).props(PROPS("Model", ComponentVariableType::ModelPath, ""));
		entt::meta<ModelRendererComponent>().func<&Drawer_SetModel<ModelRendererComponent>, entt::as_ref_t>("setModel"_hs);
		entt::meta<ModelRendererComponent>().func<&Drawer_GetMaterialName<ModelRendererComponent>, entt::as_ref_t>("getMaterialName"_hs);
		entt::meta<ModelRendererComponent>().func<&Drawer_RemoveModel<ModelRendererComponent>, entt::as_ref_t>("removeModel"_hs);
		entt::meta<ModelRendererComponent>().func<&Drawer_SetMaterial<ModelRendererComponent>, entt::as_ref_t>("setMaterial"_hs);
		entt::meta<ModelRendererComponent>().func<&Drawer_RemoveMaterial<ModelRendererComponent>, entt::as_ref_t>("removeMaterial"_hs);
		RegisterComponentForEditor<ModelRendererComponent>("Model Renderer", ICON_FA_CUBE, defaultDrawFlags, "Rendering");

		// Sprite renderer
		entt::meta<SpriteRendererComponent>().data<&SpriteRendererComponent::m_isEnabled>("enabled"_hs);
		entt::meta<SpriteRendererComponent>().data<&SpriteRendererComponent::m_materialPaths>("matpath"_hs).props(PROPS("Material", ComponentVariableType::MaterialPath, ""));
		RegisterComponentForEditor<SpriteRendererComponent>("Sprite", ICON_MD_TOYS, defaultDrawFlags, "Rendering");

	}

	void ComponentDrawer::Initialize()
	{
		Lina::Event::EventSystem::Get()->Connect<EComponentOrderSwapped, &ComponentDrawer::OnComponentOrderSwapped>(this);
		Lina::Event::EventSystem::Get()->Connect<ETransformPivotChanged, &ComponentDrawer::OnTransformPivotChanged>(this);
	}

	void ComponentDrawer::AddIDToDrawList(Lina::ECS::TypeID id)
	{
		// Add only if it doesn't exists.
		if (std::find(m_componentDrawList.begin(), m_componentDrawList.end(), id) == m_componentDrawList.end())
			m_componentDrawList.push_back(id);
	}

	void ComponentDrawer::SwapComponentOrder(Lina::ECS::TypeID id1, Lina::ECS::TypeID id2)
	{
		// Swap iterators.
		std::vector<TypeID>::iterator it1 = std::find(m_componentDrawList.begin(), m_componentDrawList.end(), id1);
		std::vector<TypeID>::iterator it2 = std::find(m_componentDrawList.begin(), m_componentDrawList.end(), id2);
		std::iter_swap(it1, it2);
	}

	void ComponentDrawer::OnComponentOrderSwapped(EComponentOrderSwapped ev)
	{
		SwapComponentOrder(ev.m_id1, ev.m_id2);
	}

	void ComponentDrawer::ClearDrawList()
	{
		m_componentDrawList.clear();
	}

	AddComponentMap ComponentDrawer::GetCurrentAddComponentMap(Lina::ECS::Entity entity)
	{
		AddComponentMap map;

		for (auto& category : m_addComponentMap)
		{
			for (auto& pair : category.second)
			{
				Lina::ECS::TypeID tid = pair.second;
				auto meta = entt::resolve(tid);
				bool hasComponent = meta.func("has"_hs).invoke({}, entity).cast<bool>();

				if (!hasComponent)
				{
					map[category.first].push_back(std::make_pair(pair.first, tid));
				}
			}
		}

		return map;
	}

	void ComponentDrawer::DrawDebugPointLight(Lina::ECS::Entity ent)
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

	void ComponentDrawer::DrawDebugSpotLight(Lina::ECS::Entity ent)
	{
		ECS::EntityDataComponent& data = Lina::ECS::Registry::Get()->get<ECS::EntityDataComponent>(ent);
		ECS::SpotLightComponent& sLight = Lina::ECS::Registry::Get()->get<ECS::SpotLightComponent>(ent);
		Vector3 end1 = data.GetLocation() + (sLight.m_distance * data.GetRotation().GetForward());
		Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end1, Lina::Color::Red, 1.4f);
	}

	void ComponentDrawer::DrawDebugDirectionalLight(Lina::ECS::Entity ent)
	{
		ECS::EntityDataComponent& data = Lina::ECS::Registry::Get()->get<ECS::EntityDataComponent>(ent);
		Vector3 dir = Vector3::Zero - data.GetLocation();
		Vector3 end1 = data.GetLocation() + dir;
		Lina::Graphics::RenderEngineBackend::Get()->DrawLine(data.GetLocation(), end1, Lina::Color::Red, 1.4f);
	}

	void ComponentDrawer::PushComponentToDraw(Lina::ECS::TypeID tid, Lina::ECS::Entity ent)
	{
		if (std::find(m_componentDrawList.begin(), m_componentDrawList.end(), tid) == m_componentDrawList.end())
			m_componentDrawList.push_back(tid);
	}

	void ComponentDrawer::DrawAllComponents(Lina::ECS::Entity ent)
	{
		for (auto tid : m_componentDrawList)
			DrawComponent(tid, ent);
	}

	void ComponentDrawer::OnTransformPivotChanged(ETransformPivotChanged ev)
	{
		m_isTransformPivotGlobal = ev.m_isGlobal;
	}

	void ComponentDrawer::DrawEntityData(Lina::ECS::Entity entity, bool* transformFoldoutOpen, bool* physicsFoldoutOpen)
	{
		auto* ecs = Lina::ECS::Registry::Get();
		auto* physicsEngine = Lina::Physics::PhysicsEngineBackend::Get();
		EntityDataComponent& data = ecs->get<EntityDataComponent>(entity);
		Lina::ECS::TypeID entityDataTid = GetTypeID<EntityDataComponent>();
		Lina::ECS::TypeID physicsTid = GetTypeID<PhysicsComponent>();
		PhysicsComponent& phy = ecs->get<PhysicsComponent>(entity);

		bool disabled = entity == ecs->GetEntity("Editor Camera");
		std::string caretLabel = "Transformation " + std::string((m_isTransformPivotGlobal ? "(Global)" : "(Local)"));
		if (phy.m_isSimulated)
			caretLabel += " (Physics Active)";

		bool copied = false;
		bool pasted = false;
		bool resetted = false;

		if (disabled)
			ImGui::BeginDisabled();

		if (WidgetsUtility::ComponentHeader(entityDataTid, transformFoldoutOpen, caretLabel.c_str(), nullptr, nullptr, nullptr, disabled ? nullptr : &copied, disabled ? nullptr : &pasted, disabled ? nullptr : &resetted, false))
		{
			WidgetsUtility::PropertyLabel("Location");

			if (phy.m_isSimulated && !phy.m_isKinematic)
				ImGui::BeginDisabled();
			
			Vector3 location = m_isTransformPivotGlobal ? data.GetLocation() : data.GetLocalLocation();
			ImGui::DragFloat3("##loc", &location.x);

			if (m_isTransformPivotGlobal)
				data.SetLocation(location);
			else
				data.SetLocalLocation(location);

			WidgetsUtility::PropertyLabel("Rotation");
			Vector3 rot = m_isTransformPivotGlobal ? data.GetRotationAngles() : data.GetLocalRotationAngles();
			ImGui::DragFloat3("##rot", &rot.x);
			if (m_isTransformPivotGlobal)
				data.SetRotationAngles(rot);
			else
				data.SetLocalRotationAngles(rot);

			WidgetsUtility::PropertyLabel("Scale");
			Vector3 scale = m_isTransformPivotGlobal ? data.GetScale() : data.GetLocalScale();
			ImGui::DragFloat3("##scale", &scale.x);

			if (m_isTransformPivotGlobal)
				data.SetScale(scale);
			else
				data.SetLocalScale(scale);

			if (phy.m_isSimulated && !phy.m_isKinematic)
				ImGui::EndDisabled();

		}

		if (copied)
			Drawer_Copy(entity, entityDataTid);

		if (pasted)
			Drawer_Paste<EntityDataComponent>(entity);

		if (resetted)
			Drawer_Reset<EntityDataComponent>(entity);

		copied = pasted = resetted = false;
		if (WidgetsUtility::ComponentHeader(entityDataTid, physicsFoldoutOpen, "Physics", nullptr, nullptr, nullptr, disabled ? nullptr : &copied, disabled ? nullptr : &pasted, disabled ? nullptr : &resetted, false))
		{

			WidgetsUtility::PropertyLabel("Simulated");
			const bool currentSimulation = phy.m_isSimulated;
			ImGui::Checkbox("##enabled", &phy.m_isSimulated);
			if (currentSimulation != phy.m_isSimulated)
			{
				physicsEngine->SetBodySimulation(entity, phy.m_isSimulated);
			}

			WidgetsUtility::PropertyLabel("Kinematic");
			if (ImGui::IsItemHovered())
				WidgetsUtility::Tooltip("Kinematic bodies are physically simulated and affect other bodies around them, but are not affected by other forces or collision.");

			const bool currentKinematic = phy.m_isKinematic;
			ImGui::Checkbox("##kinematic", &phy.m_isKinematic);
			if (currentKinematic != phy.m_isKinematic)
			{
				physicsEngine->SetBodyKinematic(entity, phy.m_isKinematic);
			}
	
			WidgetsUtility::PropertyLabel("Mass");
			const float currentMass = phy.m_mass;
			ImGui::DragFloat("##mass", &phy.m_mass);
			if (phy.m_mass != currentMass)
			{
				physicsEngine->SetBodyMass(entity, phy.m_mass);
			}

			WidgetsUtility::PropertyLabel("Material");
			const std::string currentMaterial = phy.m_physicsMaterialPath;
			WidgetsUtility::PhysicsMaterialComboBox("##phyMat", phy.m_physicsMaterialPath, nullptr);
			if (phy.m_physicsMaterialPath.compare(currentMaterial) != 0)
			{
				physicsEngine->SetBodyMaterial(entity, Physics::PhysicsMaterial::GetMaterial(phy.m_physicsMaterialPath));
			}

			WidgetsUtility::PropertyLabel("Shape");
			const Physics::CollisionShape currentShape = phy.m_collisionShape;
			phy.m_collisionShape = (Physics::CollisionShape)WidgetsUtility::CollisionShapeComboBox("##collision", (int)phy.m_collisionShape);
			if (phy.m_collisionShape != currentShape)
			{
				physicsEngine->SetBodyCollisionShape(entity, phy.m_collisionShape);
			}
			if (phy.m_collisionShape == Physics::CollisionShape::Box || phy.m_collisionShape == Physics::CollisionShape::Cylinder)
			{
				WidgetsUtility::PropertyLabel("Half Extents");
				const Vector3 currentExtents = phy.m_halfExtents;
				ImGui::DragFloat3("##halfextents", &phy.m_halfExtents.x);
				if (currentExtents != phy.m_halfExtents)
				{
					physicsEngine->SetBodyHalfExtents(entity, phy.m_halfExtents);
				}
			}
			else if (phy.m_collisionShape == Physics::CollisionShape::Sphere)
			{
				WidgetsUtility::PropertyLabel("Radius");
				const float currentRadius = phy.m_radius;
				ImGui::DragFloat("##radius", &phy.m_radius);
				if (currentRadius != phy.m_radius)
				{
					physicsEngine->SetBodyRadius(entity, phy.m_radius);
				}
			}
			else if (phy.m_collisionShape == Physics::CollisionShape::Capsule)
			{
				WidgetsUtility::PropertyLabel("Radius");
				const float currentRadius = phy.m_radius;
				ImGui::DragFloat("##radius", &phy.m_radius);
				if (currentRadius != phy.m_radius)
				{
					physicsEngine->SetBodyRadius(entity, phy.m_radius);
				}

				WidgetsUtility::PropertyLabel("Height");
				const float currentHeight = phy.m_capsuleHeight;
				ImGui::DragFloat("##height", &phy.m_capsuleHeight);
				if (currentHeight != phy.m_capsuleHeight)
				{
					physicsEngine->SetBodyHeight(entity, phy.m_capsuleHeight);
				}
			}

			ImGui::SetCursorPosX(CURSOR_X_LABELS);

		}

		if (copied)
			Drawer_Copy(entity, entityDataTid);

		if (pasted)
			Drawer_Paste<PhysicsComponent>(entity);

		if (resetted)
		{
			phy.Reset();
			physicsEngine->SetBodySimulation(entity, phy.m_isSimulated);
		}

		if (disabled)
			ImGui::EndDisabled();
	}

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
			bool enabledPrevious = enabled;
			WidgetsUtility::ComponentHeader(tid, &m_componentFoldoutState[tid], title, icon, drawToggle ? &enabled : nullptr, drawRemove ? &remove : nullptr, drawCopy ? &copy : nullptr, drawPaste ? &paste : nullptr, drawReset ? &reset : nullptr);

			if (enabled != enabledPrevious)
				resolvedData.func("setEnabled"_hs).invoke({}, ent, enabled);

			if (remove)
			{
				resolvedData.func("remove"_hs).invoke({}, ent);

				for (std::vector<Lina::ECS::TypeID>::iterator it = m_componentDrawList.begin(); it != m_componentDrawList.end(); it++)
				{
					if (*it == tid)
					{
						m_componentDrawList.erase(it);
						break;
					}
				}
				return;
			}

			if (reset)
				resolvedData.func("reset"_hs).invoke({}, ent);

			if (copy)
				resolvedData.func("copy"_hs).invoke({}, ent, tid);

			if (paste)
				resolvedData.func("paste"_hs).invoke({}, ent);



			if (m_componentFoldoutState[tid])
			{

				// Draw each reflected property in the component according to it's type.

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
					WidgetsUtility::PropertyLabel(label);

					auto tooltipPropery = data.prop("Tooltip"_hs);
					if (!tooltipPropery)
					{
						std::string tooltip = tooltipPropery.value().cast<std::string>();
						if (ImGui::IsItemHovered() && tooltip.compare("") != 0)
							WidgetsUtility::Tooltip(tooltip.c_str());
					}

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

						std::vector<std::string> materials = data.get(instance).cast<std::vector<std::string>>();

						for (int i = 0; i < materials.size(); i++)
						{
							// Material selection.
							char matPathC[128] = "";
							strcpy(matPathC, materials[i].c_str());

							// Draw material name
							std::string materialName = resolvedData.func("getMaterialName"_hs).invoke({}, ent, i).cast<std::string>();
							WidgetsUtility::PropertyLabel(materialName.c_str());


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
							debugFunc.invoke({}, entt::forward_as_meta(*this), ent);
					}
				}
			}



		}

	}

	/*

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
	*/

}
