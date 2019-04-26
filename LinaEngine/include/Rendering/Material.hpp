/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Material
Timestamp: 4/26/2019 1:12:18 AM

*/

#pragma once

#ifndef Material_HPP
#define Material_HPP

#include "Core/DataStructures.hpp"
#include "Utility/Math/Matrix.hpp"

namespace LinaEngine::Graphics
{
	struct Material
	{
		LinaMap<LinaString, LinaString> textureNames;
		LinaMap<LinaString, float> floats;
		LinaMap<LinaString, Vector3F> vectors;
		LinaMap<LinaString, Matrix4F> matrices;
	};
}


#endif