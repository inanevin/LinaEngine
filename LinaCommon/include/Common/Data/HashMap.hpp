/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Common/Data/Mutex.hpp"
#include "MallocAllocator.hpp"
#include "parallel_hashmap/phmap.h"

namespace Lina
{

	template <typename T, typename U> using HashMap					   = phmap::flat_hash_map<T, U>;
	template <typename T, typename U> using ParallelHashMap			   = phmap::parallel_flat_hash_map<T, U>;
	template <typename T, typename U> using ParallelHashMapMutex	   = phmap::parallel_flat_hash_map<T, U, phmap::priv::hash_default_hash<T>, phmap::priv::hash_default_eq<T>, phmap::priv::Allocator<T>, 4, Mutex>;
	template <typename T, typename U> using ParallelHashMapMutexMalloc = phmap::parallel_flat_hash_map<T, U, phmap::priv::hash_default_hash<T>, phmap::priv::hash_default_eq<T>, MallocAllocator<T>, 4, Mutex>;

	namespace Internal
	{
		template <typename T, typename U> T KeyFromValue(const HashMap<T, U>& map, U findValue)
		{
			for (const auto& [key, value] : map)
			{
				if (value == findValue)
					return key;
			}

			return T();
		}
	} // namespace Internal

} // namespace Lina
