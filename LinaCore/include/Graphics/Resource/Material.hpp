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

#ifndef Material_HPP
#define Material_HPP

#include "Resources/Core/IResource.hpp"
#include "Graphics/Resource/MaterialProperty.hpp"

namespace Lina
{
	class MaterialPropertyBase;
	class Shader;
	class Renderer;

	class Material : public IResource
	{
	public:
		Material(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid);
		virtual ~Material();

		void SetShader(StringID shader);
		bool SetTexture(uint32 textureIndex, StringID texture);
		bool SetTexture(const String& name, StringID texture);
		void UpdateBuffers(uint32 imageIndex);
		void GetPropertyBlob(uint8*& outData, size_t& outSize);

		inline const Vector<MaterialProperty<StringID>>& GetTextures() const
		{
			return m_textures;
		}

		inline StringID GetShaderHandle() const
		{
			return m_shaderHandle;
		}

		inline uint32 GetTotalPropertySize() const
		{
			return m_totalPropertySize;
		}

		inline int32 GetGPUHandle() const
		{
			return m_gpuHandle;
		}

		inline uint32 GetTotalAlignedSize() const
		{
			return m_totalAlignedSize;
		}

		inline Shader* GetShader() const
		{
			return m_shader;
		}

		template <typename T> bool SetProperty(uint32 index, T value)
		{
			if (m_properties.size() <= index)
			{
				LINA_WARN("[Material] -> Can't set property because index is overflowing.");
				return false;
			}

			MaterialProperty<T>* p = static_cast<MaterialProperty<T>*>(m_properties[index]);
			p->SetValue(value);
			m_propertiesDirty = true;
			return true;
		}

		template <typename T> bool SetProperty(const String& name, T value)
		{
			int32 selected = -1;
			int32 index	   = 0;
			for (auto& p : m_properties)
			{
				if (p->GetName().compare(name) == 0)
				{
					selected = index;
					break;
				}
				index++;
			}

			if (selected == -1)
			{
				LINA_WARN("[Material] -> Can't set property because name isn't found!");
				return false;
			}

			return SetProperty(static_cast<uint32>(selected), value);
		}

	protected:
		// Inherited via IResource
		virtual void LoadFromFile(const char* path) override;
		virtual void SaveToStream(OStream& stream) override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void BatchLoaded() override;

		uint32 GetPropertyTypeAlignment(MaterialPropertyType type);
		uint32 GetPropertiesTotalAlignedSize();

	private:
		Renderer*						   m_renderer = nullptr;
		Vector<MaterialPropertyBase*>	   m_properties;
		Vector<MaterialProperty<StringID>> m_textures;
		Vector<uint32>					   m_dirtyTextureIndices;
		bool							   m_propertiesDirty   = false;
		int32							   m_gpuHandle		   = -1;
		StringID						   m_shaderHandle	   = 0;
		Shader*							   m_shader			   = nullptr;
		uint32							   m_totalPropertySize = 0;
		uint32							   m_totalAlignedSize  = 0;
	};

} // namespace Lina

#endif
