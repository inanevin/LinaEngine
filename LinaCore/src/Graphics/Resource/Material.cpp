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
#include "Core/Graphics/GfxContext.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Pipeline/DescriptorSet.hpp"

#include "Common/Serialization/Serialization.hpp"

#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina
{
	Material::~Material()
	{
		DestroyProperties();
	}

	void Material::DestroyProperties()
	{
		for (MaterialProperty* p : m_properties)
			delete p;
		m_properties.clear();
	}

	void Material::SetShader(Shader* shader)
	{
		m_shader	 = shader->GetID();
		m_shaderType = shader->GetShaderType();

		const Vector<ShaderPropertyDefinition>& properties = shader->GetPropertyDefinitions();

		Vector<MaterialProperty*> props = {};

		for (const ShaderPropertyDefinition& def : properties)
		{
			MaterialProperty* prop = new MaterialProperty();
			prop->propDef		   = def;

			size_t dataSize = 0;

			if (prop->propDef.type == ShaderPropertyType::Float)
				dataSize = sizeof(float);
			else if (prop->propDef.type == ShaderPropertyType::UInt32)
				dataSize = sizeof(uint32);
			else if (prop->propDef.type == ShaderPropertyType::Bool)
				dataSize = sizeof(uint32);
			else if (prop->propDef.type == ShaderPropertyType::Vec2)
				dataSize = sizeof(Vector2);
			else if (prop->propDef.type == ShaderPropertyType::Vec3)
				dataSize = sizeof(Vector3);
			else if (prop->propDef.type == ShaderPropertyType::Vec4)
				dataSize = sizeof(Vector4);
			else if (prop->propDef.type == ShaderPropertyType::IVec2)
				dataSize = sizeof(Vector2i);
			else if (prop->propDef.type == ShaderPropertyType::IVec3)
				dataSize = sizeof(Vector3i);
			else if (prop->propDef.type == ShaderPropertyType::IVec4)
				dataSize = sizeof(Vector4i);
			else if (prop->propDef.type == ShaderPropertyType::Matrix4)
				dataSize = sizeof(Matrix4);
			else if (prop->propDef.type == ShaderPropertyType::Texture2D)
				dataSize = sizeof(LinaTexture2D);
			else
			{
				LINA_ASSERT(false, "");
			}

			prop->data = {new uint8[dataSize], dataSize};

			auto it = linatl::find_if(m_properties.begin(), m_properties.end(), [&def](MaterialProperty* prop) -> bool { return prop->propDef.sid == def.sid && prop->propDef.type == def.type; });

			if (it != m_properties.end())
			{
				MaterialProperty* propInShader = *it;
				MEMCPY(prop->data.data(), propInShader->data.data(), dataSize);
			}
			else
				MEMSET(prop->data.data(), 0, dataSize);

			props.push_back(prop);
		}

		DestroyProperties();
		m_properties = props;
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
		stream << m_shaderType;
	}

	void Material::LoadFromStream(IStream& stream)
	{
		DestroyProperties();

		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;
		stream >> m_shader;
		stream >> m_properties;
		stream >> m_shaderType;
	}

	size_t Material::GetSize() const
	{
		size_t total = 0;
		for (MaterialProperty* p : m_properties)
			total += sizeof(MaterialProperty) + p->data.size();
		return total;
	}

	void Material::GenerateHW()
	{
	}

	void Material::DestroyHW()
	{
	}

	size_t Material::GetBufferSize()
	{
		size_t totalSize = 0;

		for (MaterialProperty* prop : m_properties)
		{
			if (prop->propDef.type == ShaderPropertyType::Texture2D)
				totalSize += sizeof(LinaTexture2DBinding);
			else
				totalSize += prop->data.size();
		}
		return totalSize;
	}

	void Material::BufferDataInto(Buffer& buf, size_t padding, ResourceManagerV2* rm, GfxContext* context)
	{

		for (MaterialProperty* prop : m_properties)
		{
			if (prop->propDef.type == ShaderPropertyType::Texture2D)
			{
				LinaTexture2D* txt = reinterpret_cast<LinaTexture2D*>(prop->data.data());

				Texture*		texture = rm->GetIfExists<Texture>(txt->texture);
				TextureSampler* sampler = rm->GetIfExists<TextureSampler>(txt->sampler);

				// if (!texture)
				// 	texture = rm->GetIfExists<Texture>(ENGINE_TEXTURE_EMPTY_ALBEDO_ID);
				// if (!sampler)
				// 	sampler = rm->GetIfExists<TextureSampler>(ENGINE_SAMPLER_DEFAULT_ID);
				LINA_ASSERT(texture != nullptr && sampler != nullptr, "");

				LinaTexture2DBinding binding = {
					.textureIndex = texture ? texture->GetBindlessIndex() : 0,
					.samplerIndex = sampler ? sampler->GetBindlessIndex() : 0,
				};

				buf.BufferData(padding, (uint8*)&binding, sizeof(LinaTexture2DBinding));
				padding += sizeof(LinaTexture2DBinding);
			}
			else
			{

				buf.BufferData(padding, prop->data.data(), prop->data.size());
				padding += prop->data.size();
			}
		}
	}

	void Material::CopyPropertiesFrom(Material* mat)
	{
		const Vector<MaterialProperty*> propsToCopy = mat->GetProperties();
		DestroyProperties();
		for (MaterialProperty* p : propsToCopy)
		{
			MaterialProperty* newProp = new MaterialProperty();
			newProp->propDef		  = p->propDef;
			newProp->data			  = {new uint8[p->data.size()], p->data.size()};
			MEMCPY(newProp->data.data(), p->data.data(), p->data.size());
			m_properties.push_back(newProp);
		}
	}

} // namespace Lina
