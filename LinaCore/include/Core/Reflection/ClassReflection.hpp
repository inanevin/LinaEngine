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

#include "Common/StringID.hpp"
#include "Core/World/ComponentCache.hpp"
#include "Core/Reflection/ReflectionSystem.hpp"
#include "Core/GUI/Widgets/WidgetCache.hpp"
#include "Core/Resources/ResourceCache.hpp"

namespace Lina
{
	class SystemEventDispatcher;
	class ReflectionClassUtility
	{
	public:
		template <typename T> static inline void* REF_CreateComponentCacheFunc(EntityWorld* world, GameEventDispatcher* dispatcher)
		{
			Lina::ComponentCache<T>* c = new Lina::ComponentCache<T>(world, dispatcher);
			return static_cast<void*>(c);
		}

		template <typename T> static inline void* REF_CreateWidgetCacheFunc()
		{
			Lina::WidgetCache<T>* c = new Lina::WidgetCache<T>();
			return static_cast<void*>(c);
		}

		template <typename T> static inline void* REF_CreateResourceCacheFunc()
		{
			Lina::ResourceCache<T>* c = new Lina::ResourceCache<T>();
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

#define STRINGIFY(x) #x
#define TOSTRING(x)	 STRINGIFY(x)

#define LINA_REFLECTHELPER_ADDCLASSCOMPPROPERTIES(ClassName, TITLE, CATEGORY)                                                                                                                                                                                      \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddProperty<Lina::String>(Lina::TO_SIDC("Title"), TITLE);                                                                                                                                                      \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddProperty<Lina::String>(Lina::TO_SIDC("Category"), CATEGORY);

/* BEGINNING A REFLECTED COMPONENT */
#define LINA_REFLECTCOMPONENT_BEGIN(ClassName, TITLE, CATEGORY)                                                                                                                                                                                                    \
	class ClassName;                                                                                                                                                                                                                                               \
	class ClassName##_LinaReflected                                                                                                                                                                                                                                \
	{                                                                                                                                                                                                                                                              \
	public:                                                                                                                                                                                                                                                        \
		ClassName##_LinaReflected()                                                                                                                                                                                                                                \
		{                                                                                                                                                                                                                                                          \
			static bool reflected = false;                                                                                                                                                                                                                         \
			if (reflected)                                                                                                                                                                                                                                         \
				return;                                                                                                                                                                                                                                            \
			reflected = true;                                                                                                                                                                                                                                      \
			LINA_REFLECTHELPER_ADDCLASSCOMPPROPERTIES(ClassName, TITLE, CATEGORY)                                                                                                                                                                                  \
			Lina::ReflectionSystem::Get().Meta<ClassName>().AddFunction<void*(EntityWorld * w, GameEventDispatcher * dispatcher)>(                                                                                                                                 \
				Lina::TO_SIDC("CreateCompCache"), std::bind(&Lina::ReflectionClassUtility::REF_CreateComponentCacheFunc<ClassName>, std::placeholders::_1, std::placeholders::_2));

	/* BEGINNING A REFLECTED CLASS */
#define LINA_REFLECTCLASS_BEGIN(ClassName, TITLE)                                                                                                                                                                                                                  \
	class ClassName;                                                                                                                                                                                                                                               \
	class ClassName##_LinaReflected                                                                                                                                                                                                                                \
	{                                                                                                                                                                                                                                                              \
	public:                                                                                                                                                                                                                                                        \
		ClassName##_LinaReflected()                                                                                                                                                                                                                                \
		{                                                                                                                                                                                                                                                          \
			static bool reflected = false;                                                                                                                                                                                                                         \
			if (reflected)                                                                                                                                                                                                                                         \
				return;                                                                                                                                                                                                                                            \
			reflected = true;                                                                                                                                                                                                                                      \
			Lina::ReflectionSystem::Get().Meta<ClassName>().AddProperty<Lina::String>(Lina::TO_SIDC("Title"), TITLE);                                                                                                                                              \
			Lina::ReflectionSystem::Get().Meta<ClassName>().AddFunction<void*()>(Lina::TO_SIDC("Create"), std::bind(&Lina::ReflectionClassUtility::REF_CreateFunc<ClassName>));                                                                                    \
			Lina::ReflectionSystem::Get().Meta<ClassName>().AddFunction<void(void* ptr)>(Lina::TO_SIDC("Delete"), std::bind(&Lina::ReflectionClassUtility::REF_DestroyFunc<ClassName>, std::placeholders::_1));

#define LINA_REFLECTENUM_BEGIN(EnumName, TITLE)                                                                                                                                                                                                                    \
	enum class EnumName;                                                                                                                                                                                                                                           \
	class EnumName##_LinaReflected                                                                                                                                                                                                                                 \
	{                                                                                                                                                                                                                                                              \
	public:                                                                                                                                                                                                                                                        \
		EnumName##_LinaReflected()                                                                                                                                                                                                                                 \
		{                                                                                                                                                                                                                                                          \
			static bool reflected = false;                                                                                                                                                                                                                         \
			if (reflected)                                                                                                                                                                                                                                         \
				return;                                                                                                                                                                                                                                            \
			reflected = true;                                                                                                                                                                                                                                      \
			Lina::ReflectionSystem::Get().Meta<EnumName>().AddProperty<Lina::String>(Lina::TO_SIDC("Title"), TITLE);

/* BEGINNING A REFLECTED WIDGET */
#define LINA_REFLECTWIDGET_BEGIN(ClassName, Category)                                                                                                                                                                                                              \
	class ClassName;                                                                                                                                                                                                                                               \
	class ClassName##_LinaReflected                                                                                                                                                                                                                                \
	{                                                                                                                                                                                                                                                              \
	public:                                                                                                                                                                                                                                                        \
		ClassName##_LinaReflected()                                                                                                                                                                                                                                \
		{                                                                                                                                                                                                                                                          \
			static bool reflected = false;                                                                                                                                                                                                                         \
			if (reflected)                                                                                                                                                                                                                                         \
				return;                                                                                                                                                                                                                                            \
			reflected = true;                                                                                                                                                                                                                                      \
			Lina::ReflectionSystem::Get().Meta<ClassName>().AddFunction<void*()>(Lina::TO_SIDC("CreateWidgetCache"), std::bind(&Lina::ReflectionClassUtility::REF_CreateWidgetCacheFunc<ClassName>));                                                              \
			Lina::ReflectionSystem::Get().Meta<ClassName>().AddProperty<uint32>("WidgetIdent"_hs, 0);                                                                                                                                                              \
			Lina::ReflectionSystem::Get().Meta<ClassName>().AddProperty<Lina::String>("Title"_hs, TOSTRING(ClassName));                                                                                                                                            \
			Lina::ReflectionSystem::Get().Meta<ClassName>().AddProperty<Lina::String>("Category"_hs, TOSTRING(Category));

/* BEGINNING A REFLECTED RESOURCE */
#define LINA_REFLECTRESOURCE_BEGIN(ClassName)                                                                                                                                                                                                                      \
	class ClassName;                                                                                                                                                                                                                                               \
	class ClassName##_LinaReflected                                                                                                                                                                                                                                \
	{                                                                                                                                                                                                                                                              \
	public:                                                                                                                                                                                                                                                        \
		ClassName##_LinaReflected()                                                                                                                                                                                                                                \
		{                                                                                                                                                                                                                                                          \
			static bool reflected = false;                                                                                                                                                                                                                         \
			if (reflected)                                                                                                                                                                                                                                         \
				return;                                                                                                                                                                                                                                            \
			reflected = true;                                                                                                                                                                                                                                      \
			Lina::ReflectionSystem::Get().Meta<ClassName>().AddFunction<void*()>(Lina::TO_SIDC("CreateResourceCache"), std::bind(&Lina::ReflectionClassUtility::REF_CreateResourceCacheFunc<ClassName>));

/* PROPERTIES AND FUNCTIONS PER REFLECTED CLASS*/
#define LINA_FIELD(ClassName, FieldName, TITLE, TYPE, SUBTYPE)                                                                                                                                                                                                     \
	const Lina::StringID sid_##FieldName = Lina::TO_SIDC(TITLE);                                                                                                                                                                                                   \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddField<&ClassName::FieldName, ClassName>(sid_##FieldName);                                                                                                                                                   \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Title"), TITLE);                                                                                                                           \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::StringID>(Lina::TO_SIDC("Type"), Lina::TO_SIDC(TYPE));                                                                                                            \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::TypeID>(Lina::TO_SIDC("SubType"), SUBTYPE);

#define LINA_FIELD_TOOLTIP(ClassName, FieldName, TOOLTIP) Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Tooltip"), TOOLTIP);

#define LINA_FIELD_DEPENDSON(ClassName, FieldName, DEPENDSON) Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::StringID>(Lina::TO_SIDC("DependsOn"), Lina::TO_SIDC(DEPENDSON));

#define LINA_FIELD_LIMITS(ClassName, FieldName, MIN, MAX)                                                                                                                                                                                                          \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Min"), MIN);                                                                                                                               \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Max"), MAX);

/* ENDING A REFLECTED CLASS */
#define LINA_REFLECTCLASS_END(ClassName)                                                                                                                                                                                                                           \
	}                                                                                                                                                                                                                                                              \
	}                                                                                                                                                                                                                                                              \
	;                                                                                                                                                                                                                                                              \
	inline static ClassName##_LinaReflected ClassName##__ = ClassName##_LinaReflected();
#define LINA_REFLECTCOMPONENT_END(ClassName) LINA_REFLECTCLASS_END(ClassName)
#define LINA_REFLECTWIDGET_END(ClassName)	 LINA_REFLECTCLASS_END(ClassName)
#define LINA_REFLECTENUM_END(EnumName)		 LINA_REFLECTCLASS_END(EnumName)
#define LINA_REFLECTRESOURCE_END(ClassName)	 LINA_REFLECTCLASS_END(ClassName)
#define LINA_REFLECTION_ACCESS(ClassName)	 friend class ClassName##_LinaReflected;

	// static const ClassName##....

} // namespace Lina
