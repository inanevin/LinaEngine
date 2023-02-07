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

#ifndef MaterialProperty_HPP
#define MaterialProperty_HPP

#include "Graphics/Core/CommonGraphics.hpp"
#include "Serialization/ISerializable.hpp"

namespace Lina
{
	class Material;

	class MaterialPropertyBase : public ISerializable
	{
	public:
		MaterialPropertyBase() = default;
		MaterialPropertyBase(const String& name, MaterialPropertyType type) : m_name(name), m_type(type){};
		virtual ~MaterialPropertyBase()										   = default;
		virtual uint32				 GetTypeSize()							   = 0;
		virtual void*				 GetData()								   = 0;
		virtual void				 CopyValueFrom(MaterialPropertyBase* base) = 0;
		static MaterialPropertyBase* CreateProperty(MaterialPropertyType type, const String& name);

		inline MaterialPropertyType GetType()
		{
			return m_type;
		}

		inline String GetName()
		{
			return m_name;
		}

	protected:
		String				 m_name = "";
		MaterialPropertyType m_type = MaterialPropertyType::Float;
	};

	template <typename T> class MaterialProperty : public MaterialPropertyBase
	{
	public:
		MaterialProperty() = default;
		MaterialProperty(const String& name, MaterialPropertyType type) : MaterialPropertyBase(name, type){};
		virtual ~MaterialProperty() = default;

		template <typename U = T> std::enable_if_t<std::disjunction_v<std::is_same<U, Vector2>, std::is_same<U, Vector2i>, std::is_same<U, Vector4>, std::is_same<U, Matrix>>, void> TemplatedLoad(IStream& stream)
		{
			m_value.LoadFromStream(stream);
		}

		// Function for T not equal to Vector2, Vector2i, Vector4, or Mat4
		template <typename U = T> std::enable_if_t<!std::disjunction_v<std::is_same<U, Vector2>, std::is_same<U, Vector2i>, std::is_same<U, Vector4>, std::is_same<U, Matrix>>, void> TemplatedLoad(IStream& stream)
		{ /* implementation */
			stream >> m_value;
		}

		template <typename U = T> std::enable_if_t<std::disjunction_v<std::is_same<U, Vector2>, std::is_same<U, Vector2i>, std::is_same<U, Vector4>, std::is_same<U, Matrix>>, void> TemplatedSave(OStream& stream)
		{
			m_value.SaveToStream(stream);
		}

		// Function for T not equal to Vector2, Vector2i, Vector4, or Mat4
		template <typename U = T> std::enable_if_t<!std::disjunction_v<std::is_same<U, Vector2>, std::is_same<U, Vector2i>, std::is_same<U, Vector4>, std::is_same<U, Matrix>>, void> TemplatedSave(OStream& stream)
		{ /* implementation */
			stream << m_value;
		}

		// Inherited via MaterialPropertyBase
		virtual void SaveToStream(OStream& stream) override
		{
			TemplatedSave(stream);
		}

		virtual void LoadFromStream(IStream& stream) override
		{
			TemplatedLoad(stream);
		}

		virtual uint32 GetTypeSize() override
		{
			return static_cast<uint32>(sizeof(T));
		}

		virtual void* GetData() override
		{
			return static_cast<void*>(&m_value);
		}

		inline T GetValue() const
		{
			return m_value;
		}

		void SetValue(T val)
		{
			m_value = val;
		}

		virtual void CopyValueFrom(MaterialPropertyBase* base)
		{
			MaterialProperty<T>* derived = static_cast<MaterialProperty<T>*>(base);
			m_value						 = derived->m_value;
		}

	private:
		T m_value = T();
	};

} // namespace Lina

#endif
