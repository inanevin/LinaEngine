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
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Core/IGpuStorage.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Math/Math.hpp"

// #define DEBUG_RELOAD_PROPERTIES

namespace Lina
{
	Material::Material(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid) : IResource(rm, isUserManaged, path, sid, GetTypeID<Material>())
	{
		m_renderer = rm->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager)->GetRenderer();
	}

	Material::~Material()
	{
		for (auto p : m_properties)
			delete p;

		if (m_gpuHandle == -1)
			return;

		m_renderer->DestroyMaterial(m_gpuHandle);
	}

	void Material::SetShader(StringID shader)
	{
		if (m_shader != nullptr && m_shaderHandle == shader)
			return;

		m_shaderHandle = shader;

		// Delete & reassign properties.
		for (auto p : m_properties)
			delete p;

		m_shader			 = m_resourceManager->GetResource<Shader>(m_shaderHandle);
		const auto& props	 = m_shader->GetProperties();
		const auto& textures = m_shader->GetTextures();

		for (auto p : props)
		{
			MaterialPropertyBase* newProp = MaterialPropertyBase::CreateProperty(p->GetType(), p->GetName());
			m_properties.push_back(newProp);
		}
		m_totalPropertySize = 0;

		// Calculate props sizes, textures have double cause they have sampler index as well.
		for (auto p : m_properties)
			m_totalPropertySize += p->GetType() == MaterialPropertyType::Texture ? p->GetTypeSize() * 2 : p->GetTypeSize();

		m_totalAlignedSize = GetPropertiesTotalAlignedSize();
		m_renderer->GenerateMaterial(this);
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
			const uint32 typeSize	= p->GetTypeSize();
			const uint32 alignment	= GetPropertyTypeAlignment(p->GetType());
			void*		 src		= p->GetData();
			uint32		 textureSrc = 0;
			uint32		 samplerSrc = 0;

			if (p->GetType() == MaterialPropertyType::Texture)
			{
				MaterialProperty<StringID>* prop	   = static_cast<MaterialProperty<StringID>*>(p);
				StringID					textureSID = prop->GetValue();

				if (textureSID == 0)
					textureSID = DEFAULT_TEXTURE_SID;

				textureSrc = m_renderer->GetTextureIndex(textureSID);
				samplerSrc = m_renderer->GetSamplerIndex(textureSID);
			}

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

			if (p->GetType() == MaterialPropertyType::Texture)
			{
				MEMCPY(outData + offset, &textureSrc, typeSize);
				offset += typeSize;
				MEMCPY(outData + offset, &samplerSrc, typeSize);
			}
			else
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

		stream >> m_totalPropertySize >> m_totalAlignedSize;
	}

	void Material::BatchLoaded()
	{
		SetShader(m_shaderHandle);
	}

	uint32 Material::GetPropertyTypeAlignment(MaterialPropertyType type)
	{
		if (type == MaterialPropertyType::Vector4 || type == MaterialPropertyType::Mat4)
			return static_cast<uint32>(sizeof(float) * 4);
		else if (type == MaterialPropertyType::Vector2)
			return static_cast<uint32>(sizeof(float) * 2);
		else if (type == MaterialPropertyType::Vector2i)
			return static_cast<uint32>(sizeof(int) * 2);
		else if (type == MaterialPropertyType::Vector4i)
			return static_cast<uint32>(sizeof(int) * 4);
		else if (type == MaterialPropertyType::Int || type == MaterialPropertyType::Texture)
			return static_cast<uint32>(sizeof(int));

		return static_cast<uint32>(sizeof(float));
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

			if (p->GetType() == MaterialPropertyType::Texture)
				offset += typeSize;
		}

#ifdef LINA_GRAPHICS_DX12
		return ALIGN_SIZE_POW(offset, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
#endif

		return offset;
	}
} // namespace Lina
