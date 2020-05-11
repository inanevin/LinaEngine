/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Internal
Timestamp: 4/17/2019 1:49:50 AM

*/

#pragma once

#ifndef Internal_HPP
#define Internal_HPP

#include <string>

namespace LinaEngine
{
	namespace Internal
	{

		// Type comparator traits.
		template <typename T> struct comparison_traits {
			static bool equal(const T& a, const T& b) {
				return a == b;
			}
		};

		// Generic stringifier.
		template<typename T>
		static inline std::string ToString(T val)
		{
			std::ostringstream convert;
			convert << val;
			return convert.str();
		}

		// Converts to underlying type.
		template <typename E>
		constexpr auto to_underlying(E e) noexcept
		{
			return static_cast<std::underlying_type_t<E>>(e);
		}
	}
}

#endif