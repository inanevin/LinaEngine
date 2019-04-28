/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: RenderableMeshSystem
Timestamp: 4/27/2019 5:38:44 PM

*/

#pragma once

#ifndef RenderableMeshSystem_HPP
#define RenderableMeshSystem_HPP

#include "ECS/EntityComponentSystem.hpp"
#include "ECS/ECSSystem.hpp"
#include "Rendering/GameRenderContext.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/RenderableMeshComponent.hpp"

using namespace LinaEngine::Graphics;

namespace LinaEngine::ECS
{
	class RenderableMeshSystem : public BaseECSSystem
	{

	public:


		RenderableMeshSystem(GameRenderContext& contextIn) : BaseECSSystem(), context(contextIn)
		{
			AddComponentType(TransformComponent::ID);
			AddComponentType(RenderableMeshComponent::ID);
		}

		virtual void UpdateComponents(float delta, BaseECSComponent** components);

	private:

		GameRenderContext& context;
	};
}


#endif