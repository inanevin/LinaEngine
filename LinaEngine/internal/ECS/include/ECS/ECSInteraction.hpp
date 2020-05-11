/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: ECSInteraction
Timestamp: 5/1/2019 2:40:19 AM

*/

#pragma once

#ifndef ECSInteraction_HPP
#define ECSInteraction_HPP

#include "EntityComponentSystem.hpp"

namespace LinaEngine::ECS
{
	class ECSInteraction
	{
	public:

		ECSInteraction() {};
		virtual ~ECSInteraction() {};

		virtual void Interact(float delta, BaseECSComponent** interactorComponents, BaseECSComponent** interacteeComponents) {};

		FORCEINLINE const LinaArray<uint32>& GetInteractorComponents() { return interactorComponentTypes; }
		FORCEINLINE const LinaArray<uint32>& GetInteracteeComponents() { return interacteeComponentTypes; }

	protected:

		FORCEINLINE void AddInteractorComponentType(uint32 type)
		{
			interactorComponentTypes.push_back(type);
		}

		FORCEINLINE void AddInteracteeComponentType(uint32 type)
		{
			interacteeComponentTypes.push_back(type);
		}

	private:

		LinaArray<uint32> interactorComponentTypes;
		LinaArray<uint32> interacteeComponentTypes;
	};
}


#endif