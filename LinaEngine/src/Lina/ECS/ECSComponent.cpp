/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ECSComponent
Timestamp: 4/7/2019 3:24:08 PM

*/

#include "LinaPch.hpp"
#include "ECSComponent.hpp"  

namespace LinaEngine
{

	uint32 BaseECSComponent::registerComponentType(ECSComponentCreateFunction createfn, ECSComponentFreeFunction freefn, size_t size)
	{
		uint32 componentID = componentTypes.size();
		componentTypes.push_back(std::tuple<ECSComponentCreateFunction, ECSComponentFreeFunction, size_t>(createfn, freefn, size));
		return componentID;
	}
}

