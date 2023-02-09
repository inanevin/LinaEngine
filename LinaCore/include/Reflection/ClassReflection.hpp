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

#ifndef ComponentReflection_HPP
#define ComponentReflection_HPP

#include "Core/StringID.hpp"
#include "World/Core/ComponentCache.hpp"
#include "Reflection/ReflectionSystem.hpp"

namespace Lina
{
	class IEventDispatcher;
	class ReflectionClassUtility
	{
	public:
		template <typename T> static inline void* REF_CreateComponentCacheFunc(EntityWorld* world, IEventDispatcher* dispatcher)
		{
			Lina::ComponentCache<T>* c = new Lina::ComponentCache<T>(world, dispatcher);
			return static_cast<void*>(c);
		}

		template <typename T> static inline void* REF_CreateFunc()
		{
			T* c = new T();
			return static_cast<void*>(c);
		}

		template <typename T> static inline void REF_DestroyFunc(void* ptr)
		{
			T* obj = static_cast<T*>(ptr);
			delete obj;
		}
	};

#define LINA_REFLECTHELPER_ADDCLASSCOMPPROPERTIES(ClassName, TITLE, CATEGORY)                                                                                                                                                                                      \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddProperty<Lina::String>(Lina::TO_SIDC("Title"), TITLE);                                                                                                                                                      \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddProperty<Lina::String>(Lina::TO_SIDC("Category"), CATEGORY);

/* BEGINNING A REFLECTED CLASS */
#define LINA_REFLECTCOMPONENT_BEGIN(ClassName, TITLE, CATEGORY)                                                                                                                                                                                                    \
	class ClassName;                                                                                                                                                                                                                                               \
	class ClassName##_LinaReflected                                                                                                                                                                                                                                \
	{                                                                                                                                                                                                                                                              \
	public:                                                                                                                                                                                                                                                        \
		ClassName##_LinaReflected()                                                                                                                                                                                                                                \
		{                                                                                                                                                                                                                                                          \
			LINA_REFLECTHELPER_ADDCLASSCOMPPROPERTIES(ClassName, TITLE, CATEGORY)                                                                                                                                                                                  \
			Lina::ReflectionSystem::Get().Meta<ClassName>().AddFunction<void*(EntityWorld * w, IEventDispatcher * dispatcher)>(Lina::TO_SIDC("CreateCompCache"),                                                                                                   \
																															   std::bind(&Lina::ReflectionClassUtility::REF_CreateComponentCacheFunc<ClassName>, std::placeholders::_1, std::placeholders::_2));

	/* BEGINNING A REFLECTED CLASS */
#define LINA_REFLECTCLASS_BEGIN(ClassName, TITLE, CATEGORY)                                                                                                                                                                                                        \
	class ClassName;                                                                                                                                                                                                                                               \
	class ClassName##_LinaReflected                                                                                                                                                                                                                                \
	{                                                                                                                                                                                                                                                              \
	public:                                                                                                                                                                                                                                                        \
		ClassName##_LinaReflected()                                                                                                                                                                                                                                \
		{                                                                                                                                                                                                                                                          \
			LINA_REFLECTHELPER_ADDCLASSCOMPPROPERTIES(ClassName, TITLE, CATEGORY)                                                                                                                                                                                  \
			Lina::ReflectionSystem::Get().Meta<ClassName>().AddFunction<void*()>(Lina::TO_SIDC("Create"), std::bind(&Lina::ReflectionClassUtility::REF_CreateFunc<ClassName>));                                                                                    \
			Lina::ReflectionSystem::Get().Meta<ClassName>().AddFunction<void(void* ptr)>(Lina::TO_SIDC("Delete"), std::bind(&Lina::ReflectionClassUtility::REF_DestroyFunc<ClassName>, std::placeholders::_1));

/* PROPERTIES AND FUNCTIONS PER REFLECTED CLASS*/
#define LINA_REFLECT_FIELD(ClassName, FieldName, TITLE, TYPE, CATEGORY, TOOLTIP, DEPENDSON)                                                                                                                                                                        \
	const Lina::StringID sid_##FieldName = Lina::TO_SIDC(TITLE);                                                                                                                                                                                                   \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddField<&ClassName::FieldName, ClassName>(sid_##FieldName);                                                                                                                                                   \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Title"), TITLE);                                                                                                                           \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Type"), TYPE);                                                                                                                             \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Category"), CATEGORY);                                                                                                                     \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Tooltip"), TOOLTIP);                                                                                                                       \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("DependsOn"), DEPENDSON);

/* ENDING A REFLECTED CLASS */
#define LINA_REFLECTCLASS_END(ClassName)                                                                                                                                                                                                                           \
	}                                                                                                                                                                                                                                                              \
	}                                                                                                                                                                                                                                                              \
	;                                                                                                                                                                                                                                                              \
	inline static ClassName##_LinaReflected ClassName##__ = ClassName##_LinaReflected();
#define LINA_REFLECTCOMPONENT_END(ClassName) LINA_REFLECTCLASS_END(ClassName)
#define LINA_REFLECTION_ACCESS(ClassName)	 friend class ClassName##_LinaReflected;

	// static const ClassName##....

} // namespace Lina

#endif
