/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: TestInteraction
Timestamp: 5/1/2019 4:41:54 PM

*/

#pragma once

#ifndef TestInteraction_HPP
#define TestInteraction_HPP

#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/ColliderComponent.hpp"
#include "ECS/Components/MotionComponent.hpp"
#include "ECS/ECSInteraction.hpp"

namespace LinaEngine::ECS
{
	class TestInteraction : public ECSInteraction
	{
	public:

		TestInteraction() : ECSInteraction()
		{
			AddInteractorComponentType(TransformComponent::ID);
			AddInteractorComponentType(ColliderComponent::ID);
			AddInteractorComponentType(MotionComponent::ID);
			AddInteracteeComponentType(TransformComponent::ID);
			AddInteracteeComponentType(ColliderComponent::ID);
			AddInteracteeComponentType(MotionComponent::ID);
		}

		virtual void Interact(float dleta, BaseECSComponent** components, BaseECSComponent** interacteeComponents) override;

	};
}


#endif