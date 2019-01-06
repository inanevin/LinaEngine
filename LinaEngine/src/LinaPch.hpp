/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Lina_Pch
Timestamp: 12/30/2018 5:29:20 PM

*/

#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <cstdint>


#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "Lina/Utility/Log.hpp"
#include "Lina/Utility/Math/Math.hpp"
#include "Lina/Utility/Math/Quaternion.hpp"
#include "Lina/Utility/ResourceConstants.hpp"


namespace LinaEngine
{
	namespace Internal
	{
		template <typename T> struct comparison_traits {
			static bool equal(const T& a, const T& b) {
				return a == b;
			}
		};
	}
}

#ifdef LINA_PLATFORM_WINDOWS
#include <Windows.h>
#endif