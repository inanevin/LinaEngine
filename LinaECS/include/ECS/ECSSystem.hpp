/*
Author: Inan Evin - Thanks to the lectures & contributions of Benny Bobaganoosh, thebennybox.
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: ECSSystem
Timestamp: 4/8/2019 5:28:34 PM

*/

#pragma once

#ifndef ECSSystem_HPP
#define ECSSystem_HPP


#include "ECSComponent.hpp"


namespace LinaEngine::ECS
{
	class BaseECSSystem
	{

	public:

		enum
		{
			FLAG_OPTIONAL = 1,
		};

		BaseECSSystem() {};

		virtual void UpdateComponents(float delta, BaseECSComponent** components) { };
		FORCEINLINE const LinaArray<uint32> GetComponentTypes() { return componentTypes; }
		FORCEINLINE const LinaArray<uint32>& GetComponentFlags() { return componentFlags; }
		bool IsValid();

	protected:

		FORCEINLINE void AddComponentType(uint32 componentType, uint32 componentFlag = 0)
		{
			componentTypes.push_back(componentType);
			componentFlags.push_back(componentFlag);
		}

	private:

		LinaArray<uint32> componentTypes;
		LinaArray<uint32> componentFlags;
		
	};

	class ECSSystemList
	{
	public:

		/* Adds a system */
		FORCEINLINE bool AddSystem(BaseECSSystem& system)
		{
			if (!system.IsValid()) return false;
			systems.push_back(&system);
			return true;
		}

		FORCEINLINE size_t Size()
		{
			return systems.size();
		}

		FORCEINLINE BaseECSSystem* operator[](uint32 index)
		{
			return systems[index];
		}

		/* Remove a system */
		bool RemoveSystem(BaseECSSystem& system);


	private:

		/* Array of EntityComponentSystem systems */
		LinaArray<BaseECSSystem*> systems;
	};
}


#endif