/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: InteractionWorld
Timestamp: 4/30/2019 7:43:36 PM

*/

#pragma once

#ifndef InteractionWorld_HPP
#define InteractionWorld_HPP

#include "ECS/EntityComponentSystem.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/ColliderComponent.hpp"

using namespace LinaEngine::ECS;

namespace LinaEngine
{
	class InteractionWorld : public ECSListener
	{
	public:

		InteractionWorld() : ECSListener()
		{
			AddComponentID(TransformComponent::ID);
			AddComponentID(ColliderComponent::ID);
		}

		virtual void OnMakeEntity(EntityHandle handle) override;
		virtual void OnRemoveEntity(EntityHandle handle) override;
		virtual void OnAddComponent(EntityHandle handle) override;
		virtual void OnRemoveComponent(EntityHandle handle) override;

	private:


	};
}


#endif