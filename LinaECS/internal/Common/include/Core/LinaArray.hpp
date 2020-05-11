/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: LinaArray
Timestamp: 12/29/2018 10:43:46 PM

*/

#pragma once
#ifndef LinaArray_HPP
#define LinaArray_HPP

#include <vector>

namespace LinaEngine
{
	// Custom array w/ extension functions defined.
	template<typename T>
	class LinaArray : public std::vector<T>
	{
	public:
		LinaArray<T>() : std::vector<T>() {}
		LinaArray<T>(size_t n) : std::vector<T>(n) {}

		// Swap the element to the end of this vector & pop it out.
		inline void swap_remove(size_t index)
		{
			std::swap((*this)[index], (*this)[this->size() - 1]);
			this->pop_back();
		}
	};
}

#endif