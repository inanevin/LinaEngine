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
#include "Common/Reflection/ReflectionSystem.hpp"

namespace Lina
{
	enum class FieldType
	{
		UInt8,
		Int8,
		UInt16,
		Int16,
		UInt32,
		Int32,
		Float,
		Boolean,
		String,
		StringFixed,
		Label,
		Info,
		Warning,
		Error,
		UserClass,
		Vector,
		Enum,
		Color,
		ColorGrad,
		Vector2,
		Vector2ui,
		Vector2i,
		Vector3,
		Vector3i,
		Vector3ui,
		Vector4,
		Vector4ui,
		Vector4i,
		Rect,
		Rectui,
		Recti,
		TBLR,
		Bitmask32,
		ResourceID,
		EntityID,
	};

	class ReflectionClassUtility
	{
	public:
		template <typename T> static inline void REF_SaveToStream(OStream& stream, void* obj)
		{
			MetaType*				 meta	= ReflectionSystem::Get().Meta<T>();
			const Vector<FieldPair>& fields = meta->GetFields();
			for (const FieldPair& pair : fields)
			{
				pair.field->SaveToStream(stream, obj);
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
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddProperty<Lina::String>(Lina::TO_SIDC("Title"), TITLE);                                                                                                                                                     \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddProperty<Lina::String>(Lina::TO_SIDC("Category"), CATEGORY);

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
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddFunction<void(OStream&, void*)>("SaveToStream"_hs, std::bind(&Lina::ReflectionClassUtility::REF_SaveToStream<ClassName>, std::placeholders::_1, std::placeholders::_2));                                   \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddFunction<void(OStream&, void*)>("LoadFromStream"_hs, std::bind(&Lina::ReflectionClassUtility::REF_SaveToStream<ClassName>, std::placeholders::_1, std::placeholders::_2));

/* PROPERTIES AND FUNCTIONS PER REFLECTED CLASS*/
#define LINA_FIELD(ClassName, FieldName, TITLE, TYPE, SUBTYPE)                                                                                                                                                                                                     \
	const Lina::StringID sid_##FieldName = Lina::TO_SIDC(TOSTRING(FieldName));                                                                                                                                                                                     \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddField<&ClassName::FieldName, ClassName>(sid_##FieldName);                                                                                                                                                  \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Title"), TITLE);                                                                                                                          \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddProperty<Lina::FieldType>(Lina::TO_SIDC("Type"), TYPE);                                                                                                                         \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddProperty<Lina::TypeID>(Lina::TO_SIDC("SubType"), SUBTYPE);

#define LINA_FIELD_LOCK0(ClassName, FieldName) Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddProperty<uint8>(Lina::TO_SIDC("Lock0"), 0);

#define LINA_FIELD_VEC(ClassName, FieldName, TITLE, SUBTYPE, SUBTYPERAW, SUBTYPETID)                                                                                                                                                                               \
	const Lina::StringID sid_##FieldName = Lina::TO_SIDC(TOSTRING(FieldName));                                                                                                                                                                                     \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->AddField<&ClassName::FieldName, ClassName>(sid_##FieldName);                                                                                                                                                  \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Title"), TITLE);                                                                                                                          \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddProperty<Lina::FieldType>(Lina::TO_SIDC("Type"), FieldType::Vector);                                                                                                            \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddProperty<Lina::FieldType>(Lina::TO_SIDC("SubType"), SUBTYPE);                                                                                                                   \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddProperty<Lina::TypeID>(Lina::TO_SIDC("SubTypeTID"), SUBTYPETID);                                                                                                                \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddFunction<uint32(void*)>("GetVectorSize"_hs, std::bind(&Lina::ReflectionClassUtility::REF_GetVectorSize<SUBTYPERAW>, std::placeholders::_1));                                    \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddFunction<void(void*)>("AddNewElement"_hs, std::bind(&Lina::ReflectionClassUtility::REF_AddNewVectorElement<SUBTYPERAW>, std::placeholders::_1));                                \
	Lina::ReflectionSystem::Get()                                                                                                                                                                                                                                  \
		.Meta<ClassName>()                                                                                                                                                                                                                                         \
		->GetField(sid_##FieldName)                                                                                                                                                                                                                                \
		->AddFunction<void(void*, int32)>("DuplicateElement"_hs, std::bind(&Lina::ReflectionClassUtility::REF_DuplicateVectorElement<SUBTYPERAW>, std::placeholders::_1, std::placeholders::_2));                                                                  \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddFunction<void(void*, int32)>("RemoveElement"_hs, std::bind(&Lina::ReflectionClassUtility::REF_RemoveVectorElement<SUBTYPERAW>, std::placeholders::_1, std::placeholders::_2));  \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddFunction<void(void*)>("ClearVector"_hs, std::bind(&Lina::ReflectionClassUtility::REF_ClearVector<SUBTYPERAW>, std::placeholders::_1));                                          \
	Lina::ReflectionSystem::Get()                                                                                                                                                                                                                                  \
		.Meta<ClassName>()                                                                                                                                                                                                                                         \
		->GetField(sid_##FieldName)                                                                                                                                                                                                                                \
		->AddFunction<void*(void*, int32 index)>("GetElementAddr"_hs, std::bind(&Lina::ReflectionClassUtility::REF_GetVectorElementAddr<SUBTYPERAW>, std::placeholders::_1, std::placeholders::_2));

#define LINA_FIELD_TOOLTIP(ClassName, FieldName, TOOLTIP) Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddProperty<Lina::String>(Lina::TO_SIDC("Tooltip"), TOOLTIP);

#define LINA_FIELD_DEPENDENCY(ClassName, FieldName, Dependency, Value) Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddDependency(TO_SIDC(Dependency), Value);

#define LINA_FIELD_DEPENDENCY_OP(ClassName, FieldName, Dependency, Value, Operation) Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddDependency(TO_SIDC(Dependency), Value, TO_SIDC(Operation));

#define LINA_FIELD_LIMITS(ClassName, FieldName, MIN, MAX, STEP)                                                                                                                                                                                                    \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddProperty<float>(Lina::TO_SIDC("Min"), MIN);                                                                                                                                     \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddProperty<float>(Lina::TO_SIDC("Max"), MAX);                                                                                                                                     \
	Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddProperty<float>(Lina::TO_SIDC("Step"), STEP);

#define LINA_PROPERTY_STRING(ClassName, PropertySID, VALUE)							 Lina::ReflectionSystem::Get().Meta<ClassName>()->AddProperty<Lina::String>(PropertySID, VALUE);
#define LINA_FIELD_PROPERTY(ClassName, FieldName, PropertyType, PropertyName, Value) Lina::ReflectionSystem::Get().Meta<ClassName>()->GetField(sid_##FieldName)->AddProperty<PropertyType>(Lina::TO_SIDC(PropertyName), Value);

#define LINA_REFLECTION_ACCESS(ClassName) friend class ClassName##_Reflected;

} // namespace Lina
