/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ECSSystem
Timestamp: 4/8/2019 11:27:15 PM

*/

#include "LinaPch.hpp"
#include "ECSSystem.hpp"  

namespace LinaEngine
{
	bool BaseECSSystem::IsValid()
	{
		for (uint32 i = 0; i < componentFlags.size(); i++)
		{
			if (componentFlags[i] & BaseECSSystem::FLAG_OPTIONAL == 0)
				return true;
		}

		return false;
	}

	bool ECSSystemList::RemoveSystem(BaseECSSystem& system)
	{
		// Iterate through systems.
		for (uint32 i = 0; i < systems.size(); i++)
		{
			// If the addr of the target system matches any system, erase it from the array.
			if (&system == systems[i])
			{
				systems.erase(systems.begin() + i);
				return true;
			}
		}

		return false;
	}
}

