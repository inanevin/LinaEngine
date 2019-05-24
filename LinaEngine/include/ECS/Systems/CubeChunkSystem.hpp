/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: CubeChunkSystem
Timestamp: 4/30/2019 12:05:16 AM

*/

#pragma once

#ifndef CubeChunkSystem_HPP
#define CubeChunkSystem_HPP

#include "ECS/ECSSystem.hpp"
#include "ECS/Components/CubeChunkComponent.hpp"

namespace LinaEngine::ECS
{
	class CubeChunkSystem  : public BaseECSSystem
	{
	public:

		CubeChunkSystem() : BaseECSSystem()
		{
			AddComponentType(CubeChunkComponent::ID);
		}

		virtual void UpdateComponents(float delta, BaseECSComponent** components);

		FORCEINLINE void SetMovementIndex(int index)
		{
			m_MovementIndex = index;
		}
	private:

		int m_MovementIndex = 0;
	};
}


#endif