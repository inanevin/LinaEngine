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

#include "Graphics/Resource/Material.hpp"
#include "Serialization/StringSerialization.hpp"
#include "Graphics/Resource/Shader.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Graphics/Platform/GfxManagerIncl.hpp"
#include "Math/Math.hpp"

// #define DEBUG_RELOAD_PROPERTIES

namespace Lina
{
	Material::~Material()
	{
		for (auto p : m_properties)
			delete p;

		m_textures.clear();

		if (m_gpuHandle == -1)
			return;

		m_resourceManager->GetSystem()->GetSubsystem<GfxManager>(SubsystemType::GfxManager)->GetGPUStorage().DestroyMaterial(m_gpuHandle);
	}

	void Material::SetShader(StringID shader)
	{
		m_shaderHandle = shader;

		// Delete & reassign properties.
		for (auto p : m_properties)
			delete p;
		m_textures.clear();

		auto*		sh		 = m_resourceManager->GetResource<Shader>(m_shaderHandle);
		const auto& props	 = sh->GetProperties();
		const auto& textures = sh->GetTextures();

		for (auto p : props)
		{
			MaterialPropertyBase* newProp = MaterialPropertyBase::CreateProperty(p->GetType(), p->GetName());
			m_properties.push_back(newProp);
		}

		for (auto t : textures)
		{
			MaterialProperty<StringID> matPropTxt = MaterialProperty<StringID>(t->GetName(), t->GetType());

			// Assign default black.
			if (matPropTxt.GetValue() == 0)
				matPropTxt.SetValue("Resources/Core/Textures/DummyBlack_32.png"_hs);

			m_textures.push_back(matPropTxt);
		}

		m_totalPropertySize = 0;

		for (auto p : m_properties)
			m_totalPropertySize += p->GetTypeSize();

		m_totalAlignedSize = GetPropertiesTotalAlignedSize();

		m_gpuHandle = m_resourceManager->GetSystem()->GetSubsystem<GfxManager>(SubsystemType::GfxManager)->GetGPUStorage().GenerateMaterial(this, m_gpuHandle);
	}

	bool Material::SetTexture(uint32 index, StringID texture)
	{
		const uint32 size = static_cast<uint32>(m_textures.size());
		if (index >= size)
		{
			LINA_WARN("[Material] -> Can't set texture because index is overflowing.");
			return false;
		}

		m_textures[index].SetValue(texture);
		m_dirtyTextureIndices.push_back(index);
		return true;
	}

	bool Material::SetTexture(const String& name, StringID texture)
	{
		const uint32 size = static_cast<uint32>(m_textures.size());
		for (uint32 i = 0; i < size; i++)
		{
			auto& t = m_textures[i];
			if (t.GetName().compare(name) == 0)
				return SetTexture(i, texture);
		}

		return false;
	}

	void Material::UpdateBuffers()
	{
		if (m_propertiesDirty)
			m_resourceManager->GetSystem()->GetSubsystem<GfxManager>(SubsystemType::GfxManager)->GetGPUStorage().UpdateMaterialProperties(this);

		if (!m_dirtyTextureIndices.empty())
			m_resourceManager->GetSystem()->GetSubsystem<GfxManager>(SubsystemType::GfxManager)->GetGPUStorage().UpdateMaterialTextures(this, m_dirtyTextureIndices);

		m_propertiesDirty = false;
		m_dirtyTextureIndices.clear();
	}

	void Material::GetPropertyBlob(uint8*& outData, size_t& outSize)
	{
		if (m_totalAlignedSize == 0)
			return;

		outData = new uint8[m_totalAlignedSize];
		outSize = m_totalAlignedSize;

		uint32 offset = 0;

		for (auto& p : m_properties)
		{
			const uint32 typeSize  = p->GetTypeSize();
			const uint32 alignment = GetPropertyTypeAlignment(p->GetType());
			void*		 src	   = p->GetData();

			if (offset != 0)
			{
				auto mod = offset % alignment;

				if (mod != 0)
				{
					if (offset < alignment)
						offset = alignment;
					else
						offset = static_cast<uint32>(Math::CeilToInt(static_cast<float>(offset) / static_cast<float>(alignment)) * alignment);
				}
			}

			MEMCPY(outData + offset, src, typeSize);
			offset += typeSize;
		}
	}

