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
#include "Common/Serialization/StringSerialization.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/Serialization/Serialization.hpp"

namespace Lina
{
	Material::~Material()
	{
		if (m_data != nullptr)
			delete[] m_data;
	}

	void Material::SetShader(StringID sid)
	{
		m_shader = sid;

		if (m_data != nullptr)
			delete[] m_data;

		Shader* shader = m_resourceManager->GetResource<Shader>(m_shader);

		if (shader == nullptr)
		{
			// possibly deleted shader.
			m_shader = DEFAULT_SHADER_SID;
			shader	 = m_resourceManager->GetResource<Shader>(m_shader);
		}

		if (shader)
		{
			// m_uboDefinition = shader->GetMaterialUBO();
			//
			// m_totalDataSize = 0;
			// for (const auto& mem : m_uboDefinition.members)
			// 	m_totalDataSize += mem.alignment;
			//
			// if (m_totalDataSize == 0)
			// 	return;
			//
			// m_data = new uint8[m_totalDataSize];
		}
	}

	void Material::GetDataBlob(uint8* ptr, size_t size)
	{
		ptr = new uint8[m_totalDataSize];
		MEMCPY(ptr, m_data, m_totalDataSize);
	}

	void Material::LoadFromFile(const char* path)
	{
		IStream stream = Serialization::LoadFromFile(path);

		if (stream.GetDataRaw() != nullptr)
			LoadFromStream(stream);

		stream.Destroy();
	}

	void Material::SaveToStream(OStream& stream)
	{
		stream << m_shader << m_totalDataSize;

		if (m_totalDataSize != 0)
			stream.WriteEndianSafe(m_data, m_totalDataSize);
	}

	void Material::LoadFromStream(IStream& stream)
	{
		stream >> m_shader >> m_totalDataSize;

		if (m_totalDataSize != 0)
		{
			m_data = new uint8[m_totalDataSize];
			stream.ReadEndianSafe(m_data, m_totalDataSize);
		}
	}

	void Material::BatchLoaded()
	{
		SetShader(m_shader);
	}

} // namespace Lina
