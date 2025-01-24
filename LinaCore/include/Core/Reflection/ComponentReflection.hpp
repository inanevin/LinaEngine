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
#include "Core/World/ComponentCache.hpp"

namespace Lina
{
	class ReflectionComponentUtility
	{
	public:
		template <typename T> static inline void* REF_CreateComponentCacheFunc(EntityWorld* world)
		{
			Lina::ComponentCache<T>* c = new Lina::ComponentCache<T>();
			return static_cast<void*>(c);
		}

		template <typename T> static inline void REF_SaveToStream(OStream& stream, void* obj)
		{
			// MetaType&							 meta	= ReflectionSystem::Get().Resolve<T>();
			// const HashMap<StringID, FieldBase*>& fields = meta.GetFields();
			//
			// const uint32 size = static_cast<uint32>(fields.size());
			// stream << size;
			//
			// for (auto [sid, field] : fields)
			// {
			// 	stream << sid;
			// 	const uint32 typeSize = static_cast<uint32>(field->GetTypeSize());
			// 	stream << typeSize;
			// 	field->SaveToStream(stream, obj);
			// }
		}

		template <typename T> static inline void REF_LoadFromStream(IStream& stream, void* obj)
		{
			// MetaType&							 meta	= ReflectionSystem::Get().Resolve<T>();
			// const HashMap<StringID, FieldBase*>& fields = meta.GetFields();
			//
			// uint32 size = 0;
			// stream >> size;
			//
			// for (uint32 i = 0; i < size; i++)
			// {
			// 	StringID sid	  = 0;
			// 	uint32	 typeSize = 0;
			// 	stream >> sid;
			// 	stream >> typeSize;
			//
			// 	if (meta.HasField(sid))
			// 	{
			// 		meta.GetField(sid)->LoadFromStream(stream, obj);
			// 	}
			// 	else
			// 		stream.SkipBy(typeSize);
			// }
		}
	};

#define LINA_COMPONENT_BEGIN(ClassName, Category)                                                                                                                                                                                                                  \
	LINA_CLASS_BEGIN(ClassName)                                                                                                                                                                                                                                    \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddFunction<void*(EntityWorld*)>("CreateComponentCache"_hs, std::bind(&Lina::ReflectionComponentUtility::REF_CreateComponentCacheFunc<ClassName>, std::placeholders::_1));                                    \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddFunction<void(OStream&, void*)>("SaveToStream"_hs, std::bind(&Lina::ReflectionComponentUtility::REF_SaveToStream<ClassName>, std::placeholders::_1, std::placeholders::_2));                               \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddFunction<void(IStream&, void*)>("LoadFromStream"_hs, std::bind(&Lina::ReflectionComponentUtility::REF_LoadFromStream<ClassName>, std::placeholders::_1, std::placeholders::_2));                           \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddProperty<Lina::String>("Title"_hs, TOSTRING(ClassName));                                                                                                                                                   \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddProperty<Lina::String>("Category"_hs, TOSTRING(Category));

} // namespace Lina
