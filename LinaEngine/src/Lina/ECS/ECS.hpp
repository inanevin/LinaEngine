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
Timestamp: 4/8/2019 5:43:51 PM

*/

#pragma once

#ifndef ECS_HPP
#define ECS_HPP

#include "ECSSystem.hpp"

namespace LinaEngine
{
	class ECS
	{
	public:
		
	private:

		/* Array of ECS systems */
		Array<BaseECSSystem*> systems;

		/* Map of id & for each id a seperate array for each comp type */
		Map<uint32, Array<uint8>> components;
		
		/* Entities with pair ids. */
		Array<Pair<uint32, Array<Pair<uint32, uint32>>>*> entities;

		FORCEINLINE Pair<uint32, Array<Pair<uint32, uint32> > >* handleToRawType(EntityHandle handle)
		{
			(Pair<uint32, Array<Pair<uint32, uint32> > >*)handle;
		}

		FORCEINLINE uint32 handleToEntityIndex(EntityHandle handle)
		{
			return handleToRawType(handle)->first;
		}

		FORCEINLINE Array<Pair<uint32, uint32> >& handleToEntity(EntityHandle handle)
		{
			return handleToRawType(handle)->second;
		}
	};
}


#endif