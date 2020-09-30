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
Timestamp: 9/30/2020 3:27:33 PM

*/
#pragma once

#ifndef GizmoLayer_HPP
#define GizmoLayer_HPP

// Headers here.
#include "Core/Layer.hpp"
#include "Rendering/RenderEngine.hpp"
#include "ECS/ECSSystem.hpp"
#include "btBulletDynamicsCommon.h"

namespace LinaEngine
{
	class Application;

	namespace World
	{
		class Level;
	}

	namespace ECS
	{
		struct TransformComponent;
	}

}

namespace LinaEditor
{

	class GizmoLayer : public LinaEngine::Layer
	{
		
	public:
		
		GizmoLayer() : LinaEngine::Layer("GizmoLayer")
		{

		}

		FORCEINLINE void Setup(LinaEngine::Graphics::RenderEngine& engine, LinaEngine::Application* application, LinaEngine::ECS::ECSRegistry& ecs) { m_renderEngine = &engine; m_application = application; m_ecs = &ecs; }

		// OVERRIDES
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnEvent() override;

	private:

		LinaEngine::Graphics::Material* m_gizmoMaterial = nullptr;
		LinaEngine::Graphics::RenderEngine* m_renderEngine = nullptr;
		LinaEngine::Application* m_application = nullptr;
		LinaEngine::ECS::ECSRegistry* m_ecs = nullptr;
	};
}

#endif
