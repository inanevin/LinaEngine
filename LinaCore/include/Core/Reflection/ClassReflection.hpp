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
#include "Common/Common.hpp"
#include "Common/Data/Streams.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
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

		template <typename T> static inline void REF_SaveToStream(OStream& stream, void* obj)
		{
			MetaType&							 meta	= ReflectionSystem::Get().Meta<T>();
			const HashMap<StringID, FieldBase*>& fields = meta.GetFields();
			for (auto [sid, field] : fields)
			{
				field->SaveToStream(stream, obj);
			}
		}

		template <typename T> static inline void REF_LoadFromStream(IStream& stream, void* obj)
		{
			// MetaType& meta = ReflectionSystem::Get().Meta<T>();
			// const HashMap<StringID, FieldBase*>& fields = meta.GetFields();
			// for(auto [sid, field] : fields)
			//{
			//
			// }
		}

		template <typename T> static inline uint32 REF_GetVectorSize(void* ptr)
		{
			Vector<T>* vec = static_cast<Vector<T>*>(ptr);
			return static_cast<uint32>(vec->size());
		}

		template <typename T> static inline void REF_ClearVector(void* ptr)
		{
			Vector<T>* vec = static_cast<Vector<T>*>(ptr);
			vec->clear();
		}

		template <typename T> static inline void* REF_GetVectorElementAddr(void* ptr, int32 index)
		{
			Vector<T>* vec = static_cast<Vector<T>*>(ptr);
			return &(*vec)[index];
		}

		template <typename T> static inline void REF_AddNewVectorElement(void* ptr)
		{
			Vector<T>* vec = static_cast<Vector<T>*>(ptr);
			vec->push_back(T());
		}

		template <typename T> static inline void REF_RemoveVectorElement(void* ptr, int32 index)
		{
			Vector<T>* vec = static_cast<Vector<T>*>(ptr);
			vec->erase(vec->begin() + index);
		}

		template <typename T> static inline void REF_DuplicateVectorElement(void* ptr, int32 index)
		{
			Vector<T>* vec = static_cast<Vector<T>*>(ptr);
			vec->insert(vec->begin() + index + 1, vec->at(index));
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

#define LINA_CLASS_BEGIN(ClassName)                                                                                                                                                                                                                                \
	class ClassName##_Reflected                                                                                                                                                                                                                                    \
	{                                                                                                                                                                                                                                                              \
	public:                                                                                                                                                                                                                                                        \
		ClassName##_Reflected()                                                                                                                                                                                                                                    \
		{                                                                                                                                                                                                                                                          \
			static bool reflected = false;                                                                                                                                                                                                                         \
			if (reflected)                                                                                                                                                                                                                                         \
				return;                                                                                                                                                                                                                                            \
			reflected = true;

#define LINA_CLASS_END(ClassName)                                                                                                                                                                                                                                  \
	}                                                                                                                                                                                                                                                              \
	}                                                                                                                                                                                                                                                              \
	;                                                                                                                                                                                                                                                              \
	inline static ClassName##_Reflected ClassName##__ = ClassName##_Reflected();

#define LINA_SERIALIZE_FUNCTIONS(ClassName)                                                                                                                                                                                                                        \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddFunction<void(OStream&, void*)>("SaveToStream"_hs, std::bind(&Lina::ReflectionClassUtility::REF_SaveToStream<ClassName>, std::placeholders::_1, std::placeholders::_2));                                    \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddFunction<void(OStream&, void*)>("LoadFromStream"_hs, std::bind(&Lina::ReflectionClassUtility::REF_SaveToStream<ClassName>, std::placeholders::_1, std::placeholders::_2));

#define LINA_WIDGET_BEGIN(ClassName, Category)                                                                                                                                                                                                                     \
	LINA_CLASS_BEGIN(ClassName)                                                                                                                                                                                                                                    \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddFunction<void*()>("CreateWidgetCache"_hs, std::bind(&Lina::ReflectionClassUtility::REF_CreateWidgetCacheFunc<ClassName>));                                                                                  \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddProperty<uint32>("WidgetIdent"_hs, 0);                                                                                                                                                                      \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddProperty<Lina::String>("Title"_hs, TOSTRING(ClassName));                                                                                                                                                    \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddProperty<Lina::String>("Category"_hs, TOSTRING(Category));

	/*
	 LINA_REFLECTHELPER_ADDCLASSCOMPPROPERTIES(ClassName, TITLE, CATEGORY)                                                                                                                                                                                  \
	 Lina::ReflectionSystem::Get().Meta<ClassName>().AddFunction<void*(EntityWorld * w, GameEventDispatcher * dispatcher)>(                                                                                                                                 \
	 Lina::TO_SIDC("CreateCompCache"), std::bind(&Lina::ReflectionClassUtility::REF_CreateComponentCacheFunc<ClassName>, std::placeholders::_1, std::placeholders::_2));
	 */

#define LINA_RESOURCE_BEGIN(ClassName)                                                                                                                                                                                                                             \
	LINA_CLASS_BEGIN(ClassName)                                                                                                                                                                                                                                    \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddFunction<void*()>(Lina::TO_SIDC("CreateResourceCache"), std::bind(&Lina::ReflectionClassUtility::REF_CreateResourceCacheFunc<ClassName>));

/* PROPERTIES AND FUNCTIONS PER REFLECTED CLASS*/
#define LINA_FIELD(ClassName, FieldName, TITLE, TYPE, SUBTYPE)                                                                                                                                                                                                     \
	const Lina::StringID sid_##FieldName = Lina::TO_SIDC(TOSTRING(FieldName));                                                                                                                                                                                     \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddField<&ClassName::FieldName, ClassName>(sid_##FieldName);                                                                                                                                                   \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Title"), TITLE);                                                                                                                           \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::StringID>(Lina::TO_SIDC("Type"), Lina::TO_SIDC(TYPE));                                                                                                            \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::TypeID>(Lina::TO_SIDC("SubType"), SUBTYPE);

#define LINA_FIELD_VEC(ClassName, FieldName, TITLE, SUBTYPE)                                                                                                                                                                                                       \
	const Lina::StringID sid_##FieldName = Lina::TO_SIDC(TOSTRING(FieldName));                                                                                                                                                                                     \
	Lina::ReflectionSystem::Get().Meta<ClassName>().AddField<&ClassName::FieldName, ClassName>(sid_##FieldName);                                                                                                                                                   \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Title"), TITLE);                                                                                                                           \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::StringID>(Lina::TO_SIDC("Type"), "Vector"_hs);                                                                                                                    \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::TypeID>(Lina::TO_SIDC("SubType"), GetTypeID<SUBTYPE>());                                                                                                          \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::StringID>(Lina::TO_SIDC("SubTypeSid"), TO_SIDC(TOSTRING(SUBTYPE)));                                                                                               \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddFunction<uint32(void*)>("GetVectorSize"_hs, std::bind(&Lina::ReflectionClassUtility::REF_GetVectorSize<SUBTYPE>, std::placeholders::_1));                                        \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddFunction<void(void*)>("AddNewElement"_hs, std::bind(&Lina::ReflectionClassUtility::REF_AddNewVectorElement<SUBTYPE>, std::placeholders::_1));                                    \
	Lina::ReflectionSystem::Get()                                                                                                                                                                                                                                  \
		.Meta<ClassName>()                                                                                                                                                                                                                                         \
		.GetField(sid_##FieldName)                                                                                                                                                                                                                                 \
		->AddFunction<void(void*, int32)>("DuplicateElement"_hs, std::bind(&Lina::ReflectionClassUtility::REF_DuplicateVectorElement<SUBTYPE>, std::placeholders::_1, std::placeholders::_2));                                                                     \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddFunction<void(void*, int32)>("RemoveElement"_hs, std::bind(&Lina::ReflectionClassUtility::REF_RemoveVectorElement<SUBTYPE>, std::placeholders::_1, std::placeholders::_2));      \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddFunction<void(void*)>("ClearVector"_hs, std::bind(&Lina::ReflectionClassUtility::REF_ClearVector<SUBTYPE>, std::placeholders::_1));                                              \
	Lina::ReflectionSystem::Get()                                                                                                                                                                                                                                  \
		.Meta<ClassName>()                                                                                                                                                                                                                                         \
		.GetField(sid_##FieldName)                                                                                                                                                                                                                                 \
		->AddFunction<void*(void*, int32 index)>("GetElementAddr"_hs, std::bind(&Lina::ReflectionClassUtility::REF_GetVectorElementAddr<SUBTYPE>, std::placeholders::_1, std::placeholders::_2));

#define LINA_FIELD_TOOLTIP(ClassName, FieldName, TOOLTIP) Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Tooltip"), TOOLTIP);

#define LINA_FIELD_DEPENDSON(ClassName, FieldName, DEPENDSON) Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::StringID>(Lina::TO_SIDC("DependsOn"), TO_SIDC(DEPENDSON));

#define LINA_FIELD_DEPENDENCY_POS(ClassName, FieldName, Dependency, Value) Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddPositiveDependency(TO_SIDC(Dependency), Value);
#define LINA_FIELD_DEPENDENCY_NEG(ClassName, FieldName, Dependency, Value) Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddNegativeDependency(TO_SIDC(Dependency), Value);

#define LINA_FIELD_LIMITS(ClassName, FieldName, MIN, MAX, STEP)                                                                                                                                                                                                    \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Min"), MIN);                                                                                                                               \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Max"), MAX);                                                                                                                               \
	Lina::ReflectionSystem::Get().Meta<ClassName>().GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Step"), STEP);

#define LINA_PROPERTY_STRING(ClassName, PropertySID, VALUE) Lina::ReflectionSystem::Get().Meta<ClassName>().AddProperty<Lina::String>(PropertySID, VALUE);

#define LINA_REFLECTION_ACCESS(ClassName) friend class ClassName##_Reflected;

	// static const ClassName##....

	LINA_CLASS_BEGIN(Direction)
	LINA_PROPERTY_STRING(Direction, 0, "Top")
	LINA_PROPERTY_STRING(Direction, 1, "Bottom")
	LINA_PROPERTY_STRING(Direction, 2, "Left")
	LINA_PROPERTY_STRING(Direction, 3, "Right")
	LINA_PROPERTY_STRING(Direction, 4, "Center")
	LINA_CLASS_END(Direction)

	LINA_CLASS_BEGIN(DirectionOrientation)
	LINA_PROPERTY_STRING(DirectionOrientation, 0, "Horizontal")
	LINA_PROPERTY_STRING(DirectionOrientation, 1, "Vertical")
	LINA_CLASS_END(DirectionOrientation)

	LINA_CLASS_BEGIN(LinaVGAlignment)
	LINA_PROPERTY_STRING(LinaVG::TextAlignment, 0, "Left")
	LINA_PROPERTY_STRING(LinaVG::TextAlignment, 1, "Center")
	LINA_PROPERTY_STRING(LinaVG::TextAlignment, 2, "Right")
	LINA_CLASS_END(LinaVGAlignment)

LINA_CLASS_BEGIN(LinaGXFormat)
LINA_PROPERTY_STRING(LinaGX::Format, 0, "UNDEFINED")
LINA_PROPERTY_STRING(LinaGX::Format, 1,  "R8_SINT")
LINA_PROPERTY_STRING(LinaGX::Format, 2,  "R8_UINT")
LINA_PROPERTY_STRING(LinaGX::Format, 3,  "R8_UNORM")
LINA_PROPERTY_STRING(LinaGX::Format, 4,  "R8_SNORM")

LINA_PROPERTY_STRING(LinaGX::Format, 5,  "R8G8_SINT")
LINA_PROPERTY_STRING(LinaGX::Format, 6,  "R8G8_UINT")
LINA_PROPERTY_STRING(LinaGX::Format, 7,  "R8G8_UNORM")
LINA_PROPERTY_STRING(LinaGX::Format, 8,  "R8G8_SNORM")

LINA_PROPERTY_STRING(LinaGX::Format, 9,  "R8G8B8A8_SINT")
LINA_PROPERTY_STRING(LinaGX::Format, 10,  "R8G8B8A8_UINT")
LINA_PROPERTY_STRING(LinaGX::Format, 11,  "R8G8B8A8_UNORM")
LINA_PROPERTY_STRING(LinaGX::Format, 12,  "R8G8B8A8_SNORM")
LINA_PROPERTY_STRING(LinaGX::Format, 13,  "R8G8B8A8_SRGB")

LINA_PROPERTY_STRING(LinaGX::Format, 14,  "B8G8R8A8_UNORM")
LINA_PROPERTY_STRING(LinaGX::Format, 15,  "B8G8R8A8_SRGB")

LINA_PROPERTY_STRING(LinaGX::Format, 16,  "R16_SINT")
LINA_PROPERTY_STRING(LinaGX::Format, 17,  "R16_UINT")
LINA_PROPERTY_STRING(LinaGX::Format, 18,  "R16_UNORM")
LINA_PROPERTY_STRING(LinaGX::Format, 19,  "R16_SNORM")
LINA_PROPERTY_STRING(LinaGX::Format, 20,  "R16_SFLOAT")

LINA_PROPERTY_STRING(LinaGX::Format, 21,  "R16G16_SINT")
LINA_PROPERTY_STRING(LinaGX::Format, 21,  "R16G16_UINT")
LINA_PROPERTY_STRING(LinaGX::Format, 22,  "R16G16_UNORM")
LINA_PROPERTY_STRING(LinaGX::Format, 23,  "R16G16_SNORM")
LINA_PROPERTY_STRING(LinaGX::Format, 24,  "R16G16_SFLOAT")
                     
LINA_PROPERTY_STRING(LinaGX::Format, 25,  "R16G16B16A16_SINT")
LINA_PROPERTY_STRING(LinaGX::Format, 26,  "R16G16B16A16_UINT")
LINA_PROPERTY_STRING(LinaGX::Format, 27,  "R16G16B16A16_UNORM")
LINA_PROPERTY_STRING(LinaGX::Format, 28,  "R16G16B16A16_SNORM")
LINA_PROPERTY_STRING(LinaGX::Format, 29,  "R16G16B16A16_SFLOAT")

LINA_PROPERTY_STRING(LinaGX::Format, 30,  "R32_SINT")
LINA_PROPERTY_STRING(LinaGX::Format, 31,  "R32_UINT")
LINA_PROPERTY_STRING(LinaGX::Format, 32,  "R32_SFLOAT")

LINA_PROPERTY_STRING(LinaGX::Format, 33,  "R32G32_SINT")
LINA_PROPERTY_STRING(LinaGX::Format, 34,  "R32G32_UINT")
LINA_PROPERTY_STRING(LinaGX::Format, 35,  "R32G32_SFLOAT")

LINA_PROPERTY_STRING(LinaGX::Format, 36,  "R32G32B32_SFLOAT")
LINA_PROPERTY_STRING(LinaGX::Format, 37,  "R32G32B32_SINT")
LINA_PROPERTY_STRING(LinaGX::Format, 38,  "R32G32B32_UINT")

LINA_PROPERTY_STRING(LinaGX::Format, 39,  "R32G32B32A32_SINT")
LINA_PROPERTY_STRING(LinaGX::Format, 40,  "R32G32B32A32_UINT")
LINA_PROPERTY_STRING(LinaGX::Format, 41,  "R32G32B32A32_SFLOAT")

LINA_PROPERTY_STRING(LinaGX::Format, 42,  "D32_SFLOAT")
LINA_PROPERTY_STRING(LinaGX::Format, 43,  "D24_UNORM_S8_UINT")
LINA_PROPERTY_STRING(LinaGX::Format, 44,  "D16_UNORM")

LINA_PROPERTY_STRING(LinaGX::Format, 45,  "R11G11B10_SFLOAT")
LINA_PROPERTY_STRING(LinaGX::Format, 46,  "R10G0B10A2_INT")
LINA_PROPERTY_STRING(LinaGX::Format, 47,  "BC3_BLOCK_SRGB")
LINA_PROPERTY_STRING(LinaGX::Format, 48,  "BC3_BLOCK_UNORM")
LINA_PROPERTY_STRING(LinaGX::Format, 49,  "R11G11B10_SFLOAT")

LINA_CLASS_END(LinaGXFormat)

    LINA_CLASS_BEGIN(LinaGXMipmapMode)
    LINA_PROPERTY_STRING(LinaGX::MipmapMode, 0, "Nearest")
    LINA_PROPERTY_STRING(LinaGX::MipmapMode, 1, "Linear")
    LINA_CLASS_END(LinaGXMipmapMode)

LINA_CLASS_BEGIN(LinaGXMipmapFilter)
LINA_PROPERTY_STRING(LinaGX::MipmapFilter, 0, "Default")
LINA_PROPERTY_STRING(LinaGX::MipmapFilter, 1, "Box")
LINA_PROPERTY_STRING(LinaGX::MipmapFilter, 2, "Triangle")
LINA_PROPERTY_STRING(LinaGX::MipmapFilter, 3, "CubicSpline")
LINA_PROPERTY_STRING(LinaGX::MipmapFilter, 4, "CatmullRom")
LINA_PROPERTY_STRING(LinaGX::MipmapFilter, 5, "Mitchell")
LINA_CLASS_END(LinaGXMipmapFilter)

LINA_CLASS_BEGIN(LinaGXFilter)
LINA_PROPERTY_STRING(LinaGX::Filter, 0, "Anisotropic")
LINA_PROPERTY_STRING(LinaGX::Filter, 1, "Nearest")
LINA_PROPERTY_STRING(LinaGX::Filter, 2, "Linear")
LINA_CLASS_END(LinaGXFilter)

LINA_CLASS_BEGIN(LinaGXImageChannelMask)
LINA_PROPERTY_STRING(LinaGX::ImageChannelMask, 0, "G")
LINA_PROPERTY_STRING(LinaGX::ImageChannelMask, 1, "GA")
LINA_PROPERTY_STRING(LinaGX::ImageChannelMask, 2, "RGB")
LINA_PROPERTY_STRING(LinaGX::ImageChannelMask, 3, "RGBA")
LINA_CLASS_END(LinaGXImageChannelMask)


} // namespace Lina
