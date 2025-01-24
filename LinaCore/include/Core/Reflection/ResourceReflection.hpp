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

#include "Common/Reflection/ClassReflection.hpp"
#include "Core/Resources/ResourceCache.hpp"

namespace Lina
{
	class ReflectionResourceUtility
	{
	public:
		template <typename T> static inline void* REF_CreateResourceCacheFunc()
		{
			Lina::ResourceCache<T>* c = new Lina::ResourceCache<T>();
			return static_cast<void*>(c);
		}

		template <typename T> static inline void* REF_Allocate()
		{
			return new T(0, "");
		}

		template <typename T> static inline void REF_Deallocate(void* ptr)
		{
			T* obj = static_cast<T*>(ptr);
			delete obj;
		}
	};

#define LINA_RESOURCE_BEGIN(ClassName)                                                                                                                                                                                                                             \
	LINA_CLASS_BEGIN(ClassName)                                                                                                                                                                                                                                    \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddFunction<void*()>(Lina::TO_SIDC("CreateResourceCache"), std::bind(&Lina::ReflectionResourceUtility::REF_CreateResourceCacheFunc<ClassName>));                                                              \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddFunction<void*()>(Lina::TO_SIDC("Allocate"), std::bind(&Lina::ReflectionResourceUtility::REF_Allocate<ClassName>));                                                                                        \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddFunction<void(void* ptr)>(Lina::TO_SIDC("Deallocate"), std::bind(&Lina::ReflectionResourceUtility::REF_Deallocate<ClassName>, std::placeholders::_1));

} // namespace Lina
