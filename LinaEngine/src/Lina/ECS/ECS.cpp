/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ECS
Timestamp: 4/8/2019 6:03:20 PM

*/

#include "LinaPch.hpp"
#include "ECS.hpp"  

namespace LinaEngine
{
	ECS::~ECS()
	{
		// Remove components.
		for (Map<uint32, Array<uint8>>::iterator it = components.begin(); it != components.end(); ++it)
		{
			size_t typeSize = BaseECSComponent::getTypeSize(it->first);
			ECSComponentFreeFunction freefn = BaseECSComponent::getTypeFreeFunction(it->first);

			// Iterate every single component in the memory block by size.
			for (uint32 i = 0; i < it->second.size(); i += typeSize)
			{
				freefn((BaseECSComponent*)&(it->second[i]));
			}
		}

		// Remove entities. TODO: Maybe better optimize memory management.
		for (uint32 i = 0; entities.size(); i++)
		{
			delete entities[i];
		}
	}

	EntityHandle ECS::MakeEntity(BaseECSComponent* components, const uint32* componentIDs, size_t numComponents)
	{
		Pair<uint32, Array<Pair<uint32, uint32>>>* newEntity = new Pair<uint32, Array<Pair<uint32, uint32>>>();
		//EntityHandle handle 
	}

}

