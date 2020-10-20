/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Rendering/Material.hpp"
#include "Rendering/Texture.hpp"
#include <cereal/archives/json.hpp>
#include <stdio.h>
#include <cereal/archives/binary.hpp>
#include <fstream>

namespace LinaEngine::Graphics
{
	void Material::LoadMaterialData(Material& mat, const std::string& path)
	{
		std::ifstream stream(path);
		{
			cereal::BinaryInputArchive iarchive(stream);

			// Read the data into it.
			iarchive(mat);
		}
	}

	void Material::SaveMaterialData(const Material& mat, const std::string& path)
	{
		std::ofstream stream(path);
		{
			cereal::BinaryOutputArchive oarchive(stream); // Create an output archive

			oarchive(mat); // Write the data to the archive
		}
	}

	void Material::SetTexture(const std::string& textureName, Texture* texture, TextureBindMode bindMode)
	{
		if (!(m_sampler2Ds.find(textureName) == m_sampler2Ds.end()))
		{
			m_sampler2Ds[textureName].m_boundTexture = texture;
			m_sampler2Ds[textureName].m_bindMode = bindMode;
			m_sampler2Ds[textureName].m_isActive = texture == nullptr ? false : true;
			m_sampler2Ds[textureName].m_path = texture->GetPath();
		}
		else
		{
			LINA_CORE_ERR("This material doesn't support texture slot with the name {0}, returning...", textureName);
			return;
		}
	}
	void Material::RemoveTexture(const std::string& textureName)
	{
		if (!(m_sampler2Ds.find(textureName) == m_sampler2Ds.end()))
		{
			m_sampler2Ds[textureName].m_boundTexture = nullptr;
			m_sampler2Ds[textureName].m_isActive = false;
		}
		else
		{
			LINA_CORE_ERR("This material doesn't support texture slot with the name {0}, returning...", textureName);
			return;
		}
	}
	Texture& Material::GetTexture(const std::string& name)
	{
		if (!(m_sampler2Ds.find(name) == m_sampler2Ds.end()))
			return *m_sampler2Ds[name].m_boundTexture;
		else
		{
			LINA_CORE_WARN("This material doesn't support texture slot with the name {0}, returning empty texture", name);
			return Texture();
		}
	}
}