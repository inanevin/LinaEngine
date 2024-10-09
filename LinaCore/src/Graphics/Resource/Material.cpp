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
#include "Core/Graphics/BindlessContext.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Pipeline/DescriptorSet.hpp"

#include "Common/Serialization/Serialization.hpp"

#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina
{

	void Material::BindingData::SaveToStream(OStream& stream) const
	{
		stream << static_cast<int32>(bufferData[0].buffers.size());

		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			for (const auto& buf : bufferData[i].buffers)
				stream << buf;
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
				stream >> bufferData[i].buffers[j];
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
		for (ShaderProperty* p : m_properties)
			delete p;
	}

	void Material::SetShader(Shader* shader)
	{
		m_shader.raw = shader;
		m_shader.id	 = shader->GetID();

		const Vector<ShaderProperty*>& properties = m_shader.raw->GetProperties();

		// If property this material has does not exists in shader delete it from this material.
		for (Vector<ShaderProperty*>::iterator it = m_properties.begin(); it != m_properties.end();)
		{
			ShaderProperty* p = *it;

			auto found = linatl::find_if(properties.begin(), properties.end(), [p](ShaderProperty* prop) -> bool { return p->sid == prop->sid && p->type == prop->type; });
			if (found == properties.end())
			{
				delete p;
				it = m_properties.erase(it);
			}
			else
				++it;
		}

		// If property shader has does not exists in this material add it.
		Vector<ShaderProperty*> addList;
		for (ShaderProperty* p : properties)
		{
			auto it = linatl::find_if(m_properties.begin(), m_properties.end(), [p](ShaderProperty* prop) -> bool { return p->sid == prop->sid && p->type == prop->type; });
			if (it == m_properties.end())
			{
				ShaderProperty* prop = new ShaderProperty();
				prop->type			 = p->type;
				prop->sid			 = p->sid;
				prop->name			 = p->name;
				prop->data			 = {new uint8[p->data.size()], p->data.size()};
				MEMCPY(prop->data.data(), p->data.data(), p->data.size());
				addList.push_back(prop);
			}
		}
		for (ShaderProperty* newProp : addList)
			m_properties.push_back(newProp);
	}

	void Material::SetShaderID(ResourceID id)
	{
		m_shader.id = id;
	}

	bool Material::LoadFromFile(const String& path)
	{
		IStream stream = Serialization::LoadFromFile(path.c_str());

		if (!stream.Empty())
			LoadFromStream(stream);
		else
			return false;

		stream.Destroy();
		return true;
	}

	void Material::SaveToStream(OStream& stream) const
	{
		Resource::SaveToStream(stream);
		stream << VERSION;
		stream << m_shader;
		stream << m_properties;
	}

	void Material::LoadFromStream(IStream& stream)
	{
		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;
		stream >> m_shader;
		stream >> m_properties;
	}

	Shader* Material::GetShader(ResourceManagerV2* rm)
	{
		if (m_shader.raw == nullptr)
			m_shader.raw = rm->GetResource<Shader>(m_shader.id);

		return m_shader.raw;
	}

	size_t Material::BufferDataInto(Buffer& buf, size_t padding, ResourceManagerV2* rm, BindlessContext* context)
	{
		size_t totalSize = 0;

		for (ShaderProperty* prop : m_properties)
		{
			if (prop->type == ShaderPropertyType::Texture2D)
			{
				LinaTexture2D* bindless	  = reinterpret_cast<LinaTexture2D*>(prop->data.data());
				uint32		   txtIdx	  = context->GetBindlessIndex(rm->GetResource<Texture>(bindless->texture));
				uint32		   samplerIdx = context->GetBindlessIndex(rm->GetResource<TextureSampler>(bindless->sampler));
				buf.BufferData(padding, (uint8*)&txtIdx, sizeof(uint32));
				buf.BufferData(padding + sizeof(uint32), (uint8*)&samplerIdx, sizeof(uint32));
				padding += sizeof(uint32) * 2;
				totalSize += sizeof(uint32) * 2;
			}
			else
			{
				buf.BufferData(padding, prop->data.data(), prop->data.size());
				padding += prop->data.size();
				totalSize += prop->data.size();
			}
		}

		return totalSize;
	}

} // namespace Lina