	void Material::LoadFromFile(const char* path)
	{
		IResource::LoadFromFile(path);

		m_totalPropertySize = 0;

		for (auto p : m_properties)
			m_totalPropertySize += p->GetTypeSize();

		m_totalAlignedSize = GetPropertiesTotalAlignedSize();
	}

	void Material::SaveToStream(OStream& stream)
	{
		stream << m_shaderHandle;

		const uint32 propSize = static_cast<uint32>(m_properties.size());
		stream << propSize;

		for (auto& p : m_properties)
		{
			String		name = p->GetName();
			const uint8 type = static_cast<uint8>(p->GetType());
			stream << type;
			StringSerialization::SaveToStream(stream, name);
			p->SaveToStream(stream);
		}

		const uint32 txtSize = static_cast<uint32>(m_textures.size());
		stream << txtSize;

		for (auto& t : m_textures)
		{
			String name = t.GetName();
			StringSerialization::SaveToStream(stream, name);
			t.SaveToStream(stream);
		}

		stream << m_totalPropertySize << m_totalAlignedSize;
	}

	void Material::LoadFromStream(IStream& stream)
	{
		stream >> m_shaderHandle;

		uint32 propSize = 0;

		stream >> propSize;

		for (uint32 i = 0; i < propSize; i++)
		{
			uint8  typeInt = 0;
			String name	   = "";
			stream >> typeInt;
			StringSerialization::LoadFromStream(stream, name);

			const MaterialPropertyType type = static_cast<MaterialPropertyType>(typeInt);
			auto					   prop = MaterialPropertyBase::CreateProperty(type, name);
			prop->LoadFromStream(stream);
			m_properties.push_back(prop);
		}

		uint32 txtSize = 0;

		stream >> txtSize;

		for (uint32 i = 0; i < txtSize; i++)
		{
			String name = "";
			StringSerialization::LoadFromStream(stream, name);

			MaterialProperty<StringID> texture = MaterialProperty<StringID>(name, MaterialPropertyType::Texture);
			texture.LoadFromStream(stream);
			m_textures.push_back(texture);
		}

		stream >> m_totalPropertySize >> m_totalAlignedSize;
	}

	void Material::BatchLoaded()
	{
		m_gpuHandle = m_resourceManager->GetSystem()->GetSubsystem<GfxManager>(SubsystemType::GfxManager)->GetGPUStorage().GenerateMaterial(this, m_gpuHandle);
	}

	uint32 Material::GetPropertyTypeAlignment(MaterialPropertyType type)
	{
		if (type == MaterialPropertyType::Vector4 || type == MaterialPropertyType::Mat4)
			return static_cast<uint32>(16);
		else if (type == MaterialPropertyType::Vector2 || type == MaterialPropertyType::Vector2i)
			return static_cast<uint32>(8);
		return static_cast<uint32>(4);
	}

	uint32 Material::GetPropertiesTotalAlignedSize()
	{
		uint32 offset = 0;

		for (auto& p : m_properties)
		{
			const uint32 typeSize  = p->GetTypeSize();
			const uint32 alignment = GetPropertyTypeAlignment(p->GetType());
			void*		 src	   = p->GetData();

			if (offset != 0)
			{
				auto mod = offset % alignment;

				if (mod != 0)
				{
					if (offset < alignment)
						offset = alignment;
					else
						offset = static_cast<uint32>(Math::CeilToInt(static_cast<float>(offset) / static_cast<float>(alignment)) * alignment);
				}
			}

			offset += typeSize;
		}

		return offset;
	}
} // namespace Lina
