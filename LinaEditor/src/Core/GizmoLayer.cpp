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

	void GizmoLayer::OnTick(float deltaTime)
	{
		
	}

	void GizmoLayer::OnEvent()
	{

	}

	
}