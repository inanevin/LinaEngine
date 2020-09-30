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

Class: GizmoLayer
Timestamp: 9/30/2020 3:27:46 PM

*/

#include "Core/GizmoLayer.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/RigidbodyComponent.hpp"
#include "Utility/UtilityFunctions.hpp"


namespace LinaEditor
{
	void GizmoLayer::OnAttach()
	{
		m_gizmoMaterial = &m_renderEngine->CreateMaterial(LinaEngine::Utility::GetUniqueID(), LinaEngine::Graphics::Shaders::STANDARD_UNLIT);
	}

	void GizmoLayer::OnDetach()
	{
		m_renderEngine->UnloadMaterialResource(m_gizmoMaterial->m_MaterialID);
	}

	void GizmoLayer::OnUpdate()
	{
		for (std::map<LinaEngine::ECS::ECSEntity, std::pair<LinaEngine::ECS::ECSEntity, LinaEngine::ECS::TransformComponent*>>::iterator it = m_gizmoMap.begin(); it != m_gizmoMap.end(); ++it)
		{
			if (!m_ecs->has<LinaEngine::ECS::TransformComponent>(it->second.first)) continue;
			if (!m_ecs->has<LinaEngine::ECS::RigidbodyComponent>(it->first)) continue;

			// Get transform
			LinaEngine::ECS::TransformComponent& entityTransform = m_ecs->get<LinaEngine::ECS::TransformComponent>(it->second.first);
			LinaEngine::ECS::TransformComponent& originalTransform = *it->second.second;

			entityTransform.transform.location = originalTransform.transform.location;
			entityTransform.transform.rotation = originalTransform.transform.rotation;
			entityTransform.transform.scale = 2.0f * m_ecs->get<LinaEngine::ECS::RigidbodyComponent>(it->first).m_halfExtents;
		}
	}

	void GizmoLayer::OnEvent()
	{

	}

	void GizmoLayer::RegisterGizmo(LinaEngine::ECS::ECSEntity handle, LinaEngine::Graphics::Primitives primitiveType, LinaEngine::ECS::TransformComponent& originalTransform)
	{
		if (!(m_gizmoMap.find(handle) == m_gizmoMap.end())) return;

		LinaEngine::ECS::ECSEntity entity;
		entity = m_ecs->create();

		// Create mesh renderer component 
		LinaEngine::ECS::MeshRendererComponent mr;
		mr.meshID = primitiveType;
		mr.materialID = m_gizmoMaterial->m_MaterialID;

		// Create transform component
		LinaEngine::ECS::TransformComponent tr;
		
		// Attach & assign to map.
		m_ecs->emplace<LinaEngine::ECS::TransformComponent>(entity, tr);
		m_ecs->emplace<LinaEngine::ECS::MeshRendererComponent>(entity, mr);
		m_gizmoMap[handle] = std::make_pair(entity, &originalTransform);
	}

	void GizmoLayer::UnregisterGizmo(LinaEngine::ECS::ECSEntity handle)
	{
		if ((m_gizmoMap.find(handle) == m_gizmoMap.end())) return;

		// Remove gizmo entity.
		m_ecs->remove(handle);
		m_gizmoMap.erase(handle);
	}
}