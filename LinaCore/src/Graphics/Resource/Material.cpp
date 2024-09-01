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

#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Pipeline/DescriptorSet.hpp"
#include "Common/System/System.hpp"

#include "Common/Serialization/Serialization.hpp"
#include "Common/Serialization/VectorSerialization.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina
{

	void Material::BindingData::SaveToStream(OStream& stream) const
	{
		stream << static_cast<int32>(bufferData[0].buffers.size());

		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			for (const auto& buf : bufferData[i].buffers)
				buf.SaveToStream(stream);
		}

		stream << static_cast<int32>(textures.size());
		stream << static_cast<int32>(samplers.size());

		for (const auto& txt : textures)
			stream << txt;

		for (const auto& smp : samplers)
			stream << smp;
	}

	void Material::BindingData::LoadFromStream(IStream& stream)
	{
		int32 buffersSz = 0, texturesSz = 0, samplersSz = 0;
		stream >> buffersSz >> texturesSz >> samplersSz;

		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			bufferData[i].buffers.resize(buffersSz);

			for (int32 j = 0; j < buffersSz; j++)
				bufferData[i].buffers[j].LoadFromStream(stream);
		}

		textures.resize(texturesSz);
		samplers.resize(samplersSz);

		for (int32 i = 0; i < texturesSz; i++)
			stream >> textures[i];

		for (int32 i = 0; i < samplersSz; i++)
			stream >> samplers[i];
	}

	Material::~Material()
	{
	}

	void Material::SetShader(Shader* shader)
	{
		m_shader	 = shader;
		m_shaderSID	 = shader->GetSID();
		m_shaderPath = shader->GetPath();
	}

	void Material::ResetProperties()
	{
		m_properties = m_shader->GetProperties();
	}

	void Material::LoadFromFile(const char* path)
	{
		IStream stream = Serialization::LoadFromFile(path);

		if (stream.GetDataRaw() != nullptr)
			LoadFromStream(stream);

		stream.Destroy();
	}

	void Material::SaveToStream(OStream& stream) const
	{
		stream << VERSION;
		stream << m_id;
		stream << m_shaderSID;
		stream << m_shaderPath;
		VectorSerialization::SaveToStream_OBJ(stream, m_properties);
	}

	void Material::LoadFromStream(IStream& stream)
	{
		uint32 version = 0;
		stream >> version;
		stream >> m_id;
		stream >> m_shaderSID;
		stream >> m_shaderPath;
		VectorSerialization::LoadFromStream_OBJ(stream, m_properties);
	}

	Shader* Material::GetShader(ResourceManagerV2* rm)
	{
		if (m_shader == nullptr)
			m_shader = rm->GetResource<Shader>(m_shaderSID);

		return m_shader;
	}

	size_t Material::BufferDataInto(Buffer& buf, size_t padding)
	{
		m_bindlessBytePadding = padding;
		size_t totalSize	  = 0;
		for (const auto& prop : m_properties)
		{
			if (prop.type == ShaderPropertyType::Texture2D)
			{
				// LinaTexture2D stringIDs = std::get<LinaTexture2D>(prop.data);
				// LinaTexture2D bindless	= {
				// 	 .texture = m_resourceManager->GetResource<Texture>(stringIDs.texture)->GetBindlessIndex(),
				// 	 .sampler = m_resourceManager->GetResource<TextureSampler>(stringIDs.sampler)->GetBindlessIndex(),
				//  };
				// buf.BufferData(padding, (uint8*)&bindless, prop.size);
			}
			else
				buf.BufferData(padding, (uint8*)&prop.data, prop.size);
			padding += prop.size;
			totalSize += prop.size;
		}

		return totalSize;
	}

} // namespace Lina
